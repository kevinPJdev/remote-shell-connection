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
    int sock1, ret, l; //To handle sock commands

    if((sock1 = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("------Socket Creation Failed------");
        exit(1);
    }

    //setting domain
    server_addr.sin_family = AF_INET;
    //setting port
    server_addr.sin_port = PORT;
    //setting internet address
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if((ret = bind(sock1, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) {
         printf("------Socket Binding Failed------");
         exit(1);
    }

    if((l = listen(sock1, 10)) < 0) {
        printf("------Listener Failed------");
        exit(1);        
    }       

    while(1) {    
    }


    return 0;






}