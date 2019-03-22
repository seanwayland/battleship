#include <stdio.h>
#include <jmorecfg.h>
#include <memory.h>
#include <stdlib.h>
#include <stdlib.h>
#include <errno.h>
#include <zconf.h>
#include <time.h>

#define STARTMSG "START GAME\n"
#define POSITIONMSG "POSITIONING SHIPS\n"
#define INPOSITIONMSG "SHIPS IN POSITION\n"
#define HITMSG "HIT\n"
#define MISSMSG "MISS\n"



long messageType = -1;
int length = 0;
char input[256];
boolean number = FALSE;
char * endptr;
int board [9][9];
int shotBoard[9][9];
int numShots = 0;
int shipPlaced;

/// set board to empty
void initializeBoard() {

    for ( int i = 0; i < 9 ; i ++ ) {
        for (int j = 0; j < 9; j++) {
            board[i][j] = 0;
            shotBoard[i][j] = 0;
        }
    }

}

void initializeInput() {
    for ( int i = 0; i <245; i++)
        input[i] = 0;
}

/// check to see if all ships are sunk
/// returning 1 means board is empty

int scanBoard() {
    int result = 1;
    for ( int i = 0; i < 9 ; i ++ ){
        for ( int j = 0 ; j < 9 ; j++)
            if (board[i][j] != 0){
                result = 0;
            }
    }
    return result;
}

void printBoard(){
    printf("\nBOARD");
    printf("\n");
    {
        printf("\n* A B C D E F G H I");
    }
    for ( int i = 0; i < 9; i++){
        printf("\n%d", i + 1);
         for (int j = 0; j < 9; j++){
             printf(" %d", board[i][j]);
         }

    }
}

void printShotBoard(){
    printf("\nBOARD");
    printf("\n");
    {
        printf("\n* A B C D E F G H I");
    }
    for ( int i = 0; i < 9; i++){
        printf("\n%d", i + 1);
        for (int j = 0; j < 9; j++){
            printf(" %d", shotBoard[i][j]);
        }

    }
}

void shoot(){
    int row = input[0] - 64 - 1; // convert uppercase letter to row
    //printf("\nrow %d", row);
    int col = input[1] - '0' - 1;
    //printf("\ncol %d", col);

    if (board[col][row] > 0) {
        board[col][row] = 0;
        printf("\nHIT");
        shotBoard[col][row] = 2;



    }
    else {
        printf("\nMISS");
        shotBoard[col][row] = 1;
    }

    printShotBoard();

}
/// stick a ship on the board
/// returns a one when finished
int placeShip(int size){
    int result = 0;
    int direction = rand() & 1; // 0 is vertical 1 is horizontal
    /// horizontal is a row
    /// [row][columns]
    /// a horizontal row is represented by [x][0] [x][1] [x][2] [x][3] ..
    /// a vertical column is represented by [0][y] [1][y] [2][y]
    int rowColNumber = rand() % 9; // generate random row or column number
    int pos = rand() % 9 ; // generate random position

    /// if it's horizontal check the row for space
    if (direction == 1){
        /// if row doesn't have space for ship beyond position reset
        if ((8 - pos)  < size ) {
            pos = 8 - size;
        }
        /// check for free space
        int freeSpace = 0;
        for ( int j = 0 ; j < size ; j ++){
         freeSpace = freeSpace +  board[rowColNumber][pos + j];
    }  /// if there is enough space then place ship
    if (freeSpace == 0) {
        for ( int j = 0 ; j < size ; j ++){
            board[rowColNumber][pos + j] = size ;
        }
        result = 1; /// ship has been placed
        //printf("\nShip Placed, size is %d direction is %d ", size, direction);
    }
        }

        /// if it's vertical check the column for space
    if (direction == 0){
        /// if column doesn't have space for ship beyond position reset
        if ((8 - pos)  < size ) {
            pos = 8 - size;
        }
        int freeSpace = 0;
        for ( int j = 0 ; j < size ; j ++){
            freeSpace = freeSpace +  board[pos + j][rowColNumber];
        }  /// if there is enough space then place ship
        if (freeSpace == 0) {
            for ( int j = 0 ; j < size ; j ++){
                board[pos+j][rowColNumber] = size ;
            }
            result = 1; /// ship has been placed
            //printf("\nShip Placed, size is %d direction is %d ", size, direction);
        }
    }

return result;
}
/***Canberra-class Landing Helicopter Dock	5
Hobart-class Destroyer	4
Leeuwin-class Survey Vessel	3
Armidale-class Patrol Boat	2
 ***/

