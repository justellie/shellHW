#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lectura.h"
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
                printf("\n [%d] %d %s  by %d \n", size, pid, "terminated", SIGINT);
                kill(aux->pid, SIGINT);
        }
}
//SIGCHLD
void sigchld_handler(int sig)
{
        char msg[] = "caught sigchld\n";
        write(1, msg, sizeof(msg));
}

void verify_childrens()
{
        int status;
        pid_t rc_pid;
        if (HEAD != NULL)
        {
                int size = getSize(&HEAD);
                node aux = getNode(&HEAD, size);
                waitpid(-1, &status, WNOHANG);
                for (int i = 1; i < size + 1; i++)
                {
                        node aux = getNode(&HEAD, size);
                        rc_pid = waitpid(aux->pid, &status, WNOHANG);
                        if (rc_pid > 0)
                        {
                                if (WIFEXITED(status))
                                {
                                        printf("Child exited with RC=%d\n", WEXITSTATUS(status));
                                }
                                if (WIFSIGNALED(status))
                                {
                                        printf("Child exited via signal %d\n", WTERMSIG(status));
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
        char **arguments; //puntero de argumentos

        status = 1;

        while (status)
        {

                signal(SIGINT, sigint_handler);
                signal(SIGTSTP, sigtstp_handler);
                signal(SIGCHLD, sigtstp_handler);
                verify_childrens();
                printf("> ");
                command_line = readCommandLine();                                    //lee la linea de comando
                if (strcmp(command_line, "") == 0 || strcmp(command_line, " ") == 0) //si es vacia continua
                {
                        free(command_line);
                        continue;
                }
                verify_childrens();
                arguments = splitLine(command_line); //obtengo los argumentos
                status = consolaEjecuta(arguments, &HEAD);
                free(*arguments);
                free(arguments);
        }
        //if(HEAD != NULL) free(HEAD);
        //free(HEAD);
}

char *readCommandLine(void)
{
        int position = 0;
        int buf_size = 1024;
        char *buffer = (char *)malloc(sizeof(char) * buf_size); //esto es basicamente un vector char
        char c;

        // Leo caracter por caracter
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

                // expando el buffer si es necesario

                if (position >= buf_size)
                {
                        buf_size += 64;
                        buffer = realloc(buffer, buf_size); //guardo y expando el buffer
                }

                c = getchar();
        }
        buffer[position] = '\0';
        return buffer;
}

char **splitLine(char *command)
{
        int position = 0;
        int no_of_tokens = 64; //numero de palabras en el comando recibido
        char **tokens = malloc(sizeof(char *) * no_of_tokens);
        char delim[2] = " ";

        // Divido el comando en tokens con el espacio como delimitador
        char *token = strtok(command, delim); //Obtengo todas las palabras delimitadas por " "
        while (token != NULL)
        {
                tokens[position] = token; // si no es null la añado a mi vector de char
                position++;
                token = strtok(NULL, delim); //avanzo con los token
        }
        tokens[position] = NULL;
        return tokens;
}
