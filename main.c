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

  do {
    printf("8[ ");
    //parse command
    parseCmd(&cmd);
    printf("%s", cmd.argv[0]);

    //fork child


    //execute command
    //wait for child to finish
  } while (!feof(stdin) && strcmp(cmd.argv[0], quit));

  // struct Cmd inputCmd;
  // int quitFlag=0;
  // char quit[4]="quit";
  // while(!quitFlag) {
  //   printf("8[ ");
  //   quitFlag=parseCmd(&inputCmd);
  //   break;
  // }
  return 1;
}

int batchMode(char *batchfile) {
  return 1;
}

int readCmd(struct Cmd *inputCmd) {
  //need to read line -> parse 
  //while((
  
 // while((nextWrd=getchar()) !='\n') {
 //   if(nextWrd[i]==' ') {
 //     strcpy(inputCmd.argv[index],nextWrd);
 //     printf("%s",inputCmd.argv[index]); 
 //     index++;
 //   }
 // }
  return 1;
}

void parseCmd(struct Cmd *cmd) {
  int word_index=0;
  int char_index=0;
  int prev_space=1;
  char c;

  while((c=getchar()) !='\n' && c != ';') {
    if (isspace(c)) {
      if (!prev_space) {
        word_index++;
        char_index=0;
      }
      prev_space=1;
    } else {
      cmd->argv[word_index][char_index] = c;
      char_index++;
    }
  }
}

int mystrncpy(char *dest,char *src,size_t n) {
  int i=0;
  for(;i<n;i++) {
    *(dest+i)=src[i];
  }
  return 1;
}
