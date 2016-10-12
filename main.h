#define MAXLINESIZE 1000
#define MAXWORDS 300
#define WORDSIZE 100

struct Cmd {
  char executable[WORDSIZE];
  char argv[MAXWORDS][WORDSIZE];
  int argc;
};

int interactiveMode();
int batchMode(char *batchfile);
int readCmd(struct Cmd *inputCmd);
int parseCmd(struct Cmd *inputCmd);
int mystrncpy(char *dest,char *src,size_t n);
