#include <stdio.h>

#define	CONF_FD_COUNT 7
#include "..\Configurator.h"

typedef struct {
	TConfiguratorHead  Head;
	TCField_pChar      VERSION;
	TCField_pChar      DATE;
	TCField_i32        BAUD;
	TCField_i8         DE_PIN;
	TCField_float      ADC_K0;
	TCField_float      ADC_K1;
	TCField_wInt       PORT;
} TMyConfigurator;


code const TMyConfigurator Configurator = {
	    {CONFIGURATOR_KEY, CONF_FD_COUNT, CONF_FORMAT_BE, CONF_SIZEOFPOINTER},
			{"VERSION", CONF_FD_PCHAR, "V1.101"}, //0:
			{"DATE",    CONF_FD_PCHAR, __DATE__}, //1:
			{"BAUD",    CONF_FD_INT32, 115200},   //2: 
			{"DE_PIN",  CONF_FD_BYTE,  10},       //3:
			{"ADC_K0",  CONF_FD_FLOAT, 0},        //4:
			{"ADC_K1",  CONF_FD_FLOAT, 0.0196},   //5:
			{"PORT",    CONF_FD_INT16, 0x55AA}    //6:
		};


void InitUART(int baud)
{
	printf("baud=%d\n", baud);
}

int main (void)
{
	InitUART(Configurator.BAUD.Values.iValue);
	printf("VERSION.... %s\n", Configurator.VERSION.Values.pChar);
	printf("DATE....... %s\n", Configurator.DATE.Values.pChar);
	printf("BAUD....... %d\n", Configurator.BAUD.Values.iValue);
	printf("DE_PIN..... %d\n", Configurator.DE_PIN.Values.bValue);
	printf("ADC_K0..... %f\n", Configurator.ADC_K0.Values.fValue);
	printf("ADC_K1..... %f\n", Configurator.ADC_K1.Values.fValue);
	printf("PORT....... %x\n", Configurator.PORT.Values.wValue);
	while (1);
}
