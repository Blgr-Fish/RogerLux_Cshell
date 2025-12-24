#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "utils.h"



int exec_word(Command command);

// built-in methods
int exec_exit();
int exec_cd(char ** args);
int exec_history(char ** args);
void replace_tild(char ** argv);

#endif