#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#pragma warning(disable: 4996)

#define MAX_ARRAY_SIZE	2048

typedef int bool;
	#define FALSE	0	
	#define TRUE	1

typedef struct { //주소 구조체
	unsigned int addr;
	unsigned int tag, Index, wordOffset, byteOffset;
	unsigned int blockAddr, wordAddr;
	int setIndex;
	int result; //1->hit, 0->miss
}Address;

typedef struct { //블록 구조체
	unsigned int tag; //태그를 저장하고
	bool isFull; //비어있는지 아닌지 상태
	int LRU;
}Block;

int n = 0, s = 0, m = 0;
char* fileName;
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
		fileName= (char*)malloc(sizeof(char) * len);
		strcpy(fileName, arg[index - 1]);
	}
}

int main(int argc, char* args[]) {
	Address address[MAX_ARRAY_SIZE] = { 0, };
	
	int setindex_bit, wordoffset_bit;
	int way = 0;
	int hit = 0, miss = 0;
	int ADDRESS_SIZE = 0;

	FILE* fp = fopen("test.txt", "r");
	FILE* fp2 = fopen("result.txt", "w");
	char c;
	while ((c = fgetc(fp)) != EOF)
		if (c == '\n')
			ADDRESS_SIZE++;
	setup(argc, args);

	if (ADDRESS_SIZE > MAX_ARRAY_SIZE) {
		fprintf(stderr, "ERROR: Array Size overflow\n");
		return -1;
	} 

	setindex_bit = log2(s);
	wordoffset_bit = log2(m);

	fseek(fp, 0, SEEK_SET);

	for (int i = 0; i < ADDRESS_SIZE; i++) {
		fscanf(fp, "%x", &address[i].addr);
	}

	way = n;

	Block** cache = (Block **)malloc(sizeof(Block*) * s); //Block들이 모여서 cache가 된다.
	for (int i = 0; i < s; i++) {
		cache[i] = (Block*)malloc(sizeof(Block) * way);
	}

	for (int i = 0; i < s; i++) {
		for (int j = 0; j < way; j++) {
			cache[i][j].isFull = FALSE;
			cache[i][j].LRU = 0;
		}
	}

	for (int i = 0; i < ADDRESS_SIZE; i++) { //주소 정보 업데이트
		address[i].blockAddr = address[i].addr >> (wordoffset_bit + 2);
		address[i].wordAddr = address[i].addr >> 2;

		address[i].tag = address[i].addr >> (setindex_bit + wordoffset_bit + 2);
		address[i].Index = (address[i].addr & ((1 << (wordoffset_bit + 2)) * ((1 << setindex_bit) - 1))) >> (wordoffset_bit + 2);
		address[i].wordOffset = (address[i].addr & ((1 << (wordoffset_bit + 2)) - 4)) >> 2;
		address[i].byteOffset = address[i].addr & 0x00000003;

		//address[i].setIndex = address[i].blockAddr % s;

		address[i].result = 0;
	}

	for (int i = 0; i < ADDRESS_SIZE; i++) {
		if (i == 0) { //첫번째 주소면 miss
			fprintf(fp2, "0x%08X\tmiss\n", address[i].addr);
			miss += 1;
			continue;
		}
		bool isHit;
		for (int j = 0; j < s; j++) { //블럭 LRU 업데이트
			for (int k = 0; k < way; k++) {
				if (cache[j][k].isFull)
					cache[j][k].LRU++;
			}
		}
		for (int k = 0; k < way; k++) {
			if (address[i].addr == cache[address[i].Index][k].tag) { //해당 set에서 같은 태그를 찾았으면
				isHit = TRUE;
				hit += 1; //hit!
				cache[address[i].Index][k].LRU += 1; //그 블럭의 LRU 증가
				address[i].result = 1;
				break; //더 이상 탐색할 필요가 없으므로 반복문 탈출
			}
		}
		if (address[i].result == 0) { //위의 반복문에서 hit이 일어나지 않았다면
			isHit = FALSE;
			miss += 1; //miss!
			address[i].result = 0;
			for (int j = 0; j < s; j++) {
				for (int k = 0; k < way; k++) { //비어있는 block이 있는지 확인
					if (cache[j][k].isFull == TRUE) { //block이 이미 채워져 있다면
						continue; //일단 진행
					}
					else { //비어있는 블록을 발견하면
						cache[j][k].tag = address[i].addr; //채워 넣는다.
						break; //채워 넣었다면 반복문 탈출
					}
				}
				if (j == s - 1) { //반복문을 다 돌았지만 비어있는 block을 찾지 못했다면(반복문을 탈출하지 못했다면)
					int max_LRU = 0;
					int max_LRU_set = 0, max_LRU_way = 0;
					for (int k = 0; k < s; k++) {
						for (int l = 0; l < way; l++) { //LRU가 큰 블럭 탐색
							if (cache[k][l].LRU > max_LRU) {
								max_LRU = cache[k][l].LRU;
								max_LRU_set = k;
								max_LRU_way = l;
							}
						}
						if (max_LRU == 0) { //못찾았다면
							cache[0][0].tag = address[i].addr; //set의 0번째 block에 넣기
							cache[0][0].LRU = 0;
						}
						else { //찾았다면
							cache[max_LRU_set][max_LRU_way].tag = address[i].addr;
							cache[max_LRU_set][max_LRU_way].LRU = 0;
						}
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
