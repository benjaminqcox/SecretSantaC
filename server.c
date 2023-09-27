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

person_t *findParticpantById(int id, person_t **participants, int num_participants)
{
    // Go through each participant in participants
    // if the id of the current participant matches the chosen id, return that participant
    for (int i = 0; i < num_participants; i++)
    {
        if (participants[i]->id == id)
        {
            return participants[i];
        }
    }
    // No participant found, return NULL
    return NULL;
}


person_t *findGifteeBySantaId(int id, person_t **participants, int num_participants)
{
    // Loop through each participant in participants
    for (int i = 0; i < num_participants; i++)
    {
        // if the id of the current participant matches the chosen id
        if (participants[i]->id == id)
        {
            
            if (i == num_participants - 1)
            {
                // if the current participant is the final participant, the giftee is the first particpant
                return participants[0];
            }
            else
            {
                // the giftee is the next participant
                return participants[i+1];
            }
        }
    }
    // No participant found with that id
    return NULL;
}

person_t *findSantaByGifteeId(int id, person_t **participants, int num_participants)
{
    // Loop through each participant in participants
    for (int i = 0; i < num_participants; i++)
    {
        // if the id of the current participant matches the chosen id
        if (participants[i]->id == id)
        {
            if(i == 0)
            {
                // if the current participant is the first participant, the santa is the last participant
                return participants[num_participants - 1];
            }
            else
            {
                // the santa is the previous participant
                return participants[i-1];
            }
        }
    }
    // No participant found with that id
    return NULL;
}

void findGiftee(int cl_sd, person_t **participants, int num_participants)
{
    // Declare variable to store the giftee id received from client
    int santaId;
    // Send the number of participants to the client
    send(cl_sd, &num_participants, sizeof(num_participants), 0);
    // Recieve the chosen santa id and store it in the santa id variable
    recv(cl_sd, &santaId, sizeof(santaId), 0);
    // Store the participant found with the chosen id
    person_t *santa = findParticpantById(santaId, participants, num_participants);
    // Send the found participant to the client (used for clean output)
    send(cl_sd, santa, sizeof(person_t), 0);
    // Store the found giftee for the chosen giftee id
    person_t *foundGiftee = findGifteeBySantaId(santaId, participants, num_participants);
    // Send the found giftee to the client
    send(cl_sd, foundGiftee, sizeof(person_t), 0);
}

void findSanta(int cl_sd, person_t **participants, int num_participants)
{
    // Declare variable to store the giftee id received from client
    int gifteeId;
    // Send the number of participants to the client
    send(cl_sd, &num_participants, sizeof(num_participants), 0);
    // Recieve the chosen giftee id and store it in the giftee id variable
    recv(cl_sd, &gifteeId, sizeof(gifteeId), 0);
    // Store the participant found with the chosen id
    person_t *giftee = findParticpantById(gifteeId, participants, num_participants);
    // Send the found participant to the client (used for clean output)
    send(cl_sd, giftee, sizeof(person_t), 0);
    // Store the found santa for the chosen giftee id
    person_t *foundSanta = findSantaByGifteeId(gifteeId, participants, num_participants);
    // Send the found santa to the client
    send(cl_sd, foundSanta, sizeof(person_t), 0);
}

void listPairs(int cl_sd, person_t **participants, int num_participants)
{
    // Send the number of participants to the client
    send(cl_sd, &num_participants, sizeof(num_participants), 0);
    // Loop through all participants sending them to the client 1 by 1
    for (int i = 0 ; i < num_participants ; i++)
    {
        send(cl_sd, participants[i], sizeof(person_t), 0);
    }
}

void addPerson(int cl_sd, person_t ***participants, int *num_participants)
{
    char name[NAME_SIZE];
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
    new_participant->id = *num_participants; // If multithreading, will need to add a mutex lock to increment the id and add participants
    // Add the user chosen name from the client to the new participant
    strlcpy(new_participant->name, name, NAME_SIZE);
    
    // Update the final participant (not num_participants+1 as it is 0 indexed)
    *participants = (person_t **)realloc(*participants, ((*num_participants) + 1) * sizeof(person_t *));
    // Check memory for participants has been reallocated correctly
    if (participants == NULL)
    {
        // failed to allocate memory
        fprintf(stderr, "Failed to allocate memory 2\n");
        exit(EXIT_FAILURE);
    }
    (*participants)[*num_participants] = new_participant;
    // Client has been succesfully added, increment the number of participants
    int sendingNum = *num_participants;
    (*num_participants)++;
    // Send the number of participants to the client
    send(cl_sd, &sendingNum, sizeof(time_t), 0);
}

void drawNames(int cl_sd, person_t **participants, int num_participants, bool *has_drawn)
{
    time_t t;
    // Send the result of has_drawn to the client if the number of participants is less than the minimum num participants
    if (num_participants < MIN_PARTICIPANTS)
    {
        send(cl_sd, has_drawn, sizeof(*has_drawn), 0);
        return;
    }
    // Setup the random nuber generator based on the time
    srand((unsigned) time(&t));
    // Declare j to store the result of the random number generator
    int j;
    // Assign memory to temp participant for the shuffle (draw)
    person_t *tmp = (person_t *)malloc(sizeof(person_t));
    // Check memory has been assigned correctly
    if (tmp == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for temporary participant\n");
        exit(EXIT_FAILURE);
    }
    // Shuffle the participants
    for (int i = num_participants-1; i > 0; i--) {
        // Generate a random number for the shuffle
        j = rand() % (i + 1);
        tmp = participants[j];
        participants[j] = participants[i];
        participants[i] = tmp;
    }
    // Participants have been drawn
    *has_drawn = true;
    // Send that the draw has happened to the client
    send(cl_sd, has_drawn, sizeof(*has_drawn), 0);
}

int main()
{
    // Declare the variables needed to store connection based information
    int ret, sd, cl_sd;
    struct sockaddr_in sv_addr, cl_addr;
    socklen_t addrlen = sizeof(cl_addr);
    ssize_t sent_bytes;
    bool active_client = false;

    // Create socket
    sd = socket(AF_INET, SOCK_STREAM, TCP_MODE);
    // Check successful socket creation
    if (sd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
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
            // Declare secret santa based variables
            char name[NAME_SIZE];
            bool has_drawn = false;
            person_t **participants = NULL;
            int num_participants = 0;
              
            int gifteeId;
            
            while (1)
            {
                // Send to client the result of has_drawn
                send(cl_sd, &has_drawn, sizeof(has_drawn), 0);
                int menuChoice;
                // Get the menuChoice from the client
                recv(cl_sd, &menuChoice, sizeof(menuChoice), 0);
                if (menuChoice == QUIT)
                {
                    printf("Client has closed the connection\n");
                    continue;
                }
                if (has_drawn)
                {
                    switch(menuChoice)
                    {
                        case FIND_GIFTEE:
                            findGiftee(cl_sd, participants, num_participants);
                            break;
                        case FIND_SANTA:
                            findSanta(cl_sd, participants, num_participants);
                            break;
                        case LIST_PAIRS:
                            listPairs(cl_sd, participants, num_participants);
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
                            addPerson(cl_sd, &participants, &num_participants);
                            break;
                        case DRAW_NAMES:
                            drawNames(cl_sd, participants, num_participants, &has_drawn);
                            break;
                        default:
                            printf("Invalid selection\n");
                            break;
                    }
                }
                

            }
            // Free all used memory
            freeParticipants(participants, num_participants);
            free(participants);
            
            // Close child socket connection and exit child
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