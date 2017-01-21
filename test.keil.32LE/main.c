#include <stdio.h>

#define	CONF_FD_COUNT 8
#include "..\Configurator.h"

		const TConfigurator Configurator = {CONFIGURATOR_KEY, CONF_FD_COUNT, CONF_FORMAT_LE, CONF_SIZEOFPOINTER,
		{ // C/C++: [C99 Mode]
			{"VERSION", CONF_FD_PCHAR, {.pChar = "V1.101"}}, //0:
			{"DATE",    CONF_FD_PCHAR, {.pChar = __DATE__}}, //1:
			{"BAUD",    CONF_FD_INT32, 115200},              //2: 
			{"DE_PIN",  CONF_FD_BYTE,  {.bValue = 10}},      //3:
			{"ADC_K0",  CONF_FD_FLOAT, {.fValue = 0}},       //4:
			{"ADC_K1",  CONF_FD_FLOAT, {.fValue = 0.0196}},  //5:
			{"PORT",    CONF_FD_INT16, {.wValue = 0x55AA}},   //6:
			//{"COMPILER",CONF_FD_PCHAR, {.pChar = __ARMCC_VERSION}}   //6:
    }};

void InitUART(int baud)
{
	printf("baud=%d\n", baud);
}

int main (void)
{
	InitUART(Configurator.Fields[2].Values.iValue);
	printf("VERSION.... %s\n", Configurator.Fields[0].Values.pChar);
	printf("DATE....... %s\n", Configurator.Fields[1].Values.pChar);
	printf("BAUD....... %d\n", Configurator.Fields[2].Values.iValue);
	printf("DE_PIN..... %d\n", Configurator.Fields[3].Values.bValue);
	printf("ADC_K0..... %f\n", Configurator.Fields[4].Values.fValue);
	printf("ADC_K1..... %f\n", Configurator.Fields[5].Values.fValue);
	printf("PORT....... %x\n", Configurator.Fields[6].Values.wValue);
	while (1);
}
