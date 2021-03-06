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
	When the matrix is spilt into blocks of size 2x2, a function was needed 
	to map a 2D index of a block to a 1D index.
	This function returns the mapped position in the 1D array indexing for a 
	2x2 block located at a 2D index.
*/
int getBlockLocation2D(int index[2], int N_matrix)
{
	int N_blocks = N_matrix/2;
	return getElementLocation2D(index, N_blocks);
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



int *blockElementsIndex(int blockIndex, int N_matrix)
{	
	int Num_Blocks = N_matrix/2;
	int *blockElemIndex = (int*)calloc(4,sizeof(int));
	
	*blockElemIndex =  (blockIndex*2)+ ((int)(blockIndex/Num_Blocks))*N_matrix;
	blockElemIndex[1]=*blockElemIndex+1; 
	blockElemIndex[2]=*blockElemIndex+N_matrix;
	blockElemIndex[3]=*blockElemIndex+(N_matrix+1);

	return blockElemIndex;
}


/*
	Retrieves all the elements in a 2x2 block.Given a 2D index.
*/
int *blockElementsIndex2(int index[2], int N_matrix)
{
	int blockIndex = getBlockLocation2D(index, N_matrix); 
	return blockElementsIndex(blockIndex, N_matrix);
}

void swapElements(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void swapBlocks(int *matrix, int *indexBlockA, int *indexBlockB)
{	
	swapElements(matrix+indexBlockA[0], matrix+indexBlockB[0]);
	swapElements(matrix+indexBlockA[1], matrix+indexBlockB[1]);
	swapElements(matrix+indexBlockA[2], matrix+indexBlockB[2]);
	swapElements(matrix+indexBlockA[3], matrix+indexBlockB[3]);
}

void blockTranspose(int *matrix, int *indexBlockA)
{
	swapElements(matrix+indexBlockA[1], matrix+indexBlockA[2]);
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

void printHeader()
{
	printf("       * ELEN4020A Data Intensive Computing: Laboratory 2 * \n");
    printf("       ****************************************************\n");
    printf("             +--------------------+----------------+ \n");
    printf("             |    Student Name    | Student Number | \n");
    printf("             +--------------------+----------------+ \n");
    printf("             | Lynch Mwaniki      |        1043475 | \n");
    printf("             | Madimetja Sethosa  |        1076467 | \n");
    printf("             | Teboho Matsheke    |        1157717 | \n");
    printf("             +--------------------+----------------+ \n");
}

#endif
