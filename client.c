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

#define CONNECTION_ADDRESS "127.0.0.1"
#define TCP_MODE 0 
#define SERVER_PORT 18000
#define MENU_OPTION_INPUT_SIZE 5

enum MenuSelection {
	QUIT = 0,
	FIND_GIFTEE = 1,
	FIND_SANTA = 2,
	LIST_PAIRS = 3,
	ADD_PERSON = 1,
	DRAW_NAMES = 2
};

void addPerson(int sock) {
    // send menu option selected to server
    int option = ADD_PERSON;
    if (send(sock, &option, sizeof(option), 0) < 0) {
        perror("Send error");
        return;
    };
    // get user input for name
    printf("Please enter name of new participant here: \n");
    char *newName = getStringInput();
    char name[64];
    strlcpy(name, newName, 64);
    // send name to server
    if (send(sock, name, 64, 0) < 0) {
        perror("Send error");
        return;
    };
    int *personId = (int *)malloc(sizeof(int));
    // receives name and id of added person from server, as a confirmation
    if (recv(sock, personId, sizeof(personId), 0) < 0) {
        perror("Receive error");
        return;
    }
    // print person with id
    printf("Person %s (ID: %d) has been successfully added.\n", name, *personId);
    free(personId);
    // not sure if name needs to be freed here? as it will be used in the server
}

void drawNames(int sock, bool *draw_happened) {
    // send menu option selected to server
    int option = DRAW_NAMES;
    if (send(sock, &option, sizeof(option), 0) < 0) {
        perror("Send error");
        return;
    };
    // receives confirmation of successful draw from server
    bool drawSuccess;
    if (recv(sock, &drawSuccess, sizeof(drawSuccess), 0) < 0) {
        perror("Receive error");
        return;
    }
    // assign draw success boolean value to draw_happened
    *draw_happened = drawSuccess;
    // if draw happened, tell user that draw was successful, otherwise tell user it was not
    if (drawSuccess) {
        printf("Draw successful.\n");
    } 
    else {
        printf("Draw unsuccessful: please try again with more participants?\n");
    }
}

void findGiftee(int sock) {
    // send menu option to server
    int option = FIND_GIFTEE;
    if (send(sock, &option, sizeof(option), 0) < 0) {
        perror("Send error");
        return;
    };
    // get user input from user for name of Santa
    printf("Please enter name of Santa: \n");
    char *newName = getStringInput();
    char name[64];
    strlcpy(name, newName, 64);
    // send name to server
    if (send(sock, name, 64, 0) < 0) {
        perror("Send error");
        return;
    };
    // receive from server name and id of the giftee for this Santa
    char gifteeName[64];
    // receives name and id of person from server, as a confirmation
    if (recv(sock, gifteeName, sizeof(gifteeName), 0) < 0) {
        perror("Receive error");
        return;
    }
    int *gifteeId = (int *)malloc(sizeof(int));
    if (recv(sock, gifteeId, sizeof(gifteeId), 0) < 0) {
        perror("Receive error");
        return;
    }
    // print info
    printf("Santa: %s - Giftee: %s, ID: %d", name, gifteeName, *gifteeId);
    free(gifteeId);
}


void findSanta(int sock) {
    // send menu option to server
    int option = FIND_SANTA;
    if (send(sock, &option, sizeof(option), 0) < 0) {
        perror("Send error");
        return;
    };
    // get user input from user for name of Giftee
    printf("Please enter name of Giftee: \n");
    char *newName = getStringInput();
    char name[64];
    strlcpy(name, newName, 64);
    // send name to server
    if (send(sock, name, 64, 0) < 0) {
        perror("Send error");
        return;
    };
    // receive from server name and id of the Santa for this giftee
    char santaName[64];
    // receives name and id of person from server, as a confirmation
    if (recv(sock, santaName, sizeof(santaName), 0) < 0) {
        perror("Receive error");
        return;
    }
    int *santaId = (int *)malloc(sizeof(int));
    if (recv(sock, santaId, sizeof(santaId), 0) < 0) {
        perror("Receive error");
        return;
    }
    // print info
    printf("Santa: %s - Giftee: %s, ID: %d", name, santaName, *santaId);
    free(santaId);
}

