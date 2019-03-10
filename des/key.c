#include "des.h"

static const int keyTbl[KEY56_BITS] = {
	57,49,41,33,25,17,9,
	1,58,50,42,34,26,18,
	10,2,59,51,43,35,27,
	19,11,3,60,52,44,36,
	63,55,47,39,31,23,15,
	7,62,54,46,38,30,22,
	14,6,61,53,45,37,29,
	21,13,5,28,20,12,4,
};

static const int shiftList[16] = {
	1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};

static const int key48Tbl[KEY48_BITS] = {
	14,17,11,24,1,5,3,28,
	15,6,21,10,23,19,12,4,
	26,8,16,7,27,20,13,2,
	41,52,31,37,47,55,30,40,
	51,45,33,48,44,49,39,56,
	34,53,46,42,50,36,29,32,
};

void static genKey56(unsigned char* key64, unsigned char* key56, const int* tbl) {

	for(int i = 0; i < KEY64_CHAR-1; i++) *(key56 + i) = 0;
	
	shuffle(key64, key56, CHAR_BITS, CHAR_BITS, tbl, KEY56_BITS);
}

void static splitKey56(unsigned char* key56, int *leftKey, int *rightKey) {
	*leftKey = 0;
	*rightKey = 0;
	for(int i = 0; i < KEY56_BITS/2; i++){
		//left side
		unsigned int mask = 0x1;
		mask <<= CHAR_BITS - 1;
		int charIdxKey = i / CHAR_BITS;
		int x = i % CHAR_BITS;
		mask >>= x;
		mask &= *(key56 + charIdxKey);
		mask >>= ((CHAR_BITS - 1) - x);
		*leftKey |= mask <<= ((KEY56_BITS/2 - 1) - i);
		//right side
		mask = 0x1;
		mask <<= CHAR_BITS - 1;
		int j = i + KEY56_BITS/2;
		charIdxKey = j / CHAR_BITS;
		x = j % CHAR_BITS;
		mask >>= x;
		mask &= *(key56 + charIdxKey);
		mask >>= ((CHAR_BITS - 1) - x);
		*rightKey |= mask <<= ((KEY56_BITS/2 - 1) - i);
	}
}

void static shiftLeftKey(int *leftKey, int *rightKey) {
	unsigned int mask = 0x08000000;
	mask &= *leftKey;
	mask >>= KEY56_BITS/2 - 1;
	*leftKey <<= 1;
	*leftKey |= mask;
	mask = 0x0fffffff;
	*leftKey &= mask;

	mask = 0x08000000;
	mask &= *rightKey;
	mask >>= KEY56_BITS/2 - 1;
	*rightKey <<= 1;
	*rightKey |= mask;
	mask = 0x0fffffff;
	*rightKey &= mask;
}

void static mergeKey(int* leftKey, int *rightKey, unsigned char *key56) {

	for(int i = 0; i < KEY64_CHAR-1; i++) *(key56 + i) = 0;

	for(int i = 0; i < KEY56_BITS/2; i++) {
		//merge left side
		unsigned int mask = 0x08000000;
		mask >>= i;
		mask &= *leftKey;
		mask >>= (KEY56_BITS/2 - 1 - i);
		int charIdxKey = i / CHAR_BITS;
		int x = i % CHAR_BITS;
		*(key56 + charIdxKey) |= (mask << (CHAR_BITS - 1 -x));
		//merge right side
		mask = 0x08000000;
		mask >>= i;
		mask &= *rightKey;
		mask >>= (KEY56_BITS/2 - 1 - i);
		int j = i + KEY56_BITS/2;
		charIdxKey = j / CHAR_BITS;
		x = j % CHAR_BITS;
		*(key56 + charIdxKey) |= (mask << (CHAR_BITS - 1 -x));
			
	}
}

void static genKey48(unsigned char* key56, unsigned char* key48, int shiftStep) {

	for(int i = 0; i < KEY64_CHAR-2; i++) *(key48 + i) = 0;

	int left, right;
	splitKey56(key56, &left, &right);
	for(int i = 0; i < shiftStep; i++) shiftLeftKey(&left, &right);
	mergeKey(&left, &right, key56);
	shuffle(key56, key48, CHAR_BITS, CHAR_BITS, key48Tbl, KEY48_BITS);
}

void subkey(unsigned char *key64, unsigned char subkeys[16][6]) {
	unsigned char key56[KEY64_CHAR-1];
	genKey56(key64, key56, keyTbl);
	unsigned char subkey[6];
	for(int i = 0; i < 16; i++) {
		genKey48(key56, subkeys[i], shiftList[i]);
	}
}