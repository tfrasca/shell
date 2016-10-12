#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "main.h"

int main (int argc, char *argv[]) {
  if(argc == 1) {
    interactiveMode();
  } else if(argc == 2) {
    batchMode(argv[1]);
  } else {
    fputs("Wrong number of command line arguments.\n", stderr);
    exit(1);
  }

  return 0;
}

int interactiveMode() {
  struct Cmd cmd;
  char quit[] = "quit";
  int processPid;
  int *status;
  int quitFlag=0;

  do {
  //promt
    printf("8[ ");
    //parse command
    quitFlag = parseCmd(&cmd);
    printf("%s\n",cmd.executable);
    if(strncmp(cmd.executable,quit,4)==0 || quitFlag) {
      exit(0);
    }

    //fork child
    processPid = fork();
    if ( processPid ==0) {
      //not taking argv becuase supposed to be char *argv[] we have char argv[][]??
      execve(cmd.executable,cmd.argv[0],NULL);
      printf("child\n");
    }
    else {
      printf("adult\n");
    }
    waitpid(processPid,status,0);


    //execute command
    //wait for child to finish
  } while (!feof(stdin) && strncmp(cmd.executable, quit,4)!=0);

  return 1;
}

int batchMode(char *batchfile) {
  return 1;
}

int readCmd(struct Cmd *inputCmd) {
  return 1;
}

int parseCmd(struct Cmd *cmd) {
  int word_index=0;
  int char_index=0;
  int prev_space=0;
  char c;

  while((c=getchar()) !='\n' && c != ';' ) {
    if(feof(stdin)) {
      return 1;
    }
    if (isspace(c)) {
      if (!prev_space) {
        word_index++;
        char_index=0;
      } 
      prev_space=1;
    } else {
      if(word_index==0) {
        cmd->executable[char_index]=c;
      } else {
        cmd->argv[word_index-1][char_index] = c;
      }
      char_index++;
    }
  }
  return 0;
}

int mystrncpy(char *dest,char *src,size_t n) {
  int i=0;
  for(;i<n;i++) {
    *(dest+i)=src[i];
  }
  return 1;
}
