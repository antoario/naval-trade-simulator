#include "semafori.h"

int sem_decrementa(int id_semaforo)
{
    sem_operazione(id_semaforo, -1, 0);
    return 0;
}

int sem_aspetta_zero(int id_semaforo)
{
    return sem_operazione(id_semaforo, 0, 0);
}

int sem_operazione(int id_semaforo, short valore, int numero_semaforo)
{
    struct sembuf sem_buf;
    int ret;
    sem_buf.sem_num = numero_semaforo;
    sem_buf.sem_op = valore;
    sem_buf.sem_flg = 0;
    do
    {
        ret = semop(id_semaforo, &sem_buf, 1);
    } while (ret == -1 && errno == EINTR);
    if (ret == -1)
    {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    return ret;
}

int sem_imposta_valore(int semid, int semnum, int val)
{
    union semun arg;
    int ret;
    arg.val = val;
    do
    {
        ret = semctl(semid, semnum, SETVAL, val);
    } while (ret == -1 && errno == EINTR);
    return ret;
}