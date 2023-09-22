#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>


/* Start of shared between server and client */
#define CONNECTION_ADDRESS "127.0.0.1"
#define SERVER_PORT 18000
#define BACKLOG_SIZE 10
#define TCP_MODE 0

enum MenuSelection {
    FIND_GIFTEE = 1,
    FIND_SANTA = 2,
    LIST_PAIRS = 3,
    ADD_PERSON = 1,
    DRAW_NAMES = 2
};
/* End of shared between server and client*/

typedef struct {
    unsigned char id;
    char *name;
} person_t;

/*
// Struct format
struct sockaddr_in {
    // sin = sockaddr_in -> s_in -> sin
    sa_family_t sin_family; // address family: AF_INET
    in_port_t sin_port; // port in network byte order
    struct in_addr sin_addr; // internet address
};

// Internet address
struct in_addr {
    uint32_t s_addr; // address in network byte order
};
*/

int main()
{
    // This is for when the client disconnects, it causes a broken pipe which terminates the entire program (this is only when I am not using child processes)
    // This is happening because the server is sleeping for 5 seconds and the client is not waiting for the result of its final request before disconnecting
    //signal(SIGPIPE, SIG_IGN);

    // The fix is to use child processes and the parent can deal with the connection and the children can deal with processing

    
    int ret, sd, cl_sd;
    struct sockaddr_in sv_addr, cl_addr;
    socklen_t addrlen = sizeof(cl_addr);
    ssize_t sent_bytes;
    bool active_client = false;

    // Create socket
    sd = socket(AF_INET, SOCK_STREAM, TCP_MODE);
    // Check successful socket creation

    // Create address
    memset(&sv_addr, 0, sizeof(sv_addr)); // sets the first count bytes of dest to the value c
    sv_addr.sin_family = AF_INET; // set the sockadress transport address to AF_INET (Address Family InterNET)
    sv_addr.sin_port = htons(SERVER_PORT); // converts unsigned short integer (hostshort) from host byte order to netword byte order
    inet_pton(AF_INET, CONNECTION_ADDRESS, &sv_addr.sin_addr); // converts IPv4 and IPv6 addresses from text to binary form

    // assigns the local socket address to a socket identified by descriptor socket that has no local socket address assigned
    // sockets created with socket() are initially unnamed; they are identified by their address family
    ret = bind(sd, (struct sockaddr*)&sv_addr, sizeof(sv_addr)); 
    ret = listen(sd, BACKLOG_SIZE); // listen on the created socket with a maximum backlog size of 10
    // Check the listen was successfully setup
    if (ret == -1) {
        // If failed, exit the program
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Print port listening on
    printf("Server is listening at %d:%d...\n", CONNECTION_ADDRESS, SERVER_PORT);

    while (1)
    {
        // extract the first connection request in the queue of pending connections, create a new socket with the same socket type protocol
        // and address family as the specified socket, and allocate a new file descriptor for that socket
        cl_sd = accept(sd, (struct sockaddr*)&cl_addr, &addrlen);
        // Check client connection was made successfully
        if (cl_sd == -1) {
            // If not successful, loop back to wait for new connection
            perror("accept");
            continue;
        }
        
        if (fork() == 0)
        {
            // Close the connection to the main socket from the client and just use the client connection socket
            close(sd);
            
            bool hasDrawn = false;
            person_t **participants;
            
            while (1)
            {
                // While the client is connected wait initially for the selected menu option
                // from the selected menu option, choose the case statement to enter
                int menuChoice = ADD_PERSON;
                if (hasDrawn)
                {
                    switch(menuChoice)
                    {
                        case FIND_GIFTEE:
                            // do something
                            break;
                        case FIND_SANTA:
                            // do something
                            break;
                        case LIST_PAIRS:
                            // do something
                            break;
                        default:
                            printf("Invalid selection");
                            break;
                    }
                }
                else
                {
                    switch (menuChoice)
                    {
                        case ADD_PERSON:
                            // do something
                            break;
                        case DRAW_NAMES:
                            // check enough names are in names array
                            // do something
                            break;
                        default:
                            printf("Invalid selection");
                            break;
                    }
                }
                

            }
            close(cl_sd);
            exit(EXIT_SUCCESS);
        }
        else
        {
           // Close client connection if internal loop exits
            close(cl_sd); 
        }
        
    }
    // Close the main socket
    close(sd);

    return 0;
}