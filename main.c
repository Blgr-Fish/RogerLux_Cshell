#include "utils.h"
#include "executor.h"
#include "parser.h"
#include "reader.h"



/* Loop to read and execute commands */
void loop() {
    char * line ;
    Line words ; // parsed line
    int shell_status = SHELL_VALID ; // last command status 
    

    int w_size = 1024 ;
    char working_directory[w_size] ;


    do {
        
        // for the style effect
        getcwd(working_directory, w_size) ;
        printf("\n╭─(%s)\n╰─>",working_directory);


        // enabling terminal raw mode to read real bytes and not modified values by the terminal
        enable_raw_mode() ;
        line = read_line() ;
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
            free(words.cmds[tcmds].argv);
        }
        free(words.cmds);
        free(line);
        

    } while (shell_status != SHELL_EXIT) ;

}

int main() {
    loop();
    return 0;
}