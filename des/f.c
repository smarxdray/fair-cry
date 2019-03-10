#include "des.h"

static const int ETbl[KEY48_BITS] = {
	32,1,2,3,4,5,
	4,5,6,7,8,9,
	8,9,10,11,12,13,
	12,13,14,15,16,17,
	16,17,18,19,20,21,
	20,21,22,23,24,25,
	24,25,26,27,28,29,
	28,29,30,31,32,1,
};

static const int SBox1[SBOX_LEN] = {
	14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
	0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
	4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
	15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13,
};

static const int SBox2[SBOX_LEN] = {
	15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
	3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
	0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
	13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9,
};

static const int SBox3[SBOX_LEN] = {
	10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
	13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
	13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
	1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12,
};

static const int SBox4[SBOX_LEN] = {
	7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
	13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
	10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
	3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14,
};

static const int SBox5[SBOX_LEN] = {
	2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
	14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
	4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
	11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3,
};

static const int SBox6[SBOX_LEN] = {
	12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
	10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
	9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
	4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13,
};

static const int SBox7[SBOX_LEN] = {
	4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
	13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
	1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
	6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12,
};

static const int SBox8[SBOX_LEN] = {
	13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
	1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
	7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
	2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11,
};

static const int *SBoxs[8] = {
	SBox1,SBox2,SBox3,SBox4,SBox5,SBox6,SBox7,SBox8,
};

static const int PBox[PACK_BITS/2] = {
	16,7,20,21,29,12,28,17,
	1,15,23,26,5,18,31,10,
	2,8,24,14,32,27,3,9,
	19,13,30,6,22,11,4,25,
};

static const int splitKRTbl[KEY48_BITS] = {
	1,2,3,4,5,6,
	7,8,9,10,11,12,
	13,14,15,16,17,18,
	19,20,21,22,23,24,
	25,26,27,28,29,30,
	31,32,33,34,35,36,
	37,38,39,40,41,42,
	43,44,45,46,47,48,
};

void static expandRightPack(unsigned char *rightPack, unsigned char *expension, const int *tbl) {
	for(int i = 0; i < KEY64_CHAR-2; i++) *(expension + i) = 0;
	shuffle(rightPack, expension, CHAR_BITS, CHAR_BITS, tbl, KEY48_BITS);
}

void static splitKR(unsigned char* KR48, unsigned char* subKRs, const int* tbl) {
	shuffle(KR48, subKRs, CHAR_BITS, CHAR_BITS-2, tbl, KEY48_BITS);
}

void static SBoxReplace(unsigned char* subKR6, unsigned char* subKR4, const int *SBox) {
	int row = 0, col = 0;
	unsigned char maskHigh = 0x20, maksLow = 0x01;
	maskHigh &= *subKR6;
	maskHigh >>= (SUBKR_BITS - 1) - 1;
	maksLow &= *subKR6;
	maksLow |= maskHigh;
	row = maksLow;
	unsigned char maskMiddle;
	unsigned char masks[4] = {0x10, 0x08, 0x04, 0x02};
	for(int i = 0; i < 4; i++) {
		maskMiddle = masks[i];
		maskMiddle &= *subKR6;
		maskMiddle >>= 1;
		col |= maskMiddle;
	}	
	int idxSBox = row * 16 + col;
	*subKR4 = SBox[idxSBox];
}

void static mergeKR(unsigned char *subKR4s, unsigned char *KR32) { 
	for(int i = 0; i < PACK_CHAR/2; i++) *(KR32 + i) = 0;
	for(int i = 0; i < PACK_CHAR; i+=2) {
		subKR4s[i] <<= 4;
		subKR4s[i] |= subKR4s[i+1];
		KR32[i/2] = subKR4s[i];
	}
}

void f(unsigned char *rightPack, unsigned char *subkey, unsigned char *KR32) {

	unsigned char expansion[KEY64_CHAR-2];
	expandRightPack(rightPack, expansion, ETbl);

	xorChars(subkey, expansion, PACK_CHAR/2);
	unsigned char *KR48 = expansion;

	unsigned char subKR6s[KEY64_CHAR] = {0};
	unsigned char subKR4s[KEY64_CHAR] = {0};
	splitKR(KR48, subKR6s, splitKRTbl);
	for(int i = 0; i < KEY64_CHAR; i++) {
		SBoxReplace((subKR6s + i), (subKR4s + i), SBoxs[i]);
	}
	mergeKR(subKR4s, KR32);
}