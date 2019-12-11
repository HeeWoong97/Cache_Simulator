#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_BUFF_SIZE	1024

int main(int argc, char* args[]) {
	srand((unsigned int)time(NULL));
	const char* HIT_MISS[] = { "hit", "miss" };
	FILE* fp = fopen("test.txt", "r");
	char buff[MAX_BUFF_SIZE] = { 0 };
	system("rm ./result.txt");
	if (fp) {
		while (1) {
			int addr = 0;
			if(fscanf(fp, "%x", &addr)==EOF)
				break;
			sprintf(buff, "0x%08x %s\n", addr, HIT_MISS[rand()%2]);
			printf("%s", buff);
			FILE* fp2 = fopen("result.txt", "a");
			fprintf(fp2, "%s", buff);
			fclose(fp2);
		}
		fclose(fp);
	}
	else printf("ERROR!\n");
	return 0;
}