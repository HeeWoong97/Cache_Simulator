#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#pragma warning(disable: 4996)

int binaryToDemical(int* arr, int size);

int main(void) {
	int n, s, m;
	int _n, _s, _m;
	int memAddr[32];
	int addr;
	int* blockAddr, * wordAddr;
	int byteOffset[2] = { 0, };
	int* tag, * setIndex, * wordOffset;
	int setIndex_D;

	printf("Enter # of cache blocks(2^n blocks): ");
	scanf("%d", &_n);
	n = (int)pow(2, _n);
	printf("Enter # of sets in the cache(2^s sets): ");
	scanf("%d", &_s);
	s = (int)pow(2, _s);
	printf("Enter block size(2^m words): ");
	scanf("%d", &_m);
	m = (int)pow(2, _m);
	printf("Enter 32-bit Memory Address: ");
	scanf("%x", &addr);

	for (int i = 0; i < 32; i++, addr <<= 1) {
		if (addr & 0x80000000) memAddr[i] = 1;
		else memAddr[i] = 0;
	}

	blockAddr = (int*)malloc(sizeof(int) * (32 - (m + 2)));
	wordAddr = (int*)malloc(sizeof(int) * 30);

	tag = (int*)malloc(sizeof(int) * (32 - (s + m + 2)));
	setIndex = (int*)malloc(sizeof(int) * s);
	wordOffset = (int*)malloc(sizeof(int) * m);

	for (int i = 0; i < 32 - (m + 2); i++)
		blockAddr[i] = memAddr[i];
	for (int i = 0; i < 30; i++)
		wordAddr[i] = memAddr[i];

	for (int i = 0; i < 32 - (s + m + 2); i++) 
		tag[i] = memAddr[i];
	for (int i = 0; i < s; i++) 
		setIndex[i] = memAddr[32 - (s + m + 2) + i];
	for (int i = 0; i < m; i++) 
		wordOffset[i] = memAddr[32 - (m + 2) + i];
	for (int i = 0; i < 2; i++)
		byteOffset[i] = memAddr[i + 30];

	setIndex_D = binaryToDemical(setIndex, sizeof(setIndex));

	printf("\n%d-way set associative cache...\n", _n - _s);

	printf("\nConverted 2-bit Memory Address: ");
	for (int i = 0; i < 32; i++)
		printf("%d", memAddr[i]);
	printf("\nWord Address: ");
	for (int i = 0; i < 30; i++)
		printf("%d", wordAddr[i]);
	printf("\nBlock Address: ");
	for (int i = 0; i < 32 - (m + 2); i++)
		printf("%d", blockAddr[i]);
	printf("\nSet index: %d", setIndex_D);
	printf("\n");

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

	return 0;
}

int binaryToDemical(int* arr, int size) {
	int demical = 0;
	for (int i = 0; i < size; i++) {
		if (arr[i]) demical += (int)pow(2, size - 1. - i);
	}
	return demical;
}