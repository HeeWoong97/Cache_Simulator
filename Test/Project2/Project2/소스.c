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

typedef struct { //��� ����ü
	unsigned int addr; //�޸� �ּҸ� �����ϰ�
	bool isFull; //����ִ��� �ƴ��� ����
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

	cache = (Block**)malloc(sizeof(Block*) * s); //Block���� �𿩼� cache�� �ȴ�.
	for (int i = 0; i < s; i++)
		cache[i] = (Block*)malloc(sizeof(Block) * way);

	for (int i = 0; i < s; i++) {
		for (int j = 0; i < way; j++)
			cache[s][way].isFull = false;
	}

	for (int i = 0; i < ARRAY_SIZE; i++) { //�ּ� ���� ������Ʈ
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
			if (cache[address[i].setIndex][j].addr == address[i].addr) { //�ش� set���� ���� �ּҸ� ã������
				hit += 1; //hit!
				address[i].LRU += 1; //LRU ����
				address[i].result = 1;
				break; //�� �̻� Ž���� �ʿ䰡 �����Ƿ� �ݺ��� Ż��
			}
		}
		if (address[i].result == 0) { //���� �ݺ������� hit�� �Ͼ�� �ʾҴٸ�
			miss += 1; //miss!
			address[i].result = 0;
			for (int j = 0; j < way; j++) { //����ִ� block�� �ִ��� Ȯ��
				if (cache[address[i].setIndex][j].isFull == true) { //block�� �̹� ä���� �ִٸ�
					continue; //�ϴ� ����
				}
				else { //����ִ� ����� �߰��ϸ�
					cache[address[i].setIndex][j].addr = address[i].addr; //ä�� �ִ´�.
					break; //ä�� �־��ٸ� �ݺ��� Ż��
				}

				if (j == way - 1) { //�ݺ����� �� �������� ����ִ� block�� ã�� ���ߴٸ�(�ݺ����� Ż������ ���ߴٸ�)
					//���� ��ҵ��� LRU Ȯ��
					//LRU�� ���� ū �༮�� change, ���ο� ����� LRU->0
					//��� ���ٸ� �� ó���� �ٲٱ�, ���ο� ����� LRU->0 
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