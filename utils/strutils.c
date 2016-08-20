#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static unsigned int get_posPoint(char * option, char divider) {
    int i;
    if (option != 0) {
        for (i = 0; 1; i++) {
            if (option[i] == 0) break;
            if (option[i] == divider) return i;
        }
    }
    return -1;

}

// n : номер части от 1!
// ("dir\\subdir1\\subdir2", 1, "\\") -> "dir"
// ("dir\\subdir1\\subdir2", 3, "\\") -> "subdir2"
char * sutil_get_OptionPart(char * option, int n, char divider) {
    static char param[120] = "";
    char localoption[140] = "";
    int i, j;
    int startPos = 0;
    int nOpt = 0;
    if (option != 0) {
        //"CRC16.L.0x8005.0x0000" -> "CRC16.L.0x8005.0x0000." для удобства...
        //"dir\subdir1\subdir2"
        snprintf(localoption, sizeof (localoption), "%s%c", option, divider );
        // printf("\n localoption = %s", localoption);
        for (i = 0; i < n; i++) {
            j = get_posPoint(&localoption[startPos], divider);
            if (j > 0) {
                j = j + startPos; // не забудем что get_posPoint(&localoption[startPos]) - это относительное начало...
                nOpt++;
                if (nOpt == n) { // обработка нужной части
                    localoption[ j ] = 0; // заменим "." концом строки
                    snprintf(param, sizeof (param), "%s", &localoption[startPos]);
                    return param;
                }
                startPos = j + 1;
            } else
                break;
        }
    }
    sprintf(param, "");
    return param;

}

