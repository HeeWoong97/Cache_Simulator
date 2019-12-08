#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#pragma warning(disable: 4996)

int main(int argc, char* args[]) {
	int n, s, m;
	int _n, _s, _m;
	int memAddr[32];
	int addr;
	int byteOffset[2] = { 0, };
	int* tag, * setIndex, * wordOffset;

	if(argc > 4){
		printf("Enter # of cache blocks(2^n blocks): ");
		printf("%s\n", args[1]);
		sscanf(args[1], "%d", &_n);
		n = (int)pow(2, _n);
		printf("Enter # of sets in the cache(2^s sets): ");
		printf("%s\n", args[2]);
		sscanf(args[2], "%d", &_s);
		s = (int)pow(2, _s);
		printf("Enter block size(2^m words): ");
		printf("%s\n", args[3]);
		sscanf(args[3], "%d", &_m);
		m = (int)pow(2, _m);
		printf("Enter 32-bit Memory Address: ");
		printf("%s\n", args[4]);
		sscanf(args[4], "%x", &addr);

		for (int i = 0; i < 32; i++, addr <<= 1) {
			if (addr & 0x80000000) memAddr[i] = 1;
			else memAddr[i] = 0;
		}

		tag = (int*)malloc(sizeof(int) * (32 - (s + m + 2)));
		setIndex = (int*)malloc(sizeof(int) * s);
		wordOffset = (int*)malloc(sizeof(int) * m);

		for (int i = 0; i < 32 - (s + m + 2); i++) 
			tag[i] = memAddr[i];
		for (int i = 0; i < s; i++) 
			setIndex[i] = memAddr[32 - (s + m + 2) + i];
		for (int i = 0; i < m; i++) 
			wordOffset[i] = memAddr[32 - (m + 2) + i];
		for (int i = 0; i < 2; i++)
			byteOffset[i] = memAddr[i + 30];

		printf("\nConverted 2-bit Memory Address: ");
		for (int i = 0; i < 32; i++)
			printf("%d", memAddr[i]);
		printf("\n");

		printf("%d-way set associative cache...\n", _n - _s);
		printf("\nTag bits: ");
		for (int i = 0; i < 32 - (s + m + 2); i++)
			printf("%d", tag[i]);
		printf("\nSet index bits: ");
		for (int i = 0; i < s; i++)
			printf("%d", setIndex[i]);
		printf("\nWord offset bits: ");
		for (int i = 0; i < m; i++)
			printf("%d", wordOffset[i]);
		printf("\nByte bits: ");
		for (int i = 0; i < 2; i++)
			printf("%d", byteOffset[i]);
		printf("\n\n");
	}
	return 0;
}