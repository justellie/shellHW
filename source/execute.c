#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "execute.h"
#include <ctype.h>
#include <sys/types.h>
#include <pwd.h>

char PWD[1024];

int launcher(char **args, node *head)
{
  pid_t pid, wpid;
  int status;
  args;
  pid = fork();
  if (pid == 0)
  {

    if (execvp(args[0], args) == -1)
    {
      perror("Error ");
    }
    exit(EXIT_FAILURE);
  }

  else if (pid < 0)
  {
    perror("Error ");
  }
  else
  {
    int size = getSize(head);
    addNode(head, running, size + 1, pid, args);
    size++;
    node aux = getNode(head, size);

    do
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status) && aux->status == running);
    if (aux->status == running)
    {
      size = getSize(head);
      deletePos(head, size);
      size--;
    }
    else if (aux->status == dead)
    {
      size = getSize(head);
      deletePos(head, size);
      size--;
    }
  }
  return 1;
}

char *commandStr[] = {
    "cd",
    "exit",
    "start",
    "bg",
    "kill",
    "jobs",
    "background",
    "fg"};

int (*commandsFunc[])(char **, node *head) = {
    &cd,
    &shellExit,
    &shellStart,
    &bg,
    &shellKill,
    &shellJobs,
    &shellBackground,
    &fg

};
int sizeCommands()
{
  return sizeof(commandStr) / sizeof(char *);
}

int cd(char **args)
{
  const char *homedir;
  struct passwd *pw = getpwuid(getuid());

  if ((homedir = getenv("HOME")) == NULL)
  {
    homedir = getpwuid(getuid())->pw_dir;
  }
  if (args[0] == NULL)
  {
    if (chdir(homedir) != 0)
    {
      perror(homedir);
    }
  }
  else
  {
    if (chdir(args[0]) != 0)
    {
      perror(args[0]);
    }
  }
  return 1;
}

int shellExit(char **args, node *head)
{
  pid_t wpid;
  int status = 1;
  int size = getSize(head);
  for (int i = 1; i < size + 1; i++)
  {
    node aux = getNode(head, i);
    if (aux->status == stopped)
    {
      if (kill(aux->pid, SIGCONT) == 0)
      {
        kill(aux->pid, SIGHUP);
      }
    }
    else
    {
      kill(aux->pid, SIGHUP);
    }
    deletePos(head, i);
  }
  if (*head != NULL)
    free(*head);
  return 0;
}
int shellStart(char **args, node *head)
{
  return launcher(args, head);
}

int executeShell(char **args, node *head)
{
  int i;
  int command = -1;
  int acum = 0;

  if (args[0] == NULL)
  {
    // No hay comandos
    return 1;
  }

  // loop through the string to extract all other tokens
  char **aux = args;

  if (strchr(args[0], '/') != NULL)
  {
    command = 2; // if only have a direction then run the programn
    while (*aux != NULL)
    {
      acum += strlen(*aux);
      if (strchr(*aux, '&') != NULL)
      {
        command = 6; // if only have a direction then run the programn
        removeChar(*args, '&', acum);
        break;
      }
      else
      {
        aux++;
      }
    }
  }
  else
  {

    for (i = 0; i < sizeCommands(); i++)
    {
      if (strcmp(args[0], commandStr[i]) == 0)
      { // else then find the right command
        command = i;

        while (*aux != NULL)
        {
          acum += strlen(*aux);
          if (strchr(*aux, '%') != NULL)
          {
            removeChar(*args, '%', acum);
            break;
          }
          else
          {
            aux++;
          }
        }

        args++;
        break;
      }
    }
  }
  if (command != -1)
  {
    return (*commandsFunc[command])(args, head);
  }
  else
  {

    char s2[] = ": command not found \n";

    // do things with s
    char *result = malloc(strlen(args[0]) + strlen(s2) + 1); // +1 for the null-terminator

    strcpy(result, args[0]);
    strcat(result, s2);

    printf(result);
    free(result);
    return command;
  }
}

int shellBackground(char **args, node *head)
{
  pid_t pid;
  pid = fork();
  if (pid == 0)
  {

    if (execvp(args[0], args) == -1)
    {
      perror("Error");
    }
    exit(EXIT_FAILURE);
  }

  else if (pid < 0)
  {
    perror("Error");
  }
  else
  {
    int size = getSize(head);
    addNode(head, background, size + 1, pid, args);
    printf("[%d] %d\n", size + 1, pid);
  }
  return 1;
}
int shellKill(char **args, node *head)
{
  pid_t wpid;
  int status = 1;
  int pos = atoi(*args);
  node aux = getNode(head, pos);
  if (kill(aux->pid, SIGTERM) == 0) //try to term
  {
    sleep(5);                                     //wait untill it finish
    kill(aux->pid, SIGKILL);                      //kill the process
    wpid = waitpid(aux->pid, &status, WUNTRACED); //notify to father process to avoid zombie process
    printf("[%d] %d terminated by signal %d\n",aux->pos, aux->pid, SIGTERM);
    deletePos(head, pos);
  }
  else
  {
    printf("The process %d can't be terminated \n", aux->pid);
  }

  return 1;
}
int shellJobs(char **args, node *head)
{
  int size = getSize(head);
  for (int i = 1; i < size + 1; i++)
  {
    node aux = getNode(head, i);
    if (aux->status != stopped)
    {
      char s2[] = "Running";
      printf("[%d] %d %s %s \n", aux->pos, aux->pid, s2, aux->hist);
    }
    else
    {
      char s2[] = "Stopped";
      printf("[%d] %d %s %s \n", aux->pos, aux->pid, s2, aux->hist);
    }
  }

  return 1;
}
int bg(char **args, node *head)
{
  pid_t wpid;
  int status = 1;
  int pos = atoi(*args);
  node aux = getNode(head, pos);
  if (kill(aux->pid, SIGCONT) == 0) //try to term
  {
    aux->status = background;
  }
  else
  {
    printf("The process %d can't continue \n", aux->pid);
  }

  return 1;
}

int fg(char **args, node *head)
{
  pid_t wpid;
  int status = 1;
  int pos = atoi(*args);
  node aux = getNode(head, pos);
  int size;
  if (kill(aux->pid, SIGCONT) == 0) //try to term
  {
    aux->status = running;
    do
    {
      wpid = waitpid(aux->pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status) && aux->status == running);
    if (aux->status == running)
    {
      size = getSize(head);
      deletePos(head, size);
      size--;
    }
    else if (aux->status == dead)
    {
      size = getSize(head);
      deletePos(head, size);
      size--;
    }
  }
  else
  {
    printf("The process %d can't continue \n", aux->pid);
  }

  return 1;
}

void removeChar(char *s, char c, int n)
{

  int j = 0;
  int letter = 0;
  int i = 0;

  while (letter != n)
  {
    if (s[i] != NULL && s[i] != ' ')
    {
      letter++;
    }
    if (s[i] != c)
    {
      s[j++] = s[i];
    }
    i++;
  }
  s[j] = '\0';
}