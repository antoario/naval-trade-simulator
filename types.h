#ifndef PROGETTOSO_TYPES_H
#define PROGETTOSO_TYPES_H

#define LOCK        sem_operazione(sezione_critica, -1,0);
#define UNLOCK      sem_operazione(sezione_critica, 1,0);

enum stato_nave
{
    vuota, carica, in_porto
};

struct coordinate
{
    double x, y;
};


typedef struct dati_utili
{
    int porti_non_generanti;
    int porti_non_richiedenti;
    int n_navi_in_mare_con_carico;
    int n_navi_in_mare_senza_carico;
    int n_navi_in_banchina;
} dati_utili;


typedef struct Merce
{
    int quantita;
    int scadenza;
    int tipo;
    struct Merce *prossimo;
} Merce;

typedef struct lista_merci
{
    Merce *head;
    int num_merci;
} lista_merci;


typedef struct msg_contatto_navi_porto
{
    long type;
    int id_nave;
} msg_contatto_navi_porto;

/**
 * msgbuf per messaggio da porto a nave
 */
typedef struct msg_porto_nave_scarico_carico
{
    long mtype;
    struct Merce pacchetto;
    int id_destinazione;
} msg_porto_nave_scarico_carico;


typedef struct merce_info_dump
{
    int in_mare;
    int consegnata;
    int scaduta_in_porto;
    int scaduta_in_nave;
} merce_info_dump;

typedef struct porti_info_dump
{
    int presente;
    int spedita;
    int ricevuta;
    int non_genera;
    struct coordinate coordinate;
} porti_info_dump;

typedef struct merci_scadenza_quantita
{
    int scadenza;
    int quantita;
} merci_scadenza_quantita;

typedef struct msg_porti_vicini
{
    long msg_type;
    int msg_data[2000];
} msg_porti_vicini;

#endif