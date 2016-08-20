#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "fileutils.h"
#include "hexutils.h"
#include "strutils.h"
#include "../hexConfigTextMsg.h"


THexMem HexMem; // переменная для хранения образа памяти файла

//открытие файла конфигурации 

FILE *openTextFile(char file[], char * mode) {
    FILE * pfile;
    pfile = fopen(file, mode);
    if (pfile == NULL)
        return 0;
    return pfile;
}

int writeTextFile(FILE * f, char * str) {
    fprintf(f, "%s\n", str);
    return 0;
}

int findHexSector(int addr_hi) {
    int i;
    for (i = 0; i < HexMem.sectorsCount; i++) {
        if (HexMem.sectors[i].addr_hi == addr_hi)
            return i;
    }
    return -1;
}

int getHexSector(int addr_hi) {
    int i;
    i = findHexSector(addr_hi);
    if (i == -1) {
        i = HexMem.sectorsCount;
        if (i >= HEX_SECTORS_NUM) {
            printf(HC_TEXT_ERR_SECTOR_OVER);
            return -1;
        }
        HexMem.sectorsCount++;
        HexMem.sectors[i].addr_hi = addr_hi;
        HexMem.sectors[i].useAddrLo = HEX_SECTOR_SIZE;
        HexMem.sectors[i].useAddrHi = 0;
        HexMem.sectors[i].mem64Kb = malloc(HEX_SECTOR_SIZE);
        memset(HexMem.sectors[i].mem64Kb, 0xff, HEX_SECTOR_SIZE);
    }
    return i;
}

void writeToHexSector(int addr_hi, int addr, unsigned char len, unsigned char * data) {
    int i;
    i = getHexSector(addr_hi);
    if (i != -1) {
        memcpy(&HexMem.sectors[i].mem64Kb[addr], &data[0], len);
        if (HexMem.sectors[i].useAddrLo >= addr)
            HexMem.sectors[i].useAddrLo = addr;
        if (HexMem.sectors[i].useAddrHi <= (addr + len - 1))
            HexMem.sectors[i].useAddrHi = (addr + len - 1);
    } else
        printf(HC_TEXT_ERR_WR_SECTOR);

}

void combineHexSectors(void) {
    int i;

    if (HexMem.sectorsCount) {
        HexMem.buffer = malloc(HexMem.sectorsCount * HEX_SECTOR_SIZE);
        for (i = 0; i < HexMem.sectorsCount; i++) {
            memcpy(&HexMem.buffer[i * HEX_SECTOR_SIZE], HexMem.sectors[i].mem64Kb, HEX_SECTOR_SIZE);
            HexMem.buffSize = HexMem.sectors[i].useAddrHi + i * HEX_SECTOR_SIZE + 1;
        }
    }
}

void printHexSector(int addr_hi) {
    int i, j, n;
    i = getHexSector(addr_hi);
    if (i != -1) {
        for (n = 0; n < 0x100; n++) {
            printf("\n %04X>", n * 16);
            for (j = 0; j < 0x10; j++)
                printf("%02X ", HexMem.sectors[i].mem64Kb[n * 16 + j]);
        }
    }
}

void printHexMem(void) {
    int i;
    // printf("\n HexMem.sectorsCount = %d\n", HexMem.sectorsCount);

    for (i = 0; i < HexMem.sectorsCount; i++) {
        printHexSector(HexMem.sectors[i].addr_hi);
    }
}

char * findConfiguratorKey(char * buff, int buffsize, char * key) {
    int i;
    //    char key[] = CONFIGURATOR_KEY;
    unsigned int *iKey = (unsigned int *) &key[0]; // для ускорения поиска используем int
    unsigned int *pI;
    for (i = 0; i < buffsize; i++) {
        pI = (unsigned int *) &buff[i];
        if (*pI == *iKey)
            if (strcmp(key, &buff[i]) == 0)
                return &buff[i];
    }
    return NULL;
}

void writeHexFile(THexMem * Hex, char * fileName) {
    //
    unsigned int i, j;
    FILE * hexFile;
    unsigned char hSize;

    hexFile = openTextFile(fileName, "wt");
    for (i = 0; i < Hex->sectorsCount; i++) {
        if ((i == 0)&&(Hex->sectors[i].addr_hi == 0)&&(Hex->buffSize < HEX_SECTOR_SIZE)) { // Если адрес для первого сектора не задан, в файл не будем 
            // записывать строку адреса (для МК с памятью <=64кБ)            
        } else {
            writeTextFile(hexFile, hexUtils_buffer2str(HEX_FORMAT_ADDR_EXT, NULL, 0, Hex->sectors[i].addr_hi));
        }
        for (j = Hex->sectors[i].useAddrLo; j <= Hex->sectors[i].useAddrHi; j += HEX_LINE_DATA_SIZE) {
            hSize = HEX_LINE_DATA_SIZE;
            if (Hex->sectors[i].useAddrHi - j < HEX_LINE_DATA_SIZE) {
                hSize = Hex->sectors[i].useAddrHi - j + 1;
                //printf("\n Hex->sectors[i].useAddrHi = %d, j = %d...hSize = %d", Hex->sectors[i].useAddrHi, j, hSize);
            }
            writeTextFile(hexFile, hexUtils_buffer2str(HEX_FORMAT_DATA, &Hex->buffer[(i * HEX_SECTOR_SIZE) + j], hSize, j));
        }

    }
    if (Hex->startAddr[0])
        writeTextFile(hexFile, Hex->startAddr);
    writeTextFile(hexFile, hexUtils_buffer2str(HEX_FORMAT_ENDFILE, NULL, 0, 0));
    fclose(hexFile);
}

