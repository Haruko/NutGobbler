// File for IPC things

#ifndef IPC_H
#define IPC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "Shared.h"

// Message passing
#define MAX_MESSAGE_LENGTH 1000	// Message length that will be sent between server and clients
#define MESSAGE_DELIM "&"
#define MESSAGE_SUB_DELIM "$"

// Used for server -> client
//#define MTYPE_JOIN 1 // JOIN mqid
#define MTYPE_STRT 2 // STRT numAcorns gridString
//#define MTYPE_MOVE 3 // MOVE direction
#define MTYPE_ACKM 4 // ACKM success score numAcorns gridString
#define MTYPE_OVER 5 // OVER score
//#define MTYPE_QUIT 6 // QUIT

// For keys
#define ANCHOR_FILE "./.server_anchor"
#define SERVER_KEY_CHAR 'S'
#define CLIENT_KEY_CHAR 'C'

typedef struct
{
	long mtype; // Message Type
	
	char data[MAX_MESSAGE_LENGTH];
} GameMessage;

/*
	Retrieves the message queue associated with a key.
	Parameters:
		keyChar - character to use for key generation
	Return:
		message queue ID requested
*/
int IPC_GetMessageQueueID(char keyChar);

/*
	Initialize the IPC message queue
	Parameters:
		keyChar - character to use for key generation
	Return:
		message queue ID created
*/
int IPC_InitMessageQueue(char keyChar);

/*
	Flushes garbage from a message queue
	Parameters:
		msgq_id - ID of queue to flush
*/
void IPC_FlushMessageQueue(int msgq_id);

/*
	Sends a command to a given queue
	Parameters:
		msgq_id - message queue to send message to
		cmd - command to send (MOVE, ACKM, etc)
		data - data to send
		delim - delimiter to use for seperating components of the message
		mtype - message type
*/
void IPC_SendMessage(int msgq_id, char cmd[], char data[], char *delim, int mtype);

/*
	Receives a message from the queue
	Parameters:
		msgq_id - ID of queue to receive from
		cmdBuffer - buffer of size 5 to hold the command
		dataBuffer - buffer of size MAX_MESSAGE_LENGTH to store data after it is received
		mtype - message type to receive
		flags - flags for msgrcv
	Return:
		message type received or -1 if no message was found
*/
int IPC_ReceiveMessage(int msgq_id, char cmdBuffer[], char dataBuffer[], int mtype, int flags);

#endif
