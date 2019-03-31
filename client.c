#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <jmorecfg.h>
#include <memory.h>
#include <errno.h>
#include <zconf.h>
#include <time.h>
#include <arpa/inet.h>


#define STARTMSG "START GAME\n"
#define POSITIONMSG "POSITIONING SHIPS\n"
#define INPOSITIONMSG "SHIPS IN POSITION\n"
#define HITMSG "HIT\n"
#define MISSMSG "MISS\n"
#define EXITMSG "EXIT\n"
#define PORT 8080
#define MAX 80


/// global variables for came play
int length = 0;
int shotBoard[9][9];
int numShots = 0;
int shipPlaced;
char buff[MAX];
char lastShot[MAX];
int gameState; // variable to track game position
/// 0 is not started
/// 1 is start message sent
/// 2 is positioning ships
/// 3 is ships in position
/// 4 is playing game


/// set board to empty

void initializeBoard() {

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            shotBoard[i][j] = 0;
        }
    }

}

/// print out the shots we have tried with hits and misses
void printShotBoard() {
    printf("\nBOARD IS NOW");

    {
        printf("\n1 is a miss, 2 is a hit, 0 is unused shot");
        printf("\n* A B C D E F G H I");
    }
    for (int i = 0; i < 9; i++) {
        printf("\n%d", i + 1);
        for (int j = 0; j < 9; j++) {
            printf(" %d", shotBoard[i][j]);
        }

    }
    printf("\n");
}

/// this function parses an incoming message and checks what type it is

int getMessageType(char array[]) {


    length = strlen(array);


    char c = array[0];
    char d = array[1];

    if (array[0] == '\0') { return 9; }

    else if (strcmp(array, HITMSG) == 0) {
        //printf("\nIt's a hit");
        return 4;
    } else if (strcmp(array, MISSMSG) == 0) {
        // printf("\nIt's a miss\n");
        return 5;
    }
    else if ((d >= '1' & d <= '9') & (c >= 'A' & c <= 'J') & (length < 4)) {
       // printf("\nserver found It's a shot message");
        return 9;}


    else if ((d >= '1' & d <= '9') & (c >= '1' & c <= '9') & (length < 4)) {
        // printf("\nIt's a score message");
        return 6;
    } else if (strcmp(array, POSITIONMSG) == 0) {
        printf("\n");
        return 2;
    } else if (strcmp(array, INPOSITIONMSG) == 0) {
        printf("\n");
        return 3;
    } else if (strcmp(array, EXITMSG) == 0) {
        //  printf("\nIt's an exit message\n");
        return 8;
    } else { return -1; }
}

void func(int sockfd) {
    char buff[MAX];
    int n;
    for (;;) {

        /// at this point we are waiting for the server to set the ships up

        if (gameState == 1) {


            read(sockfd, buff, sizeof(buff));
            printf("From Server : %s", buff);
            int type = getMessageType(buff);
            /// we receive a game ready message then we can play
            if (type == 3) {
                printf("\nGame Ready ... \n");
                gameState = 2;
                bzero(buff, sizeof(buff));
            } else if (type == 8 || type < 0) {
                printf("Client Exit...\n");
                break;
            }
                /// if we get a positioning ships message clear the buffer
            else if (type == 2) {

                bzero(buff, sizeof(buff));
            } else {}

        }

        /// game hasn't started. server is waiting for START GAME MESSAGE
        if (gameState == 0) {
            bzero(buff, sizeof(buff));
            printf("BATTLESHIP GAME\n");
            printf("GAME STARTING\n");
           // n = 0;
            //while ((buff[n++] = getchar()) != '\n');
            char start[] = STARTMSG;
            write(sockfd, start, sizeof(start));

            //write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            printf("From Server : %s", buff);
            gameState = 1; /// if we get a message back from server then the game has "started"
            int type = getMessageType(buff);
            if (type == 8 || type < 0) {
                printf("Client Exit...\n");
                break;
            }
            bzero(buff, sizeof(buff));


        }





            /// at this point the game has started and we can make shots
        else if (gameState == 2) {

            bzero(buff, sizeof(buff));
            printf("\n");

            /// GET SHOT FROM USER AND CHECK FOR CORRECT INPUT
            while(1) {

                printf("Enter your shot!! \n(Capital letter ( A to I )  then number ( 1 to 9 ) \nEXIT to quit:  ");
                fgets(buff, MAX, stdin);
                if (getMessageType(buff) == 9)
                { break ;}
                else if (getMessageType(buff) == 8){
                    printf("Client Exit...\n");
                    char response[] = EXITMSG;
                    write(sockfd, response, sizeof(response));
                    break;
                }
                else { printf("\nIncorrect input!!\nPlease enter your shot again: "); }
            }




            /// SEND THE CHOT

            write(sockfd, buff, sizeof(buff));
            strncpy(lastShot, buff, MAX);


            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            printf("From Server : %s", buff);

            /// if win message
            int type = getMessageType(buff);
            if (type == 6) {
                printf("\nyou win ... \n");
                printf("\nscore is  ... %s ", buff);
                printf("Client Exit...\n");
                char response[] = EXITMSG;
                write(sockfd, response, sizeof(response));

            }

            if (type == 4) {
                /// hit
                /// store on shotBoard
                int row = lastShot[0] - 64 - 1; // convert uppercase letter to row
                int col = lastShot[1] - '0' - 1;

                shotBoard[col][row] = 2;
                printShotBoard();

            }


            if (type == 5) {
                /// miss
                /// store on shotBoard
                int row = lastShot[0] - 64 - 1; // convert uppercase letter to row
                int col = lastShot[1] - '0' - 1;
                shotBoard[col][row] = 1;
                printShotBoard();
                printf("\n");


            }


            if (type == 8 || type < 0) {
                printf("Client Exit...\n");
                break;
            } else {}

        }

    } /*** END MAIN LOOP ***/
}


int main(int argc, char const *argv[]) {

    gameState = 0; // starting !!

    /*** basic connection stuff from here :
     * https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
     * and
     * https://www.geeksforgeeks.org/socket-programming-cc/
     */
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    initializeBoard();

    /// connection stuff !
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;


    /// PORT is HERE taken from command argument
    serv_addr.sin_port = htons((unsigned short)strtoul(argv[1], NULL, 0));
    //serv_addr.sin_port = htons(PORT);

    /// servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Convert IPv4 and IPv6 addresses from text to binary form
    /// the 2nd argument can be the 2nd argument of the client arguments
    if (inet_pton(AF_INET, argv[2], &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }


    /// MAIN LOOPS

    /// function for game
    func(sock);

    /// close the socket when we break out of main loop
    close(sock);


    return 0;
}