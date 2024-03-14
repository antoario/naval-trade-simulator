#define _GNU_SOURCE

#ifndef PROGETTOSO_OTTIENI_DATI_H
#define PROGETTOSO_OTTIENI_DATI_H

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

#define SO_NAVI dati_input->so_navi
#define SO_PORTI dati_input->so_porti
#define SO_MERCI dati_input->so_merci
#define SO_SIZE dati_input->so_size
#define SO_MIN_VITA dati_input->so_min_vita
#define SO_MAX_VITA dati_input->so_max_vita
#define SO_LATO dati_input->so_lato
#define SO_SPEED dati_input->so_speed
#define SO_CAPACITY dati_input->so_capacity
#define SO_BANCHINE dati_input->so_banchine
#define SO_FILL dati_input->so_fill
#define SO_LOADSPEED dati_input->so_loadspeed
#define SO_DAYS dati_input->so_days

#define PERMESSI 0666

struct input
{
    unsigned int so_navi;
    unsigned int so_porti;
    unsigned int so_merci;
    unsigned int so_size;
    unsigned int so_min_vita;
    unsigned int so_max_vita;
    unsigned int so_lato;
    unsigned int so_speed;
    unsigned int so_capacity;
    unsigned int so_banchine;
    unsigned int so_fill;
    unsigned int so_loadspeed;
    unsigned int so_days;
};

/**
 * Funzione che legge tutti i parametri forniti da input file
 * @return puntatore alla memoria condivisa con i dati
 */
int read_input();

#endif

