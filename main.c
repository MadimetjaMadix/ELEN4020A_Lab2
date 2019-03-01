#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int* createMarix(int N)
{	
	int number_of_elements = N*N;
	int *result = (int*)calloc(number_of_elements,sizeof(int));
	int *matrix = result;
	
	for(int i=0; i<number_of_elements; ++i)
	{
		*matrix = rand()%N;
		matrix++;
	}

	return result;
}

/*
	This function returns the mapped position in the 1D array for the element
	located at the index in the 2D array.
*/
int getElementLocation2D(int index[2], int N)
{
	int row    = index[0];
	int column = index[1];

	return ((row*N)+column);
}

/*
	This function returns the value stored in the element at the specified index.
	index is a 1D array with to elements. It contains the desired row and the 
	desired column.
*/

int retrieveElement2D(int* matrix, int index[2], int N)
{
	return *(matrix+getElementLocation2D(index, N));
}

void swapElements(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void naiveAlgorithm(int *matrix, int N)
{
	int temp = 0;
	for(int i=0; i<N; ++i)
	{
		for(int j=i; j<N; ++j)
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
void print2DMatrix(int *matrix, int N)
{
	if(matrix != NULL)
	{	
		printf("\n");
		for(int i=0; i< N*N; ++i)
		{
			if( i%N == 0) printf("\n");
	
			printf("%4d ", *matrix);
			++matrix;
		}
		printf("\n");

	}else printf(" The Matrix is empty");
	
}

int main()
{
	struct timeval start, end;
	
	int N = 128;
	srand(time(0));
	
	int *matrixA = createMarix(N);
	
	//start timer
	gettimeofday(&start, NULL);
	naiveAlgorithm(matrixA,N);
	//end timer
	gettimeofday(&end, NULL);


	double time_taken; 
  
    time_taken = (end.tv_sec - start.tv_sec) * 1e6; 
    time_taken = (time_taken + (end.tv_usec -  
                              start.tv_usec)) * 1e-6;
	printf("Total time taken by CPU: %f\n", time_taken  );



	return 0;

}
