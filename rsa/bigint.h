#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CARDINAL_NUMBER 10
#define NULL_POINTER_ERR -999

typedef struct Num
{	
    char sgn;
    char* val;
    int dgt;
} Num;

Num* newNum(const char* numStr);
Num* charsToNum(const char* s, int len);
Num* charToNum(const char c);
char numToChar(const Num* n);
Num* freeNum(Num* n);
void printNum(const Num* n);
int valCmp(const Num* a, const Num* b);
int bigCpy(Num* dst, const Num* src);
int bigCmp(const Num* a, const Num* b);
Num* bigAdd(const Num* a, const Num* b);
Num* bigSub(const Num* a, const Num* b);
Num* bigMul(const Num* a, const Num* b);
Num** bigDiv(const Num* a, const Num* b);
Num* powerMod(const Num* a, const Num* b, const Num* c);
Num* bigGCDEx(Num* a, Num* b, Num* x, Num* y);

#endif