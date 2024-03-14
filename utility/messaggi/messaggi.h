#define _GNU_SOURCE

#ifndef PROGETTOSO_MESSAGGI_H
#define PROGETTOSO_MESSAGGI_H

#include <stdio.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include "../../types.h"

/**
 * Funzione che permette la ricezione del messaggio
 * @param msgid id della coda
 * @param msg messaggio ricevuto
 * @param dimensione dimensione del messaggio
 * @param mtype tipo del messaggio
 * @param msgflg flag
 * @return void
 */
void ricevo_messaggio(int msgid, void *msg, int dimensione, long mtype, int msgflg);

/**
 * Funzione che permette l'invio del messaggio
 * @param msgid id della coda
 * @param msg messaggio da inviare
 * @param dimensione dimensione del messaggio
 * @return void
 */
void invia_messaggio(int msgid, void *msg, int dimensione);

#endif
