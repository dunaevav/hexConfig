/*
 **************************************************************************
 *** описание программы ***************************************************
 **************************************************************************
 * Цель: Обеспечить модификацию файла прошивки, с учетом особенностей среды внедрения программы.
 * 
 * Основная идея состоит в том, что программа должна содержать область настройки,
 * определенного формата (в соответствии со структурой TConfiguratorField).
 * Эта область может содержать сетевой адрес, коэффициенты калибровки, частоту
 * тактирования, требуемую скорость обмена и т.п.
 * При работе программа контроллера читает значения полей структуры конфигуратора,
 * в соответстветствии с ними настраивая переферию и свой алгоритм.
 * Но в случае необходимости изменения определенных параметров (без возможности 
 * или желания перекомпилировать программу) их изменение можно произвести
 * непосредственно в файле прошивки через утилиту hexConfig. Для работы hexConfig
 * не нужно размещать структуру конфигуратора по какому-то фиксированному адресу,
 * поиск происходит по ключу ("CONFIGURATOR").
 * в отличии от утилиты SRecord 
 * 
 *  
  
 // Пример:
// const TConfigurator Configurator = {CONFIGURATOR_KEY, 2, CONF_FORMAT_LE, CONF_RESERV, 
//  "MHZ",  CONF_FD_INT, 8000000,  // Константа частоты тактирования
//  "BAUD", CONF_FD_INT, 115200    // Константа скорости порта 
// };
// Использование в программе:
//#define UART_ID 1    // индекс параметра в структуре конфигуратора
//InitUART(Configurator.Fields[UART_ID].Values.iValue); // настройка требуемой скорости порта.

// Полученный hex-файл можно модифицировать задавая требуемые параметры:
// hexConfig.exe -i inFile.hex -o outFile9600.hex --BAUD=9600
// hexConfig.exe -i inFile.hex -o outFile19200.hex --BAUD=19200
// Другие параметры:
// > Write Hex File outFile9600.hex...

 
 
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/strutils.h"
#include "utils/hexutils.h"
#include "utils/crc16utils.h"
#include "utils/fileutils.h"
#include "cmdLine/cmdLine.h"
#include "Configurator.h"
#include "hexConfigTextMsg.h"

#define xENDIAN16(e) (((e>>8)&0xFF)|((e<<8)&0xFF00))
#define xENDIAN32(e) (((e>>24)&0xFF)|((e>>8)&0xFF00)|((e<<8)&0xFF0000)|((e<<24)&0xFF000000))

float beFloat2leFloat(void * p) {
    unsigned int *i;
    unsigned int fBuffer;
    float* f = (float *) &fBuffer;

    i = (unsigned int *) p;
    fBuffer = xENDIAN32(*i);
    return *f;
}

char * getConfigurator(void) {
    return findConfiguratorKey((char *)HexMem.buffer, HEX_SECTOR_SIZE * HexMem.sectorsCount, CONFIGURATOR_KEY);
}

unsigned int getpCharAddr(unsigned int addr, int eFormat, int pointerSize) {
    unsigned int addrMask = 0xFFFF;
    int i;

    //BE(3): FF 1E D4 00 : 0x00D41EFF -> 0xFF1ED400
    //LE(3): D4 1E FF 00 : 0x00FF1ED4 => 0x00FF1ED4
    //BE(2): 3E D4 00 00 : 0x0000D43E -> 0x3ED40000

    if (eFormat != CONF_FORMAT_LE)
        addr = xENDIAN32(addr);

    switch (pointerSize) {
        case 4: // адрес полный
            break;
        case 3: // Старшая часть адреса - мусор
            if (eFormat != CONF_FORMAT_LE)
                addr = addr >> 8;
            addr = addr & 0x00FFFFFF;
            break;
        case 2: // Старшая часть адреса - мусор
            if (eFormat != CONF_FORMAT_LE)
                addr = addr >> 16;
            addr = addr & 0x0000FFFF;
            break;
        default: // неверное значение размера.
            printf(HC_TEXT_ERR_PSIZE, pointerSize);
            if (eFormat != CONF_FORMAT_LE)
                addr = addr >> 16;
            addr = addr & 0x0000FFFF; // используем 16 бит
            break;
    }

    // найдем адрес согласно размеру общего буфера.
    // Дело в том что сравнивать старшую часть адреса с полем addr_hi сектора нельзя,
    // т.к. keil для х51 формирует старшую часть адреса равную FF в то время как addr_hi = 0.
    for (i = 0; i < 8; i++) {
        if (HexMem.buffSize <= addrMask)
            break;
        addrMask = (addrMask << 1) + 1;
    }

    addr = addr & addrMask;

    return addr;
}

char * getpCharValue(unsigned int addr, int eFormat, int pointerSize) {
    static char pCharValue[100];

    addr = getpCharAddr(addr, eFormat, pointerSize);
    if (addr == 0)
        snprintf(pCharValue, sizeof (pCharValue), "NULL");
    else {
        snprintf(pCharValue, sizeof (pCharValue), "%s", &HexMem.buffer[addr]);
    }
    return pCharValue;
}

int getFieldIndex(TConfigurator * pConfigurator, char * FieldName) {
    int i;
    if (pConfigurator != NULL)
        for (i = 0; i < pConfigurator->fieldCount; i++) {
            if (strcmp(FieldName, pConfigurator->Fields[i].fieldID) == 0) {
                return i;
            }
        }
    return -1;
}

char * getFieldStrValue(TConfigurator * pConfigurator, unsigned int i) {
    static char result[100] = "";
    unsigned int iValue;
    float fValue;
    if (pConfigurator != NULL) {
        if (i < pConfigurator->fieldCount) {
            if (pConfigurator->fieldFormat != CONF_FORMAT_LE) {
                if (pConfigurator->Fields[i].fieldType == CONF_FD_INT32)
					iValue = xENDIAN32(pConfigurator->Fields[i].Values.iValue);
                if (pConfigurator->Fields[i].fieldType == CONF_FD_INT16)
                    iValue = xENDIAN16(pConfigurator->Fields[i].Values.wValue);
                if (pConfigurator->Fields[i].fieldType == CONF_FD_BYTE)
                    iValue = pConfigurator->Fields[i].Values.bValue;
                if (pConfigurator->Fields[i].fieldType == CONF_FD_FLOAT)
                    fValue = beFloat2leFloat(&pConfigurator->Fields[i].Values.fValue);
            } else {
                iValue = pConfigurator->Fields[i].Values.iValue;
                fValue = pConfigurator->Fields[i].Values.fValue;
            }

            if (pConfigurator->Fields[i].fieldType == CONF_FD_INT32)
                snprintf(result, sizeof (result), "%d", iValue);
            if (pConfigurator->Fields[i].fieldType == CONF_FD_INT16)
                snprintf(result, sizeof (result), "%d", iValue & 0xFFFF);
            if (pConfigurator->Fields[i].fieldType == CONF_FD_BYTE)
                snprintf(result, sizeof (result), "%d", iValue & 255);
            if (pConfigurator->Fields[i].fieldType == CONF_FD_FLOAT)
                snprintf(result, sizeof (result), "%f", fValue);
            if (pConfigurator->Fields[i].fieldType == CONF_FD_PCHAR)
                snprintf(result, sizeof (result), "%s", getpCharValue(pConfigurator->Fields[i].Values.iValue, pConfigurator->fieldFormat, pConfigurator->sizeOfPointer));

        } else {

        }

    }
    return result;
}

//dirOption: "_"+VERSION+"-"+DATE -> "_MK v1.23-22 Aug 2016"

char * getOutDir(TConfigurator * pConfigurator, char * dirOption) {
    int i, j;
    int len;
    char * dirOptionPart;
    char * FieldValue;
    static char result[100];

    sprintf(result, "");

    if ((pConfigurator != NULL)&&(dirOption != NULL)) {
        for (i = 1; 1; i++) {
            dirOptionPart = sutil_get_OptionPart(dirOption, i, '+');
            len = strlen(dirOptionPart);
            if (len <= 0) { // нет дополнительныйх под опций...
                return result;
            } else {// параметр может быть как константа("-"), так и переменная

                if (dirOptionPart[0] == '"') { // Указана константа
                    dirOptionPart[len - 1] = 0; // '"' <- 0
                    snprintf(&result[strlen(result)], sizeof (result), "%s", &dirOptionPart[1]);
                } else { // Возьмем значение параметра в структуре конфигуратора
                    j = getFieldIndex(pConfigurator, dirOptionPart);
                    if (j != -1) {
                        // Если параметр найден, используем его значение
                        FieldValue = getFieldStrValue(pConfigurator, j);
                        snprintf(&result[strlen(result)], sizeof (result), "%s", FieldValue);

                    } else { // параметр в структуре конфигуратора не найден
                        // printf(HC_TEXT_INFO_PARAM_UNFOND, dirOptionPart);
                        // используем введеный в команде текст
                        snprintf(&result[strlen(result)], sizeof (result), "%s", dirOptionPart);

                    }
                }

            }
        }
    }
    return result;
}

void printConfigurator(TConfigurator * pConfigurator) {
    int i;
    if (pConfigurator != NULL) {
        printf(HC_TEXT_INFO_FIELDS_NUM, pConfigurator->ConfiguratorID, pConfigurator->fieldCount);

        if (pConfigurator->fieldFormat == CONF_FORMAT_LE)
            printf(HC_TEXT_INFO_FORMAT_LE);
        else
            printf(HC_TEXT_INFO_FORMAT_BE);

        for (i = 0; i < pConfigurator->fieldCount; i++) {
            printf("%s", pConfigurator->Fields[i].fieldID);
            // printf(" (0x%08x) ", pConfigurator->Fields[i].Values.iValue);
            if (pConfigurator->Fields[i].fieldType == CONF_FD_INT32)
                printf(HC_TEXT_INFO_TYPE_INT32);
            if (pConfigurator->Fields[i].fieldType == CONF_FD_INT16)
                printf(HC_TEXT_INFO_TYPE_INT16);
            if (pConfigurator->Fields[i].fieldType == CONF_FD_BYTE)
                printf(HC_TEXT_INFO_TYPE_BYTE);
            if (pConfigurator->Fields[i].fieldType == CONF_FD_FLOAT)
                printf(HC_TEXT_INFO_TYPE_FLOAT);
            if (pConfigurator->Fields[i].fieldType == CONF_FD_PCHAR)
                printf(HC_TEXT_INFO_TYPE_PCHAR);
            printf(HC_TEXT_INFO_VALUE, getFieldStrValue(pConfigurator, i));
        }
    } else
        printf(HC_TEXT_INFO_NULL);

}

int hexCRCUpdate(TBaseParamOptions options) {
    int crcFile = 0;

    if (options.fCRC_le || options.fCRC_be) { //
        if (options.crcReplace) { // замена сушествующей CRC
            HexMem.buffSize = HexMem.buffSize - 2;
        } else { // добавление новой
            HexMem.sectors[HexMem.buffSize >> 16].useAddrHi += 2;
        }
        crcFile = crc16u_calc(HexMem.buffer, HexMem.buffSize, options.CRCParams);
        // можно было сделать чтобы при 0 CRC автоматом переходил в режим замены CRC (-r)... решил, что лучше держать все под контролем пользователя.
        if (crcFile == 0) { // что-то не так. видимо там старое CRC. Проверим форс-флаг..
            //if (options.fVerbose)
                printf(HC_TEXT_CRC_DUP);
            if (options.crcForce) {
                if (options.fVerbose)
                    printf(HC_TEXT_CRC_FORCE_MSG);
            } else {
                printf(HC_TEXT_CRC_NEED_FORCE);
                exit(1);
            }
        }
        //crcFile = 0x1122;
        if (options.fVerbose)
            printf(HC_TEXT_CRC_ALG_INFO, crcFile, crc16u_getUseOption(options.CRCParams), HexMem.buffSize);

        if (options.fCRC_be) crcFile = ((crcFile >> 8) | (crcFile << 8)) & 0xFFFF;
        // разместим результат расчета crc в буфере
        // очевидно, что если CRC попадает на границу секторов 64кб, то работать не будет.
        if (((HexMem.buffSize & 0xFFFF) > 0xFFFE) || ((HexMem.buffSize & 0xFFFF) == 0)) {
            printf(HC_TEXT_CRC_PLACE_ERR, HexMem.buffSize);
            exit(1);
        }

        HexMem.buffer[HexMem.buffSize++] = crcFile & 0xff;
        HexMem.buffer[HexMem.buffSize++] = crcFile >> 8;
    }

}

char * iValue2pChar( unsigned int i)
{
	char * result;
	result = (char *) i;
	return result;
}
void userWriteParam(TConfigurator * pConfigurator, int index, TConfiguratorField FieldValues[]) {
    //Пользователь произвел запись в параметр
    int i;
    unsigned int strAddr;
    if ((pConfigurator->Fields[index].fieldType == CONF_FD_INT32) ||
            (pConfigurator->Fields[index].fieldType == CONF_FD_FLOAT)) {
        if (pConfigurator->fieldFormat != CONF_FORMAT_LE)
            pConfigurator->Fields[index].Values.iValue = xENDIAN32(FieldValues[index].Values.iValue);
        else
            pConfigurator->Fields[index].Values.iValue = FieldValues[index].Values.iValue;

    }
    if ((pConfigurator->Fields[index].fieldType == CONF_FD_INT16)) {
        if (pConfigurator->fieldFormat != CONF_FORMAT_LE)
            pConfigurator->Fields[index].Values.wValue = xENDIAN16(pConfigurator->Fields[index].Values.wValue);
        else
            pConfigurator->Fields[index].Values.wValue = FieldValues[index].Values.wValue;
    }
    if ((pConfigurator->Fields[index].fieldType == CONF_FD_BYTE)) {
        pConfigurator->Fields[index].Values.b[0] = pConfigurator->Fields[index].Values.iValue & 255;
    }
    if ((pConfigurator->Fields[index].fieldType == CONF_FD_PCHAR)) { // проверка на допустимый размер. Новое значение не может превышать размер старой строки!
        i = strlen(getFieldStrValue(pConfigurator, index));
        strAddr = getpCharAddr(pConfigurator->Fields[index].Values.iValue, pConfigurator->fieldFormat, pConfigurator->sizeOfPointer);
        if (strlen(iValue2pChar(FieldValues[index].Values.iValue)) > i) { // Ошибка. Не достаточно места для хранения введеного значения!
            printf(HC_TEXT_INFO_WRITE_PCHAR_ERR);
        } else {
            if (strAddr) {
                // if (options.fVerbose) {printf(HC_TEXT_INFO_WRITE_PCHAR, &HexMem.buffer[strAddr], FieldValues[index].Values.pChar); }
                strcpy((char *)&HexMem.buffer[strAddr], iValue2pChar(FieldValues[index].Values.iValue));
            } else
                printf(HC_TEXT_ERR_PCHAR_ADDR);

        }
    }

}

int hexConfigurator(int argc, char **argv) {
    int i;
    int cnfgParamChange = 0;
    int cmdMode = 0;
    TConfigurator * c;
    TBaseParamOptions options;
    char oFileName[255] = "";
    char outDir[255] = "";
    int (*LoadFile) (char * iFileName);

    CmdLineBase(argc, argv, &options);

    snprintf(oFileName, sizeof (oFileName), "%s", options.oFileName);
    printf(HC_TEXT_FILE_INFO, options.iFileName, options.oFileName);

    if (strcmp(".HEX", getFileExtention(options.iFileName)) == 0)
        LoadFile = LoadHexFile;
    else
        if (strcmp(".BIN", getFileExtention(options.iFileName)) == 0)
        LoadFile = LoadBinFile;
    else {
        printf(HC_TEXT_FILE_INVALID_EXT, getFileExtention(options.iFileName));
        return 1;
    }

    if (LoadFile(options.iFileName)) {
        combineHexSectors();
        c = (TConfigurator *) getConfigurator();
        if (c != NULL) {
            if (options.fShowKeys || options.fVerbose) {
                printf(HC_TEXT_INFO_IN_PREF);
                printConfigurator(c);
            }
            if (options.fHelpParams)
                cmdMode = CONF_MODE_SHOW_HELP;

            // если задана опция целевой директории
            if (strlen(options.outDir) > 0) {
                printf("\n outDir = %s \n", options.outDir);
                printf("\n getOutDir = %p \n", getOutDir(c, options.outDir));
                
                snprintf(outDir, sizeof (outDir), "%s", getOutDir(c, options.outDir));
                if (strlen(outDir) > 0) {
                    // нужно создать указанную директорию...
                    createDir(outDir);
                    //дополним к имени файла имя целевой директории: a.hex -> dir/subdir/a.hex
                    snprintf(oFileName, sizeof (oFileName), "%s/%s", outDir, options.oFileName);
                    //printf(HC_TEXT_FILE_INFO, options.iFileName, oFileName);
                }
            } else
                snprintf(oFileName, sizeof (oFileName), "%s", options.oFileName);

            //обработаем ключи командной строки...
            cnfgParamChange = CmdLineConfigurator(argc, argv, c, cmdMode, &userWriteParam);
            if (options.fVerbose) {
                printf(HC_TEXT_INFO_OUT_PREF, cnfgParamChange);
                printConfigurator(c);
            }
        } else
            printf(HC_TEXT_INFO_NULL);

        // копирование файлов, имена которых указаны в командной строке
        // только если задана опция --out-dir
        if ((options.files[0] != NULL)&&(strlen(options.outDir) > 0)) {
            i = futil_copyFiles(options.files, outDir);
            if (options.fVerbose)
                printf(HC_TEXT_FILES_COPY_MSG, i);
        }

        //-----------------------------------------------------------------
        //-----------------------------------------------------------------
        // перерасчет CRC в памяти
        hexCRCUpdate(options);
        //-----------------------------------------------------------------
        //--- сохранение файла --------------------------------------------
        //-----------------------------------------------------------------
        // проверим были ли изменения в файле? (Если задано имя выходного файла произведем запись)
        if (((cnfgParamChange) || (options.fCRC_le) || (options.fCRC_be) || (strcmp(CMD_OPT_DEFAULT_oFILENAME, oFileName) != 0))) {
            if (options.fVerbose)
                printf(HC_TEXT_FILE_WRITE_MSG, oFileName);
            writeHexFile(&HexMem, oFileName);
        }

        //-----------------------------------------------------------------
    } else {
        printf(HC_TEXT_ERR_OPENFILE, options.iFileName);
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {

    typedef union {
        unsigned int iValue;
        unsigned char *pChar;
        unsigned char bValue;
        unsigned short wValue;
        float fValue;
        unsigned char b[4];
    } Values;

    int result;

    //...
    result = hexConfigurator(argc, argv);
    
    //...
    exit(result);
    /*    
        TConfiguratorField c = {"VERSION", CONF_FD_PCHAR, {.pChar = "V1.101"}};
        Values vv = {.pChar = "qweqweqwe"};
    
        // Пример задания полей конфигуратора:
        const TConfigurator Configurator = {CONFIGURATOR_KEY, CONF_FD_COUNT, CONF_FORMAT_LE, CONF_SIZEOFPOINTER,
        {
            {"VERSION", CONF_FD_PCHAR, {.pChar = "V1.101"}},
            {"DATE",    CONF_FD_PCHAR, {.pChar = __DATE__}},
            {"BAUD",    CONF_FD_INT32, 115200},
            {"DE_PIN",  CONF_FD_BYTE,  10},
            {"ADC_K0",  CONF_FD_FLOAT, 0},
            {"ADC_K1",  CONF_FD_FLOAT, 0.0196},
            {"PORT",    CONF_FD_INT16, 0x55AA}
        }};
     */
    return result;

}


