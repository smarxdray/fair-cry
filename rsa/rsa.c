#include "bigint.h"
#include <unistd.h>

typedef struct
{
    Num* N;
    Num* e;
} Sk;

typedef struct 
{
    Num* N;
    Num* d;
} Pk;

Num* eulerN(Num* p, Num* q)
{
    return bigMul(bigSub(p, newNum("1")), bigSub(q, newNum("1")));
}

Num** encode(const char* msg, Sk* sk)
{
    int len = strlen(msg);
    Num** ciphertext = (Num**)malloc(sizeof(Num*)*(len+1));
    for (int i = 0; i < len; i++) {
        ciphertext[i] = powerMod(charToNum(msg[i]), sk->e, sk->N);
        printNum(ciphertext[i]);
    }
    ciphertext[len] = NULL;
    return ciphertext;
}

char* decode(Num** ciphertext, Pk* pk)
{
    int len = 0;
    while(NULL != ciphertext[len++]);
    char* msg = (char*)malloc(sizeof(char)*(len));
    for (int i = 0; i < len-1; ++i) {
        msg[i] = numToChar(powerMod(ciphertext[i], pk->d, pk->N));
        printf("%c", msg[i]);
    }
    msg[len-1] = 0;
    return msg;
}

void printCiphertext(Num** ciphertext)
{
    int i = 0;
    while(NULL != ciphertext[i]) {
        printNum(ciphertext[i]);
        i++;
    }
}

int main(int argc, char const *argv[])
{
    const char* optstring = "p::q::e::m:";
    char isDefault[3] = {1, 1, 1};
    Sk sk;
    Pk pk;
    // Num* p = newNum("100001651");
    // Num* q = newNum("100001623");
    Num* p = newNum("33478071698956898786044169848212690817704794983713768568912431388982883793878002287614711652531743087737814467999489");
Num* q = newNum("36746043666799590428244633799627952632279158164343087642676032283815739666511279233373417143396810270092798736308917");
    sk.e = newNum("65537");
    const char* msg;
    int opt;
    do {
        opt = getopt(argc, (char* const*)argv, optstring);
        switch (opt) {
            case 'p':
                bigCpy(p, newNum(optarg));
                isDefault[0] = 0;
            break;
            case 'q':
                bigCpy(q, newNum(optarg));
                isDefault[1] = 0;
            break;
            case 'e':
                bigCpy(sk.e, newNum(optarg));
                isDefault[2] = 0;
            break;
            case 'm':
                msg = optarg;
            default:
            break;
        }
    } while (opt != -1);

    sk.N = bigMul(p, q);
    pk.N = bigMul(p, q);
    Num* n = eulerN(p, q);
    Num* s = newNum(NULL);
    Num* t = newNum(NULL);
    bigGCDEx(sk.e, n, s, t);
    pk.d = bigAdd(s, n);

    printf("Secret Key:");
    printf("\tN: "); printNum(sk.N);
    printf("\te: "); printNum(sk.e);
    if (isDefault[2]) printf("(default)");
    printf("\n\t\t");
    printf("p: "); printNum(p);
    if (isDefault[0]) printf("(default)");
    printf("\tq: "); printNum(q);
    if (isDefault[1]) printf("(default)");
    printf("\n");
    
    printf("Public Key:");
    printf("\tN: "); printNum(pk.N);
    printf("\td: "); printNum(pk.d);
    printf("\n");

    printf("encoded:\t");
    Num** c = encode(msg, &sk);
    printf("\ndecoded:\t");
    const char* m = decode(c, &pk);
    printf("\n");
    return 0;
}


// Num* p = newNum("33478071698956898786044169848212690817704794983713768568912431388982883793878002287614711652531743087737814467999489");
// Num* q = newNum("36746043666799590428244633799627952632279158164343087642676032283815739666511279233373417143396810270092798736308917");