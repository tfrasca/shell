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
    struct Cmd cmd;
    int moreinput = 0;
    pid_t pid;
    int num_pids = 0;

    while(1) {
        //clear previous values
        cmd = ResetCmd;

        //if interactive mode, prompt input if finished processing line
        if (!isBatch && !moreinput) {
            printf("8[ ");
        }

        //parse command
        moreinput = parseCmd(&cmd, inputFile);

        //if batch mode, echo input
        //TODO: specs say to echo each LINE, not command?
        if (isBatch) {
            for (int i = 0; i < cmd.argc; i++) {
                  printf("%s ", cmd.argv[i]);
            }
            printf("\n");
        }

        //quit loop
        //TODO: check it works 100%
        if (feof(inputFile) || strcmp(cmd.argv[0], "quit") == 0) {
            break;
        }

        if (cmd.argc > 0) {
            //fork child
            pid = fork();

            // Child process
            if (pid == 0) {
                //execute command
                execCmd(&cmd);

                //terminate child process
                return 0;
            }

            // Parent process
            //wait for child to finish if not in background execution mode
            else if (cmd.argv[cmd.argc-1][strlen(cmd.argv[cmd.argc-1])-1] != '&'){
                waitpid(pid, NULL, 0);
            } else {
                num_pids++;
            }
        }
    }

    //TODO: wait until all children finish executing. store pids?
    printf("exiting\n");
    for (int i = 0; i < num_pids; i++) {
        waitpid(-1, NULL, 0);
    }

    return 0;
}

int parseCmd(struct Cmd *cmd, FILE *input_filestream) {
    int char_index=0;
    int prev_space=1;
    char c;

    //TODO: may want to get whole line and parse the line
    while((c=fgetc(input_filestream)) != EOF && c != '\n' && c != ';') {
        if (isspace(c)) {
            if (!prev_space) {
                cmd->argc++;
                char_index=0;
          }
          prev_space=1;
        } else {
            if (cmd->argc == 0) {
                cmd->argc = 1;
            }
            cmd->argv[cmd->argc-1][char_index] = c;
            char_index++;
            prev_space=0;
        }
    }

    // don't prompt next time in interactive mode
    if (c == ';') {
        return 1;
    } else {
        return 0;
    }
}

int execCmd(struct Cmd *cmd) {
    FILE *outputFile = stdout;

    // I/O Redirection
    for (int i = 0; i < cmd->argc; i++) {
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
        for (int i = 1; i < cmd->argc; i++) {
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


