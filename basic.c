#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "functions.h"

void naiveAlgorithm(int *matrix, int N)
{
	struct timeval start, end;
	gettimeofday(&start, NULL);

	for(int i=0; i<N; ++i)
	{
		for(int j=i+1; j<N; ++j)
		{
			int index[2] = {i,j};
			int offsetA = getElementLocation2D(index, N);
			index[0] = j;
			index[1] = i;			
			int offsetB = getElementLocation2D(index, N);
			swapElements(matrix+offsetA, matrix+offsetB);
		}
	}
	gettimeofday(&end, NULL);

	double time_taken; 
    time_taken = (end.tv_sec - start.tv_sec) * 1e6; 
    time_taken = (time_taken + (end.tv_usec -  
                              start.tv_usec)) * 1e-6;
	printf("%10f ", time_taken);
	return;
}

void runTransposeAlgorithm(int N)
{
	int *matrixA = createMarix(N);
	printf("%4d|", N);
	naiveAlgorithm(matrixA, N);
	printf("\n");
	free(matrixA);
	return;
}

void printTableHeadings()
{
	printf("\n%5s %9s\n", "N  |", "Time");
	return;
}

int main()
{
	srand(time(NULL));

	printHeader();
	printf("\n Basic Algorithm Serial");
	printTableHeadings();
	
	runTransposeAlgorithm(128);
	runTransposeAlgorithm(1024);
	runTransposeAlgorithm(2048);
	runTransposeAlgorithm(4096);
	printf("\n");
	return 0;
}
