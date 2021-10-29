#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read.h"
#include <signal.h>
node HEAD = NULL; //list of jobs
void sigtstp_handler(int sig)
{
        if (HEAD != NULL)
        {
                int size = getSize(&HEAD);
                node aux = getNode(&HEAD, size);
                aux->status = stopped;
                kill(aux->pid, SIGTSTP);
        }
}

void sigint_handler(int sig)
{
        if (HEAD != NULL)
        {
                int size = getSize(&HEAD);
                node aux = getNode(&HEAD, size);
                sleep(1);
                pid_t pid = aux->pid;
                aux->status = dead;
                printf("\n [%d] %d terminated by signal %d\n",aux->pos, aux->pid, SIGTERM);

                kill(aux->pid, SIGINT);
        }
}
//SIGCHLD
void verify_childrens()
{
        int status;
        pid_t rc_pid;
        if (HEAD != NULL)
        {
                int size = getSize(&HEAD);
                node aux = getNode(&HEAD, size);
                for (int i = 1; i < size + 1; i++)
                {
                        node aux = getNode(&HEAD, size);
                        rc_pid = waitpid(aux->pid, &status, WNOHANG);
                        if (rc_pid > 0)
                        {
                                if (WIFEXITED(status))
                                {
                                        deletePos(&HEAD, size);

                                        
                                }
                                if (WIFSIGNALED(status))
                                {
                                        printf("\n [%d] %d terminated by signal %d\n",aux->pos, aux->pid, WTERMSIG(status));
                                        deletePos(&HEAD, size);
                                }
                        }
                }
        }
}

void shellLoop(void)
{

        int status;
        char *command_line;
        char *s;
        char **arguments; //pointer to args

        status = 1;

        while (status)
        {

                signal(SIGINT, sigint_handler);
                signal(SIGTSTP, sigtstp_handler);
                verify_childrens();
                printf("> ");
                command_line = readCommandLine();                                    
                if (strcmp(command_line, "") == 0 || strcmp(command_line, " ") == 0)
                {
                        free(command_line);
                        continue;
                }
                arguments = splitLine(command_line); //get the arguments
                status = executeShell(arguments, &HEAD);
                free(*arguments);
                free(arguments);
        }

}

char *readCommandLine(void)
{
        int position = 0;
        int buf_size = 1024;
        char *buffer = (char *)malloc(sizeof(char) * buf_size); 
        char c;

        // read char by char 
        c = getchar();
        int count = 0;
        while (c != EOF && c != '\n')
        {
                if (c != ' ')
                {
                        count++;
                }
                if (count != 0)
                {
                        buffer[position] = c;
                        position++;
                }

                

                if (position >= buf_size)
                {
                        buf_size += 64;
                        buffer = realloc(buffer, buf_size); //save and expand my buffer
                }

                c = getchar();
        }
        buffer[position] = '\0';
        return buffer;
}

char **splitLine(char *command)
{
        int position = 0;
        int no_of_tokens = 64;
        char **tokens = malloc(sizeof(char *) * no_of_tokens);
        char delim[2] = " ";

        // splitcommand
        char *token = strtok(command, delim); 
        while (token != NULL)
        {
                tokens[position] = token; // if not null to my array
                position++;
                token = strtok(NULL, delim); 
        }
        tokens[position] = NULL;
        return tokens;
}
