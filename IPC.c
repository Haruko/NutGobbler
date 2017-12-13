#include "IPC.h"

int IPC_GetMessageQueueID(char keyChar)
{
	key_t key;
	int msgq_id;
	
	if((key = ftok(ANCHOR_FILE, keyChar)) == -1)
		return -1;
	
	if((msgq_id = msgget(key, 0666)) == -1)
		return -1;
	
	return msgq_id;
}

int IPC_InitMessageQueue(char keyChar)
{
	// (1) Create message queue
	key_t key;
	int msgq_id;
	
	if((key = ftok(ANCHOR_FILE, keyChar)) == -1)
		return -1;
	
	if((msgq_id = msgget(key, 0666 | IPC_CREAT)) == -1)
		return -1;
	
	// (2) Make sure queue is empty
	IPC_FlushMessageQueue(msgq_id);
	
	return msgq_id;
}

void IPC_FlushMessageQueue(int msgq_id)
{
    // Non-blocking read to flush any stranded messages in queue before use
    GameMessage dummyBuffer;

    while(msgrcv(msgq_id, &dummyBuffer, MAX_MESSAGE_LENGTH, 0, IPC_NOWAIT) > 0);
}

void IPC_SendMessage(int msgq_id, char cmd[], char data[], char *delim, int mtype)
{
	GameMessage send_msg = {mtype, ""};
	memset(send_msg.data, '\0', MAX_MESSAGE_LENGTH);
	strncpy(send_msg.data, cmd, 4);
	strncat(send_msg.data, delim, 1);
	strncat(send_msg.data, data, MAX_MESSAGE_LENGTH - 5);
	
	msgsnd(msgq_id, &send_msg, MAX_MESSAGE_LENGTH, mtype);

	//printf("Sent: %s\n", send_msg.data);
}

int IPC_ReceiveMessage(int msgq_id, char cmdBuffer[], char dataBuffer[], int mtype, int flags)
{
	GameMessage received;
	memset(received.data, '\0', MAX_MESSAGE_LENGTH);
	if(msgrcv(msgq_id, &received, MAX_MESSAGE_LENGTH, mtype, flags) == -1)
		return -1;
	//printf("Received: %s\n", received.data);
	
	char *components[2];
	SplitString(received.data, components, MESSAGE_DELIM, 2);
	
	memset(cmdBuffer, '\0', 5);
	memset(dataBuffer, '\0', MAX_MESSAGE_LENGTH);
	
	if(components[0] == NULL)
		components[0] = "";
	strncpy(cmdBuffer, components[0], 4);
	if(components[1] == NULL)
		components[1] = "";
	strncpy(dataBuffer, components[1], MAX_MESSAGE_LENGTH);
	
	return received.mtype;
}
