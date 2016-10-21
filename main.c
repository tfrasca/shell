#include "main.h"

int main (int argc, char *argv[]) {
    FILE *inputFile;    //stdin in interactive mode or batchFile in batch mode
    int isBatch;        //specify whether in batch mode or interactive mode
    char *shellenv = malloc(strlen(getenv("PWD")) + strlen("/myshell"));
    
    //interactive mode
    if(argc == 1) {
        printf("Welcome to the Shell\n");
        isBatch = 0;
        inputFile = stdin;
    } 

    //batch mode
    else if(argc == 2) {
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

    //write a user manual file for the help command
    createUserManual();

    //set shell environment variable
    strcpy(shellenv, getenv("PWD"));
    strcat(shellenv, "/myshell");
    if (setenv("shell", shellenv, 1) == -1) {
        fprintf(stderr, "Unable to set shell environment variable.\n");
        exit(1);
    }

    shellLoop(inputFile, isBatch);

    if (isBatch) {
        fclose(inputFile);
    }

    return 0;
}

int shellLoop(FILE *inputFile, int isBatch) {
    int num_cmds = 0;
    struct Cmd cmd[MAXCMDS];
    pid_t pid;
    int num_pids = 0;
    pthread_t threads[MAXCMDS];    // array of threads
    int rtns[MAXCMDS];             // array of return values
    int t_succs[MAXCMDS];          // array of thread creation return values
    int i,j;

    while(1) {
        //clear previous values
        for (i=0; i < MAXCMDS; i++) {
            cmd[i] = ResetCmd;
        }

        //if interactive mode, prompt input if finished processing line
        if (!isBatch) {
            printf("8[ ");
        }

        //parse command, will return 1 if EOF in interactive mode
        num_cmds = parseLine(cmd, inputFile, isBatch);
        if (num_cmds == -1) {
            printf("exiting...\n");
            for (i = 0; i < num_pids; i++) {
                waitpid(-1, NULL, 0);
            }
            return 0;
        }

        if (num_cmds == 1) {
            if (strcmp(cmd[0].argv[0], "quit") == 0) {
                printf("exiting...\n");
                for (i = 0; i < num_pids; i++) {
                    waitpid(-1, NULL, 0);
                }
                return 0;
            }

            num_pids += (int)execCmd(&cmd[0]);
        } else {
            for (i = 0; i < num_cmds; i++) {
                // printf("argc: %d \n", cmd[i].argc);
                // for (j = 0; j < cmd[i].argc; j++) {
                //     printf("argv: %s ", cmd[i].argv[j]);
                // }
                // printf("\n");

                //quit loop
                if (strcmp(cmd[i].argv[0], "quit") == 0) {
                    printf("exiting...\n");
                    for (i = 0; i < num_pids; i++) {
                        waitpid(-1, NULL, 0);
                    }
                    return 0;
                }

                t_succs[i] = pthread_create(&threads[i], 
                        NULL, 
                        execCmd, 
                        (void *)&cmd[i] );
                
            }

            for (int i = 0; i < num_cmds; i++) {
                if (t_succs[i] == 0) {
                    pthread_join(threads[i], (void *)&rtns[i]);
                    num_pids += rtns[i];
                } else {
                    fprintf(stderr, "Could not create thread %d!\n", i+1);
                }
            }
        }
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

    //getline returns -1 if EOF
    if (line_len == -1) {
        return -1;
    }

    // if batch mode, echo input
    if (isBatch) {
        printf("%s\n", line);
    }

    //parse commands in a line separated by ';'
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

    return cmd_num;
}

int parseCmd(char *cmd_str, struct Cmd *cmd) {
    int cmd_len = strlen(cmd_str);
    char tmp_cmd[cmd_len];
    char reset_tmp[cmd_len];
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
            tmp_cmd[char_index++] = c;
            prev_space=0;
        }
    }

    return 0;
}

void *execCmd(void *v_cmd) {
    struct Cmd *cmd = (struct Cmd *)v_cmd;
    int pid;
    int isBackground = 0;
    FILE *fp;
    int fd;
    int j;

    // Background Execution mode
    isBackground = getBackgroundExecution(cmd);


    // printf("argc: %d \n", cmd->argc);
    // for (j = 0; j < cmd->argc; j++) {
    //     printf("argv: %s ", cmd->argv[j]);
    // }
    // printf("\n");

    //don't need to fork for cd
    if (strcmp(cmd->argv[0], "cd") == 0) {
        cdCmd(cmd);
    } else {
        pid = fork();

        // Child process
        if (pid == 0) {    
            // I/O Redirection
            if (getOutputFile(cmd) == 1) {
                fprintf(stderr, "Invalid redirect file\n");
                exit(1);
            }

            //execute command
            cmdCases(cmd);

            exit(0);
        }

        // Parent process
        //wait for child to finish if not in background execution mode
        else if (!isBackground){
            waitpid(pid, NULL, 0);
        }
    }

    if (isBackground) {
        return (void *)1;
    }

    return (void *)0;
}

int getOutputFile(struct Cmd *cmd) {
    FILE *fp = stdout;
    int fd;
    int i;

    for (i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], ">") == 0) {
            fp = fopen(cmd->argv[i+1], "w");
            break;
        } else if (strcmp(cmd->argv[i], ">>") == 0) {
            fp = fopen(cmd->argv[i+1], "a");
            break;
        }
    }

    //if redirect and redirect file are not last two words on command
    //(excluding background execution)
    if (i < cmd->argc-2 && strcmp(cmd->argv[cmd->argc-1], "&")) {
        return 1;
    }

    if (fp == NULL) {
        fputs("Invalid output file.\n", stderr);
        return 1;
    }

    if (fileno(fp) != fileno(stdout)) {
        if (dup2(fileno(fp), fileno(stdout)) < 0) {
            fprintf(stderr, "%s\n",strerror(errno)); 
        }
        close(fd);
        fclose(fp);        
    }

    return 0;
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

