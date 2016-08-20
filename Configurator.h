/* 
 * File:   Configurator.h
 * Author: Dunaev
 *
 */

#ifndef CONFIGURATOR_H
#define	CONFIGURATOR_H

#pragma pack(push, 1)  

#define CONF_FD_INT32    0
#define CONF_FD_FLOAT    1
#define CONF_FD_INT16    2
#define CONF_FD_BYTE     3
#define CONF_FD_PCHAR    4
// #define CONF_FD_PDOUBLE  5 (unsupported)

#define CONF_FORMAT_LE 0
#define CONF_FORMAT_BE 1
#define CONF_SIZEOFPOINTER    sizeof(char *)

#define CONFIGURATOR_KEY       "CONFIGURATOR" 

typedef struct {
    char fieldID[11]; 
    unsigned char fieldType; 
    union 
    {
        unsigned int iValue; 
                 char *pChar;
        unsigned char bValue;
        unsigned short wValue;
        float fValue; 
        unsigned char b[4];
    } Values;
}
TConfiguratorField;

#ifndef CONF_FD_COUNT
#define	CONF_FD_COUNT 10
#endif


typedef struct {
    char ConfiguratorID[13];   
    unsigned char fieldCount;  
    unsigned char fieldFormat; 
    unsigned char sizeOfPointer; 
    TConfiguratorField Fields[CONF_FD_COUNT];
}
TConfigurator;

//-------------------------------------------
typedef struct {
    char fieldID[11]; 
    unsigned char fieldType; 
    union 
    {
				long iValue; 
        unsigned char b[4];
    } Values;
} TCField_i32;

typedef struct {
    char fieldID[11]; 
    unsigned char fieldType; 
    union 
    {
        unsigned char bValue;
        unsigned char b[4];
    } Values;
} TCField_i8;

typedef struct {
    char fieldID[11]; 
    unsigned char fieldType; 
    union 
    {
        float fValue; 
        unsigned char b[4];
    } Values;
} TCField_float;

typedef struct {
    char fieldID[11]; 
    unsigned char fieldType; 
    union 
    {
        char *pChar;
        unsigned char b[4];
    } Values;
} TCField_pChar;

typedef struct {
    char fieldID[11]; 
    unsigned char fieldType; 
    union 
    {
        unsigned short wValue;
        unsigned char b[4];
    } Values;
} TCField_wInt;

typedef struct {
    char ConfiguratorID[13];   
    unsigned char fieldCount;  
    unsigned char fieldFormat; 
    unsigned char sizeOfPointer; 
} TConfiguratorHead;


#pragma pack(pop)

#endif	/* CONFIGURATOR_H */

