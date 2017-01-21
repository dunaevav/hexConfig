#ifndef __1986BE91_POWER_H
#define __1986BE91_POWER_H

#include "cm3_macro.h"

typedef struct
{
    unsigned PVDEN                                : 1;
    unsigned PBLS                                 : 2;
    unsigned PLS                                  : 3;
    unsigned PVBD                                 : 1;
    unsigned PVD                                  : 1;
    unsigned IEPVBD                               : 1;
    unsigned IEPVD                                : 1;
    unsigned INVB                                 : 1;
    unsigned INV                                  : 1;
    unsigned __dummy0                             : 20;
} _power_pvdcs;

typedef volatile _power_pvdcs _io_power_pvdcs;

#define offs_POWER_PVDCS_PVDEN                  0
#define offs_POWER_PVDCS_PBLS                   1
#define offs_POWER_PVDCS_PLS                    3
#define offs_POWER_PVDCS_PVBD                   6
#define offs_POWER_PVDCS_PVD                    7
#define offs_POWER_PVDCS_IEPVBD                 8
#define offs_POWER_PVDCS_IEPVD                  9
#define offs_POWER_PVDCS_INVB                   10
#define offs_POWER_PVDCS_INV                    11

#define size_POWER_PVDCS_PVDEN                  1
#define size_POWER_PVDCS_PBLS                   2
#define size_POWER_PVDCS_PLS                    3
#define size_POWER_PVDCS_PVBD                   1
#define size_POWER_PVDCS_PVD                    1
#define size_POWER_PVDCS_IEPVBD                 1
#define size_POWER_PVDCS_IEPVD                  1
#define size_POWER_PVDCS_INVB                   1
#define size_POWER_PVDCS_INV                    1

#define mask_POWER_PVDCS_PVDEN                  ((unsigned long)0x00000001)
#define mask_POWER_PVDCS_PBLS                   ((unsigned long)0x00000006)
#define mask_POWER_PVDCS_PLS                    ((unsigned long)0x00000038)
#define mask_POWER_PVDCS_PVBD                   ((unsigned long)0x00000040)
#define mask_POWER_PVDCS_PVD                    ((unsigned long)0x00000080)
#define mask_POWER_PVDCS_IEPVBD                 ((unsigned long)0x00000100)
#define mask_POWER_PVDCS_IEPVD                  ((unsigned long)0x00000200)
#define mask_POWER_PVDCS_INVB                   ((unsigned long)0x00000400)
#define mask_POWER_PVDCS_INV                    ((unsigned long)0x00000800)


typedef struct
{
    __io_reg PVDCS;
} _power;

#endif /* __1986BE91_POWER_H */
