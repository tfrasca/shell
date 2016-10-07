#include<stdio.h>
#include"main.h"

main (int argc, char *argv[]) {
  if(argc == 1) {
    interactiveMode();
  } else {
    batchMode(argc,&argv[0]);
  }
}

int interactiveMode() { 
  struct Cmd inputCmd;
  int quitFlag=0;
  char *quit="quit";
  int i=0;
  int index=0;
  char nextWrd[MAXLINESIZE];
  while(!quitFlag) {
    printf("8[ ");
    i=0;
    index=0;
    while((nextWrd[i]=getchar()) !='\n') {
      if(nextWrd[i]==' ') {
        inputCmd.argv[index]=nextWrd;
        printf("%s",inputCmd.argv[index]); 
        index++;
      }
        printf("%s",nextWrd); 
      //if(strcmp(inputCmd.argv[index],quit)==0) {
      //  printf("quit!!!\n");
      //  quitFlag=1;
      //}
      i++;
    }
    printf("\n");
  }
  return 1;
}

int batchMode(int argc, char *argv[0]) {
  return 1;
}
