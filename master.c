#include "master.h"

/* Array contenente i pid dei processi */
pid_t *array_pid;
int numero_processi = 0;

struct input *dati_input;

/* Signal handler che gestisce SIGINT(dealloca shm, sem e termina i processi) */
void signal_handler_interrupt()
{
    int i;
    terminazione_sem_shm();

    /* TERMINAZIONE PROCESSI */
    for (i = 0; i < numero_processi; i++)
    {
        kill(array_pid[i], SIGTERM);
    }
    printf("\nEsco con comando Ctrl+C\n");
    exit(EXIT_FAILURE);
}


void inizializza_merci(int id_memoria_info_merci)
{
    int i;
    struct merci_scadenza_quantita *info_merci;
    info_merci = shmat(id_memoria_info_merci, NULL, 0);
    for (i = 0; i < SO_MERCI; i++)
    {
        /* Genero la scadenza e la quantità */
        info_merci[i].scadenza = (int) random() % (SO_MAX_VITA + 1 - SO_MIN_VITA) + SO_MIN_VITA;
        info_merci[i].quantita = (int) (random() % SO_SIZE) + 1;
    }
}

void dump_merce(struct merce_info_dump *merce_info_dump, const int *in_porto, const int *merci_generate
                , const int *dati_dump_richieste, const int finale)
{
    int i, j, quant_generate, quant_porto, porto_piu_offerente = 0, temp_max_off, porto_piu_richiedente = 0,
            temp_max_ric;
#define MERCE merce_info_dump[i]
    for (i = 0; i < SO_MERCI; i++)
    {
        quant_generate = 0, quant_porto = 0, temp_max_off = -1, temp_max_ric = -1;
        for (j = 0; j < SO_PORTI; j++)
        {
            quant_porto += in_porto[j * SO_MERCI + i];
            quant_generate += merci_generate[j * SO_MERCI + i];

            if (merci_generate[j * SO_MERCI + i] > temp_max_off)
            {
                porto_piu_offerente = j;
                temp_max_off = merci_generate[j * SO_MERCI + i];
            }

            if (dati_dump_richieste[j * SO_MERCI + i] > temp_max_ric)
            {
                porto_piu_richiedente = j;
                temp_max_ric = dati_dump_richieste[j * SO_MERCI + i];
            }
        }

        printf("MERCE TIPO %d : \n"
               "Quantità totale generata:                    %-10d"
               "Quantità merce in porto:                     %-10d"
               "Quantità merce consegnata:                   %-10d\n"
               "Quantità merce in nave:                      %-10d"
               "Quantità merce scaduta in porto:             %-10d"
               "Quantità merce scaduta in nave:              %-10d\n", i, quant_generate, quant_porto,
               MERCE.consegnata, MERCE.in_mare, MERCE.scaduta_in_porto, MERCE.scaduta_in_nave);

        if (finale)
        {
            printf("Porto che ha offerto la quantità maggiore:   %-10d"
                   "Porto che ha richiesto la quantità maggiore: %-10d\n", porto_piu_offerente, porto_piu_richiedente);
        }
        printf("---------------------------\n");
    }

}

void dump_porti(struct porti_info_dump *porti_info_dump, int id_sem, struct merce_info_dump *merce_dump, int *in_porto)
{
#define PD porti_info_dump[i]
    int i, j, tot_spedite, tot_ricevute, tot_in_porto, tot_merci, tot_porti = 0;
    for (i = 0; i < SO_PORTI; i++)
    {
        tot_spedite = 0, tot_ricevute = 0, tot_in_porto = 0, tot_merci = 0;
        for (j = 0; j < SO_MERCI; j++)
        {
            tot_merci += in_porto[i * SO_MERCI + j];
        }
        tot_porti += PD.ricevuta;
        tot_spedite += PD.spedita;
        tot_ricevute += PD.ricevuta;
        tot_in_porto += PD.presente;
        printf("Il porto %-4d ha:\nmerce presente: %-10d merce spedita: %-10d merce ricevuta: %-10d banchine libere: %-10d banchine totali: %-10d\n",
               i, tot_merci, PD.spedita, PD.ricevuta, semctl(id_sem, i, GETVAL), SO_BANCHINE);
        printf("--------------------\n");
    }
}

void dump_navi(struct dati_utili *nave_dump)
{
    printf("DUMP NAVI:\ntotale navi in mare con carico: %d  totale navi in mare senza carico: %d  navi in porto: %d \n",
           nave_dump->n_navi_in_mare_con_carico, nave_dump->n_navi_in_mare_senza_carico, nave_dump->n_navi_in_banchina);
    printf("--------------------\n");
}

void report_finale(struct id_ipcs *memoria_master, struct merce_info_dump *merce_info_dump
                   , struct porti_info_dump *porti_info_dump, int *in_porto, struct dati_utili *dati_utili_master)
{
#define MERCE merce_info_dump[i]
    printf("-----------| REPORT FINALE |-----------\n");

    dump_navi(dati_utili_master);
    dump_porti(porti_info_dump, SEM_BANCHINE, merce_info_dump, in_porto);
}

