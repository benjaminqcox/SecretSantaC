#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdbool.h>
#include "userInput.h"

/* Start of shared between server and client */
#define CONNECTION_ADDRESS "127.0.0.1"
#define SERVER_PORT 18000
#define BACKLOG_SIZE 10
#define TCP_MODE 0
#define NAME_SIZE 64

enum MenuSelection {
    QUIT = 0,
    FIND_GIFTEE = 1,
    FIND_SANTA = 2,
    LIST_PAIRS = 3,
    ADD_PERSON = 1,
    DRAW_NAMES = 2
};
/* End of shared between server and client*/
typedef struct {
    int id;
    char name[NAME_SIZE];
} person_t;

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