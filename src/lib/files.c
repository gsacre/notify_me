#include "files.h"

int file_exists (char * fileName) {
        struct stat buf;
        int i = stat ( fileName, &buf );
        /* File found */
        if ( i == 0 ) {
                return 1;
        }
        return 0;
}
