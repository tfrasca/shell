#include<stdio.h>
const int MAXLINESIZE=1000;

main (int argc, char *argv[]) {
  if(argc == 1) {
    interactiveMode();
  } else {
    batchMode(argc,&argv[0]);
  }

}

int interactiveMode() { 
  int quitFlag=0;
  char line[MAXLINESIZE];
  //char *line;
  int n;
  char quit[4]="quit";
  while(!quitFlag) {
    printf("8[ ");
    scanf("%s", line);
    printf("your command: %s\n",line);
    if(strcmp(line,quit)==0) {
      printf("quit!!!\n");
      quitFlag=1;
    }
  }
  return 1;
}

int batchMode(int argc, char *argv[0]) {
  return 1;
}
