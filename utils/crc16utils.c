#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define CRC16U_DIRECTION_L 0 // <<
#define CRC16U_DIRECTION_R 1 // >>

//"CRC16.L.0x8005.0x0000" -> 0x8005
// номера частей в строке обозначения алгоритма:
#define CRC_ID_ALG       1 // обозначение группы алгоритмов CRC (CRC8/CRC16/CRC24/CRC32...)
#define CRC_ID_DIR       2 // обозначение порядка обработки бит в алгоритме (L/R)
#define CRC_ID_BASE      3 // основание CRC в реализации алгоритма
#define CRC_ID_INIT      4 // начальное значение CRC 
#define CRC_ID_USER      5 // возможны и другие поля в описании (например, пост обработка результата CRC и др.)
#define CRC_DISCRIPTION_STRING  CRC_ID_ALG.CRC_ID_DIR.CRC_ID_BASE.CRC_ID_INIT
//

static unsigned int get_posPoint(char * option) {
    int i;
    if (option != 0) {
        for (i = 0; 1; i++) {
            if (option[i] == 0) break;
            if (option[i] == '.') return i;
        }
    }
    return -1;

}

static char * get_OptionPart(char * option, int n) {
    static char param[10] = "";
    char localoption[40] = "";
    int i, j;
    int startPos = 0;
    int nOpt = 0;
    if (option != 0) {
        //"CRC16.L.0x8005.0x0000" -> "CRC16.L.0x8005.0x0000." для удобства...
        snprintf(localoption, sizeof (localoption), "%s.", option);
        for (i = 0; i < n; i++) {
            j = get_posPoint(&localoption[startPos]);
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
    return 0;

}

static unsigned int str2int(char * str, unsigned int defaultValue) {
    unsigned int result = defaultValue;
    if (str != 0) {
        if ((str[1] == 'x') || ((str[1] == 'X'))) //0x000A -> 000A -> 10
            result = hexUtils_hex2word(&str[2]);
        else
            result = atoi(str);
    }
    return result;
}


//"CRC16.L.0x8005.0x0000" -> 0x8005 (3-я часть))

static unsigned int get_CRCBase(char * option) {
    unsigned int result = 0x1021;
    char * param;
    if (option != 0) {
        param = get_OptionPart(option, CRC_ID_BASE);
        result = str2int(param, result);
    }
    return result;
}

//"CRC16.L.0x8005.0x0000" -> 0x0000 (4-я часть))

static unsigned int get_CRCInit(char * option) {
    unsigned int result = 0xFFFF;
    char * param;
    if (option != 0) {
        param = get_OptionPart(option, CRC_ID_INIT);
        result = str2int(param, result);
    }
    return result;
}

static unsigned int get_CRCDir(char * option) {
    unsigned int result = CRC16U_DIRECTION_L;
    char * param;
    if (option != 0) {
        param = get_OptionPart(option, CRC_ID_DIR);
        if (param != 0)
            if (param[0] == 'R')
                result = CRC16U_DIRECTION_R;
    }
    return result;
}

static unsigned int doShiftReg(unsigned int reg, unsigned int dir) {
    if (dir == CRC16U_DIRECTION_L)
        reg = reg << 1;
    else
        reg = reg >> 1;
    return reg;

}

unsigned int crc16u_calc(unsigned char *b, int count, char * option) {
    unsigned int crc;
    unsigned int dir;
    unsigned int base;
    unsigned int mainBitMask;
    int i;
    int n;

    crc = get_CRCInit(option);
    dir = get_CRCDir(option);
    base = get_CRCBase(option);
    //printf(" (DBG: base:%X, dir:%d, init:%d) ", base, dir, crc);

    if (dir == CRC16U_DIRECTION_L)
        mainBitMask = 0x8000;
    else
        mainBitMask = 0x0001;

    for (n = 0; n < count; n++) {
        if (dir == CRC16U_DIRECTION_L) {
            crc = crc ^ (b[n] << 8);
        } else {
            crc = crc ^ (b[n]);
        }
        // проход по битам...
        for (i = 0; i < 8; i++) {
            if (crc & mainBitMask) {
                crc = doShiftReg(crc, dir);
                crc = crc ^ base;
            } else {
                crc = doShiftReg(crc, dir);
            }
        }
    }
    return crc & 0xFFFF;
}

char * crc16u_getUseOption(char * option) {
    static char id[30];
    unsigned int crc;
    unsigned int dir;
    unsigned int base;

    crc = get_CRCInit(option);
    dir = get_CRCDir(option);
    base = get_CRCBase(option);
    if (dir == CRC16U_DIRECTION_L)
        snprintf(id, sizeof (id), "CRC16.L.0x%04X.0x%04X", base, crc);
    else
        snprintf(id, sizeof (id), "CRC16.R.0x%04X.0x%04X", base, crc);
    return id;

}

unsigned int crc16u_test(void) {

    printf("\nDefault : %04X", crc16u_calc("123456789", 9, ""));
    printf("\nCRC16.L.0x8005.0x0000 : %04X", crc16u_calc("123456789", 9, "CRC16.L.0x8005.0x0000"));
    printf("\nCRC16.L.0x1021.0xFFFF : %04X", crc16u_calc("123456789", 9, "CRC16.L.0x1021.0xFFFF"));
    printf("\nCRC16.R.0xA001.0xFFFF : %04X", crc16u_calc("123456789", 9, "CRC16.R.0xA001.0xFFFF"));
    printf("\nCRC16.L.0x1021.0x0000 : %04X", crc16u_calc("123456789", 9, "CRC16.L.0x1021.0x0000"));
    exit(0);
    /*  * */

}