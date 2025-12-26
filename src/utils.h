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

#define HISTORY_FILE ".rshell_history"
#define SHELL_NAME "ROGERLUX"

#define SEMICOLON 0 // execute next command after ;
#define AND_AND 1 // execute next command after && if previous succeeded
#define OR_OR 2 // execute next command after || if previous failed
#define PIPE 3 // execute next command with input from previous command

#define REDIRECT_IN 4 // <
#define REDIRECT_OUT 5 // >
#define REDIRECT_OUT_APPEND 6 // >>

#define AND_BACKGROUND 1 // for background processes &




/* command representation (multiple arguments) */
typedef struct  {
    char ** argv;
    int ended; 
} Command ;

/* line of multiple commands, needed for ; usage & maybe more after */
typedef struct {
    Command* cmds;
    int totalcmds;
} Line ;

typedef struct {
    char** lines;
    int total_lines;
    int current_line; // for navigating through history
  } History ;

void print_words(Line words);

extern History history ;





#endif