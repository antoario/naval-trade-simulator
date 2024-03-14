#define _GNU_SOURCE

#ifndef PROGETTOSO_NAVE_H
#define PROGETTOSO_NAVE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "utility/semafori/semafori.h"
#include "utility/ottieni_dati/ottieni_dati.h"
#include "utility/funzioni_condivise/funzioni_condivise.h"
#include "utility/memoria_condivisa/memoria_condivisa.h"
#include "utility/messaggi/messaggi.h"
#include "types.h"

#define PERMESSI 0666

/**
 * Funzione che mischia un array passato come parametro
 * @param array array da mescolare
 * @param n lunghezza array
 * @return void
 */
void shuffle(int *array, int n);

/**
 * Funzione che rimuove dalla lista la merce passata come parametro
 * @param lista lista dalla quale eliminare l'elemento
 * @param merce_da_eliminare elemento da eliminare
 * @return int per verificare se l'eliminazione si svolge correttamente
 */
int rimuovi_merce_specificata(lista_merci *lista, Merce merce_da_eliminare);

/**
 * Handler chiamato ogni giorno che abbassa le scadenze
 * @return void
 */
void passato_giorno(void);

/**
 * Funzione che effettua l'entrata in porto(con l'invio del messaggio alla nave)
 * @param porto id_porto con il quale comunicare
 * @return void
 */
void entro_in_porto(int porto);

/**
 * Funzione che cerca la prossima destinazione della nave
 * @return id_porto di destinazione
 */
int cerca_prossima_destinazione(void);

/**
 * Funzione che effettua l'uscita dal porto
 * @param id_porto id del porto per la liberazione della banchina
 * @return void
 */
void esco_dal_porto(int id_porto);

/**
 * Funzione che effettua lo scarico della merce
 * @return void
 */
void scarico_merce(void);

/**
 * Funzione che effettua il carico della nave
 * @return void
 */
void carico_merce(void);

/**
 * Funzione che ricerca il porto più vicino
 * @param coordinate_attuali coordinate attuali della nave
 * @return id_porto destinazione trovato
 */
int ricerca_porto_piu_vicino(struct coordinate coordinate_attuali);

/**
 * Funzione che calcola il tempo stimato per il movimento
 * @param distanza distanza da dover percorrere
 * @return tempo stimato trovato
 */
struct timespec tempo_stimato(double distanza);

/**
 * Funzione che permette di cambiare lo stato della nave tra: vuota, carica, in_porto
 * @param nuovo_stato nuovo stato della nave
 * @return void
 */
void cambia_stato(enum stato_nave nuovo_stato);

/**
 * Funzione che effettua il movimento della nave
 * @param coordinate_attuali coordinate di partenza della nave
 * @param destinazione coordinate di arrivo della nave
 * @return void
 */
void movimento_nave(struct coordinate coordinate_attuali, struct coordinate destinazione);

/**
 * Funzione che effettua la nanosleep per l'attesa(per movimento e carico/scarico)
 * @param travel_time tempo di attesa
 * @return void
 */
void attesa(struct timespec travel_time);

/**
 * Funzione che aggiunge la merce in stiva
 * @param tipo tipo di merce
 * @param quantita quantità di merce da caricare
 * @param scadenza scadenza della merce
 * @return void
 */
void aggiungi_merce_in_stiva(int tipo, int quantita, int scadenza);

/**
 * Funzione che calcola l'attesa per carico/scarico
 * @param quantita quantità da caricare/scaricare
 * @return tempo stimato
 */
struct timespec calcolo_attesa(int quantita);

#endif
