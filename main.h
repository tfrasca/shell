#define MAXLINESIZE 1000
#define MAXWORDS 300
#define WORDSIZE 100

struct Cmd {
  char argv[MAXWORDS][WORDSIZE];
  int argc;
};

int interactiveMode();
int batchMode(char *batchfile);
int readCmd(struct Cmd *inputCmd);
void parseCmd(struct Cmd *inputCmd);
int mystrncpy(char *dest,char *src,size_t n);
