//

#include "../Configurator.h"


//#define USE_POPT_AUTOHELP       1

#define CONF_MODE_SHOW_HELP 1

#define CMD_OPT_DEFAULT_oFILENAME  "out.hex"
#define CMD_OPT_DEFAULT_CRC_ALG    "CRC16.L.0x1021.0xFFFF"

#define CONF_MAX_FREEARGS 20

typedef struct  {
    char * iFileName;     // Имя входного файла
    char * oFileName;     // Имя выходного файла
    int    fHelpParams;   // флаг отображения помощи для доступа к параметрам конфигуратора
    int    fVerbose;      // флаг отображения отладочной информации
    int    fShowKeys;     // флаг отображения списка параметров конфигуратора
    int    fCRC_le;       // флаг пересчитать CRC файла и разместить результат в формате l-e
    int    fCRC_be;       // флаг пересчитать CRC файла и разместить результат в формате b-e
    int    crcReplace;    // флаг размещения CRC в конце файла взамен старой CRC (размер файла не меняется)
    int    crcForce;      // При добавлении CRC в конец файла, провожу проверку на CRC == 0. Если CRC == 0 выдаю ошибку. флаг crcForce подавляет ошибку.
    char * CRCParams;     // параметры расчета crc
    char * outDir;        // параметры для формирования имени директории результата
    char * files[CONF_MAX_FREEARGS];     // Указатели на имена файлов для перемещения
} TBaseParamOptions;

typedef void (*TParamWriteCallBack)(TConfigurator * pConfigurator, int paramIndex, TConfiguratorField FieldValues[]);

int CmdTest(int argc, char *argv[])  ;

int CmdLineBase(int argc, char *argv[], TBaseParamOptions * BaseParam );
int CmdLineConfigurator(int argc, char *argv[], TConfigurator * pConfigurator, int cmdMode, TParamWriteCallBack cb);


