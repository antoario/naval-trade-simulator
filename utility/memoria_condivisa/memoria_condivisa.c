#include "memoria_condivisa.h"

/* Arrays che contengono tutti gli shm_id e gli sem_id da deallocare */
int *id_to_remove_shm;
int *id_to_remove_sem;
int *id_to_remove_msg;
int numero_semafori = 0;
int numero_memorie = 0;
int numero_code_msg = 0;
int id_memoria_master;
struct id_ipcs *memoria_master;

int crea_ipcs(int id_shm_dati_input)
{
    struct input *dati_input;
    union semun
    {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;
    unsigned short *valori;
    int i;

    id_to_remove_shm = malloc(8);
    id_to_remove_sem = malloc(8);
    id_to_remove_msg = malloc(8);
    id_memoria_master = shmget(IPC_PRIVATE, sizeof(struct id_ipcs), IPC_CREAT | PERMESSI);
    memoria_master = shmat(id_memoria_master, NULL, 0);

    dati_input = shmat(id_shm_dati_input, NULL, 0);

    SHM_DATI_INPUT = id_shm_dati_input;
    aggiungi_shm_id(id_shm_dati_input);

    SEM_CREAZIONE = crea_semaforo(1, 2);
    SEM_SI_PARTE = crea_semaforo(0, 1);
    SEM_ASPETTA_GENERAZIONI = crea_semaforo(1, 1);
    SEM_SEZIONE_CRITICA_NAVI = crea_semaforo(0, 1);
    SEM_SEZIONE_CRITICA_PORTI = crea_semaforo(0, 1);

    SHM_MERCE_RICHIESTE = crea_memoria_condivisa(SO_PORTI * SO_MERCI * sizeof(int));
    SHM_MERCE_INFO = crea_memoria_condivisa(SO_MERCI * sizeof(struct merci_scadenza_quantita));
    SHM_MERCE_DUMP = crea_memoria_condivisa(SO_MERCI * sizeof(struct merce_info_dump));
    SHM_PORTI_DUMP = crea_memoria_condivisa(SO_PORTI * sizeof(struct porti_info_dump));
    SHM_DATI_UTILI = crea_memoria_condivisa(sizeof(struct dati_utili));
    SHM_IN_PORTO = crea_memoria_condivisa(SO_PORTI * SO_MERCI * sizeof(int));
    SHM_MERCE_GENERATA = crea_memoria_condivisa(SO_PORTI * SO_MERCI * sizeof(int));
    SHM_RICHIESTE_DUMP = crea_memoria_condivisa(SO_PORTI * SO_MERCI * sizeof(int));

    MSG_CONTATTO_PORTI = crea_coda_messaggi();
    MSG_PID_PROC = crea_coda_messaggi();
    MSG_PERMESSO_NAVI = crea_coda_messaggi();
    MSG_SCARICO_NAVI = crea_coda_messaggi();
    MSG_CARICO_NAVI = crea_coda_messaggi();

    /* Creazione semaforo per banchine */
    SEM_BANCHINE = semget(IPC_PRIVATE, SO_PORTI, IPC_CREAT | IPC_EXCL | PERMESSI);
    if (SEM_BANCHINE == -1)
    {
        perror("Errore creazione semaforo banchine");
        exit(EXIT_FAILURE);
    }
    aggiungi_sem_id(SEM_BANCHINE);

    /* Creazione semaforo per gestire incremento delle banchine */
    SEM_INCREMENTO_BANCHINE = semget(IPC_PRIVATE, SO_PORTI, IPC_CREAT | IPC_EXCL | PERMESSI);
    if (SEM_INCREMENTO_BANCHINE == -1)
    {
        perror("Errore creazione semaforo incremento banchine");
        exit(EXIT_FAILURE);
    }
    aggiungi_sem_id(SEM_INCREMENTO_BANCHINE);

    valori = malloc(SO_PORTI * sizeof(unsigned short));
    for (i = 0; i < SO_PORTI; i++)
    {
        valori[i] = 1;
    }
    arg.array = valori;
    semctl(SEM_INCREMENTO_BANCHINE, 0, SETALL, arg);
    free(valori);

    /* reazione semaforo per gestire il dump delle merci */
    SEM_MERCI_DUMP = semget(IPC_PRIVATE, SO_MERCI, IPC_CREAT | IPC_EXCL | PERMESSI);
    if (SEM_MERCI_DUMP == -1)
    {
        perror("Errore creazione semaforo incremento banchine");
        exit(EXIT_FAILURE);
    }

    valori = malloc(SO_MERCI * sizeof(unsigned short));

    for (i = 0; i < SO_MERCI; i++)
    {
        valori[i] = 1;
    }
    arg.array = valori;
    semctl(SEM_MERCI_DUMP, 0, SETALL, arg);
    free(valori);
    aggiungi_sem_id(SEM_MERCI_DUMP);

    aggiungi_shm_id(id_memoria_master);
    return id_memoria_master;
}

int crea_semaforo(int sezione_critica, int numeri_semafori)
{
    int sem_id = semget(IPC_PRIVATE, numeri_semafori, IPC_CREAT | IPC_EXCL | PERMESSI);
    if (sezione_critica == 0)
    {
        if (semctl(sem_id, 0, SETVAL, 1) == -1)
        {
            perror("semctl in crea_semaforo non riuscita");
            exit(EXIT_FAILURE);
        }
    }
    if (sem_id == -1)
    {
        perror("Creazione semaforo non riuscita");
        exit(EXIT_FAILURE);
    }
    aggiungi_sem_id(sem_id);
    return sem_id;
}


int crea_memoria_condivisa(int spazio_necessario)
{
    int shm_id = shmget(IPC_PRIVATE, spazio_necessario, IPC_CREAT | PERMESSI);
    if (shm_id == -1)
    {
        perror("Creazione memoria condivisa non riuscita");
        exit(EXIT_FAILURE);
    }
    aggiungi_shm_id(shm_id);

    return shm_id;
}


int crea_coda_messaggi(void)
{
    int msg_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (msg_id == -1)
    {
        perror("Creazione coda di messaggi non riuscita");
        exit(EXIT_FAILURE);
    }
    aggiungi_msg_id(msg_id);

    return msg_id;
}

void aggiungi_sem_id(int sem_id)
{
    id_to_remove_sem[numero_semafori] = sem_id;
    numero_semafori++;
    id_to_remove_sem = realloc(id_to_remove_sem, 8 * numero_semafori);
}

void aggiungi_shm_id(int shm_id)
{
    id_to_remove_shm[numero_memorie] = shm_id;
    numero_memorie++;
    id_to_remove_shm = realloc(id_to_remove_shm, 8 * numero_memorie);
}

void aggiungi_msg_id(int msg_id)
{
    id_to_remove_msg[numero_code_msg] = msg_id;
    numero_code_msg++;
    id_to_remove_msg = realloc(id_to_remove_msg, 8 * numero_code_msg);
}

void terminazione_sem_shm()
{
    int i;

    /* RIMOZIONE SHM */
    for (i = 0; i < numero_memorie; i++)
    {
        if (shmctl(id_to_remove_shm[i], IPC_RMID, NULL) == -1)
        {
            perror("Rimozione memoria condivisa non riuscita");
            exit(EXIT_FAILURE);
        }
    }

    /* RIMOZIONE SEM */
    for (i = 0; i < numero_semafori; i++)
    {
        semctl(id_to_remove_sem[i], 0, IPC_RMID);
    }

    /* RIMOZIONE MSG queues */
    for (i = 0; i < numero_code_msg; i++)
    {
        msgctl(id_to_remove_msg[i], IPC_RMID, NULL);
    }
}