int cmdCases(struct Cmd *cmd) {
    int i;
    char *out;
    int cmd_size = 0;

    //length of entire command
    for (i = 0; i < cmd->argc; i++) {
        cmd_size += strlen(cmd->argv[i]) + 1;
    }

    if (strcmp(cmd->argv[0], "clr") == 0) {
        system("clear");
    } else if (strcmp(cmd->argv[0], "dir") == 0) {
        dirCmd(cmd, cmd_size);
    } else if (strcmp(cmd->argv[0], "environ") == 0) {
        environCmd();
    } else if (strcmp(cmd->argv[0], "echo") == 0) {
        echoCmd(cmd);
    } else if (strcmp(cmd->argv[0], "help") == 0) {
        helpCmd(cmd, cmd_size);
    } else if (strcmp(cmd->argv[0], "pause") == 0) {
        pauseCmd();
    } else {
        //default case:
        // runs external programs,
        //or deals with invalid commands
        extCmd(cmd);
    }
    return 0;
}

void cdCmd(struct Cmd *cmd) {
    char *oldpwd;
    char cwd[1024];

    if(cmd->argc > 1) {
        oldpwd = getenv("OLDPWD");

        //change cwd
        if (chdir(cmd->argv[1]) == -1) {
            fprintf(stderr, "%s\n",strerror(errno)); 
        } else {
            //change pwd and oldpwd
            setenv("OLDPWD", getenv("PWD"), 1);

            if(setenv("PWD", getcwd(cwd, sizeof(cwd)), 1) == -1) {
                chdir(getenv("OLDPWD"));
                setenv("OLDPWD",oldpwd,1);
                fprintf(stderr,"can't cd into %s\n",cmd->argv[1]);
            }
        }
    } else {
        // ask about reporting
        printf("%s\n",getenv("PWD"));
    }
}

void dirCmd(struct Cmd *cmd, int cmd_size) {
    int i;
    char *out;

    out = malloc(cmd_size + strlen(getenv("PWD")) + 1);

    strcpy(out, "ls ");

    //if directory is not specified
    if (cmd->argc == 1 || strcmp(cmd->argv[1], ">") == 0 || strcmp(cmd->argv[1], ">>") == 0) {
        strcat(out, getenv("PWD"));
    }

    //specified directory and possible redirect
    for (i = 1; i < cmd->argc; i++) {
        strcat(out, " ");
        strcat(out, cmd->argv[i]);
    }

    sem_wait(&io_sem);
    printf("%s\n", out);
    if (system(out) == -1) {
        fprintf(stderr, "%s\n",strerror(errno)); 
    }
    sem_post(&io_sem);

    free(out);
}

void environCmd() {
    int i = 0;

    sem_wait(&io_sem);
    while(environ[i]) {
        printf("%s\n", environ[i++]); 
    }
    sem_post(&io_sem);
}

void echoCmd(struct Cmd *cmd) {
    int i;

    sem_wait(&io_sem);
    for (i = 1; i < cmd->argc; i++) {
        //break if i/o redirect command
        if (strcmp(cmd->argv[i], ">") == 0 || strcmp(cmd->argv[i], ">>") == 0) {
            break;
        }
        printf("%s ", cmd->argv[i]);
    }
    printf("\n");
    sem_post(&io_sem);
}

void helpCmd(struct Cmd *cmd, int cmd_size) {
    int i;
    char *out;

    out = malloc(cmd_size + strlen("more userManual") + 1);

    strcpy(out, "more userManual");
    for (i = 1; i < cmd->argc; i++) {
        strcat(out, " ");
        strcat(out, cmd->argv[i]);
    }

    sem_wait(&io_sem);
    if (system(out) == -1) {
        fprintf(stderr, "%s\n",strerror(errno)); 
    }
    sem_post(&io_sem);

    free(out);
}

void pauseCmd() {
    // entire operation even background?
    sem_wait(&io_sem);
    printf("Press Enter to continue ---------------\n");
    sem_post(&io_sem);
    getchar();
}

void extCmd(struct Cmd *cmd) {
    if (setenv("parent", getenv("shell"), 1) == -1) {
        fprintf(stderr, "%s\n",strerror(errno)); 
    }
    printf("executing %s\n",cmd->argv[0]);
    if(execve(cmd->argv[0],cmd->argv,environ)==-1) {
        fprintf(stderr, "%s\n",strerror(errno)); 
    }
}

int createUserManual() {
    FILE *outputFile;
    outputFile = fopen("userManual", "w");
    
    fprintf(outputFile, "Shell environment developed by Lisa Fan and Tyler Frasca\n");
    fprintf(outputFile, "cd <directory> --Change current directory to <directory>\n");
    fprintf(outputFile, "clr --Clear the Screen\n");
    fprintf(outputFile, "dir <directory> --List contents of <directory>\n");
    fprintf(outputFile, "echo --Display <comment> '>' overwrites file '>>' appends to file\n");
    fprintf(outputFile, "environ --Display environment strings\n");
    fprintf(outputFile, "help --View User Manual\n");
    fprintf(outputFile, "pause --Pauses operation until Enter\n");
    fprintf(outputFile, "quit --Exit shell\n");

    fclose(outputFile);

    return 0;
}
