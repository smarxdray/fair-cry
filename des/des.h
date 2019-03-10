#ifndef __DES_H__
#define __DES_H__

#define CHAR_BITS 8
#define PACK_BITS 64
#define PACK_CHAR 8
#define KEY48_BITS 48
#define KEY64_CHAR 8
#define KEY56_BITS 56
#define SBOX_LEN 64
#define SUBKR_BITS 6
#define ROUNDS 16
#define ENCODE 0
#define DECODE 15

/**
 * des.c
 */
void shuffle(unsigned char *src, unsigned char *dst, int bitCapSrc, int bitCapDst, const int *tbl, int tblLen);

void xorChars(unsigned char *x, unsigned char* r, int arrLen);

void cpyChars(unsigned char *src, unsigned char *dst, int arrLen);
/**
 * ip.c
 */
extern const int IPTbl[PACK_BITS];

extern const int _IPTbl[PACK_BITS];

void ip(unsigned char* pack, const int* tbl);

void splitPack(unsigned char* pack, unsigned char* leftPack, unsigned char* rightPack);

void mergePack(unsigned char* leftPack, unsigned char* rightPack, unsigned char* pack);
/**
 * key.c
 */
void subkey(unsigned char *key64, unsigned char subkeys[16][6]);
/**
 * f.c
 */
void f(unsigned char *rightPack, unsigned char *subkey, unsigned char *KR32);

#endif