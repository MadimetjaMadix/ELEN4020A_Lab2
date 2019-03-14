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
	printf("\n------ Diagonal Algorithm ------\n");
	printf("Total time taken by CPU: %f\n", time_taken);
	printf("================================\n");

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

int main()
{
	srand(time(NULL));
	int N = 4096;
	int *matrixA = createMarix(N);
	
	printf("No. of threads = %d \n", (int)num_threads);
	
	runDiagonalAlgorithm(matrixA, N);
	
	
	return 0;
}
