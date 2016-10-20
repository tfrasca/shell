#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

//change with values from xargs --show-limits
#define MAXLINESIZE 1000
#define MAXWORDS 300
#define WORDSIZE 100

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
FILE *getOutputFile(struct Cmd *cmd);
int getBackgroundExecution(struct Cmd *cmd);
int cmdCases(struct Cmd *cmd, FILE *outputFile);
int createUserManual();
