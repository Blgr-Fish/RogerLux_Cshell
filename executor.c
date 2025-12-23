#include "executor.h"


/* Execute a command */
int exec_word(Command command) {
    pid_t child_pid ;
    int exec_status ;

   
        if (strcmp(command.argv[0],"exit") == 0) {
            return exec_exit();
        }
        if (strcmp(command.argv[0],"cd") == 0) {
            return exec_cd(command.argv);
        }

        child_pid = fork() ;

        if (child_pid == 0) {
            if (execvp(command.argv[0], command.argv) < 0) {
                printf("error: command doesn't exist : %s\n", command.argv[0]);
                exit(SHELL_UNKOWN_COMMAND);
            }
        } else if (child_pid == -1) {
            fprintf(stderr, "error: process creation error\n");
            return SHELL_ERROR;
        } else {
            waitpid(child_pid, &exec_status, 0) ; // 0 since we don't need any option for now
            if (WIFEXITED(exec_status)) {
                printf("exec_status = %d\n", WEXITSTATUS(exec_status));
                return WEXITSTATUS(exec_status) ;
            }

            
        }
    
    

    return SHELL_ERROR ;

}

int exec_exit() {
    return SHELL_EXIT ;
}

int exec_cd(char ** words) {

    int status = SHELL_VALID ;

    if (words[1] == NULL || strcmp(words[1], "~") == 0) {
        words[1] = getenv("HOME") ;
    }


    if (chdir(words[1]) != 0) {
        printf("error: directory doesn't exist : %s\n", words[1]);
        status = SHELL_ERROR ;
    }


    return status ;
}

int exec_history(char ** words) {
    return SHELL_VALID;
}