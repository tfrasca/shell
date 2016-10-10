#define MAXLINESIZE 1000
#define MAXWORDS 300

struct Cmd {
  char *argv[MAXWORDS];
  int argc;
};

int interactiveMode();
int batchMode(int argc, char *argv[0]);
int readCmd(struct Cmd *inputCmd);
int parseCmd(struct Cmd *inputCmd);
int mystrncpy(char *dest,char *src,size_t n);
