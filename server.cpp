#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

using namespace std;
unsigned int microseconds;

int board[9];
int player = 1, playerChoice;//Variabile player necessaria per l'algoritmo minimax, playerChoice scelta mossa client

// #################################################################### FUNZIONI ####################################################################

int win(const int board[9]);//Cointrollo progresso partita -> Vinto, perso, pareggio e in progresso
void draw_board(int b[9]);//Stampa campo
char gridChar(int i);//Funzione utilizzata per la gestione della visualizzazzione del campo
int minimax(int board[9], int player);//Algoritmo per l'AI del server
int computerMove(int board[9]);//Attraverso l'algoritmo minimax calcola la mossa migliore e la posiziona sul campo
int playerMove(int board[9], int playerChoice,int newsockfd, char ritorno[]);//Arrivo mossa client e posizionamento sul campo
void reset_field(int board[9]);

int main(int argc, char **argv)
{
    int n, k;
    int newsockfd, portno, clilen;
    char buffer [8]; // Buffer utilizzato per contenere le informazioni che arrivano dal client
    int gm_stat; // Controllo partita
	struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
	
/*********************************************
	-1 CLIENT WON
	1 SERVER WON
	2 GAME IN PROGRESS
	0 DRAW
**********************************************/
 
    if (argc < 2) {
        cout<<"Errore,numero di porta richiesto\n";
        exit(1);
    }
    newsockfd = socket(AF_INET, SOCK_STREAM, 0); // DOMINIO, TIPO DI CONNESSIONE, OPZIONI

	if (newsockfd < 0) {
        cout<<"Errore di apertura del socket\n";
		exit(-1);
	}
    
    memset(&serv_addr,0,sizeof(serv_addr)); //azzero la struttura serv_addr
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;

    inet_aton("0.0.0.0",&(serv_addr.sin_addr)); // inet_aton() converts the Internet host address from the IPv4 numbers-and-dots notation into binary form (in network byte order).
    serv_addr.sin_port = htons(portno); // The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.

    if (bind(newsockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){ // Assigns serv_addr to socket. The n byte is specified.
		cout<<"Errore di collegamento\n";
		exit(-1);
	}

    cout << "Waiting for the client...\n";

    listen(newsockfd,1); // listen() marks the socket referred to by sockfd as a passive socket, that is, as a socket that will be used to accept incoming connection requests.

    clilen = sizeof(cli_addr);

    newsockfd = accept(newsockfd,(struct sockaddr *) &cli_addr,(socklen_t *)&clilen); // It extracts the first connection request on the queue of
																					  // pending connections for the listening socket,
																					  // sockfd, creates a new connected socket.
	if (newsockfd < 0){ //Controllo correttezza socket
        cout<<"Errore su accept()\n";
		exit(-1);
	}

	char ritorno [8];	//Buffer utilizzato per ritornare al client le informazioni necessarie (mossa server, game status)
	do{
		reset_field(board);
		do
		{
			memset(buffer, 0, strlen(buffer)); //Azzero il buffer
			memset(ritorno, 0, strlen(ritorno)); //Azzero il ritorno
	
			draw_board(board);
	  
			do{
				n = read(newsockfd,buffer,1);	//buffer = char --->  lettura	 
				if (n < 0)
					 cout<<"Errore di lettura dal socket\n";
				else
				{
					//Buffer[0] --> Scelta client
					playerChoice = buffer[0] - 48;//Conversione in int		
					k = playerMove(board, playerChoice,newsockfd,ritorno);
				}
			}while(k != 0);

			ritorno[0] = computerMove(board);		//Turno server
			// Ritorno la mossa segnata dal server al client affinche anche il campo del client si aggiorni
	
			gm_stat = win(board);		//Controllo situazione game
			  
	
			ritorno[1] = gm_stat; //Ritorno stato game
			 
			n = write(newsockfd,ritorno,2); //Manda al client lo stato della game
				  
			if (n < 0) 				// ---------------------------> write
				cout<<"Errore di scrittura nel socket.\n";

		}while(win(board) == 2);
	
		draw_board(board);
		
		//Stampa la situazione finale del game
		switch(win(board)) {
		case 0:
			printf("Draw.\n");	
			break;
		case 1:
			printf("Server won.\n");
			break;
		case -1:
			printf("Client Won.\n");
			break;
		}
		memset(buffer, 0, strlen(buffer));
		n = read(newsockfd,buffer,3);
		if(n < 0)
			cout << "Errore di lettura dal socket\n";
		cout << "Client : " << buffer << endl;
	}while(strcmp(buffer,"200") == 0); // while client says play again
	
	close(newsockfd);//End connection

	return 0;
}

// ########################################################################################################################################
//																FUNZIONI 
// ########################################################################################################################################

/*******************************************************************
    CHECK WIN
********************************************************************/

int win(const int board[9]) {
 
    unsigned wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};//Tutte le possibile combinazioni per vincere
    int i;
    
    for(i = 0; i < 8; ++i) {//8 possibili combinazioni totali
        if(board[wins[i][0]] != 0 && board[wins[i][0]] == board[wins[i][1]] && board[wins[i][0]] == board[wins[i][2]])
            return board[wins[i][2]];//Server won (returns 1) or Client won (returns -1)
    }
	
	// se tutte le celle sono occupate e tutte != 0
	if (board[1] != 0 && board[2] != 0 && board[3] != 0 && board[4] != 0 && board[5] != 0 && board[6] != 0 && board[7] != 0 && board[8] != 0 && board[9] != 0)	
		return 0;//Game ended with a draw
	else
		return 2;//Game in progress
}

