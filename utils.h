#ifndef UTILS_H
#define UTILS_H

#include "includes.h"

#define BUFFER_SIZE 1024 
#define TOKEN_DELIMITER " \t" // set of delimiters
#define MAX_CMDS 10
#define HISTORY_SIZE 100

#define DELETE_CHAR 127
#define ESC_CHAR 27

#define SHELL_VALID 0
#define SHELL_ERROR 1
#define SHELL_UNKOWN_COMMAND 127
#define SHELL_EXIT 256

#define HISTORY_FILE ".cshell_history"





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

typedef struct {
    char** lines;
    int total_lines;
  } History ;

void print_words(Line words);

extern History history ;





#endif