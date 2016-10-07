#define MAXLINESIZE 1000
#define MAXWORDS 300

int interactiveMode();
int batchMode(int argc, char *argv[0]);

struct Cmd {
  char *argv[MAXWORDS];
  int argc;
};
