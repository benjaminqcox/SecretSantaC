#include <time.h>
#include "shared.h"

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

    char name[NAME_SIZE];
    int ret, sd, cl_sd;
    struct sockaddr_in sv_addr, cl_addr;
    socklen_t addrlen = sizeof(cl_addr);
    ssize_t sent_bytes;
    bool active_client = false;
    time_t t;

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
    printf("Server is listening at %s:%d...\n", CONNECTION_ADDRESS, SERVER_PORT);

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
        printf("connected\n");
        
        if (fork() == 0)
        {
            // Close the connection to the main socket from the client and just use the client connection socket
            close(sd);
            printf("Server connected to client.\n");
            bool hasDrawn = false;
            person_t **participants;
            int numParticipants = 0;
            
            while (1)
            {
                // Send to client the result of hasDrawn
                send(cl_sd, &hasDrawn, sizeof(hasDrawn), 0);
                // While the client is connected wait initially for the selected menu option
                // from the selected menu option, choose the case statement to enter
                int menuChoice; // Change this to the clients' response
                recv(cl_sd, &menuChoice, sizeof(menuChoice), 0);
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
                            // Var to store the name the client enters
                            // Server message to ask for name
                            // Request name of person from client
                            recv(cl_sd, &name, NAME_SIZE, 0);

                            // Create a new participant
                            person_t *new_participant = (person_t *)malloc(sizeof(person_t));
                            // Check memory for the new participant has been allocated correctly
                            if (new_participant == NULL)
                            {
                                // failed to allocate memory
                                fprintf(stderr, "Failed to allocate memory 1\n");
                                exit(EXIT_FAILURE);
                            }
                            new_participant->id = numParticipants; // If multithreading, will need to add a mutex lock to increment the id and add participants
                            // Add the user chosen name from the client to the new participant
                            strlcpy(new_participant->name, name, NAME_SIZE);
                            
                            // Update the final participant (not numParticipants+1 as it is 0 indexed)
                            participants = (person_t **)realloc(participants, (numParticipants + 1) * sizeof(person_t *));
                            // Check memory for participants has been reallocated correctly
                            if (participants == NULL)
                            {
                                // failed to allocate memory
                                fprintf(stderr, "Failed to allocate memory 2\n");
                                exit(EXIT_FAILURE);
                            }
                            participants[numParticipants] = new_participant;
                            // Client has been succesfully added, increment the number of participants
                            numParticipants++;
                            int sendingNum = numParticipants - 1;
                            // send id of participant back to client (need to verify)
                            printf("Participant Id: %d\n", (participants[numParticipants-1]->id));
                            send(cl_sd, &sendingNum, sizeof(time_t), 0);

                            printf("Printing all participants: \n");
                            printAll(numParticipants, participants);

                            break;
                        case DRAW_NAMES:
                            srand((unsigned) time(&t));
                            int j;
                            // Assign memory to temp participant for the shuffle (draw)
                            person_t *tmp = (person_t *)malloc(sizeof(person_t));
                            // Check memory has been assigned correctly
                            if (tmp == NULL)
                            {
                                fprintf(stderr, "Failed to allocate memory for temporary participant\n");
                                exit(EXIT_FAILURE);
                            }
                            // Send the number of participants in the participants array to the client
                            send(cl_sd, &numParticipants, sizeof(int *), 0);
                            for (int i = numParticipants-1; i > 0; i--) { // for loop to shuffle
                                j = rand() % (i + 1); //randomise j for shuffle with Fisher Yates
                                tmp = participants[j];
                                participants[j] = participants[i];
                                participants[i] = tmp;
                            }
                            printAll(numParticipants, participants);
                            send(cl_sd, participants, sizeof(participants), 0);
                            // check enough names are in names array
                            // do something
                            send(cl_sd, &hasDrawn, sizeof(hasDrawn), 0);
                            break;
                        default:
                            printf("Invalid selection\n");
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