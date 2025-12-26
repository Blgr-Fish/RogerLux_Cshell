#include "executor.h"


int handle_command(Line words, int last_status){

    int shell_status = last_status ;

    // executing all the commands with their args if they have some
    for (int tcmds = 0 ; tcmds < words.totalcmds ; ++tcmds) {

        if ( words.cmds[tcmds].argv[0] != NULL) {
            if ((words.cmds[tcmds].ended == REDIRECT_OUT || words.cmds[tcmds].ended == REDIRECT_OUT_APPEND)) {
                continue ;
            } else {
                switch (words.cmds[tcmds-1].ended) {
                    case SEMICOLON:
                        shell_status = exec_word(words.cmds[tcmds]);
                        break;
                    case AND_AND:
                        if (shell_status == SHELL_VALID)
                            shell_status = exec_word(words.cmds[tcmds]);
                        break;
                    case OR_OR:
                        if (shell_status != SHELL_VALID)
                            shell_status = exec_word(words.cmds[tcmds]);
                        break;
                    case PIPE:
                        // TODO 
                        break;
                    case REDIRECT_IN: // <
                        // TODO 
                        break;
                    case REDIRECT_OUT:
                        shell_status = write_to_file("w", shell_status, words, tcmds);
                        break;
                    case REDIRECT_OUT_APPEND:
                        shell_status = write_to_file("a", shell_status, words, tcmds);
                        break;
                    default:
                        shell_status = exec_word(words.cmds[tcmds]);
                        /*printf("here " ) ;
                        printf("executing command : %s\n", words.cmds[tcmds].argv[0] ) ;
                        printf("total commds :  %d\n", words.totalcmds) ;*/
                        break;
                }
            }

        }        
    }
        return shell_status ;
}

/* Execute a command */
int exec_word(Command command) {
    pid_t child_pid ;
    int exec_status ;

    replace_tild(command.argv);
        
    if (strcmp(command.argv[0],"exit") == 0) {
        return exec_exit();
    }
    if (strcmp(command.argv[0],"cd") == 0) {
        return exec_cd(command.argv);
    }
    if (strcmp(command.argv[0],"history") == 0) {
        return exec_history();
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
            //printf("exec_status = %d\n", WEXITSTATUS(exec_status));
            return WEXITSTATUS(exec_status) ;
        }

            
    }
    
    return SHELL_ERROR ;
}

void replace_tild(char ** argv){

    int a = 0 ;
    int b = 1 ;
    char * home = getenv("HOME") ; 

    if (!home) return ;

    while (argv[a] != NULL ) {
            if (argv[a][0] == '~') {
                char * newArg = malloc(strlen(home) + strlen(argv[a]) + 1 );
                strcpy(newArg, home);
                while(argv[a][b]){
                    strncat(newArg,&argv[a][b],1);
                    b++ ; 
                }
                b = 1 ;
                free(argv[a]);
                argv[a] = newArg ;
            }
            a++;
        }
}

int exec_exit() {
    return SHELL_EXIT ;
}

int exec_cd(char ** words) {

    int status = SHELL_VALID ;
    
    if (words[1] == NULL) {
        words[1] = strdup(getenv("HOME")) ;
    }
    
    if (chdir(words[1]) != 0) {
        printf("error: directory doesn't exist : %s\n", words[1]);
        status = SHELL_ERROR ;
    }
    
    return status ;
}

int exec_history() {

    for (int i = 0 ; i < history.total_lines ; ++i){
        printf("%d : %s\n",i+1, history.lines[i]);
    }


    return SHELL_VALID;
}


int  write_to_file(char * type, int shell_status, Line words, int tcmds) {
    if (shell_status == SHELL_VALID) {
        int  stdout_bk = dup(fileno(stdout));
        FILE * inputFile = fopen(words.cmds[tcmds].argv[0], type) ;
        if ( !inputFile ) {
            printf( "error; file not found for redirection");
            shell_status = SHELL_ERROR ;
        } else {
            dup2(fileno(inputFile), fileno(stdout));
                                
            shell_status = exec_word(words.cmds[tcmds-1]);
                                
            dup2(stdout_bk, fileno(stdout));
            close(stdout_bk);
        }
    }

    return shell_status ;
}