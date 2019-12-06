#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"


int main(int argc, char * argv[] ) {
//init keygensize and an iterable
	int keygensize, i;
//array holding the capital alphabet and " "
	char alpha[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
//seeding rand
	srand(time(NULL));
//check for arg number
	if (argc != 2) {
		fprintf(stderr, "Error: Wrong number of args\n");
		return -1;
	}
//read arg
	keygensize = atoi(argv[1]);
//check if arg read success
	if (keygensize == 0) {
		fprintf(stderr, "Error: invalid size\n");
	}
//print out keygensize chars from alpha psuedorandomly
	for(i = 0; i < keygensize; i++) {
		fprintf(stdout, "%c", alpha[rand()%27]);
	}
//print out newline	
	fprintf(stdout, "\n" );

	return 0;
}
