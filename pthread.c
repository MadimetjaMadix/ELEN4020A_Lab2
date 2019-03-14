#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdbool.h>
#include "functions.h"

#define num_threads 5
int next_Diag = 0;
int N = 4096;
pthread_mutex_t updateDiagLock; // synchronize updates of next diagonal

typedef struct{
	int diagIndex, ID;
	int *matrix;
}ThreadData;

void *diagonalAlg(void *arg)
{
	ThreadData* thread_data = (ThreadData*)arg;
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
		if(next_Diag==N-1) thread_data->diagIndex = N-1;
		else if(next_Diag < N-1) 
		{
			
			thread_data->diagIndex = next_Diag;
			++next_Diag;
		}
		pthread_mutex_unlock(&updateDiagLock);
		
		if(thread_data->diagIndex == N-1) break;
	}	
	
	pthread_exit((void *) 0);	
}

int main()
{
	struct timeval start, end;
	int rc,i;
	srand(time(0));
	void *status;
	
	int *matrixA = createMarix(N);
	
	pthread_t threads[num_threads];
	ThreadData threads_data[num_threads];
	
	next_Diag = (int)num_threads;
	
	//start timer
	gettimeofday(&start, NULL);
	
	for(i=0; i<num_threads;++i)
	{
		threads_data[i].ID = i;
		threads_data[i].diagIndex = i;
		threads_data[i].matrix = matrixA;
		
		rc = pthread_create(&threads[i], NULL, diagonalAlg, &threads_data[i]);
		
		if(rc)
		{
			printf("ERROR creating thread");
			exit(-1);
		}
	}
	
	//printf("%d\n",i);
	for(int j = 0; i < num_threads; ++j) pthread_join(threads[j], NULL);
	
	//end timer
	gettimeofday(&end, NULL);
	double time_taken; 
    time_taken = (end.tv_sec - start.tv_sec) * 1e6; 
    time_taken = (time_taken + (end.tv_usec -  start.tv_usec)) * 1e-6;
	printf("Total time taken by CPU: %f\n", time_taken  );

	return 0;
}
