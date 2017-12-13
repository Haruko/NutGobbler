#include "Client.h"

int main()
{
	WINDOW *mainWindow;
	
	// Initialize ncurses
    if((mainWindow = initscr()) == NULL)
    {
		perror("Error initializing ncurses.\n");
		exit(1);
    }
	
	int client_msgq_id;
	int server_msgq_id;
	
	SeedRNG();
	if((client_msgq_id = IPC_InitMessageQueue((unsigned char)rand())) == -1)
	{
		printw("Cannot generate message queue.\n");
		refresh();
		QuitGame(mainWindow, -1, 1);
	}
	if((server_msgq_id = IPC_GetMessageQueueID(SERVER_KEY_CHAR)) == -1)
	{
		printw("Cannot retrieve server message queue ID.\n");
		refresh();
		QuitGame(mainWindow, -1, 1);
	}
	
	JoinServer(server_msgq_id, client_msgq_id);
	// More curses
	move(0, 0);
	noecho();
	keypad(mainWindow, 1);
	
	int keystroke;
	while((keystroke = getch()) != 'q')
	{
		// First check for a movement
		if(keystroke == KEY_UP || keystroke == KEY_DOWN || 
				keystroke == KEY_LEFT || keystroke == KEY_RIGHT)
		{
			HandleMovement(mainWindow, keystroke, server_msgq_id, client_msgq_id);
			refresh();
		}
		
		if(CheckGameOver(client_msgq_id))
			QuitGame(mainWindow, client_msgq_id, 0);
		
		refresh();
		move(0, 0);
	}
	
	// Only send a quit message if we actually quit
	if(keystroke == 'q')
		IPC_SendMessage(server_msgq_id, "QUIT", "", MESSAGE_DELIM, client_msgq_id);
	
	clear();
	QuitGame(mainWindow, client_msgq_id, 0);
	
	return 0;
}

void HandleMovement(WINDOW *mainWindow, int keystroke, int server_msgq_id, int client_msgq_id)
{
	char cmd_buff[5];
	char data_buff[MAX_MESSAGE_LENGTH];
	
	// Send MOVE command
	char direction[MAX_MESSAGE_LENGTH];
	memset(direction, '\0', MAX_MESSAGE_LENGTH);
	if(keystroke == KEY_UP)
		strncpy(direction, "UP", 2);
	else if(keystroke == KEY_DOWN)
		strncpy(direction, "DN", 2);
	else if(keystroke == KEY_LEFT)
		strncpy(direction, "LF", 2);
	else if(keystroke == KEY_RIGHT)
		strncpy(direction, "RT", 2);
	IPC_SendMessage(server_msgq_id, "MOVE", direction, MESSAGE_DELIM, client_msgq_id);
	
	// Wait for acknowledgement
	memset(cmd_buff, '\0', 5);
	memset(data_buff, '\0', MAX_MESSAGE_LENGTH);
	while(strncmp(cmd_buff, "ACKM", 4) != 0)
	{
		if(CheckGameOver(client_msgq_id))
			QuitGame(mainWindow, client_msgq_id, 0);
		IPC_ReceiveMessage(client_msgq_id, cmd_buff, data_buff, MTYPE_ACKM, 0);
	}
	
	// Update user
	char *components[4];
	SplitString(data_buff, components, MESSAGE_SUB_DELIM, 4);
	
	UpdateScreen(components[3], atoi(components[2]), atoi(components[1]), *components[0] == 'Y', "");
}

void JoinServer(int server_msgq_id, int client_msgq_id)
{
	char data[MAX_MESSAGE_LENGTH];
	memset(data, '\0', MAX_MESSAGE_LENGTH);
	snprintf(data, MAX_MESSAGE_LENGTH, "%i", client_msgq_id);
	
	// Send the message to join the server
	IPC_SendMessage(server_msgq_id, "JOIN", data, MESSAGE_DELIM, client_msgq_id);
	printw("Joining server. (id: %i)\n", server_msgq_id);
	refresh();
	
	// Now wait for the game to start
	char cmd_buff[5];
	char data_buff[MAX_MESSAGE_LENGTH];
	
	while(strncmp(cmd_buff, "STRT", 4) != 0)
		IPC_ReceiveMessage(client_msgq_id, cmd_buff, data_buff, MTYPE_STRT, 0);
	
	char *components[2];
	SplitString(data_buff, components, MESSAGE_SUB_DELIM, 3);
	
	UpdateScreen(components[1], atoi(components[0]), 0, 0, "Welcome to Nut Gobbler!\n");
}

void UpdateScreen(char gridString[], int numAcorns, int score, int acornFound, char *additional)
{
	clear();
	printw("%sCurrent game state:\n%s%s\nNumber of acorns remaining: %i\nYour score: %i\nPress arrow keys to move or q to quit.\n", 
			additional, gridString, (acornFound) ? "Acorn found!" : "", numAcorns, score);
}

void QuitGame(WINDOW *mainWindow, int msgq_id, int forceQuit)
{
	if(msgq_id != -1)
		msgctl(msgq_id, IPC_RMID, NULL); // Remove the message queue
	
	// Curses
	if(!forceQuit)
	{
		printw("\n(Press q to quit.)");
		refresh();
		int c;
		while((c = getch()) != 'q');
	}
	delwin(mainWindow);
    endwin();
    refresh();
    
    exit(0);
}

int CheckGameOver(int client_msgq_id)
{
	char cmd_buff[5];
	char data_buff[MAX_MESSAGE_LENGTH];

	memset(cmd_buff, '\0', 5);
	memset(data_buff, '\0', MAX_MESSAGE_LENGTH);
	
	// Just doing a quick check
	int mtype_rec = IPC_ReceiveMessage(client_msgq_id, cmd_buff, data_buff, MTYPE_OVER, IPC_NOWAIT);
	if(mtype_rec == MTYPE_OVER)
	{
		clear();
		printw("%s\n", data_buff);
		return 1;
	}
	return 0;
}
