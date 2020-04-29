#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#pragma warning(disable: 4996)

#define MAX_ARRAY_SIZE	2048

typedef int bool;
#define FALSE	0	
#define TRUE	1

typedef struct { //address struct
	unsigned int addr;
	unsigned int tag, Index, wordOffset, byteOffset;
	unsigned int blockAddr, wordAddr;
	int setIndex;
	int result; //1->hit, 0->miss
}Address;

typedef struct { //block struct
	unsigned int addr; //save memory address
	bool isFull; //empty or full
	int LRU;
}Block;

int n = 0, s = 0, m = 0;
char* fileName = NULL;
const char* HIT_MISS[] = { "miss", "hit" };

void setup(int argc, char* arg[]) {
	for (int index = 1; index < argc; index++) {
		if (arg[index][0] != '-')
			continue;
		switch (arg[index][1]) {
		case 'm':
			m = index + 1 < argc ? atoi(arg[index + 1]) : fprintf(stderr, "Wrong Syntax!\n");
			break;
		case 's':
			s = index + 1 < argc ? atoi(arg[index + 1]) : fprintf(stderr, "Wrong Syntax!\n");
			break;
		case 'n':
			n = index + 1 < argc ? atoi(arg[index + 1]) : fprintf(stderr, "Wrong Syntax!\n");
			break;
		}
		int len = strlen(arg[index - 1]);
		fileName = (char*)malloc(sizeof(char) * len);
		strcpy(fileName, arg[index - 1]);
	}
}

int main(int argc, char* args[]) {
	Address address[MAX_ARRAY_SIZE];

	int setindex_bit, wordoffset_bit;
	int way = 0;
	int hit = 0, miss = 0;
	int ARRAY_SIZE = 0;

	FILE* fp = fopen("test.txt", "r");
	FILE* fp2 = fopen("result.txt", "w");
	char c;
	while ((c = fgetc(fp)) != EOF)
		if (c == '\n')ARRAY_SIZE++;
	setup(argc, args);

	if (ARRAY_SIZE > MAX_ARRAY_SIZE) {
		fprintf(stderr, "ERROR: Array Size overflow\n");
		return -1;
	}

	setindex_bit = log2(s);
	wordoffset_bit = log2(m);

	fseek(fp, 0, SEEK_SET);

	for (int i = 0; i < ARRAY_SIZE; i++) {
		fscanf(fp, "%x", &address[i].addr);
	}

	way = n;

	Block** cache;

	cache = (Block * *)malloc(sizeof(Block*) * s); //Blocks->Cache
	for (int i = 0; i < s; i++) {
		cache[i] = (Block*)malloc(sizeof(Block) * way);
	}

	for (int i = 0; i < s; i++) {
		for (int j = 0; j < way; j++) {
			cache[i][j].isFull = FALSE;
			cache[i][j].LRU = 0;
		}
	}

	for (int i = 0; i < ARRAY_SIZE; i++) { //Update addr. information
		address[i].blockAddr = address[i].addr >> (wordoffset_bit + 2);
		address[i].wordAddr = address[i].addr >> 2;

		address[i].tag = address[i].addr >> (setindex_bit + wordoffset_bit + 2);
		address[i].Index = (address[i].addr & ((1 << (wordoffset_bit + 2)) * ((1 << setindex_bit) - 1))) >> (wordoffset_bit + 2);
		address[i].wordOffset = (address[i].addr & ((1 << (wordoffset_bit + 2)) - 4)) >> 2;
		address[i].byteOffset = address[i].addr & 0x00000003;

		address[i].setIndex = address[i].blockAddr % s;

		address[i].result = 0;
	}

	for (int i = 0; i < ARRAY_SIZE; i++) {
		if (i == 0) { //first addr is always miss
			fprintf(fp2, "0x%08X\tmiss\n", address[i].addr);
			miss += 1;
			continue;
		}
		bool isHit;
		for (int j = 0; j < s; j++) { //Update block LRU
			for (int k = 0; k < way; k++) {
				if (cache[j][k].isFull)
					cache[j][k].LRU++;
			}
		}

		for (int j = 0; j < way; j++) {
			if (address[i].addr == cache[address[i].Index][j].addr) { //If we found same addr in the cache
				isHit = TRUE;
				hit += 1; //hit!
				cache[address[i].Index][j].LRU += 1; //LRU++
				address[i].result = 1;
				break; 
			}
		}
		if (address[i].result == 0) { //If there were any hit
			isHit = FALSE;
			miss += 1; //miss!
			address[i].result = 0;
			for (int j = 0; j < way; j++) { //Check empty block
				if (cache[address[i].Index][j].isFull == TRUE) { //If block is full
					continue; 
				}
				else { //If find empty block
					cache[address[i].Index][j].addr = address[i].addr;
					break; 
				}

				if (j == way - 1) { //If we doesn't break loop
					int max_LRU = 0;
					for (int k = 0; i < way; k++) { //Search biggest LRU
						if (cache[address[i].Index][j].LRU > max_LRU)
							max_LRU = k;
					}
					if (max_LRU == 0) { //If can't find
						cache[address[i].Index][0].addr = address[i].addr; //Insert at 1st block
						cache[address[i].Index][0].LRU = 0;
					}
					else {
						cache[address[i].Index][max_LRU].addr = address[i].addr;
						cache[address[i].Index][max_LRU].LRU = 0;
					}
				}
			}
		}
		fprintf(fp2, "0x%08X\t%s\n", address[i].addr, HIT_MISS[isHit]);
	}

	fprintf(fp2, "# of cache hits	%d\n", hit);
	fprintf(fp2, "# of cache misses	%d\n", miss);

	fclose(fp);
	fclose(fp2);
	free(cache);
	return 0;
}