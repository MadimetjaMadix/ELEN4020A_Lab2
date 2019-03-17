#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "functions.h"

#define num_threads 8

void printTimeElasped(double start, double end)
{
	double time_taken = end - start; 
	printf("Total time taken by CPU: %f\n", time_taken);
	printf("================================\n");
}

void naiveAlgorithm(int *matrix, int N)
{	
	int i = 0;
	int j = 0;
	double start = 0.0, end = 0.0;
  
	start = omp_get_wtime();
	omp_set_num_threads(num_threads);

	#pragma omp parallel for private(i, j)
	for(i=0; i<N; ++i)
	{
		for(j=i+1; j<N; ++j)
		{
			int index[2] = {i,j};
			int offsetA = getElementLocation2D(index, N);
			index[0] = j;
			index[1] = i;			
			int offsetB = getElementLocation2D(index, N);
			swapElements(matrix+offsetA, matrix+offsetB);
		}
	}
	end = omp_get_wtime();
	printf("\n------ OpenMP Naive Algorithm ------\n");
	printTimeElasped(start, end);
	return;
}

void diagonalAlgorithm(int* matrix, int N)
{
	int diagonal_index = 0;
	double start = 0.0, end = 0.0;
  
	start = omp_get_wtime();
	omp_set_num_threads(num_threads);
	const int chunk_size = 10;
	#pragma omp parallel private(diagonal_index)
	{
		#pragma omp for schedule(dynamic, chunk_size) nowait
		for(diagonal_index = 0; diagonal_index < N-1; ++diagonal_index)
		{
			for(int column = diagonal_index+1; column < N; ++column)
			{
				int index[2] = {diagonal_index, column};
				int offsetA = getElementLocation2D(index, N);
				index[0] = column;
				index[1] = diagonal_index;			
				int offsetB = getElementLocation2D(index, N);
				swapElements(matrix+offsetA, matrix+offsetB);
			}
		}
	}
	end = omp_get_wtime();
	printf("\n------ OpenMP Diagonal Algorithm ------\n");
	printTimeElasped(start, end);
	return;
}

void runTransposeAlgorithms(int N)
{
	printMatrixSize(N);
	int *matrixA = createMarix(N);
	printf("\nNumber of threads: %d", (int)num_threads);
	naiveAlgorithm(matrixA,N);
	diagonalAlgorithm(matrixA, N);
	
	free(matrixA);
	return;
}

int main()
{
	srand(time(NULL));
	runTransposeAlgorithms(128);
	runTransposeAlgorithms(1024);
	runTransposeAlgorithms(2048);
	runTransposeAlgorithms(4096);
	return 0;
}
