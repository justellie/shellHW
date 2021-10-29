#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "list.h"

node createNode()
{
    node temp;                               // declare a node
    temp = (node)malloc(sizeof(struct JID)); // allocate memory using malloc()
    temp->next = NULL;                       // make next point to NULL
    temp->pos = 0;
    return temp; //return the new node
}

void addNode(node *head, int status, int pos, pid_t pid, char **args)
{
    node temp, p;          // declare two nodes temp and p
    temp = createNode();   //createNode will return a new node with data = value and next pointing to NULL.
    temp->status = status; // add element's value to data part of node
    temp->pos = pos;       // add element's value to data part of node
    temp->pid = pid;       // add element's value to data part of node

    int count = 0;
    char **aux = args;
    int acum = 0;
    while (*aux != NULL)
    {
        count++;
        acum += strlen(*aux);
        aux++;
    }
    temp->hist = (char *)malloc(sizeof(char) * (acum + 1 + count - 1));
    aux = args;
    char space[] = " ";
    if (*args != NULL)
    {

        strcpy(temp->hist, *aux);
        if (aux != NULL)
            aux++;
        for (int i = 0; i < count - 1; i++)
        {
            strcat(temp->hist, space);
            strcat(temp->hist, *aux);
            if (aux != NULL)
                aux++;
        }
    }
    else
    {
        strcat(temp->hist, space);
    }

    if (*head == NULL)
    {
        *head = temp; //when linked list is empty
    }
    else
    {
        p = *head; //assign head to p
        while (p->next != NULL)
        {
            p = p->next; //traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp; //Point the previous last node to the new node created.
    }
}

void deletePos(node *head, int pos)
{

    node p = *head, temp;
    if (head == NULL)
    {
        perror("This process was not created by this shell");
    }
    if (p != NULL && p->pos == pos)
    {
        *head = p->next; // Changed head
        free(p->hist);
        free(p); // free old head
    }
    else
    {
        temp = p;
        while (p->pos != pos && p->next != NULL)
        {
            temp = p;
            p = p->next; //traverse the list until p is the last node.The last node always points to NULL.
        }
        if (p == NULL)
        {
            perror("This process was not created by this shell");
        }
        temp->next = p->next; //Point the previous last node to the new node created.
        free(p->hist);
        free(p);
    }
}

node getNode(node *head, int pos)
{
    node p = *head;
    node anwser = NULL;
    if (head == NULL)
    {
        perror("This process was not created by this shell");
    }
    while (p != NULL && p->pos != pos)
    {
        p = p->next; // Changed head
    }
    if (p != NULL)
    {
        anwser = p;
    }
    return anwser;
}
int getSize(node *head)
{

    int count = 1;
    node p = *head;
    if ((*head) == NULL)
        return 0;
    while (p->next != NULL)
    {
        p = p->next; //traverse the list until p is the last node.The last node always points to NULL.
        count++;
    }
    return count;
}
char *stringcopywithpointer(const char *source)
{
    int len = strlen(source);
    char *copy = malloc(len + 1);
    strcpy(copy, source);
    return copy;
}