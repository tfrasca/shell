#include "main.h"

int main (int argc, char *argv[]) {
    FILE *inputFile;    //stdin in interactive mode or batchFile in batch mode
    int isBatch;        //specify whether in batch mode or interactive mode

    if(argc == 1) {
        isBatch = 0;
        inputFile = stdin;
    } else if(argc == 2) {
        isBatch = 1;
        inputFile = fopen(argv[1], "r");
        if (inputFile == NULL) {
            fputs("Invalid batch file.\n", stderr);
            exit(1);
        }
    } else {
        fputs("Wrong number of command line arguments.\n", stderr);
        exit(1);
    }
    createUserManual();

    shellLoop(inputFile, isBatch);

    if (isBatch) {
        fclose(inputFile);
    }

    return 0;
}

int shellLoop(FILE *inputFile, int isBatch) {
    int num_cmds = 10;
    struct Cmd cmd[num_cmds];
    pid_t pid;
    int num_pids = 0;
    int i,j;

    while(1) {
        //clear previous values
        for (i=0; i < num_cmds; i++) {
            cmd[i] = ResetCmd;
        }

        //if interactive mode, prompt input if finished processing line
        if (!isBatch) {
            printf("8[ ");
        }

        //parse command, will return 1 if EOF in interactive mode
        if (parseLine(cmd, inputFile, isBatch) == 1) {
            // exit();
            printf("exiting\n");
            return 0;
        }

        for (i = 0; i < num_cmds; i++) {
            if (cmd[i].argc > 0) {
                // printf("argc: %d \n", cmd[i].argc);
                // for (j = 0; j < cmd[i].argc; j++) {
                //     printf("argv: %s ", cmd[i].argv[j]);
                // }
                // printf("\n");

                //quit loop
                if (strcmp(cmd[i].argv[0], "quit") == 0) {
                    //TODO: wait until all children finish executing. store pids?
                    // exit();
                    printf("exiting\n");
                    return 0;
                }

                execCmd(&cmd[i]);
            }
        }
    }


    // printf("exiting\n");
    // for (i = 0; i < num_pids; i++) {
    //     waitpid(-1, NULL, 0);
    // }

    return 0;
}

int parseLine(struct Cmd cmd[], FILE *input_filestream, int isBatch) {
    char *line = NULL;
    size_t len = 0;
    ssize_t line_len = 0;
    char *cmd_str;
    int cmd_num = 0;
    int i;

    line_len = getline(&line, &len, input_filestream);
    //getline returns -1 if EOF
    if (line_len == -1) {
        return 1;
    }

    // if batch mode, echo input
    if (isBatch) {
        printf("%s\n", line);
    }

    cmd_str = strtok(line, ";");

    while (cmd_str != NULL) {
        for (i = 0; i < strlen(cmd_str); i++) {
            if (!isspace(cmd_str[i])) {
                parseCmd(cmd_str, &cmd[cmd_num++]);
                break;
            }
        }

        cmd_str = strtok(NULL, ";");
    }

    return 0;
}

int parseCmd(char *cmd_str, struct Cmd *cmd) {
    int cmd_len = strlen(cmd_str);
    char tmp_cmd[WORDSIZE];
    char reset_tmp[WORDSIZE];
    int prev_space = 1;
    int char_index = 0;
    char c;
    int i,j;

    for (i = 0; i < cmd_len+1; i++) {
        c = cmd_str[i];
        if (i == cmd_len) {
            c = ' ';
        }
        if (isspace(c)) {
            if (!prev_space) {
                tmp_cmd[char_index] = '\0';
                //memory leak??
                cmd->argv[cmd->argc] = malloc(strlen(tmp_cmd));
                strcpy(cmd->argv[cmd->argc], tmp_cmd);
                strcpy(tmp_cmd, reset_tmp);
                cmd->argc++;
                char_index=0;
          }
          prev_space=1;
        } else {
            tmp_cmd[char_index] = c;
            char_index++;
            prev_space=0;
        }
    }

    return 0;
}

