#include <stdio.h>  
#include <stdlib.h>  
#include <popt.h>

#include "cmdLine.h"  
#include "../hexConfigTextMsg.h"  
#define USER_PARAM_ADDR_BASE 20000

#define PARAM_ADDR_HELP  30000
#define PARAM_ADDR_USAGE  30001

void CmdLineHelpPrint(void);
void CmdLineHelpPrintBrief(void);

/*
void CmdLineHelpCallBack(TConfigurator * pConfigurator, int paramIndex)
{
    if (paramIndex == (PARAM_ADDR_HELP - USER_PARAM_ADDR_BASE))
        CmdLineHelpPrint();
    if (paramIndex == (PARAM_ADDR_USAGE - USER_PARAM_ADDR_BASE))
        CmdLineHelpPrintBrief();
    
        
}
 */

static int CmdLineTestParam(poptContext * pc, TParamWriteCallBack cb, TConfigurator * pConfigurator, TConfiguratorField FieldValues[]) {
    int val;
    int result = 0;

    while ((val = poptGetNextOpt(*pc)) != -1) {
        if (val > 0) {
            result++;
            if (cb != NULL)
                cb(pConfigurator, val - USER_PARAM_ADDR_BASE, FieldValues );
        } else
            switch (val) {
                case POPT_ERROR_NOARG:
                    printf(HC_TEXT_CL_ERR_NOARG);
                    exit(1);
                case POPT_ERROR_BADNUMBER:
                case POPT_ERROR_OVERFLOW:
                    printf(HC_TEXT_CL_ERR_BADARG);
                    exit(1);
            }
    }
    return result;
}

//��������� ������� ������ (�� ������� �� ����� �������������)

int CmdLineBase(int argc, char *argv[], TBaseParamOptions * BaseParam) {
    int crc_le = 0;
    int crc_be = 0;
    int showflag = 0;
    int phelpflag = 0;
    int crcReplace = 0;
    int verboseflag = 0;
    int crcForce = 0;
    static char *oFileName = CMD_OPT_DEFAULT_oFILENAME; // ��� ��������� ����� �� ���������...
    static char *iFileName = "";
    static char *outDir = "";
    static char *crcFormat = CMD_OPT_DEFAULT_CRC_ALG; // ������ CRC �� ���������
    int version = 0;
    int help = 0;
    int usage = 0;
    int i = 0;

    poptContext pc;
    struct poptOption po[] = {
        {"input", 'i', POPT_ARG_STRING, &iFileName, 11001, HC_TEXT_CL_i_OPT_EX, HC_TEXT_CL_i_OPT},
        {"output", 'o', POPT_ARG_STRING, &oFileName, 11002, HC_TEXT_CL_o_OPT_EX, HC_TEXT_CL_o_OPT},
        {"params", 'p', POPT_ARG_NONE, &showflag, 11003, HC_TEXT_CL_p_OPT_EX, HC_TEXT_CL_p_OPT},
        {"phelp", 'h', POPT_ARG_NONE, &phelpflag, 11004, HC_TEXT_CL_h_OPT_EX, HC_TEXT_CL_h_OPT},
        {"verbose", 'v', POPT_ARG_NONE, &verboseflag, 11005, HC_TEXT_CL_v_OPT_EX, HC_TEXT_CL_v_OPT},
        {"crc-l-e", 'l', POPT_ARG_NONE, &crc_le, 11006, HC_TEXT_CL_l_OPT_EX, HC_TEXT_CL_l_OPT},
        {"crc-b-e", 'b', POPT_ARG_NONE, &crc_be, 11007, HC_TEXT_CL_b_OPT_EX, HC_TEXT_CL_b_OPT},
        {"crc-alg", 'c', POPT_ARG_STRING, &crcFormat, 11008, HC_TEXT_CL_c_OPT_EX, HC_TEXT_CL_c_OPT},
        {"crc-replace", 'r', POPT_ARG_NONE, &crcReplace, 11009, HC_TEXT_CL_r_OPT_EX, HC_TEXT_CL_r_OPT},
        {"crc-force", 'f', POPT_ARG_NONE, &crcForce, 11010, HC_TEXT_CL_f_OPT_EX, HC_TEXT_CL_f_OPT},
        {"out-dir", 'd', POPT_ARG_STRING, &outDir, 11011, HC_TEXT_CL_d_OPT_EX, HC_TEXT_CL_d_OPT},
        {"version", 0, POPT_ARG_NONE, &version, 11021, HC_TEXT_CL_VER_OPT_EX, ""},
#ifdef USE_POPT_AUTOHELP        
        POPT_AUTOHELP
#else
        {"help", '?', POPT_ARG_NONE, &help, PARAM_ADDR_HELP, HC_TEXT_CL_USAGE_HELP, ""},
        {"usage", 0, POPT_ARG_NONE, &usage, PARAM_ADDR_USAGE, HC_TEXT_CL_USAGE_USAGE, ""},
#endif
        {NULL}
    };

    pc = poptGetContext(NULL, argc, (const char **) argv, po, 0);
    if (argc < 2) {
#ifdef USE_POPT_AUTOHELP        
        poptSetOtherOptionHelp(pc, "[-? -p -v -h -l -b -r -f --PARAM1=VALUE1 --PARAM2=VALUE2 --PARAMn=VALUEn ]");
        poptPrintUsage(pc, stderr, 0);
#else
        CmdLineHelpPrint();
#endif
        exit(1);
    }

    CmdLineTestParam(&pc, NULL, NULL, NULL);
    BaseParam->fShowKeys = showflag;
    BaseParam->iFileName = iFileName;
    BaseParam->oFileName = oFileName;
    BaseParam->fVerbose = verboseflag;
    BaseParam->fHelpParams = phelpflag;
    BaseParam->CRCParams = crcFormat;
    BaseParam->fCRC_be = crc_be;
    BaseParam->fCRC_le = crc_le;
    BaseParam->crcReplace = crcReplace;
    BaseParam->crcForce = crcForce;
    BaseParam->outDir = outDir;
    if (version) { // вывод версии программы:
        printf("Version 1.0 Copyright @ 2016 Dunaev A.V. \n");
        exit(0);
    }
    if (help) { // вывод справки
        CmdLineHelpPrint();
        exit(0);
    }
    if (usage) { // вывод краткой справки
        CmdLineHelpPrintBrief();
        exit(0);
    }

    i = 0;
    // аргументы file1..file3: hexConfig.exe -i BNBS2.hex -v --out-dir="pp" file1 file2 file3
    while (poptPeekArg(pc) != NULL) {
        if (i < (CONF_MAX_FREEARGS - 1))
            BaseParam->files[ i++ ] = (char*) poptGetArg(pc);
    }
    BaseParam->files[ i ] = NULL;

    poptFreeContext(pc);

    return 0;
}

