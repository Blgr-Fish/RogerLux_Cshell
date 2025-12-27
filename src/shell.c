#include "shell.h"

/* Add a command to the history file. Used at the end of each command */
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

void from_history(){
    
    char * home = getenv("HOME");
    if (!home) {
        printf("error: home directory doesn't exist\n");
        return ;
    }

    char home_history[BUFFER_SIZE];
    snprintf(home_history, sizeof(home_history), "%s/%s", home, HISTORY_FILE);
    
    FILE *fptr = fopen(home_history,"r");

    if (!fptr) {
        perror("fopen");
        return;
    }

    char command[BUFFER_SIZE];

    while(fgets(command, BUFFER_SIZE, fptr)) {
            //printf("%s\n",command);


            if(history.total_lines >= history.capacity ) {;
                history.capacity += HISTORY_SIZE ;
                history.lines = realloc(history.lines, history.capacity * sizeof(char *));
            }

            command[strcspn(command, "\n")] = '\0';
            history.lines[history.total_lines++] = strdup(command);
            history.current_line = history.total_lines-1 ;

    }

}

/* Initialize history */
void init_history() {
    history.lines = malloc(sizeof(char*) * HISTORY_SIZE);
    history.total_lines = 0;
    history.current_line = 0 ;
    history.capacity = HISTORY_SIZE ;

   from_history();

   printf("voici capacité :%d\n", history.capacity);
   printf("voici total_lines :%d\n", history.total_lines);
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
        
        // handle commands depending on their ending & the last status
        shell_status = handle_command(words, shell_status);

            
        
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

        if (strlen(old_line) > 0){

            if(history.total_lines >= history.capacity ) {;
                history.capacity += HISTORY_SIZE ;
                history.lines = realloc(history.lines, history.capacity * sizeof(char *));
            }

            history.lines[history.total_lines++] = strdup(old_line);
            history.current_line = history.total_lines-1 ;
        }
        free(old_line) ;
        free(words.cmds);
        free(line);
        
    } while (shell_status != SHELL_EXIT) ;

    for (int i = 0 ; i < history.total_lines ; ++i){
        free(history.lines[i]);
    } free(history.lines);

}