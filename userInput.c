#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userInput.h"

#define BUFFERSIZE 64


char * getStringInput()
{
    size_t size = 0;
    size_t len  = 0;
    size_t last = 0;
    char *buf = NULL;

    do
    {
        size += BUFSIZ; /* BUFSIZ is defined as "the optimal read size for this platform" */
        buf = realloc(buf, size); /* realloc(NULL,n) is the same as malloc(n) */            
        /* Actually do the read. Note that fgets puts a terminal '\0' on the
           end of the string, so we make sure we overwrite this */
        if (buf == NULL)
        {
            fprintf(stderr, "Could not reallocate memory\n");
            exit(EXIT_FAILURE);
        }
        fgets(buf + last, BUFSIZ, stdin);
        // Check null on fgets
        len = strlen(buf);
        last = len - 1;
    } while (!feof(stdin) && buf[last] != '\n');
    return buf;
}


int getIntInput()
{
    char * endPtr = NULL;
    char * strVal = getStringInput();
    int chosenVal = (int)strtol(strVal, &endPtr, 10);
    // while chosenVal == 0 and the user didn't input 0, keep looping
    // strtol fills invalid inputs with 0
    while (chosenVal == 0 && strncmp(strVal, "0", 1) != 0)
    {
        endPtr = NULL;
        strVal = getStringInput();
        chosenVal = (int)strtol(strVal, &endPtr, 10);
    }

    printf("You have chosen the value: %d\n", chosenVal);
    return chosenVal;
}

int getIntInputInRange(int low, int high)
{
    // If the low range is higher than the high range then an invalid input has been used
    if (low > high)
    {
        fprintf(stderr, "The chosen range will indefinitely loop. Please choose a valid range.\n");
        exit(EXIT_FAILURE);
    }
    // Ask for user input within range
    printf("Please enter an integer within the range of %d and %d\n", low, high);
    int chosenVal = getIntInput();
    // while the value is out of the specified range, keep asking for a new input
    while (chosenVal < low || chosenVal > high)
    {
        printf("%d is not within the range of %d and %d\n", chosenVal, low, high);
        chosenVal = getIntInput();
    }
    return chosenVal;
}