#ifndef FUNCTIONS_H
#define FUNCTIONS_H

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
#endif