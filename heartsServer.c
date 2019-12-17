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

#define MAXPOINT 20

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
	bool isP1First = false;
	bool waiting = false;

	int whoseTurn = 2;
	int firstPlayer = 1;	
	int turnCount = 1;
	int bigTurnCount = 0;
	int winner = 0;

	int cards[52];
	int p1Cards[13];
	int p2Cards[13];
	int p3Cards[13];
	int p4Cards[13];

	int p1PickedCard = 0;
	int p2PickedCard = 0;
	int p3PickedCard = 0;
	int p4PickedCard = 0;
	int pickCount = 0;

	//STRENGTHS
	int s1 = 0;
	int s2 = 0;
	int s3 = 0;
	int s4 = 0;

	//POINTS
	int p1Points = 0;
	int p2Points = 0;
	int p3Points = 0;
	int p4Points = 0;
	int tPoint = 0;

bool include(int rCards[],int r);
void shufflingCards();
void whoseFirst();
void sendMessage(char msg[], int client);
void receiveMessage(int client, int turn);
void compareMessage(char * receivedMsg, int turn);
void buildCardsMessage(int player);
void buildTurnMessage(int player,int p1Pick, int p2Pick, int p3Pick, int p4Pick, int p1p, int p2p, int p3p, int p4p);
void pickCard(int player);
void calculatePoints();
int bestStrength();
void resetTurn();
void resetGame();
void getWinner();

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
	//whoseTurn = firstPlayer;

	if (firstPlayer == 2)
	{
		for (int i = 0; i < 13; i++)
		{
			if(p2Cards[i] == 41)
			{
				p2Cards[i] = p1Cards[i];
				p1Cards[i] = 41;
			}
		}		
	}
	if (firstPlayer == 3)
	{
		for (int i = 0; i < 13; i++)
		{
			if(p3Cards[i] == 41)
			{
				p3Cards[i] = p1Cards[i];
				p1Cards[i] = 41;
			}
		}		
	}
	if (firstPlayer == 4)
	{
		for (int i = 0; i < 13; i++)
		{
			if(p4Cards[i] == 41)
			{
				p4Cards[i] = p1Cards[i];
				p1Cards[i] = 41;
			}
		}		
	}
	firstPlayer = 1;
	whoseTurn = 1;
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
	memset(recMessage, 0, BUFFERSIZE+1);
	strcpy(recMessage, "");
 	rcvsize = recv(client, recMessage, bytes, 0);
	 
	if (rcvsize >= 0)
	{
        compareMessage(recMessage, turn);
    }
	else
	{		
		printf("[!]Error in \"receiveMessage()\"!\n");		
	}
}

void buildCardsMessage(int player)
{
	int* msgCards;
	if(player == 1)
	{
		msgCards = p1Cards;
		sprintf(message,"%s.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d","C1",msgCards[0],msgCards[1],msgCards[2],msgCards[3],msgCards[4],msgCards[5],msgCards[6],msgCards[7],msgCards[8],msgCards[9],msgCards[10],msgCards[11],msgCards[12]);
		printf("%s.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d","C1",msgCards[0],msgCards[1],msgCards[2],msgCards[3],msgCards[4],msgCards[5],msgCards[6],msgCards[7],msgCards[8],msgCards[9],msgCards[10],msgCards[11],msgCards[12]);
	}
	else if(player == 2)
	{
		msgCards = p2Cards;
		sprintf(message,"%s.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d","C2",msgCards[0],msgCards[1],msgCards[2],msgCards[3],msgCards[4],msgCards[5],msgCards[6],msgCards[7],msgCards[8],msgCards[9],msgCards[10],msgCards[11],msgCards[12]);
		printf("%s.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d","C2",msgCards[0],msgCards[1],msgCards[2],msgCards[3],msgCards[4],msgCards[5],msgCards[6],msgCards[7],msgCards[8],msgCards[9],msgCards[10],msgCards[11],msgCards[12]);
	}	
}

void buildTurnMessage(int player, int p1Pick, int p2Pick, int p3Pick, int p4Pick, int p1p, int p2p, int p3p, int p4p)
{	
	if(player == 1)
	{
		sprintf(message,"%s.%d.%d.%d.%d.%d.%d.%d","T1",p2Pick,p3Pick,p4Pick,p1p,p2p,p3p,p4p);
		printf("       %s.%d.%d.%d\n","T1",p2Pick,p3Pick,p4Pick);
	}
	else if(player == 2)
	{
		sprintf(message,"%s.%d.%d.%d.%d.%d.%d.%d","T2",p1Pick,p3Pick,p4Pick,p1p,p2p,p3p,p4p);
		printf("       %s.%d.%d.%d\n","T2",p1Pick,p3Pick,p4Pick);
	}	
}

