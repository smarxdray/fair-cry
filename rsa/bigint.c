#include "bigint.h"

Num* newNum(const char* numStr)
{
	Num* num = NULL;
	if (numStr == NULL) {
		num = (Num*)malloc(sizeof(Num));
		num->sgn = 0;
		num->dgt = 0;
		num->val = NULL;
	} else {
		int sgn = numStr[0] == '-' ? 1 : 0;
		int dgt = strlen(numStr);
		char tmp[dgt-sgn];
		memset(tmp, 0, dgt-sgn);
		for(int j = 0, i = dgt-1; i >= sgn;) {
			char dgt = numStr[i--] - '0';
			if (dgt < 0 || dgt > 9) return num;
			else tmp[j++] = dgt;
		}
		num = charsToNum(tmp, sizeof(tmp)/sizeof(*tmp));
		num->sgn = sgn;
	}
	return num;
}

Num* charsToNum(const char* s, int len) {
	if (NULL == s) return NULL;
	int topDgt;
	for (topDgt = len-1; topDgt > 0; topDgt--) {
		if (0 != s[topDgt]) break;
	}
	
	Num* n = newNum(NULL);
	n->sgn = 0;
	n->dgt = topDgt + 1;
	n->val = (char*)malloc(sizeof(char)*n->dgt);
	memcpy(n->val, s, n->dgt);
	return n;
}

Num* charToNum(const char c)
{
	char tmp = c;
	char s[3] = {0};
	int i = 0;
	while (tmp > 0) {
		s[i++] = tmp % 10;
		tmp /= 10;
	}
	return charsToNum(s, sizeof(s)/sizeof(*s));
}

char numToChar(const Num* n)
{
	if (NULL == n || n->dgt > 3) return 0;
	char c = 0;
	for (int i = 0; i < n->dgt; ++i) {
		c += n->val[i] * pow(CARDINAL_NUMBER, i);
	}
	return c;
}

Num* freeNum(Num* n)
{
	if (NULL == n) return NULL;
	free(n->val);
	free(n);
	return n;
}

void printNum(const Num* n)
{
	if (NULL == n) {
		printf("NaN");
		return;
	}
	if (n->sgn == 1 && valCmp(n, newNum("0")) != 0) printf("-");
	for(int i = n->dgt-1;i >= 0;i--) {
        printf("%d",n->val[i]);  
    }
}

int bigCpy(Num* dst, const Num* src)
{
	if (NULL == dst || NULL == src) return NULL_POINTER_ERR;
	dst->sgn = src->sgn;
	dst->dgt = src->dgt;
	char* _val_ = dst->val;
	free(_val_);
	dst->val = (char*)malloc(sizeof(char)*dst->dgt);
	memcpy(dst->val, src->val, dst->dgt);
	return 1;
}

int valCmp(const Num* a, const Num* b)
{
	if (NULL == a || NULL == b) return NULL_POINTER_ERR;
	int greater = 0;
	if (a->dgt > b->dgt) {
		greater = 1;
	} else if (a->dgt < b->dgt) {
	 	greater = -1;
	} else {
		for (int i = a->dgt-1; i >= 0; --i) {
			if (a->val[i] > b->val[i]) {
			 	greater = 1;
				break;
			} else if (a->val[i] < b->val[i]) {
				greater = -1;
				break;
			} else {
				continue;
			}
		}
	}
	return greater;
}

int bigCmp(const Num* a, const Num* b)
{
	if (NULL == a || NULL == b) return NULL_POINTER_ERR;
	int greater = 0;
	if (a->sgn == 0 && b->sgn == 1) {
		greater = 1;
	} else if (a->sgn == 1 && b->sgn == 0) {
		greater = -1;
	} else {
		greater = valCmp(a, b);
		if (a->sgn == 1 && greater != 0) greater *= (-1);
	}
	return greater;
}

