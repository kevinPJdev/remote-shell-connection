#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 5000

int main(int argc, char *argv[]) {

    struct sockaddr_in server_addr, client; //declare socket address structure
    int sockRet, bindRet, listenRet, acceptRet; //To handle sock commands
    socklen_t clientLen;

    if((sockRet = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("\n------Socket Creation Failed------\n");
        exit(1);
    }

    //setting domain
    server_addr.sin_family = AF_INET;
    //setting port
    server_addr.sin_port = PORT;
    //setting internet address
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //bind() is usually called by a server to bind their local  IP and a well-known port number to a socket
    if((bindRet = bind(sockRet, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) {
         printf("\n------Socket Binding Failed------\n");
         exit(1);
    }

    //listen() is called only by a TCP server to accept  connections from client sockets that will issue a  connect().
    if((listenRet = listen(sockRet, 10)) < 0) {
        printf("\n------Listener Failed------\n");
        exit(1);        
    }       

    while(1) {
        printf("\n------Waiting for pending connections------\n");
        clientLen = sizeof(client);

        /* 
        *  accept() is called by a TCP server to extract the first  connection in the queue of pending connections,  
        *  creates a new socket with the properties of s, and  allocates a new file descriptor for the newly created socket.
        */
        if((acceptRet = accept(sockRet, (struct sockaddr*)&client, &clientLen)) < 0) {
            printf("\n-------Socket Connection Failed--------\n");
            exit(1);
        }



    }


    return 0;






}