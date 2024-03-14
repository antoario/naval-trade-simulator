#include "porto.h"

extern struct id_ipcs *memoria_master;
struct coordinate coordinate;
struct input *dati_input;
struct lista_merci merci_in_porto;
int num_richieste_tipo, num_offerte_tipo, *richieste_tipo, *offerte_tipo, id_porto, quantita_generica,
        id_nave_attraccata, *richieste_porti, sezione_critica, non_generante, quantita_richieste, non_richiedente,
        *porti_piu_vicini, *dati_dump_in_porto, *merce_generata, *dati_dump_richieste;
struct porti_info_dump *info_dump;
struct merce_info_dump *merci_dump;
struct dati_utili *dati_utili_porto;
struct merci_scadenza_quantita *scadenze_tipo;
struct msg_porti_vicini msg_porti_vic;

int aggiungi_merce(int tipo)
{
    Merce *nuovo_nodo = (Merce *) malloc(sizeof(Merce));

    nuovo_nodo->tipo = tipo;
    nuovo_nodo->quantita = scadenze_tipo[tipo].quantita;
    nuovo_nodo->scadenza = scadenze_tipo[tipo].scadenza;
    nuovo_nodo->prossimo = merci_in_porto.head;
    merci_in_porto.head = nuovo_nodo;
    merci_in_porto.num_merci++;
    info_dump[id_porto].presente += scadenze_tipo[tipo].quantita;
    dati_dump_in_porto[id_porto * SO_MERCI + tipo] += scadenze_tipo[tipo].quantita;
    merce_generata[id_porto * SO_MERCI + tipo] += scadenze_tipo[tipo].quantita;
    return scadenze_tipo[tipo].quantita;
}

void genera_merci_giorno(void)
{
    int i = 0, tot = 0;
    while (quantita_generica > tot + scadenze_tipo[i % num_offerte_tipo].quantita)
    {
        tot += aggiungi_merce(offerte_tipo[i % num_offerte_tipo]);
        i++;
    }
}

void genera_richieste(void)
{
    int i = 0, tot = 0;
    while (quantita_richieste > tot)
    {
        richieste_porti[id_porto * SO_MERCI + richieste_tipo[i % num_richieste_tipo]] +=
                scadenze_tipo[richieste_tipo[i % num_richieste_tipo]].quantita;
        dati_dump_richieste[id_porto * SO_MERCI + richieste_tipo[i % num_richieste_tipo]] +=
                scadenze_tipo[richieste_tipo[i % num_richieste_tipo]].quantita;
        tot += scadenze_tipo[richieste_tipo[i % num_richieste_tipo]].quantita;
        i++;
    }
}

int compare(const void *a, const void *b)
{
    porto_distanza *portoA = (porto_distanza *) a;
    porto_distanza *portoB = (porto_distanza *) b;
    if (portoA->distanza < portoB->distanza)
        return -1;
    if (portoA->distanza > portoB->distanza)
        return 1;
    return 0;
}

void scarico_nave(void)
{
    struct msg_porto_nave_scarico_carico msg;
    while (1)
    {
        ricevo_messaggio(MSG_SCARICO_NAVI, &msg, sizeof(msg_porto_nave_scarico_carico) - sizeof(long),
                         id_nave_attraccata + 1, 0);
        if (msg.id_destinazione == -1)
            break;
        info_dump[id_porto].ricevuta += msg.pacchetto.quantita;
    }
}

void genera_tipi_richieste_offerte(void)
{
    generazione_tipi_quantita();
    offerte_tipo = malloc(num_offerte_tipo * sizeof(int));
    richieste_tipo = malloc(num_richieste_tipo * sizeof(int));
    definisci_offerte_richieste();
}

void generazione_tipi_quantita(void)
{
    int richieste_offerte = (int) random() % (SO_MERCI) + 1;
    int total_len = (int) random() % (richieste_offerte + 1);

    num_richieste_tipo = total_len;
    num_offerte_tipo = richieste_offerte - total_len;
    if (num_offerte_tipo == 0)
    {
        non_generante = 1;
        info_dump[id_porto].non_genera = 1;
        LOCK
        dati_utili_porto->porti_non_generanti += 1;
        UNLOCK
    }

    if (num_richieste_tipo == 0)
    {
        non_richiedente = 1;
        LOCK
        dati_utili_porto->porti_non_richiedenti += 1;
        UNLOCK
    }
}

