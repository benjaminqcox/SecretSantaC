#include "shared.h"

int sendall(int s, char *buf, int *len)
{
    int total = 0;
    int bytesleft = *len;
    int n;

    while (total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) {
            break;
        }
        total += n;
        bytesleft -= n;
    }

    *len = total;
    return n == -1 ? -1 : 0;
}

void printAll(int numParticipants, person_t **participants) 
{
    for (int i = 0; i < numParticipants; i++) {
        printf("\tID: %d, Name: %s\n", participants[i]->id, participants[i]->name);
    }
}

void freeParticipants(person_t **participants, int num_participants)
{
    // Loop through and free all participants
    for (int i = 0; i < num_participants; i++)
    {
        free(participants[i]);
        participants[i] = NULL;
    }
}

// Just to separate lines so that output is more clear
void separator() {
    printf("\n--------------------------------------\n");
}