int execCmd(struct Cmd *cmd) {
    int pid;
    int isBackground = 0;
    FILE *outputFile;
    int j;
    int pipefd[2];
    char *buf = "du";

    // Background Execution mode
    isBackground = getBackgroundExecution(cmd);

    // I/O Redirection
    outputFile = getOutputFile(cmd);
    if (outputFile == NULL) {
        fputs("Invalid output file.\n", stderr);
        return 1;
    }

    // printf("argc: %d \n", cmd->argc);
    // for (j = 0; j < cmd->argc; j++) {
    //     printf("argv: %s ", cmd->argv[j]);
    // }
    // printf("\n");

    if(pipe(pipefd)==-1) {
      //stderr
      printf("error");
    }
    //fork child
    if(cmdCases(cmd, outputFile)) {
      
      pid = fork();
      printf("%d\n",pid);
      // Child process
      if (pid == 0) {
          //execute command
          //need to use interprocess communication to tell parent new directory 
          close(pipefd[0]);
          write(pipefd[1],"h",2);

          //terminate child process
          exit(0);
      }

      // Parent process
      //wait for child to finish if not in background execution mode
      else if (!isBackground){
          waitpid(pid, NULL, 0);
          close(pipefd[1]);
          while(read(pipefd[0], &buf,sizeof(buf))>0) {
            printf("pipe: %s\n",buf);
          }
      } else {
          printf("not waiting\n");
          // num_pids++;
      }
    }

    return 0;
}

FILE *getOutputFile(struct Cmd *cmd) {
    FILE *outputFile = stdout;
    int i;

    for (i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], ">") == 0) {
            outputFile = fopen(cmd->argv[i+1], "w");
            cmd->argv[i] = 0;
            cmd->argc = i;
            break;
        } else if (strcmp(cmd->argv[i], ">>") == 0) {
            outputFile = fopen(cmd->argv[i+1], "a");
            cmd->argv[i] = 0;
            cmd->argc = i;
            break;
        }
    }

    return outputFile;
}

int getBackgroundExecution(struct Cmd *cmd) {
    int len_last = strlen(cmd->argv[cmd->argc-1]);
    if (cmd->argv[cmd->argc-1][len_last-1] == '&') {
        if (len_last == 1) {
            cmd->argc--;
        } else {
            cmd->argv[cmd->argc-1][len_last-1] = 0;
        }
        return 1;
    }
    return 0;
}

int cmdCases(struct Cmd *cmd, FILE *outputFile) {
    int i;
    char *out;

    if (strcmp(cmd->argv[0], "cd") == 0) {
      out =malloc(strlen(cmd->argv[0]) + strlen(cmd->argv[1]) + 2);
      strcpy(out, cmd->argv[0]);
      strcat(out," ");
      strcat(out,cmd->argv[1]);
      printf("%s\n",out);
      system(out);
      //chdir(cmd->argv[i]);
      system("pwd");

    } else if (strcmp(cmd->argv[0], "clr") == 0) {
        system("clear");
    } else if (strcmp(cmd->argv[0], "dir") == 0) {
      if(cmd->argc == 1) {
        system("ls");
      } else {
        char *tmp = "ls"; 
        out=malloc(strlen(tmp)+ strlen(cmd->argv[1]) +2);
        system("ls");  
      }
    } else if (strcmp(cmd->argv[0], "environ") == 0) {
      system("printenv");

    } else if (strcmp(cmd->argv[0], "echo") == 0) {
        for (i = 1; i < cmd->argc; i++) {
              fprintf(outputFile, "%s ", cmd->argv[i]);
        }
        fprintf(outputFile, "\n");
    } else if (strcmp(cmd->argv[0], "help") == 0) {
      system("more userManual");
    } else if (strcmp(cmd->argv[0], "shell") == 0) {
     // create new shell within shell 

    } else if (strcmp(cmd->argv[0], "pause") == 0) {
        // entire operation even background?
        //getchar();
    } else {
        //default: execute program
        //also handle invalid commands
        return 1;
    }
    return 0;

}

int createUserManual() {
    FILE *outputFile;
    outputFile = fopen("userManual", "w");
    int numCmds=8;
    int i =0;
    
    for(;i<10;i++) {
      fprintf(outputFile, "%s","Shell environment developed by Lisa Fan and Tyler Frasca\n");
      fprintf(outputFile, "%s","cd <directory> --Change current directory to <directory>\n");
      fprintf(outputFile, "%s","clr --Clear the Screen\n");
      fprintf(outputFile, "%s","dir <directory> --List contents of <directory>\n");
      fprintf(outputFile, "%s","echo --Display <comment> '>' overwrites file '>>' appends to file\n");
      fprintf(outputFile, "%s","environ --Display environment strings\n");
      fprintf(outputFile, "%s","help --View User Manual\n");
      fprintf(outputFile, "%s","pause --Pauses operation until Enter\n");
      fprintf(outputFile, "%s","quit --Exit shell\n");
    }
    fclose(outputFile);
}
