#include "des.h"
#include <string.h>
#include <stdio.h>

void static convertPack(unsigned char* pack, unsigned char subkeys[ROUNDS][KEY64_CHAR-2], const int MODE);

void static convertMsg(unsigned char *fmsg, unsigned char *msg, int fmsgLen, int msgLen, unsigned char *key64, const int MODE);

int static abs(int i);

int main(int argc, char const *argv[]) {

    if (argc != 3) {
        if (argc < 3)
            printf("error: missing arguments!\n");
        else
            printf("error: excess arguments!\n");
        printf("argument list: key msg\n");
        return -1;
    }
    
    unsigned char* key64 = (unsigned char*)argv[1];
    if (strlen((char*)key64) < KEY64_CHAR) {
        printf("error: the key is too short!\n");
        return -1;
    }
    printf("key:\t\t");
    for (int i = 0; i < KEY64_CHAR; ++i) printf("%c", key64[i]);

    unsigned char *msg = (unsigned char*)argv[2];
    int msgLen = strlen((char*)msg) + 1;//including the '\n'
    int extra = 0;
    if (msgLen % 8 != 0) extra = (8 - (msgLen % 8));
    int fmsgLen = msgLen + extra;
    unsigned char fmsg[fmsgLen];
    memset(fmsg, 0, sizeof(fmsg)); // initiate variable-sized array
    cpyChars(msg, fmsg, msgLen);

    printf("\nmsg:\t\t");
    printf("%s", fmsg);

    convertMsg(fmsg, msg, fmsgLen, msgLen, key64, ENCODE);
    printf("\nencrypted:\t");
    printf("%s", fmsg);

    convertMsg(fmsg, msg, fmsgLen, msgLen, key64, DECODE);
    printf("\ndecrypted:\t");
    printf("%s\n", fmsg);
    return 0;
}
/**
 * copy bits of specific indices in the source
 * to the destination (from left to right)
 * according to the table
 * @param src    source unsigned char array
 * @param dst    destination unsigned char array
 * @param bitCapSrc     bit capacity of src char
 * @param bitCapDst     bit capacity of dst char
 * @param tbl    bit index selecting table
 * @param tblLen table length, which equals to the number of destination bits
 */
void shuffle(unsigned char *src, unsigned char *dst, int bitCapSrc, int bitCapDst, const int *tbl, int tblLen) {
    //shuffle bits according to the table
    for(int i = 0; i < tblLen; i++) {
        int bitIdxSrc = tbl[i] - 1; //bit index in the src
        int charIdxSrc = bitIdxSrc / bitCapSrc; //unsigned char index in the src
        int x = bitIdxSrc % bitCapSrc; //bit offset in the single unsigned char

        unsigned char mask = 0x80;
        mask >>= x;//set the corresponding bit to 1 to make a mask
        mask &= *(src + charIdxSrc);//get single value(0/1) of a src bit
        mask >>= ((bitCapSrc - 1) - x);//convert mask into 0 or 1

        int charIdxDst = i / bitCapDst;
        int bitIdxDst = i % bitCapDst;
        dst[charIdxDst] |= mask << ((bitCapDst - 1) - bitIdxDst);//set the dst bit
    }
}

void xorChars(unsigned char *x, unsigned char* r, int arrLen) {
    for(int i = 0; i < arrLen; i++) {
        *(r + i) ^= *(x + i);
    }
}

void cpyChars(unsigned char *src, unsigned char *dst, int arrLen) {
    for (int i = 0; i < arrLen; ++i) {
        *(dst + i) = *(src + i);
    }
}

int static abs(int i) {
    return i < 0 ? -i : i;
}

void static convertPack(unsigned char* pack, unsigned char subkeys[ROUNDS][KEY64_CHAR-2], const int MODE) {
    ip(pack, IPTbl);
    
    unsigned char leftPack[PACK_CHAR/2], rightPack[PACK_CHAR/2];
    splitPack(pack, leftPack, rightPack);

    for(int i = 0; i < ROUNDS - 1; i++) {
        unsigned char KR32[PACK_CHAR/2] = {0};
        f(rightPack, subkeys[abs(MODE-i)], KR32);
        xorChars(KR32, leftPack, PACK_CHAR/2); 
        cpyChars(rightPack, KR32, PACK_CHAR/2); //Temporary storage of R
        cpyChars(leftPack, rightPack, PACK_CHAR/2);  //R(next)
        cpyChars(KR32, leftPack, PACK_CHAR/2); //L(next)
    }

    unsigned char KR32[PACK_CHAR/2] = {0};
    f(rightPack, subkeys[(ROUNDS-1) - MODE], KR32);
    xorChars(KR32, leftPack, PACK_CHAR/2);

    mergePack(leftPack, rightPack, pack);
    ip(pack, _IPTbl);
}

void static convertMsg(unsigned char *fmsg, unsigned char *msg, int fmsgLen, int msgLen, unsigned char *key64, const int MODE) {

    unsigned char subkeys[ROUNDS][KEY64_CHAR-2] = {0};
    subkey(key64, subkeys);

    for (int i = 0; i < fmsgLen; i+=PACK_CHAR) {
        unsigned char pack[PACK_CHAR] = {0};
        for(int j = 0; j < PACK_CHAR; ++j) *(pack + j) = *(fmsg + i + j);
        convertPack(pack, subkeys, MODE);
        for(int j = 0; j < PACK_CHAR; ++j) *(fmsg + i + j) = *(pack + j);
    }
    cpyChars(fmsg, msg, msgLen);
}