int main(void)
{
    int i, j, id_memoria_master, dati_input_id, *array_porti, *in_porto, *merci_generate, *dati_dump_richieste;
    struct sigaction sa;
    char *argomenti[4], str_id_memoria_master[13];
    struct id_ipcs *memoria_master;
    struct dati_utili *dati_utili_master;
    struct porti_info_dump *porti_info_dump;
    struct merce_info_dump *merce_info_dump;

    argomenti[3] = (char *) 0;
    /* Setting struct sa a 0 ed assegnamento dell'handler per la gestione di SIGINT */
    bzero(&sa, sizeof(sa));
    sa.sa_handler = signal_handler_interrupt;
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction per segnale SIGUSR1 non riuscita (in master)");
        exit(EXIT_FAILURE);
    }

    /* Invoco il metodo read_input per leggere i parametri da file input */
    dati_input_id = read_input();
    dati_input = shmat(dati_input_id, NULL, 0);

    if (SO_PORTI < 4)
    {
        printf("Minimo 4 porti!\n");
        exit(EXIT_FAILURE);
    }

    array_porti = malloc(SO_PORTI * sizeof(int));

    /* Capisco quanti PID mi serviranno */
    array_pid = malloc(sizeof(array_pid) * (SO_NAVI + SO_PORTI));
    numero_processi = SO_NAVI + SO_PORTI;

    /* Inizializzo la memoria master */
    id_memoria_master = crea_ipcs(dati_input_id);
    memoria_master = shmat(id_memoria_master, NULL, 0);

    dati_utili_master = shmat(SHM_DATI_UTILI, NULL, 0);
    merci_generate = shmat(SHM_MERCE_GENERATA, NULL, 0);
    dati_utili_master->porti_non_generanti = 0;
    sprintf(str_id_memoria_master, "%d", id_memoria_master);
    argomenti[1] = str_id_memoria_master;

    /* Imposto il valore predefinito del semaforo della creazione */
    sem_imposta_valore(SEM_CREAZIONE, 0, SO_PORTI + SO_NAVI);

    printf("Creazione porti e navi in corso...\n");

    /* Inizializzo le merci */
    inizializza_merci(SHM_MERCE_INFO);

    /* Creazione NAVI */
    for (i = 0; i < SO_NAVI; i++)
    {
        char str_numero_nave[13];
        int child = fork();
        switch (child)
        {
            case 0:
            {
                sprintf(str_numero_nave, "%d", i);
                argomenti[0] = PROCESSO_NAVE;
                argomenti[2] = str_numero_nave;
                execve(PROCESSO_NAVE, argomenti, NULL);
                perror("execve per nave non riuscita (in master)");
                exit(EXIT_FAILURE);
            }
            case -1:
                printf("Errore nella creazione della nave\n");
                exit(EXIT_FAILURE);
            default:
                array_pid[i] = child;
        }
    }

    sem_imposta_valore(SEM_CREAZIONE, 1, SO_PORTI);

    /* Creazione PORTI */
    for (i = 0; i < SO_PORTI; i++)
    {
        char str_numero_porto[13];
        int child = fork();
        switch (child)
        {
            case 0:
                sprintf(str_numero_porto, "%d", i);
                argomenti[0] = PROCESSO_PORTO;
                argomenti[2] = str_numero_porto;
                execve(PROCESSO_PORTO, argomenti, NULL);
                perror("execve per porto non riuscita (in master)");
                exit(EXIT_FAILURE);
            case -1:
                printf("Errore nella creazione del info_porti\n");
                exit(EXIT_FAILURE);
            default:
                array_pid[i + SO_NAVI] = child;
                array_porti[i] = child;
        }
    }

    if (sem_aspetta_zero(SEM_CREAZIONE) == -1)
    {
        perror("sem_aspetta_zero non riuscita (in master)");
        exit(EXIT_FAILURE);
    }

    printf("I porti e le navi si sono creati con successo\n");

    semctl(SEM_SI_PARTE, 0, SETVAL, 1);

    porti_info_dump = shmat(SHM_PORTI_DUMP, NULL, 0);
    merce_info_dump = shmat(SHM_MERCE_DUMP, NULL, 0);
    in_porto = shmat(SHM_IN_PORTO, NULL, 0);
    dati_dump_richieste = shmat(SHM_RICHIESTE_DUMP, NULL, 0);

    for (i = 0; i < SO_DAYS; i++)
    {
        semctl(SEM_ASPETTA_GENERAZIONI, 0, SETVAL, SO_PORTI + SO_NAVI);

        for (j = 0; j < SO_PORTI + SO_NAVI; j++)
        {
            kill(array_pid[j], SIGUSR1);
        }
        sem_aspetta_zero(SEM_ASPETTA_GENERAZIONI);

        printf("Sono passati %d giorni \n", i);
        dump_merce(merce_info_dump, in_porto, merci_generate, dati_dump_richieste, 0);
        dump_navi(dati_utili_master);
        dump_porti(porti_info_dump, SEM_BANCHINE, merce_info_dump, in_porto);

        semctl(SEM_SI_PARTE, 0, SETVAL, 0);
        sleep(1);
    }

    /* TERMINAZIONE dei processi */
    for (i = 0; i < numero_processi; i++)
    {
        kill(array_pid[i], SIGTERM);
    }

    report_finale(memoria_master, merce_info_dump, porti_info_dump, in_porto, dati_utili_master);
    dump_merce(merce_info_dump, in_porto, merci_generate, dati_dump_richieste, 1);
    terminazione_sem_shm();
    exit(EXIT_SUCCESS);
}
