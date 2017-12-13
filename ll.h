#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node
{
    char *cmd;
    char *msg;
    struct node *next;
};

	

struct node *enqueue(struct node *head, char *cmd, char *msg);

struct node *dequeue(struct node *head, char* cmd, char *msg);
