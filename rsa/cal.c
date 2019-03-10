#include "bigint.h"
#include <unistd.h>

void printFmt(Num* a, char operator, Num* b, Num* c);
void bigCal(Num* a, Num* b, char operator, void* (*cal)(Num*, Num*));

int main(int argc, char const *argv[])
{
	const char* optstring = "a:s:m:d:";
	int opt;
	do {
		opt = getopt(argc, (char* const*)argv, optstring);
		switch (opt) {
			case 'a':
				bigCal(newNum(optarg), newNum(argv[optind++]), '+', (void*)bigAdd);
				printf("\n");
			break;
			case 's':
				bigCal(newNum(optarg), newNum(argv[optind++]), '-', (void*)bigSub);
				printf("\n");
			break;
			case 'm':
				bigCal(newNum(optarg), newNum(argv[optind++]), '*', (void*)bigMul);
				printf("\n");
			break;
			case 'd':
				bigCal(newNum(optarg), newNum(argv[optind++]), '/', (void*)bigDiv);
				printf("\n");
			break;
			default:
			break;
		}
	} while ( opt != -1);
	return 0;
}

void printFmt(Num* a, char operator, Num* b, Num* c)
{
	printf("(");
	printNum(a);
	printf(")");
	printf(" %c ", operator);
	printf("(");
	printNum(b);
	printf(")");
	printf(" = ");
	printf("(");
	printNum(c);
	printf(")");
}

void bigCal(Num* a, Num* b, char operator, void* (*cal)(Num*, Num*))
{
	if (NULL == a || NULL == b) {
		printf("Error: invalid arguments!\n");
		printf("-a/s/m/d int1 int2\n");
		exit(EXIT_FAILURE);
	}
	if (operator == '/') {
		Num** ret = (Num**)cal(a, b);
		printFmt(a, operator, b, ret[0]);
		printf(" ··· ");
		printf("(");
		printNum(ret[1]);
		printf(")");
	} else {
		printFmt(a, operator, b, (Num*)cal(a, b));
	}
}