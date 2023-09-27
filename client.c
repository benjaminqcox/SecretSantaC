#include "shared.h"

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
    char name[NAME_SIZE];

    strlcpy(name, newName, 64);
    // send name to server
    int len = NAME_SIZE;
    if (sendall(sock, name, &len) < 0) {
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
    int numParticipants;
    if (recv(sock, &numParticipants, sizeof(numParticipants), 0) < 0) {
        perror("Receive error");
        return;
    }
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
    person_t *participant = (person_t *)malloc(sizeof(person_t));
    if (participant == NULL) {
        perror("Out of memory.");
    }
    for (int i = 0; i < numParticipants ; i++) {
        if (recv(sock, participant, sizeof(person_t), 0) < 0) {
            perror("Receive error");
            return;
        }
        printf("ID: %d, %s\n", participant->id, participant->name);
    }
}

void findGiftee(int sock) {
    // send menu option to server
    int option = FIND_GIFTEE;
    if (send(sock, &option, sizeof(option), 0) < 0) {
        perror("Send error");
        return;
    };
    int numParticipants;
    if (recv(sock, &numParticipants, sizeof(numParticipants), 0) < 0) {
        perror("Receive error");
        return;
    }
    // get user input from user for name of Santa
    printf("Please enter ID of Santa: \n");
    int santaId = getIntInputInRange(0, numParticipants - 1);
    if (send(sock, &santaId, sizeof(santaId), 0) < 0) {
        perror("Send error");
        return;
    };

    person_t *santa = (person_t *)malloc(sizeof(person_t));
    if (santa == NULL) {
        perror("Out of memory.");
    }
    if (recv(sock, santa, sizeof(person_t), 0) < 0) {
        perror("Receive error");
        return;
    }
    
    person_t *giftee = (person_t *)malloc(sizeof(person_t));
    if (giftee == NULL) {
        perror("Out of memory.");
    }
    if (recv(sock, giftee, sizeof(person_t), 0) < 0) {
        perror("Receive error");
        return;
    }
    // print info
    printf("Santa ID: %d, name: %s - Giftee ID: %d, name: %s\n", santa->id, santa->name, giftee->id, giftee->name);
    free(santa);
    free(giftee);
}


void findSanta(int sock) {
    // send menu option to server
    int option = FIND_SANTA;
    if (send(sock, &option, sizeof(option), 0) < 0) {
        perror("Send error");
        return;
    };
    int numParticipants;
    if (recv(sock, &numParticipants, sizeof(numParticipants), 0) < 0) {
        perror("Receive error");
        return;
    }
    // get user input from user for name of Santa
    printf("Please enter ID of Giftee: \n");
    int gifteeId = getIntInputInRange(0, numParticipants - 1);
    if (send(sock, &gifteeId, sizeof(gifteeId), 0) < 0) {
        perror("Send error");
        return;
    };

    person_t *giftee = (person_t *)malloc(sizeof(person_t));
    if (giftee == NULL) {
        perror("Out of memory.");
    }
    if (recv(sock, giftee, sizeof(person_t), 0) < 0) {
        perror("Receive error");
        return;
    }

    person_t *santa = (person_t *)malloc(sizeof(person_t));
    if (santa == NULL) {
        perror("Out of memory.");
    }
    if (recv(sock, santa, sizeof(person_t), 0) < 0) {
        perror("Receive error");
        return;
    }
    // print info
    printf("Giftee ID: %d, name: %s - Santa ID: %d, name: %s\n", giftee->id, giftee->name, santa->id, santa->name);
    free(giftee);
    free(santa);
}

void listPairs(int sock) {
    // send menu option to server
    int option = LIST_PAIRS;
    if (send(sock, &option, sizeof(option), 0) < 0) {
        perror("Send error");
        return;
    };
    // first receive from server number of participants
    int numOfParticipants;
    if (recv(sock, &numOfParticipants, sizeof(numOfParticipants), 0) < 0) {
        perror("Receive error");
        return;
    }
    person_t **participants = (person_t **)malloc(sizeof(person_t *) * numOfParticipants);
    if (participants == NULL) {
        perror("Out of memory\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < numOfParticipants ; i++) {
        participants[i] = (person_t *)malloc(sizeof(person_t));
        if (participants[i] == NULL) {
            perror("Out of memory\n");
            exit(EXIT_FAILURE);
        }
        // null check maloc
        if (recv(sock, participants[i], sizeof(person_t), 0) < 0) {
            perror("Receive error");
            return;
        }
    }
    for (int i = 0; i < numOfParticipants ; i++) {
        if (i != numOfParticipants - 1) {
            printf("Santa ID: %d, Name: %s - Giftee ID: %d, Name: %s\n", participants[i]->id, participants[i]->name, participants[i+1]->id, participants[i+1]->name);
        }
        else {
            printf("Santa ID: %d, Name: %s - Giftee ID: %d, Name: %s\n", participants[i]->id, participants[i]->name, participants[0]->id, participants[0]->name);
        }
    }    
    // print name and id of each particpant as the Santa, and the next person in the list as their giftee
    // if on person at last index, print the person at index 0 as their giftee
    // (the above logic is handled in the server)
    for (int i = 0; i < numOfParticipants ; i++) {
        free(participants[i]);
    }
    free(participants);
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
                    findGiftee(sock);
                    break;
                case FIND_SANTA:
                    printf("Finding Santa...\n");
                    findSanta(sock);
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