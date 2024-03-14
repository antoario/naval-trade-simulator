#define _GNU_SOURCE

#ifndef PROGETTOSO_PORTO_H
#define PROGETTOSO_PORTO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include "utility/semafori/semafori.h"
#include "utility/ottieni_dati/ottieni_dati.h"
#include "utility/funzioni_condivise/funzioni_condivise.h"
#include "utility/memoria_condivisa/memoria_condivisa.h"
#include "utility/messaggi/messaggi.h"
#include "types.h"

#define PERMESSI 0666
#define MERCE merci_in_porto[i]


typedef struct porto_distanza
{
    int id_porto;
    double distanza;
} porto_distanza;

/**
 * Mescolare l'array
 * @param arr array da mescolare
 * @param size grandezza dell'array
 * @return void
 */
void shuffle_array(int *arr, size_t size);

/**
 * genera tipi di quantità
 * @return void
 */
void generazione_tipi_quantita(void);

/**
 * Funzione che aggiunge merce
 * @param tipo tipo della merce
 * @return quantità aggiunta
 */
int aggiungi_merce(int tipo);

/**
 * Funzione che genera le merci giornalmente
 * @return void
 */
void genera_merci_giorno(void);

/**
 * Funzione che genera le richieste
 * @retrun void
 */
void genera_richieste(void);

/**
 * Funzione utile per il confronto nella qsort
 * @param a primo elemento da confrontare
 * @param b secondo elemento da confrontare
 * @return risultato del confronto
 */
int compare(const void *a, const void *b);

/**
 * Funzione che effettua lo scarico della nave
 * @return void
 */
void scarico_nave(void);

/**
 * Genera i tipi di merce di richieste e offerte
 * @return void
 */
void genera_tipi_richieste_offerte(void);

/**
 * Funzione che permette l'attesa dell'arrivo di una nave nel porto
 * @return id della nave
 */
int aspetto_nave(void);

/**
 * Handler chiamato ogni giorno che provvede ad abbassare le scadenze e a generazione della merce
 * @param segnale segnale ricevuto
 * @return void
 */
void passa_un_giorno(int segnale);

/**
 * Funzione che definisce le offerte e le richieste dei porti
 * @return void
 */
void definisci_offerte_richieste(void);

#endif