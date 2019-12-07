#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead, i, j;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[strlen(argv[2]) + 2];
	char alpha[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";



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

	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array

	//check key length
	if(strlen(argv[1]) > strlen(argv[2])){
		perror("key too short\n");
		exit(1);
	}

	// read plaintext to buffer
	for(i = 0; i < strlen(argv[1]); i++){
		if(argv[1][i] == '\n') break;
		if(strchr(alpha, argv[1][i]) != NULL ) {
			buffer[i] = argv[1][i];
		}
		else{
			perror("Invalid characters\n");
			exit(1);
		}

	}
	//append stop characters to buffer
	strcat(buffer, "@");

	// send plaintext to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// send key to the server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array

	for(i = 0; i < strlen(argv[2]); i++){
		if(argv[2][i] == '\n') break;
		if(strchr(alpha, argv[2][i]) != NULL ) {
			buffer[i] = argv[2][i];
		}
		else{
			perror("Invalid characters\n");
			exit(1);
		}

	}	
	
	//append stop characters to buffer
	strcat(buffer, "!");
	
	// Send message to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, strlen(argv[1])+2, 0); // Read data from the socket, leaving \0 at end

	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	close(socketFD); // Close the socket
	return 0;

}
