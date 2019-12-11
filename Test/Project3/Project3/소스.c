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

typedef struct { //��� ����ü
	unsigned int addr; //�޸� �ּҸ� �����ϰ�
	bool isFull; //����ִ��� �ƴ��� ����
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

	cache = (Block * *)malloc(sizeof(Block*) * s); //Block���� �𿩼� cache�� �ȴ�.
	for (int i = 0; i < s; i++) {
		cache[i] = (Block*)malloc(sizeof(Block) * way);
	}

	for (int i = 0; i < s; i++) {
		for (int j = 0; j < way; j++) {
			cache[i][j].isFull = false;
			cache[i][j].LRU = 0;
		}
	}

	for (int i = 0; i < ARRAY_SIZE; i++) { //�ּ� ���� ������Ʈ
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
		for (int j = 0; j < s; j++) { //�� LRU ������Ʈ
			for (int k = 0; k < way; k++) {
				if (cache[j][k].isFull)
					cache[j][k].LRU++;
			}
		}

		for (int j = 0; j < way; j++) {
			if (address[i].addr == cache[address[i].Index][j].addr) { //�ش� set���� ���� �ּҸ� ã������
				printf("%x     hit\n", address[i].addr);
				hit += 1; //hit!
				cache[address[i].Index][j].LRU += 1; //�� ���� LRU ����
				address[i].result = 1;
				break; //�� �̻� Ž���� �ʿ䰡 �����Ƿ� �ݺ��� Ż��
			}
		}
		if (address[i].result == 0) { //���� �ݺ������� hit�� �Ͼ�� �ʾҴٸ�
			printf("%x     miss\n", address[i].addr);
			miss += 1; //miss!
			address[i].result = 0;
			for (int j = 0; j < way; j++) { //����ִ� block�� �ִ��� Ȯ��
				if (cache[address[i].Index][j].isFull == true) { //block�� �̹� ä���� �ִٸ�
					continue; //�ϴ� ����
				}
				else { //����ִ� ����� �߰��ϸ�
					cache[address[i].Index][j].addr = address[i].addr; //ä�� �ִ´�.
					break; //ä�� �־��ٸ� �ݺ��� Ż��
				}

				if (j == way - 1) { //�ݺ����� �� �������� ����ִ� block�� ã�� ���ߴٸ�(�ݺ����� Ż������ ���ߴٸ�)
					int max_LRU = 0;
					for (int k = 0; i < way; k++) { //LRU�� ū �� Ž��
						if (cache[address[i].Index][j].LRU > max_LRU)
							max_LRU = k;
					}
					if (max_LRU == 0) { //��ã�Ҵٸ�
						cache[address[i].Index][0].addr = address[i].addr; //set�� 0��° block�� �ֱ�
						cache[address[i].Index][0].LRU = 0;
					}
					else { //ã�Ҵٸ�
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