int CmdLineConfigurator(int argc, char *argv[], TConfigurator * pConfigurator, int cmdMode, TParamWriteCallBack cb) {
#define MAX_pOPTION 10
    int i;
    int result = 0;
    poptContext pc;
    struct poptOption po[MAX_pOPTION] = {
        {NULL},
        {NULL},
        {NULL},
        {NULL},
        {NULL},
        {NULL},
        {NULL},
        {NULL},
        {NULL},
        {NULL}
    };
    TConfiguratorField FieldValues[MAX_pOPTION];
    

    for (i = 0; i < pConfigurator->fieldCount; i++) {
        if (i < MAX_pOPTION) {
            po[i].shortName = 0; //'a' + i;
            po[i].argDescrip = NULL;
            po[i].descrip = NULL;
            po[i].longName = pConfigurator->Fields[i].fieldID;
            po[i].arg = &FieldValues[i].Values.iValue; // &pConfigurator->Fields[i].Values.iValue;
            po[i].val = USER_PARAM_ADDR_BASE + i;
            if ((pConfigurator->Fields[i].fieldType == CONF_FD_INT32) ||
                    (pConfigurator->Fields[i].fieldType == CONF_FD_INT16) ||
                    (pConfigurator->Fields[i].fieldType == CONF_FD_BYTE))
                po[i].argInfo = POPT_ARG_INT;
            if (pConfigurator->Fields[i].fieldType == CONF_FD_FLOAT)
                po[i].argInfo = POPT_ARG_FLOAT;
            if (pConfigurator->Fields[i].fieldType == CONF_FD_PCHAR)
            { // для строкового параметра, передадим указатель
                po[i].argInfo = POPT_ARG_STRING;
            }
        }
    }
    pc = poptGetContext(NULL, argc, (const char **) argv, po, 0);
    poptSetOtherOptionHelp(pc, "...");
    if (cmdMode == CONF_MODE_SHOW_HELP)
        poptPrintUsage(pc, stderr, 0);

    result = CmdLineTestParam(&pc, cb, pConfigurator, FieldValues);

    return result;
}

