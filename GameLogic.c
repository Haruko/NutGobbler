#include "GameLogic.h"

int MoveSquirrel(int grid[], SquirrelState *squirrel, CardinalDirection direction)
{
	if(direction == North)
		squirrel->row = (squirrel->row - 1) % GRID_SIZE;
	else if(direction == South)
		squirrel->row = (squirrel->row + 1) % GRID_SIZE;
	else if(direction == East)
		squirrel->col = (squirrel->col + 1) % GRID_SIZE;
	else if(direction == West)
		squirrel->col = (squirrel->col - 1) % GRID_SIZE;
	
	while(squirrel->row < 0)
		squirrel->row += GRID_SIZE;
	while(squirrel->col < 0)
		squirrel->col += GRID_SIZE;
	
	//printf("Moved to (%i, %i)\n", squirrel->row, squirrel->col);
	
	if(RemoveAcorn(grid, squirrel->row, squirrel->col))
	{
		(squirrel->score)++;
		return 1;
	}
	return 0;
}

int RemoveAcorn(int grid[], int row, int col)
{
	if(grid[row * GRID_SIZE + col] > 0) // There is at least one acorn there
	{
		(grid[row * GRID_SIZE + col])--; // Remove one acorn from the cell
		return 1;
	}
	return 0;
}

void GridToString(int grid[], char outBuffer[], int outBufferSize)
{
	// Clear the array first
	memset(outBuffer, '\0', sizeof(char) * outBufferSize);
	
	int maxDigits = (int)log10(MAX_ACORNS_PER_CELL) + 1;
	int cell;
	for(cell = 0; cell < GRID_SIZE * GRID_SIZE; ++cell)
	{
		// Insert "# " or "#\n"
		
		char digits[maxDigits + 2]; // maxDigits + ('\n' or ' ') + terminator
		
		int numChars = 0; // Number of characters we will need to write
		if(cell % GRID_SIZE == GRID_SIZE - 1) // Last column to print so new line!
			numChars = snprintf(digits, maxDigits + 2, "%i\n", grid[cell]);
		else
			numChars = snprintf(digits, maxDigits + 2,"%i ", grid[cell]);
		
		if(strlen(outBuffer) + numChars + 1 > outBufferSize) // Don't want to segfault, +1 for the terminator
		{
			perror("GameLogic.c::GridToString() : Grid size too big or too many acorns allowed per cell\n");
			exit(1);
		}
		
		char paddedDigits[maxDigits + 2];
		PadString(digits, numChars + 1, paddedDigits, maxDigits + 2);
		
		// Copy characters into string
		strncat(outBuffer, paddedDigits, outBufferSize);
	}
}

void GenerateGrid(int gridBuffer[], int gridBufferSize, int numAcorns)
{
	// Wrong grid buffer size
	if(gridBufferSize != GRID_SIZE * GRID_SIZE)
	{
		perror("GameLogic.c::GenerateGrid() : gridBufferSize != GRID_SIZE * GRID_SIZE");
		exit(1);
	}
	
	// Too many or too few acorns!
	if(numAcorns > GRID_SIZE * GRID_SIZE * MAX_ACORNS_PER_CELL || numAcorns < 0)
	{
		perror("GameLogic.c::GenerateGrid() : numAcorns > GRID_SIZE * GRID_SIZE * MAX_ACORNS_PER_CELL || numAcorns < 0");
		exit(1);
	}
	
	// Make sure there is no garbage in here
	memset(gridBuffer, 0, sizeof(int) * GRID_SIZE * GRID_SIZE);
	
	// Yay efficiency
	if(numAcorns == GRID_SIZE * GRID_SIZE * MAX_ACORNS_PER_CELL)
	{
		int i;
		for(i = 0; i < GRID_SIZE * GRID_SIZE; ++i)
			gridBuffer[i] = MAX_ACORNS_PER_CELL;
	}
	else if(numAcorns != 0)// Nontrivial so gogogo!
	{
		// Generate grid
		while(numAcorns > 0)
		{
			// Randomize the cell number and place an acorn there if we have the space for it
			int cell = rand() % (GRID_SIZE * GRID_SIZE);
			if(gridBuffer[cell] < MAX_ACORNS_PER_CELL)
			{
				gridBuffer[cell]++;
				--numAcorns;
			}
		}
	}
}
