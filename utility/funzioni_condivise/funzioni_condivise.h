#define _GNU_SOURCE

#ifndef PROGETTOSO_FUNZIONI_CONDIVISE_H
#define PROGETTOSO_FUNZIONI_CONDIVISE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include "../../types.h"


/**
 * Funzione che controlla se due merci sono uguali in tutto
 * @param uno prima merce da confrontare
 * @param due seconda merce da confrontare
 * @return un int: 1 se vero, 0 in caso contrario
 */
int uguali(Merce *uno, Merce *due);

/**
 * Funzione che genera un double randomicamente
 * @param limit limite massimo
 * @return valore double generato
 */
double genera_double(int limit);

/**
 * Funzione che genera delle coordinate randomicamente
 * @param so_lato lato del quadrato della mappa
 * @return coordinate generate
 */
struct coordinate genera_coordinate(int so_lato);

/**
 * Funzione che abbassa la scadenza delle merci
 * @param testa head della lista
 * @param lista lista completa
 * @param so_merci merci totali
 * @return tot merce scaduta
 */
int *abbassa_scadenze(Merce **testa, lista_merci *lista, int so_merci);

/**
 * Funzione che elimina un nodo dalla lista merci
 * @param list lista da cui eliminare l'elemento
 * @param nodo_da_eliminare nodo da eliminare
 * @return void
 */
void elimina_nodo(lista_merci *list, Merce *nodo_da_eliminare);

/**
 * Funzione che calcola la distanza tra due porti
 * @param partenza coordinate porto di partenza
 * @param arrivo coordinate porto di arrivo
 * @return distanza calcolata
 */
double calcolo_distanza(struct coordinate partenza, struct coordinate arrivo);

#endif
