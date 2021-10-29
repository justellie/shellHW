

#include "list.h"
int launcher(char **args, node *head);
int cd(char **args);
int shellExit(char **args, node *head);
int sizeCommands();
int executeShell(char **args, node *head);
int shellStart(char **args, node *head);
int shellBackground(char **args, node *head);
int shellKill(char **args, node *head);
int shellJobs(char **args, node *head);
int bg(char **args, node *head);
int fg(char **args, node *head);
void removeChar(char *s, char c, int n);