static int carry(int* ds, int len)
{
	if (NULL == ds) return NULL_POINTER_ERR;
	for(int i = 0; i < len; i++) {  
        if (ds[i] >= CARDINAL_NUMBER) {
            ds[i+1]= ds[i+1] + ds[i] / CARDINAL_NUMBER;
            ds[i] = ds[i] % CARDINAL_NUMBER;
        }
    }
    return 1;
}

static Num* valAdd(const Num* max, const Num* min)
{
	if (NULL == max || NULL == min) return NULL;
	int i, maxdgt = max->dgt + 1;
    int tmp[maxdgt];
    memset(tmp, 0, sizeof(tmp));
    for (i = 0; i < max->dgt; i++) {
    	if (i < min->dgt) tmp[i] = max->val[i] + min->val[i];
    	else tmp[i] = max->val[i];
    }
    carry(tmp, maxdgt);
    for(i = maxdgt-1; i > 0; i--) { 
        if (0 != tmp[i]) break;  
    }

    Num* c = newNum(NULL);
    c->dgt = i + 1;
    c->val = (char*)malloc(sizeof(char)*c->dgt);
    while (i >= 0) {
    	c->val[i] = tmp[i];
    	i--;
	}
	c->sgn = 0;
	return c;
}

static Num* valSub(const Num* max, const Num* min)
{
	if (NULL == max || NULL == min) return NULL;
	int i;
	char minuend[max->dgt], subtrahend[max->dgt];
	memset(minuend, 0, sizeof(minuend));
	memset(subtrahend, 0, sizeof(subtrahend));
	for (i = 0; i < max->dgt; i++) minuend[i] = max->val[i];
	for (i = 0; i < min->dgt; i++) subtrahend[i] = min->val[i];
	for (i = 0; i < max->dgt; i++) {
		if (minuend[i] < subtrahend[i]) {
			minuend[i] = minuend[i] + CARDINAL_NUMBER - subtrahend[i];
			minuend[i+1]--;
		} else minuend[i] -= subtrahend[i];
	}
 	Num* c = charsToNum(minuend, sizeof(minuend)/sizeof(*minuend));
 	c->sgn = 0;
 	return c;
}

Num* bigAdd(const Num* a, const Num* b)
{
	if (NULL == a || NULL == b) return NULL;
	const Num *max = a, *min = b;
	if (valCmp(a, b) < 0) {
		max = b;
		min = a;
	}
	Num* c = NULL;
	if (max->sgn == min->sgn) {
	    c = valAdd(max, min);
	} else {
		c = valSub(max, min);
	}
	c->sgn = max->sgn;
    return c;
}

Num* bigSub(const Num* a, const Num* b)
{
	if (NULL == a || NULL == b) return NULL;
	const Num *max = a, *min = b;
	if (valCmp(a, b) < 0) {
		max = b;
		min = a;
	}
	Num* c = NULL;
	if (max->sgn == min->sgn) {
		c = valSub(max, min);
	} else {
		c = valAdd(max, min);
	} 
	c->sgn = max == a ? max->sgn : 1 - max->sgn;
    return c;
}

Num* bigMul(const Num* a, const Num* b)
{
	if(NULL == a || NULL == b) return NULL;
    int i, j;
    int maxdgt = a->dgt + b->dgt;
    int tmp[maxdgt];
    memset(tmp, 0, sizeof(tmp));

    for(i = 0; i < a->dgt; i++) {  
        for(j=0;j<b->dgt;j++) 
        	tmp[i+j] += a->val[i] * b->val[j];
    }  
    carry(tmp, maxdgt);
    for(i = maxdgt-1; i > 0; i--) {  
        if (0 != tmp[i]) break;  
    }

    Num* c = newNum(NULL);
    c->dgt = i + 1;
    c->val = (char*)malloc(sizeof(char)*c->dgt);
    while (i >= 0) {
    	c->val[i] = tmp[i];
    	i--;
    }
    c->sgn = a->sgn == b->sgn ? 0 : 1;
    return c;
}