int aspetto_nave(void)
{
    struct msg_contatto_navi_porto msg;
    ricevo_messaggio(MSG_CONTATTO_PORTI, &msg, sizeof(msg_contatto_navi_porto) - sizeof(long), id_porto + 1, 0);
    return msg.id_nave;
}

void carica_nave(int carico_max)
{
    struct msg_porto_nave_scarico_carico msg;
    int quantita_caricata = 0;
    Merce *p_merce = merci_in_porto.head;
    Merce *prossima = NULL;

    msg.mtype = id_nave_attraccata + 1;
    msg.id_destinazione = 1;
    while (p_merce != NULL)
    {
        if (carico_max < quantita_caricata + p_merce->quantita)
        {
            p_merce = p_merce->prossimo;
            continue;
        }

        msg.pacchetto = *p_merce;
        invia_messaggio(MSG_CARICO_NAVI, &msg, sizeof(struct msg_porto_nave_scarico_carico) - sizeof(long));
        if (msg.id_destinazione == -1)
        {
            return;
        }
        info_dump[id_porto].spedita += msg.pacchetto.quantita;
        info_dump[id_porto].presente -= msg.pacchetto.quantita;
        dati_dump_in_porto[id_porto * SO_MERCI + msg.pacchetto.tipo] -= msg.pacchetto.quantita;
        LOCK
        merci_dump[msg.pacchetto.tipo].in_mare += msg.pacchetto.quantita;
        UNLOCK

        quantita_caricata += p_merce->quantita;
        prossima = p_merce->prossimo;
        elimina_nodo(&merci_in_porto, p_merce);
        p_merce = prossima;

        if (quantita_caricata == carico_max)
            break;
    }


    msg.id_destinazione = -1;
    invia_messaggio(MSG_CARICO_NAVI, &msg, sizeof(struct msg_porto_nave_scarico_carico) - sizeof(long));
}


void passa_un_giorno(int segnale)
{
    int *scadute = abbassa_scadenze(&merci_in_porto.head, &merci_in_porto, SO_MERCI);
    int i = 0;
    for (; i < SO_MERCI; i++)
    {
        merci_dump[i].scaduta_in_porto += scadute[i];
        dati_dump_in_porto[id_porto * SO_MERCI + i] -= scadute[i];
        info_dump[id_porto].presente -= scadute[i];
    }

    if (!non_generante)
        genera_merci_giorno();
    if (!non_richiedente)
        genera_richieste();

    sem_operazione(SEM_ASPETTA_GENERAZIONI, -1, 0);
}


