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
  char quit[4]="quit";
  while(!quitFlag) {
    printf("8[ ");
    quitFlag=parseCmd(&inputCmd);
    break;
  }
  return 1;
}

int batchMode(int argc, char *argv[0]) {
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
int parseCmd(struct Cmd *inputCmd) {
  int index=0;
  char nextWrd[MAXLINESIZE];
  int i=0;
  while((nextWrd[i]=getchar()) !='\n') {
    if(nextWrd[i]==' ') {
      //inputCmd->argv[index]="doh";
      mystrncpy(inputCmd->argv[index],nextWrd,i);
      printf("%s\n",inputCmd->argv[index]);
    }
    i++;
  }
  return 1;
}

int mystrncpy(char *dest,char *src,size_t n) {
  int i=0;
  for(;i<n;i++) {
    *(dest+i)=src[i];
  }
}