void compareMessage(char * receivedMsg, int turn)
{
	if (strncmp(receivedMsg, "START",5) == 0 && !isStarted)
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
		printf("küldés1nek\n");
		sendMessage(message,clientSocket);				
		buildCardsMessage(2);
		printf("küldés2nek\n");
		sendMessage(message,clientSocket2);		

		isStarted = true;		
		
	}	
	else if(strncmp(receivedMsg, "START",5) == 0 )
	{
		buildCardsMessage(2);
		printf("__küldés2nek\n");
		sendMessage(message,clientSocket2);		
		buildCardsMessage(1);
		printf("__küldés1nek\n");
		sendMessage(message,clientSocket);		
	}
	else if(strcmp(receivedMsg, "FELADOM") == 0)
	{
		if(whoseTurn == 1)
		{
			printf(" [-]Az első játékos feladta.\n");
			sendMessage("UJRA",clientSocket2);		
		}
		else
		{
			printf(" [-]A második játékos feladta.\n");	
			sendMessage("UJRA",clientSocket);	
		}
		resetGame();		
	}
	else if(strcmp(receivedMsg, "UJRA") == 0)
	{
		if(whoseTurn == 1)
		{
			printf(" [-]Az első játékos újrakezdené.\n");	
			sendMessage("UJRA",clientSocket2);				
		}
		else
		{
			printf(" [-]A második játékos újrakezdené.\n");	
			sendMessage("UJRA",clientSocket);				
		}
		waiting = false;			
	}
	else if(strcmp(receivedMsg, "VEGE") == 0)
	{
		if(whoseTurn == 1)
		{
			printf(" [-]Az első játékos kilépett.\n");		
			sendMessage("VEGE",clientSocket2);	
		}
		else
		{
			printf(" [-]A második játékos kilépett.\n");	
			sendMessage("VEGE",clientSocket);	
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
			printf("P1P:%s\n",token);
			p1PickedCard = atoi(token);
			buildTurnMessage(2,p1PickedCard,p2PickedCard,p3PickedCard,p4PickedCard,p1Points,p2Points,p3Points,p4Points);
			sendMessage(message,clientSocket2);
			whoseTurn = 2;

			if (firstPlayer != 2)
			{
				pickCount++;
			}			
		}
		else if(strcmp(token,"P2P") == 0)
		{
			token = strtok(NULL,".");
			printf("P2P:%s\n",token);
			p2PickedCard = atoi(token);
			whoseTurn = 3;

			if (firstPlayer == 2)
			{
				pickCount++;
			}
			turnCount++;
		}								
	}		
}

void calculatePoints()
{	
	tPoint = 0;

	//P1
	if (p1PickedCard == 1 || p1PickedCard == 14 || p1PickedCard == 27 || p1PickedCard == 40)
		{ s1 = 13;}
	else{
		if (p1PickedCard > 14 && p1PickedCard < 27)
			{ s1 = p1PickedCard - 13;
				tPoint++;}
		else if(p1PickedCard > 26 && p1PickedCard < 40){
				s1 = p1PickedCard - 26;
				if (p1PickedCard == 38)
					{ tPoint += 13;}
			}
		else if(p1PickedCard > 39)
			{ s1 = p1PickedCard - 39;}		
	}

	//P2
	if (p2PickedCard == 1 || p2PickedCard == 14 || p2PickedCard == 27 || p2PickedCard == 40)
		{ s2 = 13;}
	else{
		if (p2PickedCard > 14 && p2PickedCard < 27)
			{ s2 = p2PickedCard - 13;
				tPoint++;}
		else if(p2PickedCard > 26 && p2PickedCard < 40){ 
				s2 = p2PickedCard - 26;
				if (p2PickedCard == 38)
					{ tPoint += 13;}
			}
		else if(p2PickedCard > 39)
			{ s2 = p2PickedCard - 39;}		
	}

	//P3
	if (p3PickedCard == 1 || p3PickedCard == 14 || p3PickedCard == 27 || p3PickedCard == 40)
		{ s3 = 13;}
	else{
		if (p3PickedCard > 14 && p3PickedCard < 27)
			{ s3 = p3PickedCard - 13;
				tPoint++;}
		else if(p3PickedCard > 26 && p3PickedCard < 40){ 
				s3 = p3PickedCard - 26;
				if (p3PickedCard == 38)
					{ tPoint += 13;}
			}
		else if(p3PickedCard > 39)
			{ s3 = p3PickedCard - 39;}		
	}

	//P4
	if (p4PickedCard == 1 || p4PickedCard == 14 || p4PickedCard == 27 || p4PickedCard == 40)
		{ s4 = 13;}
	else{
		if (p4PickedCard > 14 && p4PickedCard < 27)
			{ s4 = p4PickedCard - 13;
				tPoint++;}
		else if(p4PickedCard > 26 && p4PickedCard < 40){
				s4 = p4PickedCard - 26;
				if (p4PickedCard == 38)
					{ tPoint += 13;}
			}
		else if(p1PickedCard > 39)
			{ s4 = p4PickedCard - 39;}		
	}		
}

int bestStrength()
{
	int max = 0;

	//S1
	if (s1 >= max)	{
		max = s1;
	}
	//S2
	if (s2 >= max)	{
		max = s2;
	}
	//S3
	if (s3 >= max)	{
		max = s3;
	}
	//S4
	if (s4 >= max)	{
		max = s4;
	}

	if (max == s1){		
		p1Points += tPoint;
		return 1;
	}
	else if(max == s2){
		p2Points += tPoint;
		return 2;
	}
	else if(max == s3){
		p3Points += tPoint;
		return 3;
	}
	else if(max == s4){
		p4Points += tPoint;
		return 4;
	}	
}

