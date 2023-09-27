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
#define MIN_PARTICIPANTS 4

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

int sendall(int s, char *buf, int *len);
void printAll(int numParticipants, person_t **participants);
void freeParticipants(person_t **participants, int num_participants);
void separator();