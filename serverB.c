/*
 * Authors
 * Kevin Patrick Jacob
 * Harmanjot Singh Suri
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>

/*for getting file size using stat()*/
#include<sys/stat.h>

/*for O_RDONLY*/
#include<fcntl.h>
struct stat st = {0};

#define PORT 5096 //if you face a binding error, change the port number

struct sockaddr_in server, client;
struct stat obj;
socklen_t sock2, clientLen;
char buffer[100], command[5], filename[20]; //Character Arrays for handling user inputs
int k, i, size, c;
int filehandle, sockRet; //To handle files and sockRet for socket commands
pid_t childpid; // Child process id
int cnt = 0; //Client count

/* Handle all incoming commands to the server */
void ServiceClient(char *command, pid_t *child_pid) {
    while(1){
        recv(sock2, buffer, 100, 0);
        sscanf(buffer, "%s", command);
            
        if(!strcmp(command, "STOR"))
        {
            int c = 0, clientLen;

            // seperate the command and filename
            sscanf(buffer+strlen(command), "%s", filename);

            // receive the file size
            recv(sock2, &size, sizeof(int), 0);
            i = 1;
            while(1){

                // Create the file
                filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                if(filehandle == -1){
                    sprintf(filename + strlen(filename), "%d", i);
                }
                else{
                    break;
                }
            }

            // Dynamically Allocate the size
            char *f = (char*)malloc(size);

            // Receive the complete file from client
            recv(sock2, f, size, 0);

            // Write to file
            c = write(filehandle, f, size);

            // Close the file
            close(filehandle);

            // send status to client
            send(sock2, &c, sizeof(int), 0);
            printf("[+]213 File status OK.\n");
            printf("[+]200 Command okay.\n");
        }
        else if(!strcmp(command, "RNFR"))
        {
            char old_name[100], new_name[100];

            // get the old file name
            recv(sock2, old_name, 100, 0);

            // get new file name
            recv(sock2, new_name, 100, 0);

            // check if directory is created or not
            if (stat(buffer, &st) == 1) {
                printf("[-]Directory Does Not EXIST\n");
            }
            else {

                // Rename
                rename(old_name, new_name);
                printf("[+]Successfully rename\n");
            }

            // send status
            send(sock2, new_name, 100, 0);
            printf("[+]200 Command okay.\n");
        }

        else if(!strcmp(command, "RNTO"))
        {
            char old_name[100], new_name[100];
            recv(sock2, old_name, 100, 0);

            recv(sock2, new_name, 100, 0);

            // check if directory is created or not
            if (stat(buffer, &st) == 1) {
                
                printf("[-]Directory Does Not EXIST\n");
            }
            else {
                rename(old_name, new_name);
                printf("[+]Successfully rename\n");
            }
            send(sock2, buffer, 100, 0);
            printf("[+]200 Command okay.\n");
        }
        
        else if(!(strcmp(command,"CDUP")))
        {
            if(chdir("..") == 0)
                c = 1;
            else
                c = 0;
            send(sock2,&c,sizeof(int),0);
            printf("[+]200 Command okay");
        }

        else if(!strcmp(command, "PWD")){
            system("pwd > PWD.txt");
            i = 0;

            // Open file in reading mode
            FILE*f = fopen("PWD.txt", "r");

            // Read until end of file
            while(!feof(f)){

                // copy the PWD
                buffer[i++] = fgetc(f);
            }

            // add the null chracter at the end
            buffer[i-1] = '\0';

            // close file pointer
            fclose(f);

            // send the PWD result
            send(sock2, buffer, 100, 0);
            printf("[+]200 Command okay.\n");
        }

        else if(!strcmp(command, "MKD"))
        {
            recv(sock2, buffer, 100, 0);

            // check if directory is created or not
            if (stat(buffer, &st) == -1) {

                // make new directory
                mkdir(buffer, 0777);
                
                printf("257 %s created.\n", buffer);
            }
            else {
                printf("[-]Unable to create directory\n");
                exit(1);
            }
            send(sock2, buffer, 100, 0);
            printf("[+]200 Command okay.\n");
        }
        else if(!strcmp(command, "RMD"))
        {
            recv(sock2, buffer, 100, 0);

            // check if directory is created or not
            if (stat(buffer, &st) == 1) {
                printf("[-]Directory Does Not EXIST\n");
            }
            else {
                remove(buffer);
                printf("[+]Successfully remove directory\n");
            }
            send(sock2, buffer, 100, 0);
            printf("[+]200 Command okay.\n");
        }

        else if(!strcmp(command, "DELE"))
        {
            recv(sock2, buffer, 100, 0);

            // check if directory is created or not
            if (stat(buffer, &st) == 1) {
                printf("[-]Directory Does Not EXIST\n");
            }
            else {
                remove(buffer);
                printf("[+]Successfully remove file\n");
            }
            send(sock2, buffer, 100, 0);
            printf("[+]200 Command okay.\n");
        }

        else if(!strcmp(command, "LIST"))
        {
            system("ls > LIST.txt");
            i = 0;
            FILE*f = fopen("LIST.txt","r");
            while(!feof(f)){
                buffer[i++] = fgetc(f);
            }
            buffer[i-1] = '\0';
            fclose(f);
            send(sock2, buffer, 1024, 0);
            printf("[+]200 Command okay.\n");
        }
        /* Quit, kill child process, close connection and break from loop */
        else if(!strcmp(command, "BYE") || !strcmp(command, "QUIT"))
        {
            printf("Closing client data connection.\n");
            i = 1;
            send(sock2, &i, sizeof(int), 0);
            close(sock2); 
            kill(child_pid, SIGKILL);
            break;
        }

        else{
            printf("Command not implemented.\n");
        }
    }
}

int main(int argc,char *argv[])
{   
    FILE *fptr;
	if((sockRet = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("\n------Socket Creation Failed------\n");
        exit(1);
    }

	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;

	// Setting port number
	server_addr.sin_port = htons(PORT); //In case of Binding Error change port number ranging from 1024 to 64000

	// setting IP address
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// Binding the socket
	k = bind(sockRet,(struct sockaddr*)&server_addr, sizeof(server_addr));
	if(k == -1) {
		printf("[-]Binding error"); //[-] Shows Negative Approach 
		exit(1);
	}

	// Listen the connections
	k = listen(sockRet,10); //At a time socket are open for 10 Connection 
	if(k == -1){
		printf("[-]Listen failed");
		exit(1);
	}

	while(1){
		printf("[+]Waiting for incoming connections\n");
		clientLen = sizeof(client);

		// Accept the incoming connections
		sock2 = accept(sockRet,(struct sockaddr*)&client, &clientLen);

        // Displaying information of connected client
        printf("Connection accepted from %s:%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));

        // Print number of clients connected till now
        printf("Clients connected: %d\n\n",++cnt);

        //Print the count of server A's clients into 
        //Open file for communication
        fptr = fopen("client/serverBComm.txt", "w+");
        char buf[10];
        sprintf(buf, "%d", cnt);
        fwrite(buf, strlen(buf), 1, fptr);
        fclose(fptr);

		i = 1;

        //Forking and child execution
        if ((childpid = fork()) == -1) {
            close(sock2);            
        } else if(childpid > 0) {
            close(sock2);
            continue;
        } else if(childpid == 0) {
            printf("\nIn the child process\n");
            pid_t child_pid = getpid(); 
            ServiceClient(command, &child_pid);
        }
	}
	return 0;
}