void listPairs(int sock) {
    // send menu option to server
    int option = LIST_PAIRS;
    if (send(sock, &option, sizeof(option), 0) < 0) {
        perror("Send error");
        return;
    };
    // first receive from server number of participants
    int *numOfParticipants = (int *)malloc(sizeof(int));
    if (recv(sock, numOfParticipants, sizeof(numOfParticipants), 0) < 0) {
        perror("Receive error");
        return;
    }
    // for i = 0; i < numOfParticipants; i++
    for (int i = 0 ; i < (*numOfParticipants) ; i++) {
        // receive from server the name and id of each participant
        char santaName[64];
        if (recv(sock, santaName, sizeof(santaName), 0) < 0) {
            perror("Receive error");
            return;
        }
        int *santaId = (int *)malloc(sizeof(int));
        if (recv(sock, santaId, sizeof(santaId), 0) < 0) {
            perror("Receive error");
            return;
        }
        char gifteeName[64];
        if (recv(sock, gifteeName, sizeof(gifteeName), 0) < 0) {
            perror("Receive error");
            return;
        }
        int *gifteeId = (int *)malloc(sizeof(int));
        if (recv(sock, gifteeId, sizeof(gifteeId), 0) < 0) {
            perror("Receive error");
            return;
        }
        printf("Santa ID: %d, Name: %s ; Giftee ID: %d, Name: %s\n", *santaId, santaName, *gifteeId, gifteeName);
    }
    // print name and id of each particpant as the Santa, and the next person in the list as their giftee
    // if on person at last index, print the person at index 0 as their giftee
    // (the above logic is handled in the server)
}

int main(int argc, char const *argv[])
{
    // create a socket
    int sock, connection_result;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[64];
    int n;

    // create a tcp socket
    sock = socket(AF_INET, SOCK_STREAM, TCP_MODE);
    if (sock < 0) {
        perror("[-]Socket error\n");
        exit(1);
    }
    printf("[+] TCP server socket created.\n");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    // addr.sin_addr.s_addr = inet_addr(CONNECTION_ADDRESS);
    inet_pton(AF_INET, CONNECTION_ADDRESS, &addr.sin_addr);

    // Connect to server
    connection_result = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (connection_result == -1) {
        perror("[-] Connection error");
        exit(1);
    }

    printf("Connected to server.\n");

    // set up variable for menu option
    char charMenuOption;
    int menuOption;

    for(;;) {
    //     // first check if draw has happened
        bool draw_happened;
        recv(sock, &draw_happened, sizeof(draw_happened), 0);
        printf("%d\n", draw_happened);
        if (draw_happened) // if it has happened, we want to show a different set of menu options
        {
            printf("Select your option:\n");
            printf("[0] Disconnect from server:\n");
            printf("[1] Find giftee by Santa:\n");
            printf("[2] Find Santa by giftee:\n");
            printf("[3] List all Santa/giftee pairs:\n");
            menuOption = getIntInputInRange(0, 3);

            switch (menuOption) {
                case QUIT:
                    printf("Disconnecting...");
                    close(sock);
                    printf("Disconnected from the server. Goodbye!\n");
                    exit(0);
                    break;
                case FIND_GIFTEE:
                    printf("Finding giftee...\n");
                    listPairs(sock);
                    break;
                case FIND_SANTA:
                    printf("Finding Santa...\n");
                    listPairs(sock);
                    break;
                case LIST_PAIRS:
                    printf("Listing pairs...\n");
                    listPairs(sock);
                    break;
                default:
                    printf("Invalid input: Please select a valid option between 0 and 3.\n");
                    break;
            }
        }
        else 
        {
            printf("Select your option:\n");
            printf("[0] Disconnect from server:\n");
            printf("[1] Add person:\n");
            printf("[2] Secret Santa Draw:\n");

            menuOption = getIntInputInRange(0, 2);
            switch (menuOption) {
                case QUIT:
                    printf("Disconnecting...");
                    close(sock);
                    printf("Disconnected from the server. Goodbye!\n");
                    exit(0);
                    break;
                case ADD_PERSON:
                    printf("Adding person...\n");
                    addPerson(sock);
                    break;
                case DRAW_NAMES:
                    printf("Secret Santa Draw commencing...\n");
                    drawNames(sock, &draw_happened);
                    if (draw_happened) {
                        printf("Draw successful!\n");
                    } else {
                        printf("Draw unsuccessful.\n");
                    }
                    break;
                default:
                    printf("Invalid input: Please select a valid option between 0 and 2.\n");
                    break;
            }
        }
    }

    close(sock);
    printf("Disconnected from the server.\n");

    return 0;
}