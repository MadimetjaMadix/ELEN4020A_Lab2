#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include "functions.h"

#define num_threads 8
int next_Diag = 0;

pthread_mutex_t updateDiagLock; // synchronize updates of next diagonal

typedef struct{
	int diagIndex, ID;
	int *matrix;
	int N;
}ThreadDataDiagonal;


void *diagonalAlg(void *arg)
{
	ThreadDataDiagonal* thread_data = (ThreadDataDiagonal*)arg;
	int N = thread_data->N;
	while(true)
	{
		for(int j=thread_data->diagIndex+1; j<N; ++j)
		{
			int index[2] = {thread_data->diagIndex,j};
			int offsetA = getElementLocation2D(index, N);
			index[0] = j;
			index[1] = thread_data->diagIndex;		
			int offsetB = getElementLocation2D(index, N);
			swapElements(thread_data->matrix+offsetA, thread_data->matrix+offsetB);
		}
		
		pthread_mutex_lock(&updateDiagLock);
		if(next_Diag < N-1) 
		{
			thread_data->diagIndex = next_Diag;
			++next_Diag;
		} else thread_data->diagIndex = N-1;
		pthread_mutex_unlock(&updateDiagLock);
		
		if(thread_data->diagIndex == N-1) break;
	}	
	
	pthread_exit((void *) 0);	
}

void printTimeElasped(struct timeval start, struct timeval end)
{
	double time_taken = (end.tv_sec - start.tv_sec) * 1e6; 
    time_taken = (time_taken + (end.tv_usec -  start.tv_usec)) * 1e-6;
	printf(" %8f", time_taken);

}

void runDiagonalAlgorithm(int* matrix, int N)
{
	struct timeval start, end;
	int rc;
	
	// Create threads:
	pthread_t threads[num_threads];
	ThreadDataDiagonal threads_data[num_threads];
	
	next_Diag = (int)num_threads;
	
	//start timer
	gettimeofday(&start, NULL);
	
	for(int i=0; i < num_threads; ++i)
	{
		threads_data[i].ID        = i;
		threads_data[i].diagIndex = i;
		threads_data[i].matrix    = matrix;
		threads_data[i].N         = N;
		
		rc = pthread_create(&threads[i], NULL, diagonalAlg, &threads_data[i]);
		
		if(rc)
		{
			printf("ERROR creating thread");
			exit(-1);
		}
	}
	
	for(int j = 0; j < num_threads; ++j) pthread_join(threads[j], NULL);
	
	//end timer
	gettimeofday(&end, NULL);
	printTimeElasped(start, end);
}

//===============================BLOCK 1==========================================
typedef struct
{
	int ID;
	int row;
	int column;
	int N;
	int* matrix;
}ThreadDataBlock;

int number_of_assigned_blocks       = 0;
int next_block_column               = 0;
int next_block_diagonal	            = 0;
pthread_mutex_t computeNextBlockLock;

/*
	Function implementing block transposition.
*/
void computeNextBlock(int N)
{
	++next_block_column;
	int N_block = N/2;
	if(next_block_column % N_block == 0 && next_block_column != 0)
	{
		++next_block_diagonal;
		next_block_column = next_block_diagonal;	
	}

	if(next_block_diagonal >= N_block && next_block_column >= N_block)
	{
		next_block_diagonal = N_block;
		next_block_column = N_block;
	}else number_of_assigned_blocks++;
}

void* blockAlgorithm(void* arg)
{
	ThreadDataBlock* thread_data = (ThreadDataBlock*)arg;
	int N       = thread_data->N;
	int N_block = N/2;
	while(true)
	{
		int index[2] = {thread_data->row, thread_data->column};
		int* blockA_indexes = blockElementsIndex2(index, N);
		blockTranspose(thread_data->matrix, blockA_indexes);
		
		if(thread_data->row != thread_data->column) // Not On diagonal:
		{
			index[0] = thread_data->column;
			index[1] = thread_data->row;
			
			int* blockB_indexes = blockElementsIndex2(index, N);
			blockTranspose(thread_data->matrix, blockB_indexes);
			
			// Swap blocks:
			swapBlocks(thread_data->matrix, blockA_indexes, blockB_indexes);
			
			free(blockB_indexes);
		}
		free(blockA_indexes);
		
		pthread_mutex_lock(&computeNextBlockLock);
		
			thread_data->row    = next_block_diagonal;
			thread_data->column = next_block_column;

			computeNextBlock(N);
			
		pthread_mutex_unlock(&computeNextBlockLock);
		
		if(thread_data->row == N_block && thread_data->column == N_block) break;
	}
	
	pthread_exit((void *) 0);
}

