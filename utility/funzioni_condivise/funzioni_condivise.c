#include "funzioni_condivise.h"

int uguali(Merce *uno, Merce *due)
{
    return uno->scadenza == due->scadenza && uno->quantita == due->quantita && uno->tipo == due->tipo;
}

double genera_double(int limit)
{
    long result = random();
    struct timeval seed;
    gettimeofday(&seed, NULL);

    return (double) result / (double) RAND_MAX * limit;
}

struct coordinate genera_coordinate(int so_lato)
{
    struct coordinate coordinate_temp;
    coordinate_temp.x = genera_double(so_lato);
    coordinate_temp.y = genera_double(so_lato);
    return coordinate_temp;
}


int *abbassa_scadenze(Merce **testa, lista_merci *lista, int so_merci)
{
    int *merci_scadute = calloc(so_merci, sizeof(int));
    Merce *temp = *testa, *precedente;

    /* Rimuovi i nodi scaduti all'inizio della lista */
    while (temp != NULL && temp->scadenza == 0)
    {
        *testa = temp->prossimo;
        merci_scadute[temp->tipo] += temp->quantita;
        free(temp);
        temp = *testa;
        lista->num_merci--;
    }

    /* Scorri la lista e rimuovi i nodi scaduti */
    while (temp != NULL)
    {
        while (temp != NULL && temp->scadenza != 0)
        {
            temp->scadenza--;
            precedente = temp;
            temp = temp->prossimo;
        }

        /* Se siamo arrivati alla fine della lista, interrompi il cicl */
        if (temp == NULL)
        {
            break;
        }

        /* Rimuovi il nodo scaduto e aggiorna i puntatori */
        precedente->prossimo = temp->prossimo;
        merci_scadute[temp->tipo] += temp->quantita;
        free(temp);
        temp = precedente->prossimo;
        lista->num_merci--;
    }
    return merci_scadute;
}

void elimina_nodo(lista_merci *lista_merci, Merce *nodo_da_eliminare)
{
    Merce *corrente, *precedente;

    if (lista_merci->head == NULL)
    {
        printf("La lista è vuota\n");
        return;
    }

    corrente = lista_merci->head;
    precedente = NULL;

    if (uguali(corrente, nodo_da_eliminare))
    {
        lista_merci->head = corrente->prossimo;
        free(corrente);
        lista_merci->num_merci--;
        return;
    }

    while (corrente != NULL && !uguali(corrente, nodo_da_eliminare))
    {
        precedente = corrente;
        corrente = corrente->prossimo;
    }

    if (corrente == NULL)
    {
        printf("Il nodo non è stato trovato nella lista\n");
        return;
    }

    precedente->prossimo = corrente->prossimo;
    free(corrente);
    lista_merci->num_merci--;
}

double calcolo_distanza(struct coordinate partenza, struct coordinate arrivo)
{
    double delta_x = partenza.x - arrivo.x;
    double delta_y = partenza.y - arrivo.y;
    return sqrt(delta_x * delta_x + delta_y * delta_y);
}
