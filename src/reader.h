#ifndef READER_H
#define READER_H

#include "utils.h"

void disable_raw_mode() ;
void enable_raw_mode() ; 
char * read_line();
char * add_spaces(char * buffer) ;
void test_buffer_error(char * buffer) ;


#endif