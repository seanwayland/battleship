#include <stdio.h>
#include <jmorecfg.h>
#include <memory.h>
#include <stdlib.h>

#define STARTMSG "START GAME\n"
#define POSITIONMSG "POSITIONING SHIPS\n"
#define INPOSITIONMSG "SHIPS IN POSITION\n"
#define HITMSG "HIT\n"
#define MISSMSG "MISS\n"


int messageType = -1;
int length = 0;
char input[256];
boolean number = FALSE;


void getMessageType(char array[]) {


    length = strlen(array);


    if (strcmp(array, HITMSG) == 0) {
        printf("\nIt's a hit");
        messageType = 4;

    }

    if (strcmp(array, MISSMSG) == 0) {
        printf("\nIt's a miss");
        messageType = 5;

    }

    if (strcmp(array, STARTMSG) == 0) {
        printf("\nIt's a start message");
        messageType = 1;
    }

    if (strcmp(array, POSITIONMSG) == 0) {
        printf("\nIt's a positioning ships message");
        messageType = 2;
    }

    if (strcmp(array, INPOSITIONMSG) == 0) {
        printf("\nIt's a positioning ships message");
        messageType = 3;
    }

    char c = array[0];
    char d = array[1];

    if ((d >= '1' & d <= '9') & (c >= 'A' & c <= 'J') & (length < 4)) {
        messageType = 6;
        printf("\nIt's a shot message");
    }

    for ( int y = 0 ; y < length - 1; y ++){
        char e = array[y];
        if (e >= '1' & e <= '9' ) { number = TRUE;}
        else { number = FALSE; }
    }

    if (number == TRUE)
    { messageType = atoi(array);
    printf("\nIt's a win. Score is %d", messageType );}






}



int main() {


    /*** GET SOME INPUT
    // zero is invalid message
    // 1 is START GAME
    // 2 is POSITIONING SHIPS
    // 3 is  SHIPS IN POSITION
    // 4 is HIT
    // 5 is MISS
    // 6 is SHOT message
     ***/
    printf("Enter a string\n");
    fgets(input, 256, stdin);
    printf("Your string: %s\n", input);
    printf("\n");
    getMessageType(input);
    if (messageType < 1) { printf("\nInvalid message"); }

    return 0;
}

