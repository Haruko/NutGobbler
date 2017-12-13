#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

#include "GameLogic.h"
#include "IPC.h"
#include "Shared.h"

#define MIN_PLAYERS 2

/*
	Shut down the server.
	Parameters:
		msgq_id - message queue ID to delete
*/
void ExitServer(int msgq_id);

/*
	Waits for player. duh
	Parameters:
		server_msgq_id - message queue ID to delete
		numAcorns - pointer to the variable for putting into the thread params
		grid - pointer to the variable for putting into the thread params
		players - list of players to store them and stuff
	Return:
		number of players who joined
*/
int WaitForPlayers(int server_msgq_id, int *numAcorns, int grid[]);

/*
	Function that the squirrel thread runs
*/
void *SquirrelThreadFunc(void *parameters);

#endif
