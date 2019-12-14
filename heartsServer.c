#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFERSIZE 4000

	char message[BUFFERSIZE+1];
	char recMessage[BUFFERSIZE+1];
	int bytes = BUFFERSIZE;
	int rcvsize;

	//Server
	int serverSocket;
	struct sockaddr_in serverAddr;
			
	//Clients
	int clientSocket;
	struct sockaddr_in clientAddr;	
	int clientSocket2;
	struct sockaddr_in clientAddr2;	

	socklen_t addr_size;	
	
	bool quitGame = false;
	bool isStarted = false;		
	int whoseTurn = 2;
	int firstPlayer = 1;	

	int cards[52];
	int p1Cards[13];
	int p2Cards[13];
	int p3Cards[13];
	int p4Cards[13];

	int p1PickedCard = 0;
	int p2PickedCard = 0;
	int p3PickedCard = 0;
	int p4PickedCard = 0;

bool include(int rCards[],int r);
void shufflingCards();
void whoseFirst();
void sendMessage(char msg[], int client);
void receiveMessage(int client, int turn);
void compareMessage(char * receivedMsg, int turn);
void buildCardsMessage(int player);
void buildTurnMessage(int player,int p1Pick, int p2Pick, int p3Pick, int p4Pick);
void pickCard(int player);

bool include(int rCards[],int r)
{
	for (int i = 0; i < 52; i++)
	{
		if (rCards[i] == r)
		{
			return true;
		}						
	}
	return false;	
}

void shuffligCards()
{		
	for (int i = 0; i < 52; i++)
	{
		cards[i] = 0;
	}	

    for(int i = 0; i < 52; i++){
		RAND:;
		int r = (rand() % 52) + 1;

	   if (!include(cards,r))
	   {
			cards[i] = r;
	   }
	   else
	   {
		   goto RAND;
	   }	   	   	   	    
   }

   for(int i = 0; i < 13; i++)
   {
           p1Cards[i] = cards[i];
           p2Cards[i] = cards[i+13];
		   p3Cards[i] = cards[i+26];
		   p4Cards[i] = cards[i+39];
   }

   whoseFirst();
}

void whoseFirst()
{
	for (int i = 0; i < 52; i++)
	{
		//TREFF 2
		if (cards[i] == 41)
		{
			if (i < 13)
			{
				firstPlayer = 1;
			}
			else if(i > 12 && i < 26)
			{
				firstPlayer = 2;
			}
			else if(i > 25 && i < 39)
			{
				firstPlayer = 3;
			}
			else if(i > 38)
			{
				firstPlayer = 4;
			}						
		}		
	}	
	whoseTurn = firstPlayer;
}

void pickCard(int player)
{
	int r = rand() % 13;

	if (player == 1)
	{
		p1PickedCard = p1Cards[r];	
	}
	else if(player == 2)
	{
		p2PickedCard = p2Cards[r];	
	}
	else if(player == 3)
	{
		p3PickedCard = p3Cards[r];	
	}
	else if(player == 4)
	{
		p4PickedCard = p4Cards[r];	
	}	
}

void sendMessage(char msg[], int client)
{
	strcpy(message, msg);
	send(client, message, strlen(message), 0);
}

void receiveMessage(int client, int turn)
{			
	strcpy(recMessage, "");
 	rcvsize = recv(client, recMessage, bytes, 0);
	 
	if (rcvsize >= 0)
	{
        compareMessage(recMessage, turn);
    }
	else
	{		
		printf("[!]Error in \"receiveMessage()\" from %d\n", &client);		
	}
}

void buildCardsMessage(int player)
{
	int* msgCards;
	if(player == 1)
	{
		msgCards = p1Cards;
		sprintf(message,"%s.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d","C1",msgCards[0],msgCards[1],msgCards[2],msgCards[3],msgCards[4],msgCards[5],msgCards[6],msgCards[7],msgCards[8],msgCards[9],msgCards[10],msgCards[11],msgCards[12]);
	}
	else if(player == 2)
	{
		msgCards = p2Cards;
		sprintf(message,"%s.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d","C2",msgCards[0],msgCards[1],msgCards[2],msgCards[3],msgCards[4],msgCards[5],msgCards[6],msgCards[7],msgCards[8],msgCards[9],msgCards[10],msgCards[11],msgCards[12]);
	}	
}

void buildTurnMessage(int player, int p1Pick, int p2Pick, int p3Pick, int p4Pick)
{	
	if(player == 1)
	{
		sprintf(message,"%s.%d.%d.%d","T1",p2Pick,p3Pick,p4Pick);
		printf("       %s.%d.%d.%d","T1",p2Pick,p3Pick,p4Pick);
	}
	else if(player == 2)
	{
		sprintf(message,"%s.%d.%d.%d","T2",p1Pick,p3Pick,p4Pick);
		printf("       %s.%d.%d.%d","T2",p1Pick,p3Pick,p4Pick);
	}	
}

