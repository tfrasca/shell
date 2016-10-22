#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

//change with values from xargs --show-limits
#define MAXLINESIZE 1000
#define MAXWORDS 300
#define WORDSIZE 100
#define MAXCMDS 100

extern char **environ;

struct Cmd {
  // char argv[MAXWORDS][WORDSIZE];
  char *argv[MAXWORDS];
  int argc;
};

//sempahore for I/O
sem_t io_sem;

//empty Cmd struct for resetting values
static const struct Cmd ResetCmd;

//creates user manual file for the help command
int createUserManual();
int shellLoop(FILE *batchfile, int isBatch);

int parseLine(struct Cmd cmd[], FILE *input_filestream, int isBatch);
int parseCmd(char *cmd_str, struct Cmd *cmd);

int getOutputFile(struct Cmd *cmd);
int getBackgroundExecution(struct Cmd *cmd);

//concurrency function (return: int, arg: struct Cmd *)
void *execCmd(void *inputCmd);		
int cmdCases(struct Cmd *cmd);
void cdCmd(struct Cmd *cmd);
void dirCmd(struct Cmd *cmd, int cmd_size);
void environCmd(struct Cmd *cmd);
void echoCmd(struct Cmd *cmd);
void helpCmd(struct Cmd *cmd, int cmd_size);
void pauseCmd(struct Cmd *cmd);
void extCmd(struct Cmd *cmd);

