#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080
#define MAX 80

#include <stdio.h>
#include <jmorecfg.h>
#include <memory.h>
#include <stdlib.h>
#include <stdlib.h>
#include <errno.h>
#include <zconf.h>
#include <time.h>

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 8080
#define MAX 80

#define STARTMSG "START GAME\n"
#define POSITIONMSG "POSITIONING SHIPS\n"
#define INPOSITIONMSG "SHIPS IN POSITION\n"
#define HITMSG "HIT\n"
#define MISSMSG "MISS\n"
#define EXITMSG "EXIT\n"
#define WINMSG "WIN\n"





long messageType = -1;
int length = 0;
char input[256];
boolean number = FALSE;
char * endptr;
int board [9][9];
int shotBoard[9][9];
int numShots = 0;
int shipPlaced;
char buff[MAX];
int gameState; // variable to track game position
/// 0 is not started
/// 1 is start message sent
/// 2 is positioning ships
/// 3 is ships in position
/// 4 is playing game
int n;
//char response[MAX];




int getMessageType(char array[]) {


    length = strlen(array);



    char c = array[0];
    char d = array[1];

    if (array[0] == '\0') { return 9; }

    else if (strcmp(array, HITMSG) == 0) {
        printf("\nIt's a hit");
        return 4;
    }

    else if (strcmp(array, MISSMSG) == 0) {
        printf("\nIt's a miss\n");
        return 5;
    }

    else if ((d >= '1' & d <= '9') & (c >= '1' & c <= '9') & (length < 4)) {
        printf("\nIt's a score message");
        return 6;
    }




    else if (strcmp(array, POSITIONMSG) == 0) {
        printf("\nIt's a positioning ships message\n");
        return 2;
    }

    else if (strcmp(array, INPOSITIONMSG) == 0) {
        printf("\nIt's a positioning ships message\n");
        return 3;
    }

    else if (strcmp(array, EXITMSG) == 0) {
        printf("\nIt's an exit message\n");
        return 8;
    }



    else { return -1;}
}

void func(int sockfd)
{
    char buff[MAX];
    int n;
    for (;;) {

        if (gameState == 0)
        {
            bzero(buff, sizeof(buff));
            printf("Enter the string : ");
            n = 0;
            while ((buff[n++] = getchar()) != '\n');
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            printf("From Server : %s", buff);
            gameState = 1;
            int type = getMessageType(buff);
            if (type == 8 || type < 0) {
                printf("Client Exit...\n");
                break;
            }
            bzero(buff, sizeof(buff));


        }

        if (gameState == 1)
        {

           // write(sockfd, buff, sizeof(buff));
           // bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            printf("From Server : %s", buff);
            int type = getMessageType(buff);
            if (type == 8 || type < 0) {
                printf("Client Exit...\n");
                break;
            }
            else if (type == 2) {
                bzero(buff, sizeof(buff));
                gameState = 1;
            }
            else if (type == 3) {
                printf("\nGame Ready ... \n");
                gameState = 2;
                bzero(buff, sizeof(buff));
            }

            else { }

        }


        if (gameState == 2)
        {
            //write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            printf("Enter the string : ");
            n = 0;
            while ((buff[n++] = getchar()) != '\n');
            write(sockfd, buff, sizeof(buff));


            bzero(buff, sizeof(buff));
            read(sockfd, buff, sizeof(buff));
            printf("From Server : %s", buff);

            /// if win message
            int type = getMessageType(buff);
            if (type == 6) {
                printf("\nyou win ... \n");
                printf("\nscore is  ... %s ", buff );
                printf("Client Exit...\n");
                char response[] = EXITMSG;
                write(sockfd, response, sizeof(response));
                //break;
            }




            if (type == 8 || type < 0) {
                printf("Client Exit...\n");
                break;
            }





        }

    } /*** END MAIN LOOP ***/
}


int main(int argc, char const *argv[])
{

    gameState = 0; // starting !!

    /*** basic connection stuff from here :
     * https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
     * and
     * https://www.geeksforgeeks.org/socket-programming-cc/
     */
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client.c";
    char buffer[1024] = {0};

    /// connection stuff !
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }


    /// MAIN LOOPS

    // function for chat
    func(sock);

    // close the socket
    close(sock);


    /***
    send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent from client\n");
    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer );

     ***/
    return 0;
}