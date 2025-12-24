#include "utils.h"
#include "executor.h"
#include "parser.h"
#include "reader.h"

void init_history() {
    history.lines = malloc(sizeof(char*) * HISTORY_SIZE);
    history.total_lines = 0;
}

void add_to_history(char * cmd) {
    
    char * home = getenv("HOME");
    if (!home) {
        printf("error: home directory doesn't exist\n");
        return ;
    }

    char home_history[BUFFER_SIZE];
    // home_history = home + / + HSTORY_FILE
    snprintf(home_history, sizeof(home_history), "%s/%s", home, HISTORY_FILE);
    
    FILE *fptr = fopen(home_history,"a");

    if (!fptr) {
        perror("fopen");
        return;
    }

    fprintf(fptr, "%s\n", cmd);
    fclose(fptr);

}


/* Loop to read and execute commands */
void loop() {
    char * line ;
    Line words ; // parsed line
    int shell_status = SHELL_VALID ; // last command status 

    // create in-memory History datastructure
    init_history();
    
    int w_size = 1024 ;
    char working_directory[w_size] ;


    do {
        
        // for the style effect
        getcwd(working_directory, w_size) ;
        if (shell_status == SHELL_VALID) {
            printf("\n╭─(%s [ ✓ ])\n╰─>",working_directory);
        } else {
            printf("\n╭─(%s [ ✗ ])\n╰─>",working_directory);
        }


        // enabling terminal raw mode to read real bytes and not modified values by the terminal
        enable_raw_mode() ;
        line = read_line() ;
        char * old_line = line ;

        // add the written line ot the history file
        add_to_history(old_line);

        line = add_spaces(old_line);
        disable_raw_mode() ;
        
        // turning line into multiple commands & arguments
        words = parse_line(line) ;
        
        // executing all the commands with their args if they have some
        for (int tcmds = 0 ; tcmds < words.totalcmds ; ++tcmds) {
            if (words.cmds[tcmds].argv[0] != NULL) {
                if (tcmds == 0 
                    || shell_status == SHELL_VALID 
                    || words.cmds[tcmds-1].ended != 1 ) {
                        
                        shell_status = exec_word(words.cmds[tcmds]);
                    }
                } else {
                    shell_status = shell_status; // useless
                }
        }
            
        
        // freeing memory
        for (int tcmds = 0 ; tcmds < words.totalcmds ; ++tcmds) {
            for (int targvs = 0 ; words.cmds[tcmds].argv[targvs] ; ++targvs) {
                // free all arguments of the command
                free(words.cmds[tcmds].argv[targvs]);
            }
            // free the command pointer
            free(words.cmds[tcmds].argv);
        }
        // free the all commands pointer

        history.lines[history.total_lines++] = strdup(old_line);
        free(old_line) ;
        free(words.cmds);
        free(line);
        
    } while (shell_status != SHELL_EXIT) ;

    free(history.lines);

}

int main() {
    loop();
    return 0;
}