#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#pragma warning(disable: 4996)

#define ARRAY_SIZE 5
#define logB(x, base) log(x)/log(base)

typedef int bool;
#define false 0	
#define true 1

typedef struct {
	unsigned int addr;
	unsigned int tag, Index, wordOffset, byteOffset;
	unsigned int blockAddr, wordAddr;
	int setIndex;
	int result; //1->hit, 0->miss
}Address;

typedef struct { //블록 구조체
	unsigned int addr; //메모리 주소를 저장하고
	bool isFull; //비어있는지 아닌지 상태
	int LRU;
}Block;

int main(void) {
	Address address[ARRAY_SIZE];
	int n = 0, s = 0, m = 0;
	int setindex_bit, wordoffset_bit;
	int way = 0;
	int hit = 0, miss = 0;

	printf("#of blocks in a set: ");
	scanf("%d", &n);
	printf("#of sets in a cache: ");
	scanf("%d", &s);
	printf("#of words in a block: ");
	scanf("%d", &m);
	printf("\n");

	setindex_bit = logB(s, 2);
	wordoffset_bit = logB(m, 2);

	printf("\n");
	printf("Set index: %d bits, ", setindex_bit);
	printf("Word offset: %d bits", wordoffset_bit);
	printf("\n");

	printf("Enter 32-bit address\n");
	for (int i = 0; i < ARRAY_SIZE; i++)
		scanf("%x", &(address[i].addr));

	way = n;
	printf("\n%d-way set associative...\n", way);

	Block * *cache;

	cache = (Block * *)malloc(sizeof(Block*) * s); //Block들이 모여서 cache가 된다.
	for (int i = 0; i < s; i++) {
		cache[i] = (Block*)malloc(sizeof(Block) * way);
	}

	for (int i = 0; i < s; i++) {
		for (int j = 0; j < way; j++) {
			cache[i][j].isFull = false;
			cache[i][j].LRU = 0;
		}
	}

	for (int i = 0; i < ARRAY_SIZE; i++) { //주소 정보 업데이트
		address[i].blockAddr = address[i].addr >> (wordoffset_bit + 2);
		address[i].wordAddr = address[i].addr >> 2;

		address[i].tag = address[i].addr >> (setindex_bit + wordoffset_bit + 2);
		address[i].Index = (address[i].addr & ((1 << (wordoffset_bit + 2)) * ((1 << setindex_bit) - 1))) >> (wordoffset_bit + 2);
		address[i].wordOffset = (address[i].addr & ((1 << (wordoffset_bit + 2)) - 4)) >> 2;
		address[i].byteOffset = address[i].addr & 0x00000003;

		address[i].setIndex = address[i].blockAddr % s;

		address[i].result = 0;
	}

	printf("\n");
	for (int i = 0; i < ARRAY_SIZE; i++) {
		printf("Set index of %x: %d\n", address[i].addr, address[i].Index);
	}

	for (int i = 0; i < ARRAY_SIZE; i++) {
		for (int j = 0; j < s; j++) { //블럭 LRU 업데이트
			for (int k = 0; k < way; k++) {
				if (cache[j][k].isFull)
					cache[j][k].LRU++;
			}
		}

		for (int j = 0; j < way; j++) {
			if (address[i].addr == cache[address[i].Index][j].addr) { //해당 set에서 같은 주소를 찾았으면
				printf("%x     hit\n", address[i].addr);
				hit += 1; //hit!
				cache[address[i].Index][j].LRU += 1; //그 블럭의 LRU 증가
				address[i].result = 1;
				break; //더 이상 탐색할 필요가 없으므로 반복문 탈출
			}
		}
		if (address[i].result == 0) { //위의 반복문에서 hit이 일어나지 않았다면
			printf("%x     miss\n", address[i].addr);
			miss += 1; //miss!
			address[i].result = 0;
			for (int j = 0; j < way; j++) { //비어있는 block이 있는지 확인
				if (cache[address[i].Index][j].isFull == true) { //block이 이미 채워져 있다면
					continue; //일단 진행
				}
				else { //비어있는 블록을 발견하면
					cache[address[i].Index][j].addr = address[i].addr; //채워 넣는다.
					break; //채워 넣었다면 반복문 탈출
				}

				if (j == way - 1) { //반복문을 다 돌았지만 비어있는 block을 찾지 못했다면(반복문을 탈출하지 못했다면)
					int max_LRU = 0;
					for (int k = 0; i < way; k++) { //LRU가 큰 블럭 탐색
						if (cache[address[i].Index][j].LRU > max_LRU)
							max_LRU = k;
					}
					if (max_LRU == 0) { //못찾았다면
						cache[address[i].Index][0].addr = address[i].addr; //set의 0번째 block에 넣기
						cache[address[i].Index][0].LRU = 0;
					}
					else { //찾았다면
						cache[address[i].Index][max_LRU].addr = address[i].addr;
						cache[address[i].Index][max_LRU].LRU = 0;
					}
				}
			}
		}
	}

	printf("hit: %d\n", hit);
	printf("miss: %d\n", miss);

	free(cache);
	return 0;
}