void resetTurn()
{		
	if (winner != 0)
	{
		printf("****NYERTES: %d.játékos****",winner);

		if (winner == 1)
		{
			sendMessage("WIN.1",clientSocket);
			sendMessage("WIN.1",clientSocket2);
		}
		else if( winner == 2)
		{
			sendMessage("WIN.2",clientSocket);
			sendMessage("WIN.2",clientSocket2);
		}
		else if( winner == 3)
		{
			sendMessage("WIN.3",clientSocket);
			sendMessage("WIN.3",clientSocket2);
		}
		else if( winner == 4)
		{
			sendMessage("WIN.4",clientSocket);
			sendMessage("WIN.4",clientSocket2);
		}		
	}
	else
	{
		if (bigTurnCount >= 2)
		{
			buildTurnMessage(1,p1PickedCard,p2PickedCard,p3PickedCard,p4PickedCard,p1Points,p2Points,p3Points,p4Points);
			sendMessage(message,clientSocket);
		}
		
		turnCount = 1;
		pickCount = 0;
		isStarted = false;
		p1PickedCard = 0;
		p2PickedCard = 0;		
	}			
}

void resetGame()
{	
	turnCount = 1;
	pickCount = 0;
	isStarted = false;
	p1PickedCard = 0;
	p2PickedCard = 0;
	p3PickedCard = 0;
	p4PickedCard = 0;
	winner = 0;
}

void getWinner()
{
	int max = 0;
	int min = 500;

	if (p1Points >= MAXPOINT || p2Points >= MAXPOINT || p3Points >= MAXPOINT || p4Points >= MAXPOINT)
	{				
		if (p1Points >= max)	{
			max = p1Points;
		}
		if (p2Points >= max)	{
			max = p2Points;
		}
		if (p3Points >= max)	{
			max = p3Points;
		}
		if (p4Points >= max)	{
			max = p4Points;
		}

		if (p1Points <= min)	{
			min = p1Points;
		}
		if (p2Points <= min)	{
			min = p2Points;
		}
		if (p3Points <= min)	{
			min = p3Points;
		}
		if (p4Points <= min)	{
			min = p4Points;
		}
				
		if (min == p1Points)
		{
			winner = 1;
		}
		else if (min == p2Points)
		{
			winner = 2;
		}
		else if (min == p3Points)
		{
			winner = 3;
		}
		else if (min == p4Points)
		{
			winner = 4;
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
	
	GAME:while(!quitGame && !waiting)
	{
		if (whoseTurn == 1)
		{	
			//printf("Rec2:%s  \n",recMessage);	
			printf("[ReceiveP1:]\n");				
			receiveMessage(clientSocket, whoseTurn);			
			whoseTurn = 2;
		}
		else if (whoseTurn == 2)
		{	
			//printf("Rec1:%s  \n",recMessage);		
			printf("[ReceiveP2:]\n");						
			receiveMessage(clientSocket2, whoseTurn);
			whoseTurn = 4;
		}					
		else
		{
			printf("[SendP1:]\n");		
			pickCard(3);			
			pickCard(4);			
			printf("	[T] %d. kör: 1.:%d 2.:%d 3.:%d 4.:%d\n",turnCount-1, p1PickedCard, p2PickedCard, p3PickedCard, p4PickedCard);

			if (turnCount > 0)
			{
				calculatePoints();
				printf("		[S] %d. játékos kártyája az erősebb! 1.:%d 2.:%d 3.:%d 4.:%d\n",bestStrength(),s1,s2,s3,s4);
				printf("			[P] Kör: %d pont.\n",tPoint);	

				printf("%d\n",turnCount);
				if (turnCount == 14)
				{
					bigTurnCount++;
					printf("				[V]%d.kör eredménye: 1.:%d 2.:%d 3.:%d 4.:%d\n",bigTurnCount, p1Points, p2Points, p3Points, p4Points);
					printf("				[V]Új kör következik!\n");
					getWinner();
					resetTurn();					
					printf("		Új kör: 2.ra várok!\n");
					receiveMessage(clientSocket2, whoseTurn);					
				}		
				else if (turnCount != 14 && turnCount != 0)
				{
					buildTurnMessage(1,p1PickedCard,p2PickedCard,p3PickedCard,p4PickedCard,p1Points,p2Points,p3Points,p4Points);
					sendMessage(message,clientSocket);
					whoseTurn = 1;
				}					
			}											
		}
	}

	WAIT:while(waiting)
	{
		if (whoseTurn == 1)
		{
			receiveMessage(clientSocket, whoseTurn);
		}
		else
		{
			receiveMessage(clientSocket2, whoseTurn);
		}

		if (!waiting)
		{
			goto GAME;
		}		
	}

    printf("[+]Closing the connection.\n");

	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
	setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);

	close(clientSocket);
	close(serverSocket);	
}