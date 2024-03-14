#define _GNU_SOURCE

#ifndef PROGETTOSO_SEMAFORI_H
#define PROGETTOSO_SEMAFORI_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <errno.h>
#include <sys/sem.h>
#include <stdlib.h>
#include "../../types.h"

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/**
 * Funzione che decrementa di 1 il id_semaforo id_semaforo posizione 0
 * @param id_semaforo id del semaforo
 * @return int utile per controllo
 */
int sem_decrementa(int id_semaforo);

/**
 * Funzione che aumenta/decrementa il valore del semaforo in base al valore assegnatoli
 * @param id_semaforo id del semaforo su cui fare l'operazione
 * @param valore val per operazione
 * @param numero_semaforo numero da assegnare al semaforo
 * @return int utile per controllo
 */
int sem_operazione(int id_semaforo, short valore, int numero_semaforo);

/**
 * Funzione che aspetta che il valore del semaforo alla posizione 0 sia = a 0
 * @param id_semaforo id del semaforo
 * @return int utile per controllo
 */
int sem_aspetta_zero(int id_semaforo);

/**
 * Funzione che imposta il valore di un semaforo
 * @param semid id del semaforo
 * @param semnum numero semaforo
 * @param val valore da impostare
 * @return int utile per controllo
 */
int sem_imposta_valore(int semid, int semnum, int val);

#endif
