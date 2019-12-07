#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define SIZE 150000


void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int chartoint(char start){
	int i;
	char alpha[28] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' ','\0'};

	for(i = 0; i < 27; i++){
		if(start == alpha[i]) return i;
	}

	return 0;
}

char inttochar(int start){
	char alpha[28] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' ','\0'};

	return alpha[start];
}

void encrypt( char msg[], char key[]){
	int i, temp;


	for(i = 0; i < strlen(msg) - 1; i++) {
		temp = (chartoint(msg[i]) - chartoint(key[i])) % 27;

		if(temp < 0) temp += 27;
		msg[i] = inttochar(temp);
	}


}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo, i, j;
	char buffer[SIZE];
	char key[SIZE];
	char msg[SIZE];
	char status[32];
	struct sockaddr_in serverAddress, clientAddress;
	pid_t pid;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	while(1){
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");

		//forking for concurrency

		pid_t spawnPid = -5;

		spawnPid = fork();

		switch(spawnPid) {
			//error case
			case -1:
				perror("Something went very wrong.\n");
				exit(1);
				break;
			case 0:
				// Get the message from the client and display it
				memset(buffer, '\0', SIZE);

				charsRead = recv(establishedConnectionFD, buffer, SIZE, 0); // Read the client's message from the socket
					if (charsRead < 0) {
						error("ERROR reading from socket");
						break;
					}
				
				for(i = 0; i < strlen(buffer); i++){
					if(buffer[i] == '@') break;
					msg[i] = buffer[i];
				}
				i++;
				for(j = 0; j < strlen(buffer)-i; j++){
					if(buffer[i+j] == '!') break;
					key[j] = buffer[i+j];
				}

				encrypt(msg, key);		
				// Send a Success message back to the client
				charsRead = send(establishedConnectionFD, msg, strlen(msg), 0); // Send success back
				if (charsRead < 0) error("ERROR writing to socket");
				close(establishedConnectionFD); // Close the existing socket which is connected to the client
				exit(0);
				break;
			default:
				//do nothing but clean processes
				waitpid(-1, &status, WNOHANG);
				break;
		}
	}
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
