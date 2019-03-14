#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>
#include "functions.h"

void naiveAlgorithm(int *matrix, int N)
{	
	int i = 0;
	int j = 0;
	omp_set_num_threads(8);
	#pragma omp parallel private(i, j)
	{
		#pragma omp for schedule(auto) nowait
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
	}
	
}

int main()
{
	struct timeval start, end;
	
	int N = 6;
	srand(time(NULL));
	
	int *matrixA = createMarix(N);
	print2DMatrix(matrixA, N);

	//start timer
	gettimeofday(&start, NULL);
	naiveAlgorithm(matrixA,N);
	//end timer
	gettimeofday(&end, NULL);
	print2DMatrix(matrixA, N);
	double time_taken; 
  
    time_taken = (end.tv_sec - start.tv_sec) * 1e6; 
    time_taken = (time_taken + (end.tv_usec -  
                              start.tv_usec)) * 1e-6;
	printf("Total time taken by CPU: %f\n", time_taken  );



	return 0;

}
