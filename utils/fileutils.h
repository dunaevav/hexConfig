/* 
 * File:   fileutils.h
 * Author: Dunaev
 *
 * Created on 4 Август 2016 г., 13:26
 */

#ifndef FILEUTILS_H
#define	FILEUTILS_H

#define HEX_SECTOR_SIZE     0x10000
#define HEX_LINE_DATA_SIZE  0x10

typedef struct {
    unsigned int addr_hi; // старшая часть адреса секции
    unsigned int useAddrLo; // минимальный используемый адрес в секции
    unsigned int useAddrHi; // максимальный используемый адрес в секции
    unsigned char * mem64Kb; // область данных
} THexSector64K;

typedef struct {
#define HEX_SECTORS_NUM 16
    unsigned int sectorsCount; // кол-во секторов
    THexSector64K sectors[HEX_SECTORS_NUM]; // предусмотрим 16 секторов
    unsigned char * buffer; // объединеная область данных
    unsigned int buffSize; // резмер объединения
    unsigned char startAddr[80]; // строка настройки стартового адреса (:04000005080000C12E))

} THexMem;

char * findConfiguratorKey(char * buff, int buffsize, char * key);
void combineHexSectors(void);
int  LoadHexFile(char * iFileName);
int  LoadBinFile(char * iFileName);
void writeHexFile(THexMem * Hex, char * fileName);
char * getFileExtention(char * iFileName);

int createDir(char * dirName);
int futil_copyFiles(char * files[], char * outdir);

extern THexMem HexMem; // переменная для хранения образа памяти файла

#endif	/* FILEUTILS_H */

