#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "functions.h"

#define num_threads 8

void printTimeElasped(double start, double end)
{
	double time_taken = end - start; 
	printf(" %8f", time_taken);
	return;
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
	printTimeElasped(start, end);
	return;
}

void blockTransposeAlg(int *matrix, int N)
{
	double start = 0.0, end = 0.0;
	int N_blocks = N/2,i,j;
	
	start = omp_get_wtime();
	omp_set_num_threads(num_threads);
	#pragma omp parallel private(i,j)
	{
		#pragma omp for schedule(dynamic)
		for(i=0; i<N_blocks; ++i)
		{
			for(j=i; j<N_blocks; ++j)
			{
				int index[2] = {i,j};
				int *blockAElementsIndex = blockElementsIndex2( index, N);
				blockTranspose(matrix, blockAElementsIndex);
		
				if(j!=i)
				{
					index[0] = j;
					index[1] = i;		

					int *blockBElementsIndex = blockElementsIndex2( index, N);
					blockTranspose(matrix, blockBElementsIndex);

					swapBlocks(matrix, blockAElementsIndex, blockBElementsIndex);
					free(blockBElementsIndex);
				}
				free(blockAElementsIndex);
			}
		}
	}
	end = omp_get_wtime();
	printTimeElasped(start, end);
			
}

void printTableHeadings()
{
	printf("\n%5s %8s %8s %8s \n", "N   |" , "Naive", "Diagonal", "Block");
	return;
}

void runTransposeAlgorithms(int N)
{
	int *matrixA = createMarix(N);
	printf("%4d|", N);
	naiveAlgorithm(matrixA,N);
	diagonalAlgorithm(matrixA, N);
	blockTransposeAlg(matrixA, N);
	printf("\n");
	free(matrixA);
	return;
}

int main()
{
	srand(time(NULL));
	printHeader();
	printf("\nOpenMP Algorithms Parallel");
	printf("\nNumber of threads: %d \n", (int)num_threads);
	printTableHeadings();
	runTransposeAlgorithms(128);
	runTransposeAlgorithms(1024);
	runTransposeAlgorithms(2048);
	runTransposeAlgorithms(4096);
	return 0;
}
