#include <stdio.h>
#pragma warning(disable: 4996)

void Print_Binary(unsigned int n);

int main(void) {
	unsigned int addr;
	unsigned int tag, setIndex, wordOffset, byteOffset;
	unsigned int blockAddr, wordAddr;
	int n = 0, s = 0, m = 0;

	printf("Enter 32-bit address: ");
	scanf("%x", &addr);
	printf("n: ");
	scanf("%d", &n);
	printf("s: ");
	scanf("%d", &s);
	printf("m: ");
	scanf("%d", &m);

	blockAddr = addr >> (m + 2);
	wordAddr = addr >> 2;

	tag = addr >> (s + m + 2);
	setIndex = (addr & ((1 << (m + 2)) * ((1 << s) - 1))) >> (m + 2);
	wordOffset = (addr & ((1 << (m + 2)) - 4)) >> 2;
	byteOffset = addr & 0x00000003;

	printf("\n");

	printf("Byte address: ");
	Print_Binary(addr);
	printf("\n");

	printf("Word address: ");
	Print_Binary(wordAddr);
	printf("\n");

	printf("Block address: ");
	Print_Binary(blockAddr);
	printf("\n");
	printf("\n");

	printf("tag: ");
	Print_Binary(tag);
	printf("\n");

	printf("Set index: ");
	Print_Binary(setIndex);
	printf("\n");

	printf("Word offset: ");
	Print_Binary(wordOffset);
	printf("\n");

	printf("Byte offset: ");
	Print_Binary(byteOffset);
	printf("\n");

	return 0;
}

void Print_Binary(unsigned int n) {
	for (int i = 0; i < 32; i++, n <<= 1) {
		if (n & 0x80000000) printf("1");
		else printf("0");
	}
}