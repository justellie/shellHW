

#include "list.h"
int launcher(char **args, node *head);
int consolaMoveToDir(char **args);
int shellExit(char **args, node *head);
int sizeCommands();
int consolaEjecuta(char **args, node *head);
int shellStart(char **args, node *head);
void setPath(char **args);
int shellBackground(char **args, node *head);
int shellKill(char **args, node *head);
int shellJobs(char **args, node *head);
int bg(char **args, node *head);
void removeChar(char *s, char c, int n);