int LoadHexFile(char * iFileName) {
    FILE * hexFile;
    int addr_hi = 0;
    THexLineRec * hex;
    char lineBufferStr[160] = ""; //:20002000000000000000000000000000030200080502000800000000070200080D7A000804
    int result = 0;

    HexMem.sectorsCount = 0;
    HexMem.startAddr[0] = 0;
    hexFile = openTextFile(iFileName, "rt");
    if (hexFile) {
        result = 1; // Если файл открыт результат LoadHexFile успешен (независимо от содержимого файла)

        while (feof(hexFile) == 0) {
            if (fscanf(hexFile, "%s", lineBufferStr) > 0) {
                hex = hexUtils_str2hexRec(lineBufferStr);
                if (hex != NULL) {
                    if (hex->recType == HEX_FORMAT_ADDR_EXT)
                        addr_hi = hex->format.addr_hi;
                    if (hex->recType == HEX_FORMAT_DATA)
                        writeToHexSector(addr_hi, hex->addr_lo, hex->length, hex->format.data);
                    if (hex->recType == HEX_FORMAT_START_ADDR) // сохраним настройку адреса
                        snprintf(HexMem.startAddr, sizeof (HexMem.startAddr), "%s", hex->format.text);
                }
            }
        }
        fclose(hexFile);
    }
    return result;
}

int LoadBinFile(char * iFileName) {
    FILE * binFile;
    int addr_hi = 0;
    int result = 0;
    int i = 0;
    int readByteCount = 0;

    HexMem.sectorsCount = 0;
    binFile = openTextFile(iFileName, "rb");
    if (binFile) {
        result = 1; // Если файл открыт результат LoadBinFile успешен (независимо от содержимого файла)
        i = getHexSector(addr_hi);

        while (feof(binFile) == 0) {
            HexMem.sectors[i].useAddrLo = 0;
            readByteCount = fread(HexMem.sectors[i].mem64Kb, 1, HEX_SECTOR_SIZE, binFile);
            if (readByteCount > 0) {
                HexMem.sectors[i].useAddrHi = readByteCount - 1;
                HexMem.sectors[i].addr_hi = addr_hi >> 16; // только старшая часть 32-х разрядного адреса
                addr_hi = addr_hi + HEX_SECTOR_SIZE;
                if (readByteCount == HEX_SECTOR_SIZE)
                    i = getHexSector(addr_hi);
            } else {
                //  printf("\n  break...");
                break;
            }
        }
        fclose(binFile);
    }
    return result;
}

char * getFileExtention(char * iFileName) { //'D:\dir\file.1.Bin' -> '.BIN')
    static char FileExtention[10] = "";
    unsigned char i, j;
    unsigned int n;

    if (iFileName == NULL)
        return FileExtention; //""
    n = strlen(iFileName);
    for (i = 0; i < n; i++) {
        if (iFileName[n - i] == '.') {
            if (i >= sizeof (FileExtention)) {
                printf(HC_TEXT_ERR_FILEEXT);
                exit(1);
            }
            for (j = 0; j <= i; j++) {
                if (iFileName[n - i + j] >= 'a') // 'a' -> 'A'
                    FileExtention[j] = iFileName[n - i + j] - 0x20;
                else
                    FileExtention[j] = iFileName[n - i + j];
            }
            FileExtention[j] = 0;
            return FileExtention;
        }
    }
    return FileExtention; //""

}

int createDir(char * dirName) {
    int i;
    int len;
    char * dir;
    char dirFullName[255] = ".";
    for (i = 1; 1; i++) {
        dir = sutil_get_OptionPart(dirName, i, '/');
        len = strlen(dir);
        if (len > 0) {
            snprintf(&dirFullName[strlen(dirFullName)], sizeof (dirFullName), "/%s", dir);
//            printf("\ndirFullName = %s\n", dirFullName);
            mkdir(dirFullName, 0766); // создадим директорию 
        } else
            break;
    }

    return 0;
}

char * getFileName(char * fullFileName) {
    int i, n;
    n = strlen(fullFileName);
    for (i = 0; i < n; i++) {
        if ((fullFileName[n - i] == '\\') || ((fullFileName[n - i] == '/'))) {
            return &fullFileName[n - i + 1];
        }
    }
    return fullFileName; //""
}

int copyfile(char * inFileName, char * outFileName) {
    unsigned int readByteCount;
    FILE * fin;
    FILE * fout;
    unsigned char buffer[1024];
    int result = 0;
    

    fin = fopen(inFileName, "rb");
    fout = fopen(outFileName, "wb");
    if (fin) {
        result++;
        while (feof(fin) == 0) {
            readByteCount = fread(buffer, 1, sizeof (buffer), fin);
            fwrite(buffer, 1, readByteCount, fout);
        }
    }
    fclose(fin);
    fclose(fout);
    return result;
}

int futil_copyFiles(char * files[], char * outdir) {
    int i;
    char oFileName[255] = "";
    int result = 0;

    for (i = 0; 1; i++) {
        if (files[i] != NULL) { // копируем файл 
            snprintf(oFileName, sizeof (oFileName), "%s/%s", outdir, getFileName(files[i]));
            // printf("\n copy: '%s' -> '%s'\n", files[i], oFileName);
            result += copyfile(files[i], oFileName);
        } else
            break;
    }
    return result;
}
