#include "Server.h"

// Globals
int GAME_RUNNING = 0; // 0 if the game is not running
pthread_mutex_t GAME_RUNNING_LOCK = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t GAME_RUNNING_COND = PTHREAD_COND_INITIALIZER;

pthread_mutex_t GAME_GRID_LOCK = PTHREAD_MUTEX_INITIALIZER;

SquirrelState *players[MIN_PLAYERS]; 	// List of all players
int WINNER = -1;

int main()
{
	// Dat variables
	char cmd_buff[5];
	char data_buff[MAX_MESSAGE_LENGTH];
	
	// IPC related
	int server_msgq_id; // Message queue ID of server
	
	// Game related
	int numPlayers = 0;
	
	int grid[GRID_SIZE * GRID_SIZE]; // Grid state
	memset(grid, 0, GRID_SIZE * GRID_SIZE * sizeof(int));
	int numAcorns = 0; // Number of acorns left on the grid
	
	printf("Server started.\n");
	
	// Do some initialization
	server_msgq_id = IPC_InitMessageQueue(SERVER_KEY_CHAR);
	
	// Wait for players
	
	numPlayers = WaitForPlayers(server_msgq_id, &numAcorns, grid);
	// Initialize game
	printf("Initializing game...\n");
	
	SeedRNG();
	while((numAcorns = rand() % (GRID_SIZE * GRID_SIZE * MAX_ACORNS_PER_CELL)) == 0); // Number of acorns left on the grid
	GenerateGrid(grid, GRID_SIZE * GRID_SIZE, numAcorns);
	
	char grid_string_buff[MAX_MESSAGE_LENGTH];
	GridToString(grid, grid_string_buff, MAX_MESSAGE_LENGTH);
	printf("Generated grid:\n%s\n(Number of acorns: %i)\n", grid_string_buff, numAcorns);
	
	// Start game
	printf("Game is starting.\n");
	
	// Start threads
	pthread_mutex_lock(&GAME_RUNNING_LOCK);
	GAME_RUNNING = 1;
	pthread_cond_broadcast(&GAME_RUNNING_COND);
	pthread_mutex_unlock(&GAME_RUNNING_LOCK);
	
	// Continually get messages from the message queue and send them off to the threads
	while(GAME_RUNNING)
	{
		int msgq_id;
		if((msgq_id = IPC_ReceiveMessage(server_msgq_id, cmd_buff, data_buff, 0, IPC_NOWAIT)) != -1)
		{
			// Find who this message belongs to
			int i;
			for(i = 0; i < numPlayers; ++i)
				if(players[i]->msgq_id == msgq_id)
				{
					// Lock to add a message
					pthread_mutex_lock(&players[i]->lmsgq_mutex);
			
					// Add it to the player's local message queue
					players[i]->lmsgq = enqueue(players[i]->lmsgq, cmd_buff, data_buff);
			
					// Let the thread know there is a new message
					pthread_cond_signal(&players[i]->lmsgq_cond);
					// Unlock
					pthread_mutex_unlock(&players[i]->lmsgq_mutex);
					break;
				}
	
			// If it doesn't belong to anyone then WHO CARES? WOO!!!!
		}
	}
	
	printf("Waiting for threads to finish executing.\n");
	
	int i = 0;
	while(i < MIN_PLAYERS)
	{
		pthread_cond_signal(&players[i]->lmsgq_cond);
		pthread_join(players[i++]->thread_id, NULL);
	}
	return 0;
}

void ExitServer(int msgq_id)
{
	printf("Shutting down server.\n");
	msgctl(msgq_id, IPC_RMID, NULL); // Remove the message queue
	exit(0);
}

int WaitForPlayers(int server_msgq_id, int *numAcorns, int grid[])
{
	char cmd_buff[5];
	char data_buff[MAX_MESSAGE_LENGTH];
	int numPlayers = 0;
	
	printf("Waiting for players.\n");
	while(numPlayers < MIN_PLAYERS)
	{
		IPC_ReceiveMessage(server_msgq_id, cmd_buff, data_buff, 0, 0);
		
		if(strncmp(cmd_buff, "JOIN", 4) == 0) // Valid JOIN command
		{
			// Store player information
			int sq_msgq_id = atoi(data_buff);
			
			int i;
			for(i = 0; i < numPlayers; ++i)
				if(sq_msgq_id == players[i]->msgq_id)
				{
					printf("Duplicate message queue ID detected. Exiting.\n");
					exit(1);
				}
			
			printf("Squirrel has joined the server! (id: %i)\n", sq_msgq_id);
			
			SquirrelState *player = malloc(sizeof(SquirrelState));
			
			player->lmsgq		= NULL;
			pthread_mutex_init(&player->lmsgq_mutex, NULL);
			pthread_cond_init(&player->lmsgq_cond, NULL);
			player->grid		= grid;
			player->numAcorns	= numAcorns;
			player->msgq_id		= sq_msgq_id;
			player->row			= rand() % GRID_SIZE;
			player->col			= rand() % GRID_SIZE;
			player->score		= 0;
			
			if(pthread_create(&player->thread_id, NULL, SquirrelThreadFunc, (void *)player))
			{
				printf("Could not create thread.\n");
				exit(1);
			}
			
			printf("Cute little squirrel egg layed at (%i, %i)\n", player->row, player->col);
			
			players[numPlayers++] = player;
		}
	}
	return numPlayers;
}

