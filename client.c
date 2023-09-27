#include "shared.h"

// Add Participant to Secret Santa
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

// Initiate the Secret Santa Draw
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

// Find the giftee of a specified Santa
void findGiftee(int sock) {
    // send menu option to server
    int option = FIND_GIFTEE;
    if (send(sock, &option, sizeof(option), 0) < 0) {
        perror("Send error");
        return;
    };
    // receive number of participants from server
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
    // receive Santa from server, so that name of Santa can be extracted from id
    person_t *santa = (person_t *)malloc(sizeof(person_t));
    if (santa == NULL) {
        perror("Out of memory.");
    }
    if (recv(sock, santa, sizeof(person_t), 0) < 0) {
        perror("Receive error");
        return;
    }
    // receive giftee from server
    person_t *giftee = (person_t *)malloc(sizeof(person_t));
    if (giftee == NULL) {
        perror("Out of memory.");
    }
    if (recv(sock, giftee, sizeof(person_t), 0) < 0) {
        perror("Receive error");
        return;
    }
    // print info in readable format, displaying name and ID of Santa and their giftee
    printf("Santa ID: %d, name: %s - Giftee ID: %d, name: %s\n", santa->id, santa->name, giftee->id, giftee->name);
    free(santa);
    free(giftee);
}

// Find the Santa of a specified giftee
void findSanta(int sock) {
    // send menu option to server
    int option = FIND_SANTA;
    if (send(sock, &option, sizeof(option), 0) < 0) {
        perror("Send error");
        return;
    };
    // receive number of participants from server
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
    // receive giftee from server so that name of giftee can be extracted
    person_t *giftee = (person_t *)malloc(sizeof(person_t));
    if (giftee == NULL) {
        perror("Out of memory.");
    }
    if (recv(sock, giftee, sizeof(person_t), 0) < 0) {
        perror("Receive error");
        return;
    }
    // receive santa from server
    person_t *santa = (person_t *)malloc(sizeof(person_t));
    if (santa == NULL) {
        perror("Out of memory.");
    }
    if (recv(sock, santa, sizeof(person_t), 0) < 0) {
        perror("Receive error");
        return;
    }
    // print info in readable format, displaying name and ID of giftee and their santa
    printf("Giftee ID: %d, name: %s - Santa ID: %d, name: %s\n", giftee->id, giftee->name, santa->id, santa->name);
    free(giftee);
    free(santa);
}

// List all Santa/Giftee pairs
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
    // receive participants from server as an array of person_t pointers
    // allocate memory for the array according to the number of participants
    person_t **participants = (person_t **)malloc(sizeof(person_t *) * numOfParticipants);
    if (participants == NULL) {
        perror("Out of memory\n");
        exit(EXIT_FAILURE);
    }
    // loop through array of participants
    for (int i = 0; i < numOfParticipants ; i++) {
        // loop through array, allocating memory for each element of the array first
        participants[i] = (person_t *)malloc(sizeof(person_t));
        if (participants[i] == NULL) {
            perror("Out of memory\n");
            exit(EXIT_FAILURE);
        }
        // receive participant from server and store in array
        if (recv(sock, participants[i], sizeof(person_t), 0) < 0) {
            perror("Receive error");
            return;
        }
    }
    // print name and id of each particpant as the Santa, and the next person in the list as their giftee
    // if on person at last index, print the person at index 0 as their giftee
    for (int i = 0; i < numOfParticipants ; i++) {
        if (i != numOfParticipants - 1) {
            printf("Santa ID: %d, Name: %s - Giftee ID: %d, Name: %s\n", participants[i]->id, participants[i]->name, participants[i+1]->id, participants[i+1]->name);
        }
        else {
            printf("Santa ID: %d, Name: %s - Giftee ID: %d, Name: %s\n", participants[i]->id, participants[i]->name, participants[0]->id, participants[0]->name);
        }
    }    
    // free participants
    for (int i = 0; i < numOfParticipants ; i++) {
        // free each element of participants array
        free(participants[i]);
    }
    free(participants);
}

// Quit program and close socket
void quitProgram(int sock) {
    // Send Quit option to server to indicate closing connection
    int option = QUIT;
    if (send(sock, &option, sizeof(option), 0) < 0) {
        perror("Send error");
        return;
    }
    // close the socket
    close(sock);
    // inform user that client has disconnected from server
    printf("Disconnected from the server. Goodbye!\n");
    return;
}

int main(int argc, char const *argv[])
{
    // create a socket
    int sock, connection_result, n;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[64];

    // create a tcp socket
    sock = socket(AF_INET, SOCK_STREAM, TCP_MODE);
    if (sock < 0) {
        perror("[-]Socket error\n");
        exit(1);
    }
    printf("[+] TCP server socket created.\n");

    // Assign IP, Port
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; // Address family INET = Internet
    addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, CONNECTION_ADDRESS, &addr.sin_addr);

    // Connect to server
    connection_result = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (connection_result == -1) {
        perror("[-] Connection error");
        exit(1);
    }
    printf("Connected to server.\n");

    // set up variable for menu option
    int menuOption;

    for(;;) {
        // first check if draw has happened by receiving information from server
        bool draw_happened;
        recv(sock, &draw_happened, sizeof(draw_happened), 0);
        if (draw_happened) // if it has happened, we want to show a different set of menu options
        {   
            printf("Select your option:\n");
            printf("[0] Disconnect from server:\n");
            printf("[1] Find giftee by Santa:\n");
            printf("[2] Find Santa by giftee:\n");
            printf("[3] List all Santa/giftee pairs:\n");
            // get user input of type int in the range 0 to 3 to represent above menu options
            menuOption = getIntInputInRange(0, 3);
            // switch statement for all 4 options
            switch (menuOption) {
                case QUIT:
                    // close the socket and disconnect from server, and exit program
                    printf("Disconnecting...");
                    quitProgram(sock);
                    exit(0);
                    break;
                case FIND_GIFTEE:
                    // finding giftee function
                    printf("Finding giftee...\n");
                    findGiftee(sock);
                    break;
                case FIND_SANTA:
                    // finding santa function 
                    printf("Finding Santa...\n");
                    findSanta(sock);
                    break;
                case LIST_PAIRS:
                    // listing pairs function
                    printf("Listing pairs...\n");
                    listPairs(sock);
                    break;
                default:
                    // if no valid option was selected, inform the user of that
                    printf("Invalid input: Please select a valid option between 0 and 3.\n");
                    break;
            }
        }
        else 
        {   
            // if draw has not yet happened, user should only be able to either add participants or start the draw
            printf("Select your option:\n");
            printf("[0] Disconnect from server:\n");
            printf("[1] Add person:\n");
            printf("[2] Secret Santa Draw:\n");
            // user input from range 0 to 2 representing the above options
            menuOption = getIntInputInRange(0, 2);
            // switch statement for all 3 options
            switch (menuOption) {
                case QUIT:
                    // close the socket and disconnect from server, and exit program
                    printf("Disconnecting...");
                    quitProgram(sock);
                    exit(0);
                    break;
                case ADD_PERSON:
                    // add participant function
                    printf("Adding person...\n");
                    addPerson(sock);
                    break;
                case DRAW_NAMES:
                    // draw names
                    printf("Secret Santa Draw commencing...\n");
                    drawNames(sock, &draw_happened);
                    break;
                default:
                    // if no valid option was selected, inform the user of that
                    printf("Invalid input: Please select a valid option between 0 and 2.\n");
                    break;
            }
        }
    }
    // if exiting the infinite loop, close the socket first
    close(sock);
    printf("Disconnected from the server.\n");

    return 0;
}