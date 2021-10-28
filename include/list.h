
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

enum status
{
   running,
   stopped,
   dead,
   background
} status;

typedef struct JID // JOB I D
{
   int status; //status
   int pos;    //position
   pid_t pid;  //process id
   char *hist; //command
   struct JID *next;
} * node;

node createNode();
void addNode(node *head, int status, int pos, int pid, char **args);
void deletePos(node *head, int pos);
node getNode(node *head, int pos);
int getSize(node *head);
char *stringcopywithpointer(const char *source);