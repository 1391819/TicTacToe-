#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

using namespace std;

void print_playing_field(int board[]);
void game(int board[], int &game_status, char buffer[],int sockfd);
int check_choice(char choice, int board[]);
void winner(int game_status);
void legenda();
char gridChar(int i);
void reset_field(int board[]);

int main(int argc, char **argv){
	
	int sockfd; // socket
	int port_no; // port number
	int n;	// temp var, controllo errori
	char server_IP[100];
	char buffer[8]; // buffer TX & RX
	struct sockaddr_in server_addr; // var di tipo sockaddr_in per il server
	struct hostent * server;
	
	// game related variables
	
	int board[9];
	char play;
	
	int status_code; // used to make sure the user only sends correct moves
	int game_status = 2; // the client doesn't worry about calculating who wins, the server takes care of that
	
	if(argc != 3){
		cout << "Errore nei parametri\n" ;
		exit(-1);
	}
	
	port_no = atoi(argv[2]); // ./file server_IP PORT_NO
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // 
	
	if(sockfd < 0){ // returns a value greater than 0 if everything is ok
		cout <<  "Errore apertura socket\n" ;
		exit(-1);
	}
	
	strncpy(server_IP,argv[1],20);
	
	if(server_IP == NULL){
		cout << "Errore : host non trovato\n" ;
		exit(-1);
	}
	
	memset(&server_addr,0,sizeof(server_addr)); //The memset() function fills the first n bytes of the memory area pointed to by s with the constant byte c.
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_IP); // The inet_addr() function converts the Internet host address from IPv4 numbers-and-dots notation into binary data in network byte order.
	
	server_addr.sin_port = htons(port_no); // converts the unsigned short integer hostshort from host byte order to network byte order.
	if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){ // connects the socket referred to by the file descriptor
		cout << "Errore di connessione\n" ;										//	sockfd to the address specified by addr. The addrlen argument specifies the size of addr.
		exit(-1);
	}
	
	legenda();
	sleep(3);
		
	do{
		reset_field(board);
		game(board,game_status,buffer,sockfd);
		winner(game_status);
		
		do{
			cout << "Do you want to play again ? Y or N\n" ;
			cin >> play;
		}while(play != 'Y' && play != 'N' && play != 'y' && play != 'n');
		
		memset(buffer,0,strlen(buffer));
		if(play == 'Y' || play == 'y')
			strcpy(buffer,"200"); // 200 Y
		else
			strcpy(buffer,"100"); // 100 N
		
		n = write(sockfd,buffer,strlen(buffer));
		if(n < 0){
			cout << "Errore di scrittura nel socket\n" ;	
		}
	}while(play == 'Y' || play == 'y');

	close(sockfd);
	
	return 0;
}

void winner(int game_status){

	// checks who won
    if(game_status == -1){
	 	cout << "The client won.\n"; 
		return;
	}
	if(game_status == 0){
		cout << "Draw.\n"; 
		return;
	}
	else{
		cout << "The server won.\n";
		return;
	}
}

int check_choice(char choice,int board[]){ // this functions only allows characters from 1 to 9 to go through
		
	int temp = choice - 48 - 1; // 0 - 8
	
	if(temp < 0 || temp > 8){
		return -1;
	}
	return 0;
}

void game(int board[], int &game_status,char buffer[],int sockfd){
	
	int status_code;
	char choice; // holds both AI and user's choices
	int n; // temp var, controllo errori
	int temp_index; // used to store converted char values

	do{
		print_playing_field(board);		
		
		do{ // input client's move
			cout << "Client's turn (1 - 9):\n" ;
			do{
				cin >> choice;
				status_code = check_choice(choice,board);
			}while(status_code == -1);
			temp_index = choice - 48 - 1;
			memset(buffer,0,strlen(buffer)); 
			buffer[0] = choice - 1 ; // uses 1 byte of the buffer to communicate his choice (0 - 8)
			n = write(sockfd,buffer,1); // writes n byte, from buffer to sockfd
			if(n < 0){
				cout << "Errore di scrittura nel socket\n" ;	
			}
			else{
				memset(buffer,0,strlen(buffer));
				n = read(sockfd,buffer,3);
				if(n < 0){
					cout << "Errore di lettura del socket\n" ;	
				}
			}
		}while(strcmp(buffer,"500") == 0); // until a valid move is entered
		board[temp_index] = -1; // client's mark
		memset(buffer,0,strlen(buffer));
		n = read(sockfd,buffer,2); // reads 2 bytes, the first being the AI's choice, and the second one the game's status
		if(n < 0)				  
			cout << "Errore lettura\n" ;
		else{
			game_status = buffer[1];
			temp_index = buffer[0];
			board[temp_index] = 1; // the AI does not choose randomly,therefore there's no need to check whether his move is correct or not
			memset(buffer,0,strlen(buffer));
		}
	}while(game_status == 2); 
	
	print_playing_field(board); // printing only happens at the beginning of the while loop,therefore in order to display the
								// game ending move, we have to call this function once more
}

void print_playing_field(int b[9]){

	system("clear");	

	cout << endl;
	
	cout << "Client (X)  vs  Server (O)" << endl << endl;
	
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

void legenda(){

	system("clear");
	cout << "\nTic Tac Toe\n\n";

	cout << "     |     |     " << endl;
	cout << "  " << "1" << "  |  " << "2" << "  |  " << "3" << endl;

	cout << "_____|_____|_____" << endl;
	cout << "     |     |     " << endl;

	cout << "  " << "4" << "  |  " << "5" << "  |  " << "6" << endl;

	cout << "_____|_____|_____" << endl;
	cout << "     |     |     " << endl;

	cout << "  " << "7" << "  |  " << "8" << "  |  " << "9" << endl;

	cout << "     |     |     " << endl << endl;

}

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

void reset_field(int board[9]){

	for(int i = 0; i < 9;i++){
		board[i] = 0;
	}
}
