#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

#define BUFFER_SIZE 1024 
#define TOKEN_DELIMITER " \t" // set of delimiters


#define DELETE_CHAR 127
#define ESC_CHAR 27

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
char ** parse_line(char * line);
void print_words(char ** words);
int exec_word(char ** words);

// built-in methods
int exec_exit(char ** args);
int exec_cd(char ** args);




/* Print all the words in a list */
void print_words(char **words) {
    int i = 0;
    while (words[i] != NULL) {
        printf("%s\n", words[i]);
        i++;
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
char ** parse_line( char * line) {
    int buffer_size = BUFFER_SIZE ;
    int position = 0 ;

    char * t ;
    char ** tokens = malloc(sizeof(char*) * buffer_size);

    if (!tokens) {
        fprintf(stderr, "error: Allocation error\n");
        exit(EXIT_FAILURE);
    }

    t = strtok(line, TOKEN_DELIMITER) ;
    while (t != NULL) {
        tokens[position++] = t ;
        
        if (position >= buffer_size) {
            buffer_size += BUFFER_SIZE ;
            tokens = realloc(tokens,  sizeof(char * )* buffer_size);

            if (!tokens) {
                fprintf(stderr, "error: Allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        t = strtok(NULL, TOKEN_DELIMITER);

    }

    // to ensure the end of the word
    tokens[position] = NULL ;
    return tokens ;

}

/* Execute a command */
int exec_words(char ** words) {
    pid_t child_pid ;
    int exec_status ;

    if (strcmp(words[0],"exit") == 0) {
        return exec_exit(words);
    }
    if (strcmp(words[0],"cd") == 0) {
        return exec_cd(words);
    }

    child_pid = fork() ;

    if (child_pid == 0) {
        if (execvp(words[0], words) < 0) {
            printf("error: command doesn't exist : %s\n", words[0]);
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
    char ** words ; // parsed line
    int status = 1 ;
    

    int w_size = 1024 ;
    char working_directory[w_size] ;


    do {
        
        getcwd(working_directory, w_size) ;
        printf("\n╭─(%s)\n╰─>",working_directory);
        enable_raw_mode() ;
        line = read_line() ;
        disable_raw_mode() ;
        words = parse_line(line) ;
        //print_words(words) ;
        if (words[0] != NULL) {
            status = exec_words(words);
        }
        free(line);
        free(words);

    } while (status != 0) ;

}

int main() {
    loop();
    return 0;
}