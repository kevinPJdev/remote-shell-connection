/*
 * Authors
 * Kevin Patrick Jacob
 * Harmanjot Singh Suri
 */

#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*for getting file size using stat()*/
#include<sys/stat.h>

/*for O_RDONLY*/
#include<fcntl.h>

#define COMMAND(cmd) strcmp(command, cmd)==0

#define PORT1 5098
#define PORT2 5099

int main(int argc,char *argv[])
{
	char choice[256], arg[256]; //Variable choice for handling user choice
	FILE *fd, *fptr1, *fptr2; //File Descriptor
	char buf[100], command[5], filename[20], *f; //Character Arrays
	int k, size, status;
	int filehandle;
	int count_serverA, count_serverB; // keep count of existing connections
	int portCondA = 0; //defines whether client connects to Server A or Server B

	//Read the number of connections from server A
	fptr1 = fopen("serverAComm.txt", "r");
	fscanf(fptr1, "%d", &count_serverA);
	count_serverA++;
	printf("There are currently %d connections to server A\n", count_serverA);
	fclose(fptr1);

	//Read the number of connections from server B
	fptr2 = fopen("serverBComm.txt", "r");
	fscanf(fptr2, "%d", &count_serverB);
	count_serverB++;
	printf("There are currently %d connections to server B\n", count_serverB);
	fclose(fptr2);

	//Load balancer - First 5 clients connect to server A, next 5 connect to server B and then alternate. 
	if(count_serverA <= 5) {
		portCondA = 1; //connect to A
	} else if(count_serverA > 5 && count_serverB <= 5) {
		portCondA = 0; //connect to B
	} else if((count_serverA+count_serverB)>10 && ((count_serverA+count_serverB) %2 == 0)) {
		portCondA = 0; //connect to B if even and above 10
	} else {
		portCondA = 1; //connect to A
	}


	// TCP Protocol
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		printf("socket creation failed\n");
		exit(1);
	}
	printf("socket creation successfully\n");

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;

	// set server IP address
	server_addr.sin_addr.s_addr = INADDR_ANY; // Server IP Address

	// set server port number
	server_addr.sin_port = htons(portCondA == 1? PORT1 : PORT2); //Server Port

	int e = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)); //Create Connection
	if(e == -1){
        perror("Error in Connecting");
        exit(1);
    }
    printf("-------Connected to server %c---------\n", portCondA == 1 ? 'A': 'B');
	int i = 1;

	while(1)
	{
		/*==============================================DISPLAY-MENU===============================================*/
		printf("\nEnter a choice :\n1- RETR\n2- STOR\n7- PWD\n8- MKD\n9- RMD\n10- LIST\n11- CWD\n12- USER\n15- DELE\n16- QUIT\n17- CDUP\n18- REIN\n19- REST\n20- PORT\n21- STAT\n");
		scanf("%s", choice);

		if(strcmp(choice, "USER") == 0){

			// get from system
			int name = system("whoami");
			printf("User name ok.\n");
		}

		else if(strcmp(choice, "STOR") == 0){
			printf("Enter filename to STOR to server: ");

			// Get the input from user
			scanf("%s", filename);

			// Open file in read only mode
			filehandle = open(filename, O_RDONLY);

			// check file is exist or not
			if(filehandle == -1)
			{
				printf("No such file on the local directory\n\n");
				//break;
			}
		}
		/* Get the present working directory */
		else if(strcmp(choice, "PWD") == 0){

			// Copy the command
			strcpy(buf, "PWD");

			// send command to server
			send(sock, buf, 100, 0);

			// get the working directory
			recv(sock, buf, 100, 0);
			printf("The path of the remote directory is: %s\n", buf);
		}
		/* Make a new directory */
		else if(strcmp(choice, "MKD") == 0){

			// Copy the command
			strcpy(buf, "MKD");

			// send command to server
			send(sock, buf, 100, 0);

			printf("Enter the directory name to create SERVER side: ");

			// get input from user
			scanf("%s", filename);
			send(sock, filename, 100, 0);

			bzero(buf, sizeof(buf));

			// Receive what created
			recv(sock, buf, 100, 0);
			printf("The path of the remote directory is: %s\n", buf);
		}
		/* Remove a directory */
		else if(strcmp(choice, "RMD") == 0){
			strcpy(buf, "RMD");
			send(sock, buf, 100, 0);

			printf("Enter the directory name to delete on SERVER side: ");
			scanf("%s", filename);
			send(sock, filename, 100, 0);

			bzero(buf, sizeof(buf));
			recv(sock, buf, 100, 0);
			printf("Directory Removed Successfully\n");
		}
		/* Delete a file from server */
		else if(strcmp(choice, "DELE") == 0){
			strcpy(buf, "DELE");
			send(sock, buf, 100, 0);

			printf("Enter the file name to delete on SERVER side: ");
			scanf("%s", filename);
			send(sock, filename, 100, 0);

			bzero(buf, sizeof(buf));
			recv(sock, buf, 100, 0);
			printf("[+]File Removed Successfully\n");
		}
		/* List all files in the PWD*/
		else if(strcmp(choice, "LIST") == 0){
			strcpy(buf, "LIST");
			send(sock, buf, 100, 0);
			recv(sock, buf, 1024, 0);
			printf("The remote directory listing is as follows:\n %s\n\n", buf);
			//system();
		}
		/* Change the current working directory */
		else if(strcmp(choice, "CWD") == 0){
			strcpy(buf, "CD ");
			printf("Enter the path to change the remote directory: ");
			scanf("%s", buf + 3);
			send(sock, buf, 100, 0);
			recv(sock, &status, sizeof(int), 0);
			if(status)
				printf("Remote directory successfully changed\n");
			else
				printf("Remote directory failed to change\n");
		}
		/* Quit and close the connection */
		else if(strcmp(choice, "QUIT") == 0){
			strcpy(buf, "QUIT");
			send(sock, buf, 100, 0);
			recv(sock, &status, 100, 0);
			if(status){
				printf("Connection closed\nQuitting..\n");
				printf("Terminated Successfully...\nGood Bye...\n");
				close(sock);
				exit(0);
			}
			printf("[-]Server failed to close connection\n");
		}
		/* Command does not exist */
		else
		{
			printf("Command is not implemented\n Try another commands\n");
		}
	}	
	return 0;
}
