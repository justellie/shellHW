#include <stdio.h>
#include <stdlib.h>     
#include <string.h>     
#include "lectura.h" 
#include <signal.h>

void sigtstp_handler(int sig){
    char msg[] = "caught sigtstp\n";
    write(1, msg, sizeof(msg));
    return;
}

void sigint_handler(int sig){
    char msg[] = "caught sigint\n";
    write(1, msg, sizeof(msg));
    return;
}

void shellLoop(void)
{

        int status;
        char * command_line;
        char *s;
        char ** arguments ;//puntero de argumentos
        node head=NULL;//list of jobs

        

	status = 1;
        
        while (status)
        {
        
            signal(SIGINT, sigint_handler);
            signal(SIGTSTP, sigtstp_handler);
            printf("> ");
            command_line = readCommandLine();//lee la linea de comando
	        if ( strcmp(command_line, "") == 0 )//si es vacia continua
                {
			    continue;
		}
            arguments = splitLine(command_line);//obtengo los argumentos
            status = consolaEjecuta(arguments,&head);
            free(*arguments);
            free(arguments);    
        }
        free(head);
}

char * readCommandLine(void){
        int position = 0;
        int buf_size = 1024;
        char * buffer= (char *)malloc(sizeof(char) * buf_size);//esto es basicamente un vector char
        char c;

        // Leo caracter por caracter
        c = getchar();
        while (c != EOF && c != '\n'){
                buffer[position] = c;

                // expando el buffer si es necesario

                if (position >= buf_size){
                        buf_size += 64;
                        buffer = realloc(buffer, buf_size);//guardo y expando el buffer
                }

                position++;
                c = getchar();
        }
        buffer[position] = '\0';
        return buffer;
}

char ** splitLine(char * command){
        int position = 0;
        int no_of_tokens = 64;//numero de palabras en el comando recibido 
        char ** tokens = malloc(sizeof(char *) * no_of_tokens);
        char delim[2] = " ";

        // Divido el comando en tokens con el espacio como delimitador 
        char * token = strtok(command, delim);//Obtengo todas las palabras delimitadas por " "
        while (token != NULL){
                tokens[position] = token;// si no es null la añado a mi vector de char
                position++;
                token = strtok(NULL, delim);//avanzo con los token
        }
        tokens[position] = NULL;
        return tokens;
}



