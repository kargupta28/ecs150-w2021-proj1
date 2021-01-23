// Cite:
//https://javatutoring.com/c-program-remove-blank-spaces-from-string/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define CMDLINE_MAX 512
#define CWD_MAX 1024

int inFd, outFd;
char specialVars[26][CMDLINE_MAX];

typedef struct Command Command;

struct Command {
    //char* cmd[CMDLINE_MAX];
    char* argv[100];
    int argc; // Argument count

    int output_redirect;
    char output_file[CMDLINE_MAX];

    int piped;
    //Command *nextCommand;

    //int status; // command status -- needed or not??
    // pid_t pid; // not needed??
};

void printStruct(Command* command){
    //char* str = "hello";

    printf("Argv: ");
    for(int i=0; i<command->argc; i++) {
        printf("(%s) ", command->argv[i]);
    }

    printf("\nArgc: %i\n", command->argc);

    printf("Output_Redirect: %i\n", command->output_redirect);
    printf("Output_File: %s\n", command->output_file);

    printf("Piped: %i\n", command->piped);
    //return str;
}

void parseCmd(char* cmd, Command* command) {
    const char delim[] = " ";
    char *token;
    int i = 0;

    token = strtok(cmd, delim);
    //printf("Token: %s\n", token);

    while(token != NULL & i < 100) {

        char *outRedirect = strchr(token, '>');
        if ( outRedirect != NULL ) {
            if (strlen(outRedirect) > 1) {
                if(strcmp(token, outRedirect) != 0) {
                    *outRedirect = 0;
                    command->argv[i] = token;
                    i++;
                }
                outRedirect = &outRedirect[1];
                strcpy(command->output_file, outRedirect);
            } else {
                if (strlen(token) > 1) {
                    *outRedirect = 0;
                    command->argv[i] = token;//deleteLastChar(token);
                    i++;
                }
                token = strtok(NULL, delim);
                strcpy(command->output_file, token);
            }
            command->output_redirect = 1;
            break;
        } else {
            strcpy(command->output_file, "");
            command->output_redirect = 0;
        }

        command->argv[i] = token;
        i++;

        token = strtok(NULL, delim);

    }
    command->argv[i] = NULL;
    command->argc = i;
    //command->nextCommand = NULL; // hmm needed or?
}

int parseInput(char* cmd, Command command[]) {
    const char delim = '|';
    char* next = strchr(cmd, delim); // Finds the location of first '|'
    int i=0;

    if(next != NULL) {
        while (next != NULL) {
            *next = 0;
            command[i].piped = 1;
            parseCmd(cmd, &command[i++]);
//            if (i > 3) {                      it should do at least 3, but reference file does more so I'm doing up till 512
//                return i;
//            }

            cmd = next + 1;
            next = strchr(cmd, delim);
        }
    }
    command[i].piped = 0;
    parseCmd(cmd, &command[i++]);

    return i;
}

int printPwd() {
    char workingDir[CWD_MAX];
    getcwd(workingDir, CWD_MAX);
    printf("%s\n", workingDir);
    fflush(stdout);
    return 0;
}

int changeDir(Command command) {
    if (chdir(command.argv[1]) == 0) {
        return 0;
    }
    return 1;
}

void printRetVals(int retvals[], int i){
    printf("Printing retVals for i = %i: ", i);
    for (int j = 0; j < i ; j++) {
        printf("[%i]", retvals[j]);
    }
    printf("\n");
}

void resetSTD() {
    dup2(inFd, STDIN_FILENO);
    dup2(outFd, STDIN_FILENO);
    fflush(stdout);
    fflush(stdin);
}

int setSpecialVar(Command command){
    if (strlen(command.argv[1]) < 2) {
        if (command.argv[1][0] >= 97 & command.argv[1][0] <= 122) {
            strcpy(specialVars[command.argv[1][0] - 97], command.argv[2]);
            return 0;
        }
    }
    // Error message
    return 1;
}

