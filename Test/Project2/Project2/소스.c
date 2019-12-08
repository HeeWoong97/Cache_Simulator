#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable: 4996)

#define ARRAY_SIZE 2

typedef int bool;
	#define false 0	
	#define true 1

typedef struct {
	unsigned int addr;
	unsigned int tag, index, wordOffset, byteOffset;
	unsigned int blockAddr, wordAddr;
	int setIndex;
	int LRU;
	int result; //1->hit, 0->miss
}Address;

typedef struct { //블록 구조체
	unsigned int addr; //메모리 주소를 저장하고
	bool isFull; //비어있는지 아닌지 상태
}Block;

void Print_Binary(unsigned int n);

int main(void) {
	Address address[ARRAY_SIZE];
	int n = 0, s = 0, m = 0;
	int way;
	//int** cache; bool** isFull;
	int hit = 0, miss = 0;

	printf("n(#of cache blocks): ");
	scanf("%d", &n);
	printf("s(#of sets in the cache): ");
	scanf("%d", &s);
	printf("m(block size): ");
	scanf("%d", &m);
	printf("\n");

	printf("Enter 32-bit address\n");
	for (int i = 0; i < ARRAY_SIZE; i++)
		scanf("%x", &(address[i].addr));

	way = (n + s - 1) / s;
	printf("\n%d-way set associative...\n", way);

	Block** cache;

	cache = (Block**)malloc(sizeof(Block*) * s); //Block들이 모여서 cache가 된다.
	for (int i = 0; i < s; i++)
		cache[i] = (Block*)malloc(sizeof(Block) * way);

	for (int i = 0; i < s; i++) {
		for (int j = 0; i < way; j++)
			cache[s][way].isFull = false;
	}

	for (int i = 0; i < ARRAY_SIZE; i++) { //주소 정보 업데이트
		address[i].blockAddr = address[i].addr >> (m + 2);
		address[i].wordAddr = address[i].addr >> 2;

		address[i].tag = address[i].addr >> (s + m + 2);
		address[i].index = (address[i].addr & ((1 << (m + 2)) * ((1 << s) - 1))) >> (m + 2);
		address[i].wordOffset = (address[i].addr & ((1 << (m + 2)) - 4)) >> 2;
		address[i].byteOffset = address[i].addr & 0x00000003;

		address[i].setIndex = address[i].blockAddr % s;

		address[i].LRU = 0;
		address[i].result = 0;
	}

	printf("\n");
	for (int i = 0; i < ARRAY_SIZE; i++) {
		printf("Set index of %x: %d\n", address[i].addr, address[i].setIndex);
	}

	for (int i = 0; i < ARRAY_SIZE; i++) {
		for (int j = 0; j < way; j++) {
			if (cache[address[i].setIndex][j].addr == address[i].addr) { //해당 set에서 같은 주소를 찾았으면
				hit += 1; //hit!
				address[i].LRU += 1; //LRU 증가
				address[i].result = 1;
				break; //더 이상 탐색할 필요가 없으므로 반복문 탈출
			}
		}
		if (address[i].result == 0) { //위의 반복문에서 hit이 일어나지 않았다면
			miss += 1; //miss!
			address[i].result = 0;
			for (int j = 0; j < way; j++) { //비어있는 block이 있는지 확인
				if (cache[address[i].setIndex][j].isFull == true) { //block이 이미 채워져 있다면
					continue; //일단 진행
				}
				else { //비어있는 블록을 발견하면
					cache[address[i].setIndex][j].addr = address[i].addr; //채워 넣는다.
					break; //채워 넣었다면 반복문 탈출
				}

				if (j == way - 1) { //반복문을 다 돌았지만 비어있는 block을 찾지 못했다면(반복문을 탈출하지 못했다면)
					//구성 요소들의 LRU 확인
					//LRU가 가장 큰 녀석과 change, 새로운 요소의 LRU->0
					//모두 같다면 맨 처음꺼 바꾸기, 새로운 요소의 LRU->0 
				}
			}
		}
		//
	}
	
	free(cache);
	return 0;
}

void Print_Binary(unsigned int n) {
	for (int i = 0; i < 32; i++, n <<= 1) {
		if (n & 0x80000000) printf("1");
		else printf("0");
	}
}