/* 
 * File:   hexutils.h
 * Author: Dunaev
 *
 */

#ifndef HEXUTILS_H
#define	HEXUTILS_H

#define HEX_KEY_OFFSET      0
#define HEX_LEN_OFFSET      (HEX_KEY_OFFSET + 1)    
#define HEX_ADDR_OFFSET     (HEX_LEN_OFFSET + 2)    
#define HEX_ADDR_HI_OFFSET  (HEX_LEN_OFFSET + 2)    
#define HEX_ADDR_LO_OFFSET  (HEX_LEN_OFFSET + 4)    
#define HEX_TYPE_OFFSET     (HEX_ADDR_OFFSET + 4)    
#define HEX_DATA_OFFSET     (HEX_TYPE_OFFSET + 2)    

#define HEX_FORMAT_ADDR_EXT   (4)    
#define HEX_FORMAT_DATA       (0)    
#define HEX_FORMAT_ENDFILE    (1)    
#define HEX_FORMAT_START_ADDR (5)    


typedef struct {
    char key; //':'
    unsigned char length; // размер поля данных
    unsigned short int addr_lo; // адрес
    unsigned char recType; // тип посылки {00 | 04 | 01 | 05}
    union {
        unsigned short int addr_hi; // старшая часть адреса
        unsigned char data[80]; // данные ограничим 80 байт..
        char text[80];          // Оригинальная строка 79 символов + 0..
    } format;
    unsigned char * sum; // контрольная сумма (&data[length])

} THexLineRec;


THexLineRec* hexUtils_str2hexRec(char * hexStrLine);
char*        hexUtils_buffer2str(int hexType, char * buffer, unsigned char size, unsigned short int addr);
unsigned char hexUtils_hex2byte(char * pChar);
unsigned int  hexUtils_hex2word(char * pChar);
unsigned int  hexUtils_hex2dword(char * pChar);

#endif	/* HEXUTILS_H */