void CmdLineHelpPrintBrief(void) {
    /*
    D:\...ev\Source.other\hex.configurator>hexConfig.exe --usage
    Usage: hexConfig [-i <файл>] [-o <файл>] [-c <crc-id>] [--version]
            [--usage]
            [-? -p -v -h -l -b -r -f --PARAM1=VALUE1 --PARAM2=VALUE2 --PARAMn=VALUEn]
     */

    printf(HC_TEXT_CL_USAGE_HEADER_BRIEF, "hexConfig");
    printf(" [-i %s]", HC_TEXT_CL_i_OPT);
    printf(" [-o %s]", HC_TEXT_CL_o_OPT);
    printf(" [-c %s]", HC_TEXT_CL_c_OPT);
    printf(" [-d %s]", HC_TEXT_CL_d_OPT);
    printf(" [-phvlbr] %s", HC_TEXT_CL_FILES_OPT);
    
    printf(HC_TEXT_CL_USAGE_SMPL_HEADER);
    printf(HC_TEXT_CL_USAGE_SMPL0); // пример 1.
    printf(HC_TEXT_CL_USAGE_SMPL0_TXT); // пояснения к примеру...
    printf(HC_TEXT_CL_USAGE_SMPL1);
    printf(HC_TEXT_CL_USAGE_SMPL1_TXT);
    printf(HC_TEXT_CL_USAGE_SMPL2);
    printf(HC_TEXT_CL_USAGE_SMPL2_TXT);
    printf(HC_TEXT_CL_USAGE_SMPL3);
    printf(HC_TEXT_CL_USAGE_SMPL3_TXT);
    printf(HC_TEXT_CL_USAGE_SMPL4);
    printf(HC_TEXT_CL_USAGE_SMPL4_TXT);
    printf(HC_TEXT_CL_USAGE_SMPL5);
    printf(HC_TEXT_CL_USAGE_SMPL5_TXT);
    printf(HC_TEXT_CL_USAGE_SMPL6);
    printf(HC_TEXT_CL_USAGE_SMPL6_TXT);
}

void CmdLineHelpPrint(void) {


    printf(HC_TEXT_CL_USAGE_HEADER, "hexConfig");
    printf("\n -i, --input       %s", HC_TEXT_CL_i_OPT_EX);
    printf("\n -o, --output      %s", HC_TEXT_CL_o_OPT_EX);
    printf("\n -p, --params      %s", HC_TEXT_CL_p_OPT_EX);
    printf("\n -h, --phelp       %s", HC_TEXT_CL_h_OPT_EX);
    printf("\n -l, --crc-l-e     %s", HC_TEXT_CL_l_OPT_EX);
    printf("\n -b, --crc-b-e     %s", HC_TEXT_CL_b_OPT_EX);
    printf("\n -c, --crc-alg     %s", HC_TEXT_CL_c_OPT_EX);
    printf("\n -r, --crc-replace %s", HC_TEXT_CL_r_OPT_EX);
    printf("\n -f, --crc-force   %s", HC_TEXT_CL_f_OPT_EX);
    printf("\n -v, --verbose     %s", HC_TEXT_CL_v_OPT_EX);
    printf("\n -d, --out-dir     %s", HC_TEXT_CL_d_OPT_EX);
    printf("\n FILES             %s", HC_TEXT_CL_FILES_OPT_EX);
    
    printf(HC_TEXT_CL_USAGE_FOOTER);
    printf("\n -?, --help        %s", HC_TEXT_CL_USAGE_HELP);
    printf("\n --usage           %s", HC_TEXT_CL_USAGE_USAGE);
    printf("\n");

    /*
     D:\...ev\Source.other\hex.configurator>hexConfig.exe -?
   Usage: hexConfig [-? -p -v -h -l -b -r -f --PARAM1=VALUE1 --PARAM2=VALUE2 --PARA
   Mn=VALUEn ]
     -i, --input=<файл>      Имя входного файла (*.hex |
     *.bin)
     -o, --output=<файл>     Имя выходного файла (*.hex)
     -p, --params                Показать поля
                                 КОНФИГУРАТОРА (требуется
                                 входной файл)
     -h, --phelp                 Показать справку hexConfig для
                                 доступа к полям (требуется
                                 входной файл)
     -v, --verbose               Режим вывода
                                 информационных сообщений
     -l, --crc-l-e               Добавить CRC в конец файла
                                 (формат: little-endian)
     -b, --crc-b-e               Добавить CRC в конец файла
                                 (формат: big-endian)
     -c, --crc-alg=<crc-id>      Кодировка алгоритма CRC (По
                                 умолчанию: CRC16.L.0x1021.0xFFFF)
     -r, --crc-replace           Заменить CRC файла. (размер
                                 файла не меняется)
     -f, --crc-force             Добавить CRC в конец файла
                                 игнорируя что CRC равен 0!
     --version                   Вывод информации о версии
                                 программы.

   Help options:
     -?, --help                  Show this help message
     --usage                     Display brief usage message

     //---------------------------------------------------
  
   
  
     */

}