void runBlockTransposeAlgorithm(int* matrix, int N)
{
	struct timeval start, end;
	int rc;
	int N_block = N/2;
	
	// Create threads:
	pthread_t threads[num_threads];
	ThreadDataBlock threads_data[num_threads];
	
	for(int i = 0; i < (int)num_threads; ++i)
		computeNextBlock(N);
	
	//start timer
	gettimeofday(&start, NULL);
	int row     = 0;
	int column  = 0;
	for(int i = 0; i < num_threads; ++i)
	{
		threads_data[i].ID = i;
		if(column % N_block == 0 && column != 0)
		{
			++row;
			column = row;	
		}
		
		if(row >= N_block && column >= N_block)
		{
			row    = N_block;
			column = N_block;
		}
		
		threads_data[i].row     = row;
		threads_data[i].column  = column++;
		threads_data[i].matrix  = matrix;
		threads_data[i].N       = N;
		
		rc = pthread_create(&threads[i], NULL, blockAlgorithm, &threads_data[i]);
		
		if(rc)
		{
			printf("ERROR creating thread");
			exit(-1);
		}
	
	}
	
	for(int j = 0; j < num_threads; ++j) pthread_join(threads[j], NULL);
	
	//end timer
	gettimeofday(&end, NULL);
	printTimeElasped(start, end);
	
	return;
}
//===============================BLOCK 2==========================================

typedef struct{
	int diagIndex, ID;
	int *matrix;
	int N,blockID;
}ThreadDataBlock2;

int next_DiagBlock = 0;
pthread_mutex_t updateDiagBlockLock; // synchronize updates of next block diagonal


void *blockTransposeAlg2(void *arg)
{
	ThreadDataBlock2* thread_data = (ThreadDataBlock2*)arg;
	int N = thread_data->N;
	int N_blocks = N/2;
	
	//swap blocks
	while(true)
	{
		for(int j=thread_data->diagIndex; j<N_blocks; ++j)
		{
			int index[2] = {thread_data->diagIndex,j};
			int blockAIndex = getElementLocation2D(index, N_blocks);
			int *blockAElementsIndex = blockElementsIndex( blockAIndex, N);
			blockTranspose(thread_data->matrix, blockAElementsIndex);
			
			if(j!=thread_data->diagIndex)
			{
				index[0] = j;
				index[1] = thread_data->diagIndex;		


				int blockBIndex = getElementLocation2D(index, N_blocks);
				int *blockBElementsIndex = blockElementsIndex( blockBIndex, N);
				blockTranspose(thread_data->matrix, blockBElementsIndex);

				swapBlocks(thread_data->matrix, blockAElementsIndex, blockBElementsIndex);
				
				free(blockBElementsIndex);
			}
			free(blockAElementsIndex);
		}//end for
		
		pthread_mutex_lock(&updateDiagBlockLock);
		if(next_DiagBlock < N_blocks-1) 
		{
			thread_data->diagIndex = next_DiagBlock;
			++next_DiagBlock;
		} else thread_data->diagIndex = N_blocks-1;
		pthread_mutex_unlock(&updateDiagBlockLock);
		
		if(thread_data->diagIndex == N_blocks-1)break;
		
	}//while
	
	pthread_exit((void *) 0);	
}

void runBlockTransposeAlgorithm2(int* matrix, int N)
{
	struct timeval start, end;
	int rc;
	
	// Create threads:
	pthread_t threads[num_threads];
	ThreadDataBlock2 threads_data[num_threads];
	
	next_DiagBlock = (int)num_threads;
	//start timer
	gettimeofday(&start, NULL);
	
	for(int i=0; i < num_threads; ++i)
	{
		threads_data[i].ID        = i;
		threads_data[i].diagIndex = i;
		threads_data[i].matrix    = matrix;
		threads_data[i].N         = N;
		threads_data[i].blockID	  = i;
		
		rc = pthread_create(&threads[i], NULL, blockTransposeAlg2, &threads_data[i]);
		
		if(rc)
		{
			printf("ERROR creating thread");
			exit(-1);
		}
	}
	
	for(int j = 0; j < num_threads; ++j) pthread_join(threads[j], NULL);
	
	//end timer
	gettimeofday(&end, NULL);
	printTimeElasped(start, end);
}

void printTableHeadings()
{
	printf("\n%5s %8s %8s %8s\n", "N   |" , "Diagonal", "Block 1", "Block 2");
	return;
}

void runTransposeAlgorithms(int N)
{
	int *matrixA = createMarix(N);
	printf("%4d|", N);
	runDiagonalAlgorithm(matrixA, N);
	runBlockTransposeAlgorithm(matrixA, N);
	runBlockTransposeAlgorithm2(matrixA, N);
	printf("\n");
	free(matrixA);
	return;
}

int main()
{
	srand(time(NULL));

	printHeader();
	printf("\nPThread Algorithms Parallel");
	printf("\nNumber of threads: %d\n", (int)num_threads);
	printTableHeadings();
	runTransposeAlgorithms(128);
	runTransposeAlgorithms(1024);
	runTransposeAlgorithms(2048);
	runTransposeAlgorithms(4096);

	return 0;
}
