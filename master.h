#define _GNU_SOURCE

#ifndef PROGETTOSO_MASTER_H
#define PROGETTOSO_MASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/msg.h>
#include "utility/semafori/semafori.h"
#include "utility/ottieni_dati/ottieni_dati.h"
#include "utility/memoria_condivisa/memoria_condivisa.h"
#include "utility/funzioni_condivise/funzioni_condivise.h"
#include "types.h"

#define PERMESSI 0666
#define PROCESSO_NAVE "./out/nave"
#define PROCESSO_PORTO "./out/porto"

/**
 * Handler che gestisce il segnale di interruzione chiudendo gli ipcs e terminando i processi
 * @return void
 */
void signal_handler_interrupt();

/**
 * Funzione che permette di inizializzare la scadenza e la quantità delle merci
 * @param id_memoria_info_merci memoria in cui si salva il tutto
 * @return void
 */
void inizializza_merci(int id_memoria_info_merci);

/**
 * Funzione per la stampa del dump delle navi
 * @param merce_info_dump memoria con valori delle merci
 * @param in_porto memoria con valori delle merci nei porti
 * @param merci_generate memoria con i valori delle merci generate nei giorni
 * @param dati_dump_richieste memoria con i valori delle richieste
 * @param finale int che permette di capire se si sta stampando il report finale o meno
 * @return void
 */
void dump_merce(struct merce_info_dump *merce_info_dump, const int *in_porto, const int *merci_generate
        , const int *dati_dump_richieste, const int finale);

/**
 * Funzione per la stampa del dump dei porti
 * @param porti_info_dump struct con i valori da dover stampare
 * @param id_sem semaforo banchine
 * @param merce_dump strcut con i valori delle merci da stampare
 * @param in_porto puntatore all'array che contiene le merci per ogni tipo in ogni porto
 * @return void
 */
void dump_porti(struct porti_info_dump *porti_info_dump, int id_sem, struct merce_info_dump *merce_dump, int *in_porto);

/**
 * Funzione che stampa il report finale
 * @param memoria_master memoria contenente tutti gli ipcs
 * @param merce_info_dump  memoria con i dati delle merci da stampare
 * @param porti_info_dump memoria con i dati dei porti da stampare
 * @param in_porto puntatore all'array che contiene le merci per ogni tipo in ogni porto
 * @return void
 */
void report_finale(struct id_ipcs *memoria_master, struct merce_info_dump *merce_info_dump
                   , struct porti_info_dump *porti_info_dump, int *in_porto, struct dati_utili *dati_utili_master);

#endif