void compareMessage(char * receivedMsg, int turn)
{
	if (strcmp(receivedMsg, "START") == 0)
    {
		memset(recMessage, 0, BUFFERSIZE+1);
		shuffligCards();		

		printf(" [-]A játék elkezdődött.\n");			
				
		printf("  [-]1. játékos(P1) kártyái: ");
		for (int i = 0; i < 13; i++)
		{
			printf("%d ",p1Cards[i]);
		}		
		printf("\n");
		printf("  [-]2. játékos(P2) kártyái: ");
		for (int i = 0; i < 13; i++)
		{
			printf("%d ",p2Cards[i]);
		}		
		printf("\n");
		printf("  [-]3. játékos kártyái: ");
		for (int i = 0; i < 13; i++)
		{
			printf("%d ",p3Cards[i]);
		}		
		printf("\n");
		printf("  [-]4. játékos kártyái: ");
		for (int i = 0; i < 13; i++)
		{
			printf("%d ",p4Cards[i]);
		}		
		printf("\n");					
		printf("   [-]A(z) %d. játékos kezd!\n", firstPlayer);

		buildCardsMessage(1);
		sendMessage(message,clientSocket);
		buildCardsMessage(2);
		sendMessage(message,clientSocket2);		

		if (whoseTurn == 3 || whoseTurn == 4)
		{		
			pickCard(3);
			pickCard(4);
			whoseTurn = 4;
		}

	/*else if(whoseTurn == 1)
		{
			//waitForP1Pick
			//SendP2 P1Pick
		}
		else if(whoseTurn == 2)
		{
			//waitForP2Pick
			//SendP1 P2Pick,P3Pick,P4Pick
		}*/		
	}
	else if(strcmp(receivedMsg, "FELADOM") == 0)
	{
		if(turn == 1)
		{
			printf(" [-]Az első játékos feladta.\n");	
		}
		else
		{
			printf(" [-]A második játékos feladta.\n");	
		}
	}
	else if(strcmp(receivedMsg, "UJRA") == 0)
	{
		if(turn == 1)
		{
			printf(" [-]Az első játékos újrakezdené.\n");	
		}
		else
		{
			printf(" [-]A második játékos újrakezdené.\n");	
		}
	}
	else if(strcmp(receivedMsg, "VEGE") == 0)
	{
		if(whoseTurn == 1)
		{
			printf(" [-]Az első játékos kilépett.\n");			
		}
		else
		{
			printf(" [-]A második játékos kilépett.\n");	
		}

		quitGame = true;

		printf(" [-]A játék véget ért.\n");
		printf("[+]Closing the connection.\n");

		close(clientSocket);
		close(serverSocket);
	}
	else
	{
		char *token;
		token = strtok(receivedMsg,".");
		
		if (strcmp(token,"P1P") == 0)
		{
			token = strtok(NULL,".");
			printf("P1P: %s",token);
			whoseTurn = 2;
		}
		else if(strcmp(token,"P2P") == 0)
		{
			token = strtok(NULL,".");
			printf("P2P: %s",token);
			whoseTurn = 3;
		}								
	}	
}

void main(){

	srand(time(NULL));
	char on = 1;	

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	printf("[+]Server Socket Created Sucessfully.\n");
	memset(&serverAddr, '\0', sizeof(serverAddr));

	//SERVER
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);	
	serverAddr.sin_addr.s_addr = inet_addr("192.168.56.101");	
    
	bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	printf("[+]Bind to Port number %d.\n", PORT);

	//P1    
	listen(serverSocket, 5);
	printf("[+]Listening for P1\n");    
	clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &addr_size);
	printf("[+]Connected to P1\n");		
	sendMessage("HelloP1", clientSocket);

	//P2	
	listen(serverSocket, 5);
	printf("[+]Listening for P2\n");    
	clientSocket2 = accept(serverSocket, (struct sockaddr *) &clientAddr2, &addr_size);
	printf("[+]Connected to P2\n");	
	sendMessage("HelloP2", clientSocket2);
	
	while(!quitGame)
	{
		if (whoseTurn == 1)
		{
			printf("[ReceiveP1]\n");
			printf("%s",recMessage);
			receiveMessage(clientSocket, whoseTurn);
		}
		else if (whoseTurn == 2)
		{
			printf("[ReceiveP2]\n");
			printf("%s",recMessage);
			receiveMessage(clientSocket2, whoseTurn);
		}					
		else if (whoseTurn == 4)
		{
			printf("[SendP1]\n");
			buildTurnMessage(1,p1PickedCard,p2PickedCard,p3PickedCard,p4PickedCard);
			sendMessage(message,clientSocket);
			whoseTurn = 1;
		}
	}

    printf("[+]Closing the connection.\n");

	close(clientSocket);
	close(serverSocket);
}