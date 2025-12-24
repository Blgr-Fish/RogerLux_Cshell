#ifndef UTILS_H
#define UTILS_H

#include "includes.h"

#define BUFFER_SIZE 1024 
#define TOKEN_DELIMITER " \t" // set of delimiters
#define MAX_CMDS 10

#define DELETE_CHAR 127
#define ESC_CHAR 27

#define SHELL_VALID 0
#define SHELL_ERROR 1
#define SHELL_UNKOWN_COMMAND 127
#define SHELL_EXIT 256





/* command representation (multiple arguments) */
typedef struct  {
    char ** argv;
    int ended; // 1 if &&, 0 else for now
} Command ;

/* line of multiple commands, needed for ; usage & maybe more after */
typedef struct {
    Command* cmds;
    int totalcmds;
} Line ;

void print_words(Line words);



#endif