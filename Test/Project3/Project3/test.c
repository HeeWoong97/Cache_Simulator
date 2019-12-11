#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* args[]){
	int n = atoi(args[1]);
	printf("n: %d, log2(n): %f\n", n, log2(n));
	return 0;
}
