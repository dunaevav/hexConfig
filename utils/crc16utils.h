/* 
 * File:   crc16utils.h
 * Author: Dunaev
 *
 * Created on 2 Август 2016 г., 9:57
 */

#ifndef CRC16UTILS_H
#define	CRC16UTILS_H

// Пример строки описания алгоритма:
// "CRC16.L.0x1021.0x0000", где
// CRC16 - группа алгоритмов CRC 
// L (Left - <<)- напрявление обработки бит (от старшего к младшему). Варианты: "L","R"
// 0x1021 - полином с учетом направления обработки*. используется в операции xor для расчета CRC
// [* примечание: для варианта "R" нужно брать отраженные значения полинома]
// 0x0000 - начальное значение CRC
// Соответствие алгоритмам:
// CRC-16-IBM:    "CRC16.L.0x8005.0x0000"
// CRC-16-MODBUS: "CRC16.R.0xA001.0xFFFF"
// CRC-16-CCITT:  "CRC16.L.0x1021.0xFFFF"
// CRC-16-XMODEM: "CRC16.L.0x1021.0x0000"


unsigned int crc16u_calc(unsigned char *b, int count, char * option);
unsigned int crc16u_test(void);
char *       crc16u_getUseOption(char * option);
#endif	/* CRC16UTILS_H */

