#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define ENCODE 1
#define DECODE -1

void singleShift(char* letter, int k, int mode) {
	char base = 0;
	if (*letter >= 'A' && *letter <= 'Z') {
		base = 'A';
	} else if (*letter >= 'a' && *letter <= 'z') {
		base = 'a';
	}

	if (0 != base) {
		*letter -= base;
		*letter = (*letter + mode * k) % 26;
		if (*letter < 0) *letter += 26; //complement
		if (*letter >= 26) *letter %= 26;
		*letter += base;
	}
}

void shift(char* msg, int k, int mode) {
	for (int i = 0; i < strlen(msg); ++i) {
		singleShift(msg + i, k, mode);
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