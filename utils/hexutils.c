#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "hexutils.h"
#include "../hexConfigTextMsg.h"

static unsigned char hex4bit2byte(char byte);
static unsigned char hex2byte(char * pChar);

THexLineRec* hexUtils_str2hexRec(char * hexStrLine) {
    static THexLineRec HexLineRec;
    unsigned char i;
    unsigned char sum = 0;

    HexLineRec.key = 0;
    if (hexStrLine) {
        if (hexStrLine[HEX_KEY_OFFSET] == ':') { // работаем только с intel-форматом hex-файла
            HexLineRec.key = hexStrLine[HEX_KEY_OFFSET];
            HexLineRec.length = hex2byte(&hexStrLine[HEX_LEN_OFFSET]);
            HexLineRec.addr_lo = hex2byte(&hexStrLine[HEX_ADDR_HI_OFFSET]) << 8;
            HexLineRec.addr_lo |= hex2byte(&hexStrLine[HEX_ADDR_LO_OFFSET]);
            HexLineRec.recType = hex2byte(&hexStrLine[HEX_TYPE_OFFSET]);
            HexLineRec.sum = &HexLineRec.format.data[HexLineRec.length];

            sum = sum - HexLineRec.length;
            sum = sum - (HexLineRec.addr_lo) & 0xff;
            sum = sum - (HexLineRec.addr_lo >> 8) & 0xff;
            sum = sum - HexLineRec.recType;

            for (i = 0; i < HexLineRec.length + 1; i++) { // последний байт SUM
                HexLineRec.format.data[i] = hex2byte(&hexStrLine[HEX_DATA_OFFSET + (i << 1)]);
                sum = sum - HexLineRec.format.data[i];
            }
            if (sum == 0) { // контрольная сумма сошлась
                // анализ формата
                switch (HexLineRec.recType) {
                    case HEX_FORMAT_DATA:
                        break;
                    case HEX_FORMAT_ENDFILE:
                        break;
                    case HEX_FORMAT_START_ADDR:
                        snprintf(HexLineRec.format.text, sizeof(HexLineRec.format.text), "%s", hexStrLine);
                        break;
                    case HEX_FORMAT_ADDR_EXT:
                        // адрес задан в формате big-endian, нужно перевести...
                        HexLineRec.format.addr_hi = (HexLineRec.format.data[0] << 8) + (HexLineRec.format.data[1]);
                        break;
                    default:
                        printf(HC_TEXT_HEX_TYPE_IGNORE, hexStrLine);

                }

            } else {
                printf(HC_TEXT_HEX_CRC_IGNORE, sum, *HexLineRec.sum, hexStrLine);
            }

            return &HexLineRec;
        }
    }
    return NULL;
}



/*------------------------------------------------------------------------------
 * Hex Record Format:
 *
 * +--------+--------+------+-------+--------+------(n bytes)------+----------+
 * | RECORD | RECLEN |    OFFSET    | RECORD |                     | CHECKSUM |
 * |  MARK  |  (n)   |   (2 BYTES)  |  TYPE  |        DATA         |          |
 * |  ':'   |        |              |        |                     |          |
 * +--------+--------+------+-------+--------+------(n bytes)------+----------+
 *------------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static unsigned char hex4bit2byte(char byte) {
    unsigned char result = 0;

    switch (byte) {
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
            result = byte - 0x30;
            break;
        case 0x41:
            result = 0x0A;
            break;
        case 0x42:
            result = 0x0B;
            break;
        case 0x43:
            result = 0x0C;
            break;
        case 0x44:
            result = 0x0D;
            break;
        case 0x45:
            result = 0x0E;
            break;
        case 0x46:
            result = 0x0F;
            break;
    }
    return result;
}

static unsigned char hex2byte(char * pChar) {
    unsigned char result;
    // преобразуем два символа hex-кода в байт данных
    result = hex4bit2byte(pChar[0]) << 4;
    result += hex4bit2byte(pChar[1]);
    return result;
}


// формируем hex-строку из буфера. 
char* hexUtils_buffer2str(int hexType, char * buffer, unsigned char size, unsigned short int addr) {
    static char hexStr[160];
    char hexbody[160] = ""; // ввел доп. переменную для наглядности создания общей строки..
    unsigned char i;
    unsigned char sum = 0;
    unsigned char d;

    if (hexType == HEX_FORMAT_ADDR_EXT) {
        //  :020000040800F2 - пример
        sum = sum - 2 - 4 - (addr >> 8) - (addr & 0xff);
        sprintf(hexStr, ":02000004%04X%02X", addr, sum);
    } else
        if (hexType == HEX_FORMAT_ENDFILE) {
        sprintf(hexStr, ":00000001FF");
    } else
        if (hexType == HEX_FORMAT_DATA) {
        sum = sum - size - (addr >> 8) - (addr & 0xff);
        for (i = 0; i < size; i++) {
            d = buffer[ i ];
            sum = sum - d;
            snprintf(hexStr, sizeof (hexStr), "%s%02X", hexbody, d);
            strcpy(hexbody, hexStr);
        }
        // тип 00.
        snprintf(hexStr, sizeof(hexStr), ":%02X%04X00%s%02X", size, addr, hexbody, sum);
    } else
        return NULL;

  return hexStr;
}

unsigned char hexUtils_hex2byte(char * pChar) {
    return hex2byte(pChar);
}

unsigned int hexUtils_hex2word(char * pChar) {
    return (hex2byte(pChar) << 8) | hex2byte(&pChar[2]);
}

unsigned int hexUtils_hex2dword(char * pChar) {
    return (hexUtils_hex2word(pChar) << 16) | hexUtils_hex2word(&pChar[4]);
}