void *SquirrelThreadFunc(void *parameters)
{
	// Yay variable declarations
	SquirrelState *params = parameters;
	char grid_string_buff[MAX_MESSAGE_LENGTH];
	char cmd_buff[5];
	char data_buff[MAX_MESSAGE_LENGTH];
	
	// Just to let us know it's ready
	printf("Squirrel thread %lu waiting for game to start.\n", (unsigned long)pthread_self());
	
	// Grab the lock
	pthread_mutex_lock(&GAME_RUNNING_LOCK);
	// If game is not running, then wait for it to start
	if(!GAME_RUNNING)
		pthread_cond_wait(&GAME_RUNNING_COND, &GAME_RUNNING_LOCK);
	// Make sure to release the lock
	pthread_mutex_unlock(&GAME_RUNNING_LOCK);
	
	// Send a STRT message to the player
	GridToString(params->grid, grid_string_buff, MAX_MESSAGE_LENGTH);
	memset(data_buff, '\0', MAX_MESSAGE_LENGTH);
	snprintf(data_buff, MAX_MESSAGE_LENGTH, "%i%s%s", *params->numAcorns, MESSAGE_SUB_DELIM, grid_string_buff);
	IPC_SendMessage(params->msgq_id, "STRT", data_buff, MESSAGE_DELIM, MTYPE_STRT);
	
	// At the point the game is running
	while(*params->numAcorns > 0)
	{
		// Grab a lock on our queue
		pthread_mutex_lock(&params->lmsgq_mutex);
		// No messages, so wait for some to arrive
		if(params->lmsgq == NULL)
			pthread_cond_wait(&params->lmsgq_cond, &params->lmsgq_mutex);
		
		// There is now at least one message on the queue
		// First, check if the game is running
		pthread_mutex_lock(&GAME_RUNNING_LOCK);
		
		if(!GAME_RUNNING) // Don't need to wait because it won't affect us if it changes after this
		{
			pthread_mutex_unlock(&GAME_RUNNING_LOCK);
			break; // Game is over, let the squirrel know
		}
		pthread_mutex_unlock(&GAME_RUNNING_LOCK);
		
		// Game is not over

		char *msg;
		while(params->lmsgq != NULL) // Messages left
		{
			memset(cmd_buff, '\0', 5);
			memset(data_buff, '\0', MAX_MESSAGE_LENGTH);
			params->lmsgq = dequeue(params->lmsgq, cmd_buff, data_buff);
			if(strncmp(cmd_buff, "QUIT", 4) == 0) // QUIT
				return 0;
			if(strncmp(cmd_buff, "MOVE", 4) == 0) // Valid MOVE command
			{
				CardinalDirection direction;
				if(strncmp(data_buff, "UP", 2) == 0) // North
					direction = North;
				else if(strncmp(data_buff, "DN", 2) == 0) // South
					direction = South;
				else if(strncmp(data_buff, "LF", 2) == 0) // West
					direction = West;
				else if(strncmp(data_buff, "RT", 2) == 0) // East
					direction = East;
				else
					continue;
				
				pthread_mutex_lock(&GAME_GRID_LOCK);
				
				// Game logic
				int acorn_found = MoveSquirrel(params->grid, params, direction);
		
				char acorn_get;
				if(acorn_found)
				{
					--*params->numAcorns;
					acorn_get = 'Y';
					printf("Number of acorns remaining: %i\n", *params->numAcorns);
				}
				else
					acorn_get = 'N';
				// Send update to player
				char grid_string_buff[MAX_MESSAGE_LENGTH];
				GridToString(params->grid, grid_string_buff, MAX_MESSAGE_LENGTH);
				memset(data_buff, '\0', MAX_MESSAGE_LENGTH);
				snprintf(data_buff, MAX_MESSAGE_LENGTH, "%c%s%i%s%i%s%s", 
										acorn_get, MESSAGE_SUB_DELIM, 
										params->score, MESSAGE_SUB_DELIM, 
										*params->numAcorns, MESSAGE_SUB_DELIM, grid_string_buff);
				IPC_SendMessage(params->msgq_id, "ACKM", data_buff, MESSAGE_DELIM, MTYPE_ACKM);
				
				pthread_mutex_unlock(&GAME_GRID_LOCK);
			}
		}
		// Unlock dat shit
		pthread_mutex_unlock(&params->lmsgq_mutex);
	}
	
	// Lock so that we can set this up
	pthread_mutex_lock(&GAME_RUNNING_LOCK);
	GAME_RUNNING = 0;
	// Check winner
	if(WINNER == -1)
	{
		// Find winner
		int highscore = -1;
		int i;
		for(i = 0; i < MIN_PLAYERS && WINNER != -2; ++i)
			if(players[i]->score > highscore) // New high score relative to player list
			{
				WINNER = players[i]->msgq_id;
				highscore = players[i]->score;
			}
			else if(players[i]->score == highscore) // Tie!
				WINNER = -2;
	}
	
	pthread_mutex_unlock(&GAME_RUNNING_LOCK);
	
	memset(data_buff, '\0', MAX_MESSAGE_LENGTH);
	if(WINNER == params->msgq_id)
	{
		snprintf(data_buff, MAX_MESSAGE_LENGTH, "You won with %i acorns!\nYou're great at gobbling up those nuts!", params->score);
		IPC_SendMessage(params->msgq_id, "OVER", data_buff, MESSAGE_DELIM, MTYPE_OVER);
	}
	else if(WINNER == -2)
		IPC_SendMessage(params->msgq_id, "OVER", "Guess your nut gobbling has met its equal. (There was a tie.)", MESSAGE_DELIM, MTYPE_OVER);
	else
	{
		snprintf(data_buff, MAX_MESSAGE_LENGTH, "You lost with only %i acorns...\nYou suck nuts!", params->score);
		IPC_SendMessage(params->msgq_id, "OVER", data_buff, MESSAGE_DELIM, MTYPE_OVER);
	}
	
	printf("Thread %i terminating.\n", params->msgq_id);
	return 0;
}
