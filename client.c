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
    char *name = getStringInput();
    
    // send name to server
    if (send(sock, name, strlen(name), 0) < 0) {
        perror("Send error");
        return;
    };
    int personId;
    // receives name and id of added person from server, as a confirmation
    if (recv(sock, &personId, sizeof(personId), 0) < 0) {
        perror("Receive error");
        return;
    }
    // print person with id
    printf("Person %s (ID: %d) has been successfully added.\n", name, personId);
    
    // not sure if name needs to be freed here? as it will be used in the server
}

int main(int argc, char const *argv[])
{
    // create a socket
    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    // create a tcp socket
    sock = socket(AF_INET, SOCK_STREAM, TCP_MODE);
    if (sock < 0) {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+] TCP server socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = SERVER_PORT;
    addr.sin_addr.s_addr = inet_addr(CONNECTION_ADDRESS);

    // Connect to server
    int connection_result = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (connection_result == -1) {
        perror("[-] Connection error");
        exit(1);
    }

    printf("Connected to server.\n");

    // set up variable for menu option
    char charMenuOption;
    int menuOption;

    for(;;) {
        // first check if draw has happened
        bool draw_happened;
        recv(sock, &draw_happened, sizeof(draw_happened), 0);
        if (draw_happened) // if it has happened, we want to show a different set of menu options
        {
            printf("Select your option:\n");
            printf("[0] Disconnect from server:\n");
            printf("[1] Find giftee by Santa:\n");
            printf("[2] Find Santa by giftee:\n");
            printf("[3] List all Santa/giftee pairs:\n");
            fgets(&charMenuOption, MENU_OPTION_INPUT_SIZE, stdin);
            menuOption = (int) charMenuOption;

            switch (menuOption) {
                case QUIT:
                    printf("Disconnecting...");
                    close(sock);
                    printf("Disconnected from the server. Goodbye!\n");
                    exit(0);
                    break;
                case FIND_GIFTEE:

                    break;
                case FIND_SANTA:
                    break;
                case LIST_PAIRS:
                    break;
                default:
                    break;
            }
        }
        else 
        {
            printf("Select your option:\n");
            printf("[0] Disconnect from server:\n");
            printf("[1] Add person:\n");
            printf("[2] Secret Santa Draw:\n");

            fgets(&charMenuOption, MENU_OPTION_INPUT_SIZE, stdin);
            menuOption = (int) charMenuOption;
            
            switch (menuOption) {
                case QUIT:
                    printf("Disconnecting...");
                    close(sock);
                    printf("Disconnected from the server. Goodbye!\n");
                    exit(0);
                    break;
                case ADD_PERSON:
                    addPerson(sock);
                    break;
                case DRAW_NAMES:
                    break;
                default:
                    break;
            }
        }
    }

    close(sock);
    printf("Disconnected from the server.\n");

    return 0;
}