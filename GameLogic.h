#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "ll.h"
#include "Shared.h"

#define GRID_SIZE 2					// Maximum grid size GRID_SIZExGRID_SIZE
#define MAX_ACORNS_PER_CELL 2		// Maximum number of acorns per cell

typedef enum 
{
	North = 0,
	South = 1,
	East  = 2,
	West  = 4
} CardinalDirection;

typedef struct
{
	// Threading
	pthread_t thread_id;		// Squirrel's thread id
	
	struct node *lmsgq;				// Linked list head node for local message queue
	pthread_mutex_t lmsgq_mutex;// Mutex for the message queue
	pthread_cond_t lmsgq_cond;	// Conditional for the message queue
	
	// Game info
	int *grid;
	int *numAcorns;
	
	// Player info
	int msgq_id;				// Squirrel's message queue id
	
	int row, col;				// Position of squirrel
	int score;					// Number of acorns collected
} SquirrelState;

/*
	Moves a squirrel based on the cardinal direction given
	Parameters:
		grid - grid to read from
		squirrel - Squirrel that is moving
		direction - Cardinal direction to move
	Return:
		true if an acorn was found in the new location (it will update the user's score accordingly)
*/
int MoveSquirrel(int grid[], SquirrelState *squirrel, CardinalDirection direction);

/*
	Remove an acorn from the grid
	Parameters:
		grid - grid to read from
		row, col - coordinates of acorn to remove
	Return:
		true if an acorn was found in the new location (it will update the grid accordingly)
*/
int RemoveAcorn(int grid[], int row, int col);

/*
	Convert the grid to a string for sending in a message. Cells in the same row are delimited by spaces and at the end of each row there is a \n
	Parameters:
		grid - grid to read from
		outBuffer - string buffer to write to
		outBufferSize - size of the buffer to make sure we can hold the data properly
*/
void GridToString(int grid[], char outBuffer[], int outBufferSize);

/*
	Generates a grid
	Parameters:
		gridBuffer - grid buffer to write to
		gridBufferSize - size of the grid buffer to make sure we can hold the data properly
		numAcorns - number of acorns to place into the grid
*/
void GenerateGrid(int gridBuffer[], int gridBufferSize, int numAcorns);

#endif
