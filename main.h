#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

//change with values from xargs --show-limits
#define MAXLINESIZE 1000
#define MAXWORDS 300
#define WORDSIZE 100

extern char **environ;

struct Cmd {
  // char argv[MAXWORDS][WORDSIZE];
  char *argv[MAXWORDS];
  int argc;
};

//empty Cmd struct for resetting values
static const struct Cmd ResetCmd;

int shellLoop(FILE *batchfile, int isBatch);
int parseLine(struct Cmd cmd[], FILE *input_filestream, int isBatch);
int parseCmd(char *cmd_str, struct Cmd *cmd);
int execCmd(struct Cmd *inputCmd);
int getOutputFile(struct Cmd *cmd);
int getBackgroundExecution(struct Cmd *cmd);
int cmdCases(struct Cmd *cmd);
int createUserManual();

void cdCmd(struct Cmd *cmd);
void dirCmd(struct Cmd *cmd, int cmd_size);
void environCmd();
void echoCmd(struct Cmd *cmd);
void helpCmd(struct Cmd *cmd, int cmd_size);
void pauseCmd();
void extCmd(struct Cmd *cmd);