#define _GNU_SOURCE

#ifndef PROGETTOSO_MEMORIA_CONDIVISA_H
#define PROGETTOSO_MEMORIA_CONDIVISA_H

#include <stdio.h>
#include <strings.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <sys/msg.h>
#include "../ottieni_dati/ottieni_dati.h"
#include "../../types.h"

#define SEM_CREAZIONE memoria_master->sem_creazione
#define SHM_DATI_INPUT memoria_master->shm_dati_input
#define SHM_MATRICE_MERCI memoria_master->shm_matrice_merci
#define SHM_MERCE_INFO memoria_master->shm_merce_info
#define SHM_MERCE_DUMP memoria_master->shm_merce_dump
#define SHM_PORTI_DUMP memoria_master->shm_porto_dump
#define SHM_DATI_UTILI memoria_master->shm_dati_utili
#define SHM_MERCE_RICHIESTE memoria_master->shm_merce_richieste
#define SHM_IN_PORTO memoria_master->shm_in_porto
#define SHM_MERCE_GENERATA memoria_master->shm_merce_generata
#define SHM_RICHIESTE_DUMP memoria_master->shm_richieste_dump
#define SEM_SI_PARTE memoria_master->sem_si_parte
#define SEM_INCREMENTO_BANCHINE memoria_master->sem_incremento_banchine
#define SEM_ASPETTA_GENERAZIONI memoria_master->sem_aspetta_generazioni
#define SEM_SEZIONE_CRITICA_PORTI memoria_master->sem_sezione_critica_porti
#define SEM_SEZIONE_CRITICA_NAVI memoria_master->sem_sezione_critica_navi
#define SEM_BANCHINE memoria_master->sem_id_banchine
#define SEM_MERCI_DUMP memoria_master->sem_merci_dump
#define MSG_CONTATTO_PORTI memoria_master->msg_banchine_porti
#define MSG_PID_PROC memoria_master->msg_pid_processi
#define MSG_PERMESSO_NAVI memoria_master->msg_permesso_navi
#define MSG_SCARICO_NAVI memoria_master->msg_scarico_navi
#define MSG_CARICO_NAVI memoria_master->msg_carico_navi

#define PERMESSI 0666

struct id_ipcs
{
    int shm_dati_input;
    int shm_matrice_merci;
    int shm_merce_dump;
    int shm_dati_utili;
    int shm_porto_dump;
    int shm_merce_richieste;
    int shm_merce_info;
    int shm_in_porto;
    int shm_merce_generata;
    int shm_richieste_dump;
    int sem_creazione;
    int sem_si_parte;
    int sem_sezione_critica_navi;
    int sem_sezione_critica_porti;
    int sem_aspetta_generazioni;
    int sem_incremento_banchine;
    int sem_merci_dump;
    int msg_banchine_porti;
    int sem_id_banchine;
    int msg_permesso_navi;
    int msg_scarico_navi;
    int msg_carico_navi;
    int msg_pid_processi;
};

/**
 * Funzione che crea una struct cno tutti gli ipcs che servono
 * @param id_shm_dati_input id della memoria contenente i parametri inseriti da input file
 * @return int ID memoria che contiene tutti gli ipcs
 */
int crea_ipcs(int id_shm_dati_input);

/**
 * Crea la memoria condivisa
 * @param spazio_necessario spazio necessario per allocazione memoria condivisa
 * @return ID memoria condivisa appena creata
 */
int crea_memoria_condivisa(int spazio_necessario);

/**
 * Funzione che crea il semaforo
 * @param sezione_critica serve per capire se si sta creando una sezione critica
 * @param numeri_semafori n semafori
 * @return ID del semaforo appena creato
 */
int crea_semaforo(int sezione_critica, int numeri_semafori);

/**
 * Funzione che crea una memoria condivisa
 * @param spazio_necessario spazio della memoria
 * @return id della memoria
 */
int crea_memoria_condivisa(int spazio_necessario);

/**
 * Funzione che crea una coda di messaggi
 * @return id della coda
 */
int crea_coda_messaggi();

/**
 * Funzione che ggiunge l'ID del semaforo nell'array per la deallocazione
 * @param sem_id id del semaforo
 * @return void
 */
void aggiungi_sem_id(int sem_id);

/**
 * Funzione che aggiunge l'ID della memoria condivisa nell'array per la deallocazione
 * @param shm_id id della memoria condivisa
 * @return void
 */
void aggiungi_shm_id(int shm_id);

/**
 * Funzione che aggiunge l'ID della coda di messaggi nell'array per la deallocazione
 * @param msg_id id della coda di messaggi
 * @return void
 */
void aggiungi_msg_id(int msg_id);

/**
 * Rimuove i semafori, le code di messaggi e le memorie condivise alla fine dell'esecuzione
 * @return void
 */
void terminazione_sem_shm();

#endif
