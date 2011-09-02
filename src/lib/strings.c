#include "strings.h"

char *trim ( char *str )
{
        int i = 0;
        int j = strlen ( str ) - 1;
        int k = 0;

        while ( isspace ( str[i] ) && str[i] != '\0' )
                i++;

        while ( isspace ( str[j] ) && j >= 0 )
                j--;

        while ( i <= j )
                str[k++] = str[i++];

        str[k] = '\0';

        return str;
}
