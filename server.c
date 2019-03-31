#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <jmorecfg.h>
#include <memory.h>
#include <stdlib.h>
#include <errno.h>
#include <zconf.h>
#include <time.h>

#define STARTMSG "START GAME\n"
#define POSITIONMSG "POSITIONING SHIPS\n"
#define INPOSITIONMSG "SHIPS IN POSITION\n"
#define HITMSG "HIT\n"
#define MISSMSG "MISS\n"
#define EXITMSG "EXIT\n"
#define PORT 8080
#define MAX 80


int length = 0;
int board[9][9];
int numShots = 0;
int shipPlaced;
char buff[MAX];

/// check for a win !!!

int scanBoard() {
    int result = 1;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++)
            if (board[i][j] != 0) {
                result = 0;
            }
    }
    return result;
}


/// check a shot for a hit or a miss and update the board
int shoot() {
    {
        int row = buff[0] - 64 - 1; // convert uppercase letter to row
        int col = buff[1] - '0' - 1;
        if (board[col][row] > 0) {
            board[col][row] = 0;
            printf("\nHIT");
            return 1;
        } else {
            printf("\nMISS");
            return 2;
        }
    }


}


/***
 * 0 is waiting for client to press start
 * 1 is waiting for positioning ships
 * 2 is waiting for ships in position
 * 3 is waiting for shots
 *    shot received,  hit , miss
 * 4 is game is over
 *
 */

void printBoard() {
    printf("\nBOARD");
    printf("\n");
    {
        printf("\n* A B C D E F G H I");
    }
    for (int i = 0; i < 9; i++) {
        printf("\n%d", i + 1);
        for (int j = 0; j < 9; j++) {
            printf(" %d", board[i][j]);
        }

    }
}


/// check an incoming message for it's type
int getMessageType(char array[]) {


    length = strlen(array);

    char c = array[0];
    char d = array[1];

    if (array[0] == '\0') { return 9; }

    else if (strcmp(array, STARTMSG) == 0) {
        printf("\nserver found It's a start message");
        return 1;
    } else if (strcmp(array, EXITMSG) == 0) {
        printf("\nserver found It's an exit message");
        return 8;
    } else if ((d >= '1' & d <= '9') & (c >= 'A' & c <= 'J') & (length < 4)) {
        printf("\nserver found It's a shot message");
        return 6;

    } else if (strcmp(array, "") == 0) {
        return 9;
    } else { return -1; }
}


/// set board to empty
void initializeBoard() {

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            board[i][j] = 0;

        }
    }

}


/// stick a ship on the board
/// returns a one when finished
int placeShip(int size) {
    int result = 0;
    int direction = rand() & 1; // 0 is vertical 1 is horizontal
    /// horizontal is a row
    /// [row][columns]
    /// a horizontal row is represented by [x][0] [x][1] [x][2] [x][3] ..
    /// a vertical column is represented by [0][y] [1][y] [2][y]
    int rowColNumber = rand() % 9; // generate random row or column number
    int pos = rand() % 9; // generate random position

    /// if it's horizontal check the row for space
    if (direction == 1) {
        /// if row doesn't have space for ship beyond position reset
        if ((8 - pos) < size) {
            pos = 8 - size;
        }
        /// check for free space
        int freeSpace = 0;
        for (int j = 0; j < size; j++) {
            freeSpace = freeSpace + board[rowColNumber][pos + j];
        }  /// if there is enough space then place ship
        if (freeSpace == 0) {
            for (int j = 0; j < size; j++) {
                board[rowColNumber][pos + j] = size;
            }
            result = 1; /// ship has been placed

        }
    }

    /// if it's vertical check the column for space
    if (direction == 0) {
        /// if column doesn't have space for ship beyond position reset
        if ((8 - pos) < size) {
            pos = 8 - size;
        }
        int freeSpace = 0;
        for (int j = 0; j < size; j++) {
            freeSpace = freeSpace + board[pos + j][rowColNumber];
        }  /// if there is enough space then place ship
        if (freeSpace == 0) {
            for (int j = 0; j < size; j++) {
                board[pos + j][rowColNumber] = size;
            }
            result = 1; /// ship has been placed

        }
    }

    return result;
}


void game(int sockfd) {

    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        int type = getMessageType(buff);
        printf("From client: %s\t To client : ", buff);


        if (type == 8 || type < 0) {
            printf("Server Exit...\n");

            char die[] = EXITMSG;
            write(sockfd, die, sizeof(die));

            break;
        }

            /// if we recieve a start message set the board up
        else if (type == 1) {

            srand(time(0));
            printf("\ninitializing board");
            printBoard();
            char response[] = POSITIONMSG;
            write(sockfd, response, sizeof(response));
            bzero(buff, MAX);
            for (int i = 2; i < 6; i++) {
                shipPlaced = 0;
                // place a ship of size 2
                while (shipPlaced == 0) {
                    shipPlaced = placeShip(i);
                }
            }



            printf("\nboard finished\n");
            sleep(1); /// maybe this helps the program not hanging when the client doesnt loop back fast enough
            char response2[] = INPOSITIONMSG;
            write(sockfd, response2, sizeof(response2));
            bzero(buff, MAX);

        }


            /// if we recieve a shot message deal with it
        else if (type == 6) {

            printf("\nchecking shot\n");
            int shot = shoot();

            if (shot == 1) {
                numShots++;
                int win = scanBoard();
                if (win == 1) {
                    numShots++;
                    char response6[MAX];
                    sprintf(response6, "%d", numShots);

                    printf("You WIN ...\n");

                    write(sockfd, response6, sizeof(response6));
                    printf("Server Exit...\n");


                }
                char response3[] = HITMSG;
                write(sockfd, response3, sizeof(response3));
                printBoard();
            } else if (shot == 2) {
                numShots++;
                char response2[] = MISSMSG;
                write(sockfd, response2, sizeof(response2));
                printBoard();
            } else {
                printf("Server Exit...\n");
                char die[] = EXITMSG;
                write(sockfd, die, sizeof(die));

            }

        }

        bzero(buff, sizeof(buff));

    }
}


/*** basic connection stuff from here :
 * https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
 * and
 * https://www.geeksforgeeks.org/socket-programming-cc/
 */

int main(int argc, char const *argv[]) {
    initializeBoard();
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server.c";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                             (socklen_t *) &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }


    /// MAIN LOOPS

    // function for game
    game(new_socket);

    // close the socket
    close(new_socket);


    return 0;
}