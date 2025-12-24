#include "utils.h"


History history ; 

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