void getMessageType(char array[]) {


    length = strlen(array);


    if (strcmp(array, HITMSG) == 0) {
        //printf("\nIt's a hit");
        messageType = 4;

    }

    if (strcmp(array, MISSMSG) == 0) {
       // printf("\nIt's a miss");
        messageType = 5;

    }

    if (strcmp(array, STARTMSG) == 0) {
        //printf("\nIt's a start message");
        messageType = 1;
    }

    if (strcmp(array, POSITIONMSG) == 0) {
       // printf("\nIt's a positioning ships message");
        messageType = 2;
    }

    if (strcmp(array, INPOSITIONMSG) == 0) {
        //printf("\nIt's a positioning ships message");
        messageType = 3;
    }

    char c = array[0];
    char d = array[1];

    if ((d >= '1' & d <= '9') & (c >= 'A' & c <= 'J') & (length < 4)) {
        messageType = 6;
        //printf("\nIt's a shot message");
    }

    for ( int y = 0 ; y < length - 1; y ++){
        char e = array[y];
        if (e >= '1' & e <= '9' ) { number = TRUE;}
        else { number = FALSE; }
    }

    /***
    if (number == TRUE)
    { messageType = strtol(array, &endptr, 10);
    printf("\nIt's a win. Score is %d", messageType );

        /* test return to number and errno values
        if (array == endptr)
            printf (" number : %lu  invalid  (no digits found 0 returned)\n", messageType);
        else if (errno == ERANGE && number == LONG_MIN)
            printf (" number : %lu  invalid  (underflow occurred)\n", messageType);
        else if (errno == ERANGE && number == LONG_MAX)
            printf (" number : %lu  invalid  (overflow occurred)\n", messageType);
        else if (errno == EINVAL)  /* not in all c99 implementations - gcc OK
            printf (" number : %lu  invalid  (base contains unsupported value)\n", messageType);
        else if (errno != 0 && number == 0)
            printf (" number : %lu  invalid  (unspecified error occurred)\n", messageType);
        else if (errno == 0 && array && !*endptr)
            printf (" number : %lu    valid  (and represents all characters read)\n", messageType);
        else if (errno == 0 && array && *endptr != 0)
            printf (" number : %lu    valid  (but additional characters remain)\n", messageType);

    }

   */




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
    srand(time(0));
    initializeBoard();



    for ( int i = 2; i < 6; i ++) {
        shipPlaced = 0;
        // place a ship of size 2
        while (shipPlaced == 0) {
            shipPlaced = placeShip(i);
        }
    }

    //printBoard();



    /*
    if (scanBoard() == 1){
        printf("\nAll ships sunk !! ");
        printf("\nNumber of shots is %d", numShots);
    }
     */
    int gameOver = 0;
    while (gameOver ==0) {
        printf("\nEnter a shot\n");
        fgets(input, 256, stdin);
        getMessageType(input);
        if (messageType != 6){
            printf("\nNot a shot message game will terminate");
            break;

        }
        shoot();
        gameOver = scanBoard();
        initializeInput();
        numShots ++;
    }
    printf("\nGAME OVER ... Number of Shots is %d", numShots);


    //getMessageType(input);
    //if (messageType < 1) { printf("\nInvalid message"); }

    return 0;
}

