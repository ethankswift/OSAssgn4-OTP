#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define SIZE 100000

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead, i, j;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char key[SIZE];
	char msg[SIZE];
	char alpha[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	FILE * f;



	if (argc < 4) { fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	memset(msg, '\0', sizeof(msg)); // Clear out the buffer array

	memset(key, '\0', sizeof(key)); // Clear out the buffer array


	//open msg file
	f = fopen(argv[1], "r");
	if(f == NULL) {
		perror("no such pt file\n");
		exit(1);
	}

	fgets(msg, SIZE, f);
	
	fclose(f);

	//open key file
	f = fopen(argv[2], "r");
	if(f == NULL) {
		perror("no such key file\n");
		exit(1);
	}

	fgets(key, SIZE, f);
	
	fclose(f);

	//check key length
	if(strlen(msg) > strlen(key)){
		perror("key too short\n");
		exit(1);
	}

	//check characters
	for(i = 0; i < strlen(msg); i++){
		if(msg[i] == '\n') break;
		if(strchr(alpha, msg[i]) == NULL ) {
			perror("Invalid characters in plaintext\n");
			exit(1);
		}

	}
	for(i = 0; i < strlen(key); i++){
		if(key[i] == '\n') break;
		if(strchr(alpha, key[i]) == NULL ) {
			perror("Invalid characters in key\n");
			exit(1);
		}

	}
	//append stop characters to buffer
	strcat(msg, "@");
	strcat(key, "!");

	// send plaintext to server
	charsWritten = send(socketFD, msg, strlen(msg), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(msg)) printf("CLIENT: WARNING: Not all data written to socket!\n");



	// Send message to server
	charsWritten = send(socketFD, key, strlen(key), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(key)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(key, '\0', sizeof(key)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, key, SIZE, 0); // Read data from the socket, leaving \0 at end

	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	
	key[strlen(key)] = '\n';
	
	printf(key);

	close(socketFD); // Close the socket
	return 0;

}


