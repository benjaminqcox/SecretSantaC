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

void findGiftee(int sock) {
    // Fill buffer with 0s
    // bzero(buffer, 1024);
    // strcpy(buffer, "Sending request to server.\n");
    // printf("Client: %s\n", buffer);

    // // Send message to server
    // send(sock, buffer, strlen(buffer), 0);

    // // Receive message from server (current time)
    // bzero(buffer, 1024);
    // recv(sock, buffer, sizeof(buffer), 0);
    // printf("Server: %s", buffer);

    // // wait for 5 minutes
    // sleep(300); 
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
        perror("[-]Socket error\n");
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
            menuOption = getIntInputInRange(0, 3);

            switch (menuOption) {
                case QUIT:
                    printf("Disconnecting...\n");
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

            menuOption = getIntInputInRange(0, 2);
            
            switch (menuOption) {
                case QUIT:
                    printf("Disconnecting...\n");
                    close(sock);
                    printf("Disconnected from the server. Goodbye!\n");
                    exit(0);
                    break;
                case ADD_PERSON:
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