#include "messaggi.h"

void ricevo_messaggio(int msgid, void *msg, int dimensione, long mtype, int msgflg)
{
    ssize_t result;
    while (1)
    {

        result = msgrcv(msgid, msg, dimensione, mtype, msgflg);
        if (result == -1)
        {
            if (errno == EINTR)
            {
                continue;
            } else
            {
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }
        } else
        {
            errno = 0;
            break;
        }
    }
}

void invia_messaggio(int msgid, void *msg, int dimensione)
{
    ssize_t result;
    while (1)
    {

        result = msgsnd(msgid, msg, dimensione, 0);
        if (result == -1)
        {
            if (errno == EINTR)
            {
                continue;
            } else
            {
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }
        } else
        {
            errno = 0;
            break;
        }
    }
}