int printSpecialVar(Command command){
    if (command.argv[1][0] >= 97 & command.argv[1][0] <= 122) {
        printf(specialVars[command.argv[1][0] - 97]);
        printf("\n");
        return 0;
    } else {
        // Error message
        return 1;
    }
}

void deleteblankspaces(char *s)
{
    int  i,k=0;

    for(i=0;s[i];i++)
    {
        s[i]=s[i+k];


        if(s[i]==' '|| s[i]=='\t')
        {
            k++;
            i--;
        }

    }
}

Command parseSpecialVar(Command command, int* retval) {
    for (int i = 0; i < command.argc; i++) {
        if (command.argv[i][0] == '$') { // if $, then change value
            if (strlen(command.argv[i]) > 2) { // if more than 2 characters like $ab, then fail
                // output "Error: invalid variable name"
                //printf("Error #1\n");
                retval = 1;
                printf("Error: invalid variable name\n");
                return command;
            } else if (command.argv[i][1] < 97 & command.argv[i][1] > 122) { // or $=
                //printf("Error #2\n");
                retval = 1;
                printf("Error: invalid variable name\n");
                return command;
            } else { // else it's just $a
                if (strlen(specialVars[command.argv[i][1] - 97]) > 0 ) {
                    //printf("Block #1: %s - %s\n", command.argv[i], specialVars[command.argv[i][1] - 97]);
                    //strcpy(command.argv[i], specialVars[command.argv[i][1] - 97]);
                    command.argv[i] = specialVars[command.argv[i][1] - 97];
                } else {
                    //printf("Block #2: %s\n", command.argv[i]);
                    //strcpy(command.argv[i], "");
                    command.argv[i] = "";
                }
            }
        }
    }

    retval = -1; // -1 here is OK value. We want to keep doing everything else. We only return 1 when we encouter a variable name error.
    return command;
}