void shuffle_array(int *arr, size_t size)
{
    size_t i;
    /* Inizializzazione il generatore di numeri casuali */
    for (i = size - 1; i > 0; i--)
    {

        /* Scelta dell'indice casuale tra 0 e i (inclusi) */
        size_t j = random() % (i);

        /* Scambio l'elemento i-esimo con quello j-esimo */
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void definisci_offerte_richieste(void)
{
    int i, k, *all_nums;
    all_nums = malloc(sizeof(int) * SO_MERCI);

    for (i = 0; i < SO_MERCI; i++)
    {
        all_nums[i] = i;
    }
    shuffle_array(all_nums, SO_MERCI);

    for (i = 0; i < num_offerte_tipo; i++)
    {
        offerte_tipo[i] = all_nums[i];
    }

    for (k = 0; k < num_richieste_tipo; k++)
    {
        richieste_tipo[k] = all_nums[i + k];
    }
}


int main(int argc, char *argv[])
{
    struct sigaction act;
    int j, id_memoria_master, banchine;
    struct porto_distanza *distanza_porti;
    srandom(time(NULL) * getpid());


    merci_in_porto.head = NULL;
    merci_in_porto.num_merci = 0;
    non_generante = 0;
    non_richiedente = 0;

    /* imposto l'handler */
    bzero(&act, sizeof(act));
    act.sa_handler = &passa_un_giorno;
    if (sigaction(SIGUSR1, &act, NULL) == -1)
    {
        perror("sigaction non riuscita in main (PORTO)");
        exit(EXIT_FAILURE);
    }

    /* Ottengo id info_porti dall'argv */
    id_memoria_master = atoi(argv[1]);
    id_porto = atoi(argv[2]);

    /* Attach per memorie che servono */
    memoria_master = shmat(id_memoria_master, NULL, 0);
    dati_input = shmat(SHM_DATI_INPUT, NULL, 0);
    scadenze_tipo = shmat(SHM_MERCE_INFO, NULL, 0);
    richieste_porti = shmat(SHM_MERCE_RICHIESTE, NULL, 0);
    merci_dump = shmat(SHM_MERCE_DUMP, NULL, 0);
    info_dump = shmat(SHM_PORTI_DUMP, NULL, 0);
    dati_utili_porto = shmat(SHM_DATI_UTILI, NULL, 0);
    dati_dump_in_porto = shmat(SHM_IN_PORTO, NULL, 0);
    merce_generata = shmat(SHM_MERCE_GENERATA, NULL, 0);
    dati_dump_richieste = shmat(SHM_RICHIESTE_DUMP, NULL, 0);
    sezione_critica = SEM_SEZIONE_CRITICA_PORTI;

    for (j = 0; j < SO_MERCI; j++)
    {
        richieste_porti[id_porto * SO_MERCI + j] = 0;
    }

    /* Switch per poter creare i porti negli angoli */
    switch (id_porto)
    {
        case 0:
            coordinate.x = 0;
            coordinate.y = 0;
            break;
        case 1:
            coordinate.x = SO_LATO;
            coordinate.y = 0;
            break;
        case 2:
            coordinate.x = 0;
            coordinate.y = SO_LATO;
            break;
        case 3:
            coordinate.x = SO_LATO;
            coordinate.y = SO_LATO;
            break;
        default:
            coordinate = genera_coordinate(SO_LATO);
            break;
    }

    /* Gestione delle banchine */
    banchine = SO_BANCHINE;
    distanza_porti = malloc(sizeof(struct porto_distanza) * SO_PORTI);

    sem_imposta_valore(SEM_BANCHINE, id_porto, banchine);

    /* Genero merce e quantità */
    genera_tipi_richieste_offerte();
    sem_operazione(SEM_CREAZIONE, -1, 1);
    sem_operazione(SEM_CREAZIONE, 0, 1);

    quantita_generica = (SO_FILL / SO_DAYS / (SO_PORTI - dati_utili_porto->porti_non_generanti));
    quantita_richieste = (SO_FILL / SO_DAYS / (SO_PORTI - dati_utili_porto->porti_non_richiedenti));

    info_dump[id_porto].coordinate = coordinate;

    porti_piu_vicini = malloc(SO_PORTI * sizeof(char));


    for (j = 0; j < SO_PORTI; j++)
    {
        distanza_porti[j].distanza = calcolo_distanza(coordinate, info_dump[j].coordinate);
        distanza_porti[j].id_porto = j;
    }

    qsort(distanza_porti, SO_PORTI, sizeof(porto_distanza), compare);

    for (j = 0; j < SO_PORTI; j++)
    {
        msg_porti_vic.msg_data[j] = distanza_porti[j].id_porto;
    }

    /* Il semaforo che fa iniziare a lavorare nevi e porti */
    sem_decrementa(SEM_CREAZIONE);
    sem_aspetta_zero(SEM_SI_PARTE);

    /* Inizio a lavorare */
    while (1)
    {
        int carico_max;
        struct msg_porto_nave_scarico_carico carico;
        id_nave_attraccata = aspetto_nave();
        scarico_nave();
        ricevo_messaggio(MSG_PERMESSO_NAVI, &carico, sizeof(struct msg_porto_nave_scarico_carico) - 8,
                         id_nave_attraccata + 1, 0);
        carico_max = carico.id_destinazione;
        carica_nave(carico_max);
    }
}
