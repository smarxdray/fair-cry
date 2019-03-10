#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define ENCODE 1
#define DECODE -1


void shift(char* msg, int k, int mode) {
	for (int i = 0; i < strlen(msg); ++i) {
		msg[i] = ((msg[i] - 'a') + mode*k) % 26;
		if (msg[i] < 0) msg[i] = 26 + msg[i];
		if (msg[i] >= 26) msg[i] = msg[i] % 26;
		msg[i] += 'a';
	}
}

int main(int argc, char const *argv[])
{ 
	int k = atoi(argv[1]) % 26;

	char *msg = (char *)argv[2];
	
	shift(msg, k, ENCODE);
	printf("ciphertext:\t%s\n", msg);
	shift(msg, k, DECODE);
	printf("decoded:\t%s\n", msg);
	return 0;
}