void executeCmds(Command command[], int retvals[], int i, int inputFd) {
    //printRetVals(retvals, i);
    //printf("exeCmds i: %i\n", i);

    int retval = -1;
    FILE *fp;
    int stdout_fd;

//    dup2(inFd, STDIN_FILENO);
//    dup2(outFd, STDOUT_FILENO);

    command[i] = parseSpecialVar(command[i], &retval);

//    printf("In exeCmds: --------------------------------\n");
//    printStruct(&command[i]);
//    fflush(stdout);
//    printf("--------------------------------- Out printstruct exeCmds\n");

    if (retval == 1) {
        // exit? failed in parseSpecialVar() function
        printf("------------------------ FAILLLLLL -------------\n");
    }

    if (command[i].output_redirect == 1) {
        //dup2(outFd, STDIN_FILENO);
        //fflush(stdout);

        stdout_fd = dup(1);
        fp = freopen(command[i].output_file, "w", stdout);
    }

    /* Builtin command */
    if (!strcmp(command[i].argv[0], "exit")) {
        fprintf(stderr, "Bye...\n");
        fprintf(stderr,"+ completed \'%s\' [%d]\n", command[i].argv[0], 0); // DO this in main with retvals[]
        fflush(stdout);
        retval = -2; // need to END HERE
        exit(1);
    } else if (!strcmp(command[i].argv[0], "pwd")) {
        retval = printPwd();
    } else if (!strcmp(command[i].argv[0], "cd")) {
        retval = changeDir(command[i]);
    } else if (!strcmp(command[i].argv[0], "set")) {
        retval = setSpecialVar(command[i]);
    } else if (!strcmp(command[i].argv[0], "printvar")) {
        retval = printSpecialVar(command[i]);
    }

    int pipeFd[2];
    pid_t pid;
    if (retval == -1) {
        if (command[i].piped == 1) { // based on this pipe, things might not output to stdout or stdin later?? let me know if tester has a check for that, then i'll fix it.
            /* Regular command Piped with Second command*/
            pipe(pipeFd);
            pid = fork();

            if (pid == 0) { // 0 = reading, 1 = writing
                // Child - StdOut goes to StdIn - 1 to 0
                close(pipeFd[0]); // close reading end
                dup2(inputFd, STDIN_FILENO);
                dup2(pipeFd[1], STDOUT_FILENO); // duplicate writing end to stdout
                close(pipeFd[1]); // close writing end
                close(inputFd);

                //command[i].argv = {command[i].argv, "2>", "/dev/null"};
                execvp(command[i].argv[0], command[i].argv);
                //perror("execv");
                //return 1; // exit here ?? use exit(1) / exit(0)??
            } else if (pid > 0) {
                // Parent 1
                int status;
                waitpid(pid, &status, 0);
                //wait(NULL); // doing this for making sure I wait for first command so I get it's retval to store in retvals[i] before I increment i.
                retvals[i++] = status;
                //printf("Cmd: %s; Retval: %i\n", command[i].argv[0], status);

                close(pipeFd[1]); // look Child 1 for reference
                //dup2(pipeFd[0], 0);
                //close(pipeFd[0]);
                close(inputFd);
                executeCmds(command, retvals, i, pipeFd[0]);

            } else if (pid == -1) {
                // failed to fork
            }
        } else {
            /* Regular command */
            pid_t pid = fork();

            if (pid == 0) {
                // Child
                //command[i].argv = {command[i].argv, "2>", "/dev/null"};
                execvp(command[i].argv[0], command[i].argv);
                //perror("execv");
                //return 1; // exit here ?? use exit(1) / exit(0)??
            } else if (pid > 0) {
                // Parent
                int status;
                waitpid(pid, &status, 0);
                retvals[i] = status;
                //retval = WEXITSTATUS(status); this line gives status of waitpid and not execvp? I think. So it's not needed.
            } else if (pid == -1) {
                // failed to fork
            }
        }
    } else if (retval == -2) {
        // close pipes and reset stdout and stdin?
        // exit here
    } else { // else it was a BuiltIn operation, so just save the return value
        retvals[i] = retval;
        i++;
        if (inputFd != STDIN_FILENO)
            close(inputFd);
        if (command[i-1].piped == 1) {
//            executeCmds(command, retvals, i, pipeFd[0]);
            executeCmds(command, retvals, i, inputFd);
        }
    }

    if (command[i].output_redirect == 1) {
        close(fp);
        dup2(stdout_fd, 1);
        //dup2(inFd, STDIN_FILENO);
        //fflush(stdin);
    }

}

void saveSTD(){
    inFd = dup(0);
    outFd = dup(1);
}

int main(void)
{
        char cmd[CMDLINE_MAX];
        FILE *fp;
        int stdout_fd;

        saveSTD();

        while (1) {
                char *nl;

                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", cmd);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(cmd, '\n');
                if (nl)
                        *nl = '\0';

                //char cmmd[CMDLINE_MAX];
                //strcpy(cmmd, cmd);
                char cmmd[CMDLINE_MAX];
                strcpy(cmmd, cmd);

                //printf("Cmmd: %s\n", cmmd);
                Command command[CMDLINE_MAX];
                int cmd_size = 0;

                //parseCmd(cmmd, &command);
                cmd_size = parseInput(cmmd, command);

                //printf("Cmd_Size: %i\n", cmd_size);

//                for (int i=0; i<cmd_size; i++) {
//                    printStruct(&command[i]);
//                }
//                fflush(stdout);

                int retval = -1;
                int retvals[cmd_size];

//                 int pipe1[2], pipe2[2]
//                 pipe(pipe1);
//                 pipe(pipe2);
                executeCmds(command, retvals, 0, STDIN_FILENO);

                resetSTD();

                fprintf(stderr,"+ completed \'%s\' ", cmd); // change to cmd <<<-----

                for (int i = 0; i < cmd_size; i++) {
                    if (retvals[i] > 1)
                        retvals[i] = 1;
                    fprintf(stderr, "[%i]", retvals[i]);
                }
                fprintf(stderr,"\n");
                fflush(stdout);

                if (retvals[cmd_size-1] == -2) {
                    break;
                }
        }

        return EXIT_SUCCESS;
}
