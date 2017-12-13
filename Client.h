#ifndef CLIENT_H
#define CLIENT_H

#include <ctype.h>
#include <curses.h>
#include "IPC.h"

/*
	Handles a movement string
	Parameters:
		input_buff - input string from user
		server_msgq_id - server's message queue ID
		client_msgq_id - client's message queue ID
*/
void HandleMovement(WINDOW *mainWindow, int keystroke, int server_msgq_id, int client_msgq_id);

/*
	Joins a server with a given message queue ID
	Parameters:
		server_msgq_id - message queue id for the server
		client_msgq_id - message queue id for the client
*/
void JoinServer(int server_msgq_id, int client_msgq_id);

/*
	Updates the text on the screen
	Parameters:
		gridString - string containing the properly formatted grid for printing
		numAcorns - number of acorns left in the game
		score - player's score
		acornFound - 0 if a acorn wasn't found
		additional - additional text to print before the grid.
*/
void UpdateScreen(char gridString[], int numAcorns, int score, int acornFound, char *additional);


/*
	Quits the game
	Parameters:
		msgq_id - message queue ID of the queue
		forceQuit - force an immediate quitting of the client if 1
*/
void QuitGame(WINDOW *mainWindow, int msgq_id, int forceQuit);

/*
	Checks if the game is over by checking for a message of mtype MTYPE_OVER in the message queue.
	If the game is over, it will print out the resultant text.
	Parameters:
		client_msgq_id - message queue to check
*/
int CheckGameOver(int client_msgq_id);

#endif
