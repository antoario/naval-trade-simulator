#include "nave.h"

struct input *dati_input;
struct matrice_merci *matrice_merci;
struct id_ipcs *memoria_master;
struct coordinate coordinate;
struct dati_utili *dati_dump_nave;
struct merce_info_dump *merci_dump;
enum stato_nave stato_nave;
struct porti_info_dump *info_porti_dump;
struct lista_merci stiva;
int sezione_critica, id_nave, id_porto_destinazione = 0, carico_max;
int *richieste_porti, *daScaricare, *porti_destinazione, *porti_vicini;

void shuffle(int *array, int n)
{
    if (n > 1)
    {
        int i;
        for (i = 0; i < n - 1; i++)
        {
            int j = i + random() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

int rimuovi_merce_specificata(lista_merci *lista, Merce merce_da_eliminare)
{
    Merce *corrente = lista->head;
    Merce *precedente = NULL;

    while (corrente)
    {
        if (corrente->tipo == merce_da_eliminare.tipo && corrente->quantita == merce_da_eliminare.quantita &&
            corrente->scadenza == merce_da_eliminare.scadenza)
        {
            if (precedente)
            {
                precedente->prossimo = corrente->prossimo;
            } else
            {
                lista->head = corrente->prossimo;
            }
            free(corrente);
            lista->num_merci--;
            return 0; /* Merce trovata e rimossa */
        }
        precedente = corrente;
        corrente = corrente->prossimo;
    }
    return -1; /* Merce non trovata */
}

/* Signal headler per aggiornare la scadenza */
void passato_giorno(void)
{
    int *scadute = abbassa_scadenze(&stiva.head, &stiva, SO_MERCI);
    int i, quant_totale = 0;
    for (; i < SO_MERCI; i++)
    {
        merci_dump[i].scaduta_in_nave += scadute[i];
        merci_dump[i].in_mare -= scadute[i];
        quant_totale += scadute[i];
    }
    sem_operazione(SEM_ASPETTA_GENERAZIONI, -1, 0);
}

void entro_in_porto(int porto)
{
    struct msg_contatto_navi_porto msg;
    sem_operazione(SEM_BANCHINE, -1, porto);
    msg.type = porto + 1;
    msg.id_nave = id_nave;
    invia_messaggio(MSG_CONTATTO_PORTI, &msg, sizeof(msg_contatto_navi_porto) - sizeof(long));
}

int cerca_prossima_destinazione(void)
{
    int i, destinazione;
    Merce *p_merce = stiva.head;
    destinazione = -1;

    if (stiva.num_merci == 0)
    {
        for (i = 0; i < SO_PORTI; i++)
        {
            if (info_porti_dump[porti_destinazione[i]].non_genera != 1)
            {
                return porti_destinazione[i];
            }
        }
    }

    while (p_merce != NULL)
    {
        for (i = 0; i < SO_PORTI; i++)
        {
            if (richieste_porti[porti_destinazione[i] * SO_MERCI + p_merce->tipo] > 0)
            {
                daScaricare[p_merce->tipo] += p_merce->quantita;
                LOCK
                richieste_porti[porti_destinazione[i] * SO_MERCI + p_merce->tipo] -= p_merce->quantita;
                UNLOCK
                destinazione = porti_destinazione[i];
                return destinazione;
            }
        }

        p_merce = p_merce->prossimo;
    }
    return destinazione;
}


void esco_dal_porto(int id_porto)
{
    sem_operazione(SEM_BANCHINE, 1, id_porto);
    cambia_stato(carica);
}

void scarico_merce(void)
{
    struct msg_porto_nave_scarico_carico msg;
    int quantita = 0;
    Merce *nuova_merce;
    Merce *p_merce = stiva.head;
    msg.mtype = id_nave + 1;
    while (p_merce != NULL)
    {

        if (daScaricare[p_merce->tipo] > 0)
        {
            msg.pacchetto = *p_merce;
            invia_messaggio(MSG_SCARICO_NAVI, &msg, sizeof(msg_porto_nave_scarico_carico) - sizeof(long));
            nuova_merce = p_merce->prossimo;
            carico_max += p_merce->quantita;

            LOCK
            merci_dump[p_merce->tipo].consegnata += p_merce->quantita;
            merci_dump[p_merce->tipo].in_mare -= p_merce->quantita;
            UNLOCK

            quantita += p_merce->quantita;
            daScaricare[p_merce->tipo] -= p_merce->quantita;
            if (rimuovi_merce_specificata(&stiva, *p_merce) == -1)
            {
                printf("errore in scarico nave\n");
            }
            p_merce = nuova_merce;
        } else if (richieste_porti[id_porto_destinazione * SO_MERCI + p_merce->tipo] > 0)
        {

            msg.pacchetto = *p_merce;
            invia_messaggio(MSG_SCARICO_NAVI, &msg, sizeof(msg_porto_nave_scarico_carico) - sizeof(long));
            nuova_merce = p_merce->prossimo;
            carico_max += p_merce->quantita;

            LOCK
            merci_dump[p_merce->tipo].consegnata += p_merce->quantita;
            merci_dump[p_merce->tipo].in_mare -= p_merce->quantita;
            richieste_porti[id_porto_destinazione * SO_MERCI + p_merce->tipo] -= p_merce->quantita;
            UNLOCK

            quantita += p_merce->quantita;
            rimuovi_merce_specificata(&stiva, *p_merce);

            p_merce = nuova_merce;
        } else
            p_merce = p_merce->prossimo;

    }
    msg.id_destinazione = -1;
    invia_messaggio(MSG_SCARICO_NAVI, &msg, sizeof(msg_porto_nave_scarico_carico) - sizeof(long));

    if (quantita != 0)
        attesa(calcolo_attesa(quantita));
}

void carico_merce(void)
{
    struct msg_porto_nave_scarico_carico msg;

    int quantita_caricata = 0;
    while (1)
    {
        ricevo_messaggio(MSG_CARICO_NAVI, &msg, sizeof(msg) - sizeof(long), id_nave + 1, 0);
        if (msg.id_destinazione == -1)
        {
            break;
        }
        quantita_caricata += msg.pacchetto.quantita;
        aggiungi_merce_in_stiva(msg.pacchetto.tipo, msg.pacchetto.quantita, msg.pacchetto.scadenza);
    }
    if (quantita_caricata != 0)
        attesa(calcolo_attesa(quantita_caricata));
    else
        cambia_stato(vuota);
}

int ricerca_porto_piu_vicino(struct coordinate coordinate_attuali)
{
    int i, ritorno;
    double distanza_posizione_piu_vicina, distanza_nuova;
    struct coordinate posizione_piu_vicina;
    posizione_piu_vicina = (info_porti_dump->coordinate);
    distanza_posizione_piu_vicina = calcolo_distanza(coordinate_attuali, posizione_piu_vicina);
    ritorno = 0;

    for (i = 1; i < SO_PORTI; i++)
    {
        distanza_nuova = calcolo_distanza(coordinate_attuali, info_porti_dump[i].coordinate);
        if (distanza_nuova < distanza_posizione_piu_vicina)
        {
            distanza_posizione_piu_vicina = distanza_nuova;
            ritorno = i;
        }
    }
    return ritorno;
}

struct timespec tempo_stimato(double distanza)
{
    struct timespec output;
    double tempo_double = distanza / SO_SPEED;
    output.tv_sec = (long) tempo_double;
    output.tv_nsec = ((long) tempo_double - output.tv_sec) * 1e9;
    return output;
}

void cambia_stato(enum stato_nave nuovo_stato)
{
    LOCK
    switch (stato_nave)
    {
        case carica:
            dati_dump_nave->n_navi_in_mare_con_carico--;
            break;
        case vuota:
            dati_dump_nave->n_navi_in_mare_senza_carico--;
            break;
        case in_porto:
            dati_dump_nave->n_navi_in_banchina--;
            break;
        default:
            perror("Errore nella modifica dello vecchio stato (in nave)\n");
    }
    switch (nuovo_stato)
    {
        case carica:
            dati_dump_nave->n_navi_in_mare_con_carico++;
            break;
        case vuota:
            dati_dump_nave->n_navi_in_mare_senza_carico++;
            break;
        case in_porto:
            dati_dump_nave->n_navi_in_banchina++;
            break;
        default:
            perror("Errore nella modifica dello nuovo stato (in nave)\n");
    }
    UNLOCK
    stato_nave = nuovo_stato;
}

void movimento_nave(struct coordinate coordinate_attuali, struct coordinate destinazione)
{
    attesa(tempo_stimato(calcolo_distanza(coordinate_attuali, destinazione)));
}

void attesa(struct timespec travel_time)
{
    struct timespec remaining_time;
    int errore;
    errore = nanosleep(&travel_time, &remaining_time);
    while (errore == -1 && errno == EINTR)
    {
        struct timespec temp_time = remaining_time;
        errore = nanosleep(&temp_time, &remaining_time);
    }
}

void aggiungi_merce_in_stiva(int tipo, int quantita, int scadenza)
{
    Merce *nuovo_nodo = (Merce *) malloc(sizeof(Merce));
    nuovo_nodo->tipo = tipo;
    nuovo_nodo->quantita = quantita;
    nuovo_nodo->scadenza = scadenza;
    nuovo_nodo->prossimo = stiva.head;
    stiva.head = nuovo_nodo;
    stiva.num_merci++;
    carico_max -= quantita;
}

struct timespec calcolo_attesa(int quantita)
{
    struct timespec waiting_time;
    double time;
    time = quantita / SO_LOADSPEED;
    waiting_time.tv_sec = (long) time;
    waiting_time.tv_nsec = ((long) time - waiting_time.tv_sec) * 1e9;
    return waiting_time;
}

int main(int argc, char *argv[])
{
    int id_prossima_destinazione, i;
    struct coordinate destinazione;
    struct sigaction act;

    stiva.head = NULL;
    stiva.num_merci = 0;

    bzero(&act, sizeof(act));
    act.sa_sigaction = (void (*)(int, siginfo_t *, void *)) &passato_giorno;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(SIGUSR1, &act, NULL) == -1)
    {
        perror("sigaction non riuscita (in main NAVE)");
        exit(EXIT_FAILURE);
    }

    id_nave = atoi(argv[2]);
    srandom(time(NULL) + getpid());
    memoria_master = shmat(atoi(argv[1]), NULL, 0);


    dati_input = shmat(SHM_DATI_INPUT, NULL, 0);
    carico_max = SO_CAPACITY;
    daScaricare = malloc(sizeof(int) * SO_MERCI);

    info_porti_dump = shmat(SHM_PORTI_DUMP, NULL, 0);
    matrice_merci = shmat(SHM_MATRICE_MERCI, NULL, 0);
    richieste_porti = shmat(SHM_MERCE_RICHIESTE, NULL, 0);
    merci_dump = shmat(SHM_MERCE_DUMP, NULL, 0);
    dati_dump_nave = shmat(SHM_DATI_UTILI, NULL, 0);

    sezione_critica = SEM_SEZIONE_CRITICA_NAVI;
    coordinate = genera_coordinate(SO_LATO);

    /* inizializzo dati_dump_nave*/
    dati_dump_nave->n_navi_in_banchina = 0;
    dati_dump_nave->n_navi_in_mare_con_carico = 0;
    dati_dump_nave->n_navi_in_mare_senza_carico = 1000;

    stato_nave = vuota;

    porti_destinazione = malloc(SO_PORTI * sizeof(int));

    /* crea il vettore delle destinazioni */
    for (i = 0; i < SO_PORTI; i++)
    {
        porti_destinazione[i] = i;
    }

    shuffle(porti_destinazione, SO_PORTI);
    sem_decrementa(SEM_CREAZIONE);
    sem_aspetta_zero(SEM_SI_PARTE);

    id_porto_destinazione = ricerca_porto_piu_vicino(coordinate);

    while (1)
    {
        struct msg_porto_nave_scarico_carico carico;

        /* naviga */
        destinazione = info_porti_dump[id_porto_destinazione].coordinate;
        porti_vicini = malloc(sizeof(int) * SO_PORTI);
        movimento_nave(coordinate, destinazione);

        /* entra */
        coordinate = destinazione;
        entro_in_porto(id_porto_destinazione);
        cambia_stato(in_porto);

        /* scarica */
        scarico_merce();

        /* comunica quanto puo caricare */
        carico.mtype = id_nave + 1;
        carico.id_destinazione = carico_max;
        invia_messaggio(MSG_PERMESSO_NAVI, &carico, sizeof(struct msg_porto_nave_scarico_carico) - 8);

        /* carica */
        carico_merce();

        /* cerco prossima destinazione */
        id_prossima_destinazione = cerca_prossima_destinazione();
        if (id_prossima_destinazione < 0)
        {
            id_prossima_destinazione = (int) random() % (int) SO_PORTI;
        }

        /* esce dal porto e naviga */
        esco_dal_porto(id_porto_destinazione);
        id_porto_destinazione = id_prossima_destinazione;
        shuffle(porti_destinazione, SO_PORTI);
    }
}
