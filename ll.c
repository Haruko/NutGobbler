#include "ll.h"

struct node *enqueue(struct node *head, char *cmd, char *msg) {

    struct node *newnode = (struct node *)malloc(sizeof(struct node));
    memset(newnode, 0, sizeof(struct node));
    newnode->cmd = cmd;
    newnode->msg = msg;
    struct node *currnode;


    currnode = head;
    //empty queue
    if (currnode == NULL)
        head = newnode;
    else
    {
        //non-empty queue
        while(currnode->next != NULL)
            currnode = currnode->next;
        currnode->next = newnode;
    }
	return head;
}


struct node *dequeue(struct node *head, char* cmd, char* msg) {

    char *tempmsg;

    strcpy(cmd, head->cmd);
    strcpy(msg, head->msg);
    
    struct node *tmp = head;
    head = head->next;
    free(tmp);
    return head;
}
