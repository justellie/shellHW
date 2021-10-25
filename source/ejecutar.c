#include <stdlib.h> 
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "ejecutar.h"

char PWD[1024];		// Directorio de trabajo actual
enum status{running, stopped} status;

int lanzador(char **args,node *head)
{
  pid_t pid, wpid;
  int status;
  args;
  pid = fork();//Guardo el pid del proceso que voy a crear
  if (pid == 0) //proceso hijo
  {
    //si hay un error lo lanzo      
    if (execvp(args[0], args) == -1) {
      perror("Error de proceso");
    }
    exit(EXIT_FAILURE);
  } 
  //esto es un error de forking
  else if (pid < 0) 
  {
    perror("Error de forking");
  } 
  else 
  {
    int size=getSize(head);
    addNode(head, running,size+1,pid,args);
    size++;
    /*
    * Proceso padre espera hasta que haya finalizado
    * O porque  una señal
    */
    do 
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    size=getSize(head);
    deletePos(head,size);
    size--;

  }
  //retorna 1 para que pueda seguir en el loop de consola
  return 1;
}
/*
 * Lista de comandos
 */
//Vecto de string con el nombre de comando interno de la shell
char *comandosStr[] = {
  "cd",
  "exit",
  "start",
  "whereami",
  "bg",
  "kill",
  "jobs",
  "history"
};
//puntero a funciones 
int (*comandosFunc[]) (char **,node *head) = {
  &consolaMoveToDir,
  &consolaByeBye,
  &consolaStart,
  &consolaLs,
  &consolaBackground,
  &consolaKill,
  &consolaJobs
};
int numeroComandos() 
{
  return sizeof(comandosStr) / sizeof(char *);
}
/*
  Funciones de los comandos.
*/
int consolaMoveToDir(char **args)
{
  if (args[1] == NULL) 
  {
    fprintf(stderr, "Esperando direccion \n");
  } 
  else 
  {
    if (chdir(args[1]) != 0) 
    {
      perror("Error directorio no existe!.");
    }
  }
  return 1;
}

int consolaByeBye(char **args,node* head)
{
  pid_t wpid;
  int status=1;
  int size=getSize(head);
  for (int i = 0; i < size; i++)
  {
    node aux=getNode(head, i);
    if (aux->status==stopped){
        if( kill(aux->pid, SIGCONT)==0){
        sleep(1);//espera que termine
        kill(aux->pid, SIGHUP);//mata el proceso
        wpid = waitpid(aux->pid, &status, WUNTRACED);//le dice al padre que esta muerto para evitar que sea zombie
      }
    }
    else{
       kill(aux->pid, SIGHUP);
    }


  }
  free(*head); 
  return 0;
}
int consolaStart(char **args,node *head)
{
  return lanzador(args,head);
}

int consolaEjecuta(char **args,node *head)
{
  int i;
  int command=-1;

  if (args[0] == NULL) {
    // No hay comandos
    return 1;
  }
  if(strchr(args[0], '/') != NULL){
    command=2; // if only have a direction then run the programn
  }
  else{
    for (i = 0; i < numeroComandos(); i++) {
      if (strcmp(args[0], comandosStr[i]) == 0) {// else then find the right command
          command=i;
          args++;
          break;
      }
    }
  }
  if(command != -1){
    return (*comandosFunc[command])(args,head);//lanzo la funcion de los comandos internos
  }
  else{

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
int consolaLs(char ** args){
  getcwd(PWD, sizeof(PWD));	// Iinicializar PWD
	printf("%s\n", PWD);
	return 1;
}

int consolaBackground(char **args,node *head)
{
  pid_t pid;
  pid = fork();//Guardo el pid del proceso que voy a crear
  if (pid == 0) //proceso hijo
  {
    //si hay un error lo lanzo      
    if (execvp(args[0], args) == -1) {
      perror("Error de proceso");
    }
    exit(EXIT_FAILURE);
  } 
  //esto es un error de forking
  else if (pid < 0) 
  {
    perror("Error de forking");
  } 
  else 
  {
      int size=getSize(head);
      addNode(head, running,size+1,pid,args);
      printf("%d\n",pid);

  }
  return 1;
}
int consolaKill(char **args,node *head)
{
  pid_t wpid;
  int status=1;
  int pos=atoi(*args);
  node aux=getNode(head, pos);
  if( kill(aux->pid, SIGTERM)==0)//try to term
  {
    sleep(5);//wait untill it finish
    kill(aux->pid, SIGKILL);//kill the process
    wpid = waitpid(aux->pid, &status, WUNTRACED);//notify to father process to avoid zombie process
    deletePos(head,pos);
  }
  else
  {
    printf("The process %d can't be terminated \n",aux->pid);
  }

  return 1;
}
int consolaJobs(char **args,node *head){
  int size=getSize(head);
  for (int i = 0; i < size; i++)
  {
    node aux=getNode(head, i);
    if(aux->status=running){
      char s2[] = ": Running\n";
      printf("[%d] %d %s %s \n",aux->pos,aux->pid,s2,aux->hist);
    }
    else{
      char s2[] = ": Stopped\n";
       printf("[%d] %d %s %s \n",aux->pos,aux->pid,s2,aux->hist);
      }
    }
   
  return 1;
}
