#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define ENCODE 1
#define DECODE -1

void singleShift(char* letter, char key, int mode) {
	char base = 0;
	if (*letter >= 'A' && *letter <= 'Z') {
		base = 'A';
	} else if (*letter >= 'a' && *letter <= 'z') {
		base = 'a';
	}

	if (0 != base) {
		*letter -= base;
		*letter = (*letter + mode * (key - base)) % 26;
		if (*letter < 0) *letter += 26; //complement
		if (*letter >= 26) *letter %= 26;
		*letter += base;
	}
}

void shift(char* msg, char *key, int mode) {
	int keyLen = strlen(key);
	for (int i = 0; i < strlen(msg); ++i) {
		singleShift(msg + i, key[i % keyLen], mode);
	}
}

int main(int argc, char const *argv[])
{
	char *key = (char *)argv[1];
	char *msg = (char *)argv[2];
	shift(msg, key, ENCODE);
	printf("ciphertext:\t%s\n", msg);
	shift(msg, key, DECODE);
	printf("plaintext:\t%s\n", msg);
	return 0;
}