#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 5000

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;
    int sockRet, connectRet;
    socklen_t serverLen;
    char choice[5];

    /*--------------------------------------------SOCKET CREATION-----------------------------------------*/

    //create socket
    if((sockRet = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("\n------Socket Creation Failed------\n");
        exit(1);
    }
    printf("\n+++ Socket created successfully\n");

    //setting domain
    server_addr.sin_family = AF_INET;
    //setting port
    server_addr.sin_port = PORT;
    //setting internet address
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //connect to the server
    serverLen = sizeof(server_addr);
    if((connectRet = connect(sockRet, (struct sockaddr*)&server_addr, serverLen)) < 0) {
        printf("\n________Connection Failed__________\n");
        exit(1);
    }
    printf("\n+++ Connection Successful\n");

    while(1) {
        printf("\nEnter a choice :\n1- RETR\n2- STOR\n3- APPE\n4- REST\n5- RNFR\n6- RNTO\n7- PWD\n8- MKD\n9- RMD\n10- LIST\n11- CWD\n12- USER\n13- SYST\n14- ABOR\n15- DELE\n16- QUIT\n17- CDUP\n18- REIN\n19- REST\n20- PORT\n21- STAT\n");
		scanf("%s", choice);
    }

    return 0;
}