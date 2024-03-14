#include "ottieni_dati.h"

int read_input()
{
    int m_id;
    struct input *dati_input;
    FILE *input_file;

    m_id = shmget(IPC_PRIVATE, sizeof(*dati_input), IPC_CREAT | IPC_EXCL | PERMESSI);
    dati_input = shmat(m_id, NULL, 0);

    /* Lettura da input file */
    input_file = fopen("Parameters.txt", "r");
    if (input_file == NULL)
    {
        printf("Non ho trovato il file \n");
        exit(-1);
    }
    fscanf(input_file, "SO_NAVI: %u\n"
                       "SO_PORTI: %u\n"
                       "SO_MERCI: %u\n"
                       "SO_SIZE: %u\n"
                       "SO_MIN_VITA: %u\n"
                       "SO_MAX_VITA: %u\n"
                       "SO_LATO: %u\n"
                       "SO_SPEED: %u\n"
                       "SO_CAPACITY: %u\n"
                       "SO_BANCHINE: %u\n"
                       "SO_FILL: %u\n"
                       "SO_LOADSPEED: %u\n"
                       "SO_DAYS: %u\n", &SO_NAVI, &SO_PORTI, &SO_MERCI,
           &SO_SIZE, &SO_MIN_VITA, &SO_MAX_VITA, &SO_LATO,
           &SO_SPEED, &SO_CAPACITY, &SO_BANCHINE, &SO_FILL,
           &SO_LOADSPEED, &SO_DAYS);
    fclose(input_file);
    return m_id;
}