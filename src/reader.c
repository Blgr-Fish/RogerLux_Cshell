#include "reader.h"
#include <termios.h>

static struct termios orig_termios;
 

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

/* Read a line from the standard input */
char * read_line() {
    int buffer_size = BUFFER_SIZE ;
    int position = 0 ;

    char * buffer = malloc(sizeof(char) * buffer_size) ;

    char c ;

    // no space for allocation
    test_buffer_error(buffer);

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
                 
            switch (seq2) {
                case 'A': // up

                    if (history.lines[0]) {

                        int c_line = history.current_line-- ;
                        if (c_line <= 0 ){
                            c_line = 0 ; 
                            history.current_line = 0 ;
                        } 
                            
                        char * old_command = history.lines[c_line] ;
                        int old_size = strlen(old_command);

                        if (old_size + 1 > buffer_size ) {
                            buffer_size += BUFFER_SIZE + 1 ;
                        }

                        buffer = realloc(buffer, buffer_size);
                        test_buffer_error(buffer);
                        strcpy(buffer,old_command);
                        position = old_size;

                        printf("\r\33[2K╰─>%s",old_command);
                        fflush(stdout);  
                    }
                    break;
                case 'B': // down

                    if (history.lines[0]) {

                        int c_line = history.current_line++ ;
                        if (c_line >= history.total_lines ){
                            c_line = history.total_lines-1 ; 
                            history.current_line = history.total_lines-1 ;
                        } 
                            
                        char * old_command = history.lines[c_line] ;
                        int old_size = strlen(old_command);

                        if (old_size + 1 > buffer_size ) {
                            buffer_size += BUFFER_SIZE + 1 ;
                        }

                        buffer = realloc(buffer, buffer_size);
                        test_buffer_error(buffer);
                        strcpy(buffer,old_command);
                        position = old_size;

                        printf("\r\33[2K╰─>%s",old_command);
                        fflush(stdout);  
                    }
                    break;
                case 'C': // right
                    break;
                case 'D': // left
                    break;
                default:
                    break;
            }
                
        } else {
            buffer[position++] = c ;
            printf("%c", c);
        }

        if (position >= buffer_size) {

            buffer_size += BUFFER_SIZE ;
            buffer = realloc(buffer, sizeof(char)* buffer_size);

            // no space for allocation
            test_buffer_error(buffer);
        }
    }
}


char * add_spaces(char * buffer) {

    int ci = 0 ;
    
    char * clean_line = malloc(2*strlen(buffer)+1);

    size_t sbuffer = strlen(buffer);

    
    for (size_t i = 0 ; i <= sbuffer ; ++i) {


        if ((buffer[i] == '&' && buffer[i+1] == '&') || (buffer[i] == '|' && buffer[i+1] == '|') || (buffer[i] == '>' && buffer[i+1] == '>')) {
            clean_line[ci++] = ' ';
            clean_line[ci++] = buffer[i];
            clean_line[ci++] = buffer[i+1];
            clean_line[ci++] = ' ';
            i++; 
        }
        else if (buffer[i] == ';' || buffer[i] == '>' || buffer[i] == '<' || buffer[i] == '&' || buffer[i] == '|') {
            clean_line[ci++] = ' ';
            clean_line[ci++] = buffer[i];
            clean_line[ci++] = ' ';
        }  else {
            clean_line[ci++] = buffer[i];
        }
    }

    return clean_line ;
}

void test_buffer_error(char * buffer){
    if (!buffer) {
        fprintf(stderr, "error: Allocation error\n");
        exit(EXIT_FAILURE);
    }
}