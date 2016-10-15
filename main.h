#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define MAXLINESIZE 1000
#define MAXWORDS 300
#define WORDSIZE 100

struct Cmd {
  char argv[MAXWORDS][WORDSIZE];
  // char *argv[MAXWORDS];
  int argc;
};

//empty Cmd struct for resetting values
static const struct Cmd ResetCmd;

int shellLoop(FILE *batchfile, int batch);
int parseCmd(struct Cmd *inputCmd, FILE *file_stream);
int execCmd(struct Cmd *inputCmd);
