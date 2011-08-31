#include "strings.h"

char *trim(char *str)
{
        char *ibuf = str, *obuf = str;
        int i = 0, cnt = 0;

        if (str) {
                for (ibuf = str; *ibuf && isspace(*ibuf); ++ibuf)
                        ;
                if (str != ibuf)
                        memmove(str, ibuf, ibuf - str);

                while (*ibuf) {
                        if (isspace(*ibuf) && cnt)
                                ibuf++;
                        else {
                                if (!isspace(*ibuf))
                                        cnt = 0;
                                else {
                                        *ibuf = ' ';
                                        cnt = 1;
                                }
                                obuf[i++] = *ibuf++;
                        }
                }

                obuf[i] = NUL;

                while (--i >= 0) {
                        if (!isspace(obuf[i]))
                                break;
                }

                obuf[++i] = NUL;
        }

        return str;
}

