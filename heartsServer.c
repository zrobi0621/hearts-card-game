#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFERSIZE 4000

//SERVER

void main(){

	int serverSocket;
	struct sockaddr_in serverAddr;

	int clientSocket;
	struct sockaddr_in clientAddr;	

	socklen_t addr_size;
	char message[BUFFERSIZE+1];
	int bytes = BUFFERSIZE;
	int rcvsize;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	printf("[+]Server Socket Created Sucessfully.\n");
	memset(&serverAddr, '\0', sizeof(serverAddr));

	//SERVER
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);	
	serverAddr.sin_addr.s_addr = inet_addr("192.168.56.101");

    //BIND
	bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	printf("[+]Bind to Port number %d.\n", PORT);
//P1
    //LISTEN
	listen(serverSocket, 5);
	printf("[+]Listening for P1\n");
    
	clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &addr_size);
	printf("[+]Connected to P1\n");

	//SEND
	strcpy(message, "HelloP1");
	send(clientSocket, message, strlen(message), 0);

//P2
	//LISTEN
	listen(serverSocket, 5);
	printf("[+]Listening for P2\n");
    
	clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &addr_size);
	printf("[+]Connected to P2\n");

	//SEND
	strcpy(message, "HelloP2");
	send(clientSocket, message, strlen(message), 0);

/*	//RECEIVE
 	rcvsize = recv(clientSocket, message, bytes, 0 );
	if (rcvsize < 0)
	{
        printf("Error\n");
    }
	else
	{		
		printf(" [-]Message received: %s\n", message);	
	}*/
	
    

	printf("[+]Closing the connection.\n");
	close(clientSocket);
	close(serverSocket);
}