/*******************************************************************
    DRAW BOARD
********************************************************************/

void draw_board(int b[9]){
	
	system("clear");
	
	cout << "Client-player (X)  -  Server (O)" << endl << endl;
	cout << endl;

	cout << "     |     |     " << endl;
	cout << "  " << gridChar(b[0]) << "  |  " << gridChar(b[1]) << "  |  " << gridChar(b[2]) << endl;

	cout << "_____|_____|_____" << endl;
	cout << "     |     |     " << endl;

	cout << "  " << gridChar(b[3]) << "  |  " << gridChar(b[4]) << "  |  " << gridChar(b[5]) << endl;

	cout << "_____|_____|_____" << endl;
	cout << "     |     |     " << endl;

	cout << "  " << gridChar(b[6]) << "  |  " << gridChar(b[7]) << "  |  " << gridChar(b[8]) << endl;

	cout << "     |     |     " << endl << endl;
}

/*******************************************************************
    PLAYER MOVE
********************************************************************/

int playerMove(int board[9], int playerChoice, int newsockfd, char ritorno[]) {
	
	int move = playerChoice;//Mossa client - 1 per facilitare l'input del client -> (1-9 e non 0-8)
	int n;
	
	if(board[move] != 0) {//Controlla che la casella non sia gia occupata
		printf("It's already occupied!");
		strcpy(ritorno,"500");
		n = write(newsockfd,ritorno,3);
		if(n < 0)
			cout << "Errore scrittura\n" ;
		return -1;
	}
	else{
		strcpy(ritorno,"400");
		n = write(newsockfd,ritorno,3);
		if(n < 0)
			cout << "Errore scrittura\n" ;
		printf("\n");
		board[move] = -1; // Mark
		return 0;
	}
}

/*******************************************************************
    COMPUTER MOVE
********************************************************************/
int computerMove(int board[9]) {

    int move = -1;//-1 = Computer move
    int score = -2;//Losing moves is preferred to no move
    int i;
    for(i = 0; i < 9; ++i) {//For all moves,
        if(board[i] == 0) {//If legal,
            board[i] = 1;//Try the move
            int tempScore = -minimax(board, -1); //-1 = computer
            board[i] = 0;//Reset board after an attempt	
            if(tempScore > score) {//Calcola la mossa peggiore per il client provando varie mosse e resettando ogni volta il campo
                score = tempScore;
                move = i;
            }
        }
    }
    //returns a score based on minimax tree at a given node.
    board[move] = 1; //Mark
    return move; //Returns the move because we need to send it to the client too
}

/*******************************************************************
    AI ALGORITHM

L'algoritmo minimax assegna un valore ad ogni mossa legale, proporzionale a quanto essa diminuisce il valore della posizione per l'altro giocatore. 
Il minimax, nella teoria delle decisioni, e un metodo per minimizzare la massima (minimax) perdita possibile;
in alternativa, per massimizzare il minimo guadagno (maximin). - Wikipedia

********************************************************************/

int minimax(int board[9], int player) {
    // How is the position like for player (their turn) on board?
    int winner = win(board);
    if(winner != 0 && winner != 2) 
		return winner*player; // Checks game status 

    int move = -1;
    int score = -2; // Worst possible outcome
    int i;
    for(i = 0; i < 9; ++i) { // For every move
        if(board[i] == 0) { // If legal
            board[i] = player;// Try the move
            int thisScore = -minimax(board, player*-1);
            if(thisScore > score) {
                score = thisScore;
                move = i;
            } // Pick the one that's worst for the opponent
            board[i] = 0; // Reset board after try
        }
    }
    if(move == -1) return 0;
    return score;
}
/*******************************************************************
    BOARD CHARS
********************************************************************/

char gridChar(int i) {
    switch(i) {
        case -1:
            return 'X';
        case 0:
            return ' ';
        case 1:
            return 'O';
    }
}

/*******************************************************************
    RESET FIELD
********************************************************************/
void reset_field(int board[9]){

	for(int i = 0; i < 9;i++){
		board[i] = 0;
	}
}
/*******************************************************************
	END OF PROJECT
********************************************************************/

