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

    shellLoop(inputFile, isBatch);

    fclose(inputFile);

    return 0;
}

int shellLoop(FILE *inputFile, int isBatch) {
    int num_cmds = 10;
    struct Cmd cmd[num_cmds];
    int moreinput = 0;
    pid_t pid;
    int num_pids = 0;
    int i;
    int j;

    while(1) {
        //clear previous values
        for (i=0; i < num_cmds; i++) {
            cmd[i] = ResetCmd;
        }

        //if interactive mode, prompt input if finished processing line
        if (!isBatch && !moreinput) {
            printf("8[ ");
        }

        //parse command
        moreinput = parseLine(cmd, inputFile, isBatch);

        for (i = 0; i < num_cmds; i++) {
            if (cmd[i].argc > 0) {
                printf("argc: %d \n", cmd[i].argc);
                for (j = 0; j < cmd[i].argc; j++) {
                    printf("argv: %s \n", cmd[i].argv[j]);
                }
                printf("\n");
            }
        }


        //quit loop
        //TODO: check it works 100%
        // if (feof(inputFile) || strcmp(cmd.argv[0], "quit") == 0) {
        //     break;
        // }

        // if (cmd.argc > 0) {
        //     //fork child
        //     pid = fork();

        //     // Child process
        //     if (pid == 0) {
        //         //execute command
        //         execCmd(&cmd);

        //         //terminate child process
        //         return 0;
        //     }

        //     // Parent process
        //     //wait for child to finish if not in background execution mode
        //     else if (cmd.argv[cmd.argc-1][strlen(cmd.argv[cmd.argc-1])-1] != '&'){
        //         waitpid(pid, NULL, 0);
        //     } else {
        //         num_pids++;
        //     }
        // }
    }

    //TODO: wait until all children finish executing. store pids?
    printf("exiting\n");
    for (i = 0; i < num_pids; i++) {
        waitpid(-1, NULL, 0);
    }

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

    cmd_str = strtok(line, ";");

    while (cmd_str != NULL) {
        printf("cmdst: %s\n", cmd_str);
        for (i = 0; i < strlen(cmd_str); i++) {
            if (!isspace(cmd_str[i])) {
                parseCmd(cmd_str, &cmd[cmd_num++]);
                printf("cmd_num: %d\n", cmd_num);
                break;
            }
        }

        cmd_str = strtok(NULL, ";");
    }

    // if batch mode, echo input
    if (isBatch) {
        printf("%s\n", line);
    }

    // for (i = 0; i < line_len; i++) {
    //     c = line[i];
    //     if (c != EOF) {
    //         if (c == ';' && /*len prev > 0*/) {
    //             cmd_num++;
    //         } 
    //     }
    // }
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

    printf("cmdlen:%d\n", cmd_len);
    //TODO: EOF?
    for (i = 0; i < cmd_len+1; i++) {
        c = cmd_str[i];
        if (i == cmd_len) {
            c = ' ';
        }
        // printf("'%c'", c);
        if (isspace(c)) {
            if (!prev_space) {
                tmp_cmd[char_index] = '\0';
                printf("tmp:%s\n", tmp_cmd);
                cmd->argv[cmd->argc] = malloc(strlen(tmp_cmd));
                strcpy(cmd->argv[cmd->argc], tmp_cmd);
                strcpy(tmp_cmd, reset_tmp);
                cmd->argc++;
                char_index=0;
          }
          prev_space=1;
        } else {
            // if (cmd->argc == 0) {
            //     cmd->argc = 1;
            // }
            tmp_cmd[char_index] = c;
            char_index++;
            prev_space=0;
        }
    }

    return 0;
}

int execCmd(struct Cmd *cmd) {
    FILE *outputFile = stdout;
    int i;

    // I/O Redirection
    for (i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], ">") == 0) {
            outputFile = fopen(cmd->argv[i+1], "w");
        } else if (strcmp(cmd->argv[i], ">>") == 0) {
            outputFile = fopen(cmd->argv[i+1], "a");
        }

        if (outputFile == NULL) {
            fputs("Invalid output file.\n", stderr);
            return 1;
        }
    }
                
    if (strcmp(cmd->argv[0], "cd") == 0) {

    } else if (strcmp(cmd->argv[0], "clr") == 0) {
        system("clear");
    } else if (strcmp(cmd->argv[0], "dir") == 0) {

    } else if (strcmp(cmd->argv[0], "environ") == 0) {

    } else if (strcmp(cmd->argv[0], "echo") == 0) {
        for (i = 1; i < cmd->argc; i++) {
              fprintf(outputFile, "%s ", cmd->argv[i]);
        }
        fprintf(outputFile, "\n");
    } else if (strcmp(cmd->argv[0], "help") == 0) {

    } else if (strcmp(cmd->argv[0], "pause") == 0) {

    } else {
        //default: execute program
        //also handle invalid commands
    }

    fclose(outputFile);

    return 0;
}