static int expandDivisor(Num* dst, const Num* src, int diff)
{
	if (NULL == dst || NULL == src) return NULL_POINTER_ERR;
	int dgtExp = 0;
	if (src->dgt >= dst->dgt + diff && diff > 0) {
		Num* tmp = newNum(NULL);
		tmp->dgt = dst->dgt + diff;
		tmp->val = (char*)malloc(sizeof(char)*tmp->dgt);
		for (int i = tmp->dgt-1; i >= 0; i--) {
			if (i - diff >= 0) tmp->val[i] = dst->val[i-diff];
			else tmp->val[i] = 0;
		}
		if (valCmp(src, tmp) >= 0) {
			bigCpy(dst, tmp);
			dgtExp = diff;
		} else {
			dgtExp = -1;
		}
		freeNum(tmp);
	}
	return dgtExp;
}

Num** bigDiv(const Num* a, const Num* b)
{
	if (NULL == a || NULL == b) return NULL;
	if (0 == valCmp(b, newNum("0"))) return NULL;
	Num* quotient = NULL;
	Num* remainder = NULL;
	int diff = valCmp(a, b);
	if (diff > 0) {
		Num *dividend = newNum(NULL), *divisor = newNum(NULL);
		bigCpy(divisor, b);
		bigCpy(dividend, a);
		int dgtDiff = dividend->dgt - divisor->dgt;
		char quoTmp[dgtDiff + 1];
		memset(quoTmp, 0, sizeof(quoTmp));
		int diffCut = 0;

		while (valCmp(dividend, b) >= 0) {
			bigCpy(divisor, b);
			dgtDiff = dividend->dgt - divisor->dgt;
			int order = dgtDiff + diffCut;
			int dgtExp = expandDivisor(divisor, dividend, order);
			diffCut = dgtExp < 0 ? -1 : 0;
			if (diffCut < 0) continue;
			while (valCmp(dividend, divisor) >= 0) {
				bigCpy(dividend, valSub(dividend, divisor));
				quoTmp[order]++;
			}
		}
		quotient = charsToNum(quoTmp, sizeof(quoTmp)/sizeof(*quoTmp));
	    remainder = charsToNum(dividend->val, dividend->dgt);
		freeNum(dividend);
		freeNum(divisor);
	} else if (diff < 0) {
		remainder = newNum(NULL);
		bigCpy(remainder, a);
		quotient = newNum("0");
	} else {
		quotient = newNum("1");
		remainder = newNum("0");
	}
	quotient->sgn = a->sgn == b->sgn ? 0 : 1;
	remainder->sgn = a->sgn; 
	Num** ret = (Num**)malloc(sizeof(Num*)*2);
	ret[0] = quotient;
	ret[1] = remainder;
	return ret;
}

Num* powerMod(const Num* m, const Num* e, const Num* n)
{
	if (NULL == m || NULL == e || NULL == n) return NULL;
	Num *a = newNum(NULL), *b = newNum(NULL), *c = newNum(NULL);
	bigCpy(a, m);
	bigCpy(b, e);
	bigCpy(c, n);
	Num* ans = newNum("1");
	bigCpy(a, bigDiv(a, c)[1]);
	while (bigCmp(b, newNum("0")) > 0)
	{
		if (bigCmp(bigDiv(b, newNum("2"))[1], newNum
			("1")) == 0)
			bigCpy(ans, bigDiv(bigMul(ans, a), c)[1]);
		bigCpy(b, bigDiv(b, newNum("2"))[0]);
		bigCpy(a, bigDiv(bigMul(a, a), c)[1]);
	}
	return ans;
}

Num* bigGCDEx(Num* a, Num* b, Num* x, Num* y) 
{
	 if(valCmp(b, newNum("0")) == 0) {
	     bigCpy(x, newNum("1"));
	     bigCpy(y, newNum("0"));
	     return a;
	 } else {
	 	Num* r = newNum(NULL);
	 	bigCpy(r, bigGCDEx(b, bigDiv(a, b)[1], x, y));
	    Num* t = newNum(NULL);
	    bigCpy(t, x);
	    bigCpy(x, y);
	    bigCpy(y, bigSub(t, bigMul(bigDiv(a, b)[0], y)));
	    freeNum(t);
	    return r;
	 }
}
