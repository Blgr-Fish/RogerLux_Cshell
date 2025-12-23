#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

#define BUFFER_SIZE 1024 
#define TOKEN_DELIMITER " \t" // set of delimiters
#define MAX_CMDS 10

#define DELETE_CHAR 127
#define ESC_CHAR 27


/* command representation (multiple arguments) */
typedef struct  {
    char ** argv;
} Command ;

/* line of multiple commands, needed for ; usage & maybe more after */
typedef struct {
    Command* cmds;
    int totalcmds;
} Line ;

struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); 
}

void enable_raw_mode() {
    struct termios raw;
    atexit(disable_raw_mode);
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


// methods enumeration
void loop();
char * read_line();
Line parse_line(char * line);
void print_words(Line words);
int exec_word(Command command);

// built-in methods
int exec_exit(char ** args);
int exec_cd(char ** args);




/* Print all the words in a list */
void print_words(Line words) {
    int i ;

    for (int tcmds = 0 ; tcmds <= words.totalcmds ; ++ tcmds) {
        i = 0;
        while (words.cmds[tcmds].argv[i] != NULL) {
            printf("%s\n", words.cmds[tcmds].argv[i]);
            i++;
        }
    }
    
}

/* Read a line from the standard input */
char * read_line() {
    int buffer_size = BUFFER_SIZE ;
    int position = 0 ;

    char * buffer = malloc(sizeof(char) * buffer_size) ;

    char c ;

    // no space for allocation
    if (!buffer) {
        fprintf(stderr, "error: Allocation error\n") ;
        exit(EXIT_FAILURE) ;
    }

    while (1) {

        c = getchar() ;

        // enf of line
        if (c == EOF || c == '\n') {
            buffer[position] = '\0' ;
            printf("\n");
            return buffer ;
        } 
        // delete character
        else if (c == DELETE_CHAR) {
            
            if (position > 0) {
                position -- ;
                printf("\b \b") ;
                fflush(stdout) ;
            }
        // only for arrow characters for now
        }else if (c == ESC_CHAR) {
            
            // arrows are composed of 3 bytes, so we need to take of the 2 next chars
            char seq1 = getchar();
            char seq2 = getchar();
                /* 
                * for later if i want to handle this...
                    switch (seq2) {
                        case 'A': // up
                        case 'B': // down
                        case 'C': // right
                        case 'D': // left
                    }
                */

        } else {
            buffer[position++] = c ;
            printf("%c", c);
        }

        if (position >= buffer_size) {

            buffer_size += BUFFER_SIZE ;
            buffer = realloc(buffer, sizeof(char)* buffer_size);

            // no space for allocation
            if (!buffer) {
                fprintf(stderr, "error: Allocation error\n") ;
                exit(EXIT_FAILURE) ;
            }
        }
    }
}

/* Parse a line into tokens */
Line parse_line( char * line) {
    int buffer_size = BUFFER_SIZE ;
    int position = 0 ;

    char * t ;
    //char ** tokens = malloc(sizeof(char*) * buffer_size);
    Line pline;
    pline.cmds = malloc(sizeof(Command) * MAX_CMDS);
    pline.totalcmds = 0 ;
    pline.cmds[pline.totalcmds].argv = malloc(sizeof(char*) * buffer_size);


    if (!pline.cmds[pline.totalcmds].argv) {
        fprintf(stderr, "error: Allocation error\n");
        exit(EXIT_FAILURE);
    }

    t = strtok(line, TOKEN_DELIMITER) ;
    while (t != NULL) {

        if (strcmp(t, ";") == 0) {
            
            pline.cmds[pline.totalcmds].argv[position] = NULL ;
            
            // reset values for new cmd
            position = 0 ;
            buffer_size = BUFFER_SIZE ;
            pline.totalcmds ++ ;

            if (pline.totalcmds >= MAX_CMDS) {
                fprintf(stderr, "error: too many commands\n");
                exit(EXIT_FAILURE);
            }

            pline.cmds[pline.totalcmds].argv = malloc(sizeof(char*) * buffer_size);

            if (!pline.cmds[pline.totalcmds].argv) {
                fprintf(stderr, "error: Allocation error\n");
                exit(EXIT_FAILURE);
            }

        } else {
            pline.cmds[pline.totalcmds].argv[position++] = t ;
        }
        
        if (position >= buffer_size) {
            buffer_size += BUFFER_SIZE ;
            pline.cmds[pline.totalcmds].argv = realloc(pline.cmds[pline.totalcmds].argv,sizeof(char * )* buffer_size);

            if (!pline.cmds[pline.totalcmds].argv) {
                fprintf(stderr, "error: Allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        t = strtok(NULL, TOKEN_DELIMITER);

    }

    // to ensure the end of the word
    pline.cmds[pline.totalcmds].argv[position] = NULL ;
    pline.totalcmds++;
    return pline ;

}

/* Execute a command */
int exec_words(Command command) {
    pid_t child_pid ;
    int exec_status ;

   
        if (strcmp(command.argv[0],"exit") == 0) {
            return exec_exit(command.argv);
        }
        if (strcmp(command.argv[0],"cd") == 0) {
            return exec_cd(command.argv);
        }

        child_pid = fork() ;

        if (child_pid == 0) {
            if (execvp(command.argv[0], command.argv) < 0) {
                printf("error: command doesn't exist : %s\n", command.argv[0]);
                exit(EXIT_FAILURE);
            }
        } else if (child_pid == -1) {
            fprintf(stderr, "error: process creation error\n");
            exit(EXIT_FAILURE);
        } else {
            waitpid(child_pid, &exec_status, 0) ; // 0 since we don't need any option for now
        }
    
    

    return 1 ;

}

int exec_exit(char ** words) {
    return 0 ;
}

int exec_cd(char ** words) {

    int status = 1 ;

    if (words[1] == NULL || strcmp(words[1], "~") == 0) {
        words[1] = getenv("HOME") ;
    }


    if (chdir(words[1]) != 0) {
        printf("error: directory doesn't exist : %s\n", words[1]);
        status = -1 ;
    }


    return status ;
}

/* Loop to read and execute commands */
void loop() {
    char * line ;
    Line words ; // parsed line
    int status = 1 ;
    

    int w_size = 1024 ;
    char working_directory[w_size] ;


    do {
        
        getcwd(working_directory, w_size) ;
        printf("\n╭─(%s)\n╰─>",working_directory);
        enable_raw_mode() ;
        line = read_line() ;
        disable_raw_mode() ;
        //printf("line is : %s",line);
        words = parse_line(line) ;
        //print_words(words) ;
        
        for (int tcmds = 0 ; tcmds <= words.totalcmds ; ++tcmds) {
            if (words.cmds[tcmds].argv[0] != NULL) {
                status = exec_words(words.cmds[tcmds]);
            }
        }
        
        for (int tcmds = 0 ; tcmds < words.totalcmds ; ++tcmds) {
            free(words.cmds[tcmds].argv);
        }
        free(words.cmds);
        free(line);
        

    } while (status != 0) ;

}

int main() {
    loop();
    return 0;
}