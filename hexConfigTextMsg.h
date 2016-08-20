/* 
 * File:   hexConfigTextMsg.h
 * Author: Dunaev
 *
 * Created on 4 Август 2016 г., 7:45
 */

#ifndef HEXCONFIGTEXTMSG_H
#define	HEXCONFIGTEXTMSG_H

// текстовые сообщения выводимые программой
// при сборке под разные ОС нужно конвертировать кодировку

// сообщения для модуля обработки командной строки:

#define	HC_TEXT_CL_USAGE_HEADER_BRIEF "\nИспользование (кратко): %s"          
#define	HC_TEXT_CL_USAGE_HEADER       "\nИспользование: %s [-? -pvhlbrf --PARAM1=VALUE1 --PARAM2=VALUE2 --PARAMn=VALUEn] FILES"          
#define	HC_TEXT_CL_USAGE_FOOTER       "\nОпции помощи:"          
#define	HC_TEXT_CL_USAGE_HELP         "Вывод данной справки"          
#define	HC_TEXT_CL_USAGE_USAGE        "Вывод справки в сокращенном виде"          
#define	HC_TEXT_CL_USAGE_SMPL_HEADER  "\nПримеры использования:"          
#define	HC_TEXT_CL_USAGE_SMPL0        "\n  hexConfig -i fl.hex -p -h"          
#define	HC_TEXT_CL_USAGE_SMPL0_TXT    ""          
#define	HC_TEXT_CL_USAGE_SMPL1        "\n  hexConfig -i fl.hex -o fl.hex --dir-out=./ver+VERSION"          
#define	HC_TEXT_CL_USAGE_SMPL1_TXT    ""          
#define	HC_TEXT_CL_USAGE_SMPL2        "\n  hexConfig -i fl.hex -o fl.hex --BAUD=9600 --DEPIN=10"          
#define	HC_TEXT_CL_USAGE_SMPL2_TXT    ""          
#define	HC_TEXT_CL_USAGE_SMPL3        "\n  hexConfig -i fl.hex -o fl_crc.hex --crc-alg=CRC16.R.0xA001.0xFFFF --crc-b-e"          
#define	HC_TEXT_CL_USAGE_SMPL3_TXT    ""          
#define	HC_TEXT_CL_USAGE_SMPL4        "\n  hexConfig -i fl.hex -o fl_crc.hex --BAUD=9600 --crc-l-e"          
#define	HC_TEXT_CL_USAGE_SMPL4_TXT    ""          
#define	HC_TEXT_CL_USAGE_SMPL5        "\n  hexConfig -i fl_crc.hex -o fl_crc.hex --BAUD=115200 --crc-l-e --crc-replace"          
#define	HC_TEXT_CL_USAGE_SMPL5_TXT    ""          
#define	HC_TEXT_CL_USAGE_SMPL6        "\n  hexConfig -i fl.hex -o fl.hex --dir-out=./ver+VERSION info.doc readme.txt"          
#define	HC_TEXT_CL_USAGE_SMPL6_TXT    "\n"          

//#define	HC_TEXT_CL_USAGE_HEADER       "\nUsage: %s [-? -p -v -h -l -b -r -f --PARAM1=VALUE1 --PARAM2=VALUE2 --PARAMn=VALUEn]"          
#define	HC_TEXT_CL_ERR_NOARG          "\nНет значения для параметра!\n"
#define	HC_TEXT_CL_ERR_BADARG         "\nОшибка преобразования в число!\n"

#define	HC_TEXT_CL_i_OPT              "<файл>"
#define	HC_TEXT_CL_i_OPT_EX           "Имя входного файла (*.hex | *.bin)"
#define	HC_TEXT_CL_o_OPT              "<файл>"
#define	HC_TEXT_CL_o_OPT_EX           "Имя выходного файла (*.hex)"
#define	HC_TEXT_CL_p_OPT              "" //params show
#define	HC_TEXT_CL_p_OPT_EX           "Показать поля КОНФИГУРАТОРА (требуется входной файл)"
#define	HC_TEXT_CL_h_OPT              "" //"params help"
#define	HC_TEXT_CL_h_OPT_EX           "Показать справку hexConfig для доступа к полям (требуется входной файл)"
#define	HC_TEXT_CL_v_OPT              "" //"verbose mode"
#define	HC_TEXT_CL_v_OPT_EX           "Режим вывода информационных сообщений"
#define	HC_TEXT_CL_l_OPT              "" //"append CRC"
#define	HC_TEXT_CL_l_OPT_EX           "Добавить CRC в конец файла (формат: little-endian)"
#define	HC_TEXT_CL_b_OPT              "" //"append CRC"
#define	HC_TEXT_CL_b_OPT_EX           "Добавить CRC в конец файла (формат: big-endian)"
#define	HC_TEXT_CL_c_OPT              "<crc-id>" //NULL //"" //"Алгоритм CRC"
#define	HC_TEXT_CL_c_OPT_EX           "Кодировка алгоритма CRC. По умолчанию: CRC16.L.0x1021.0xFFFF"
#define	HC_TEXT_CL_r_OPT              "" // "CRC replace"
#define	HC_TEXT_CL_r_OPT_EX           "Заменить CRC файла. (размер файла не меняется)"
#define	HC_TEXT_CL_f_OPT              "" //"CRC force"
#define	HC_TEXT_CL_f_OPT_EX           "Добавить CRC в конец файла игнорируя что CRC равен 0!"
#define	HC_TEXT_CL_d_OPT              "<директория>" 
#define	HC_TEXT_CL_d_OPT_EX           "Директория размещения результата и доп. файлов"
#define	HC_TEXT_CL_VER_OPT_EX         "Вывод информации о версии программы."
#define	HC_TEXT_CL_FILES_OPT          "ФАЙЛЫ" 
#define	HC_TEXT_CL_FILES_OPT_EX       "Файлы для копирования в папку размещения результата." 
// -----------------------------------------------------------------------------
// сообщения модуля разбора hex-файла:
#define	HC_TEXT_HEX_TYPE_IGNORE       "\n Тип hex-строки не обрабатывается (Строка: %s)...Игнорирование строки."
#define	HC_TEXT_HEX_CRC_IGNORE        "\n Ошибка контрольной суммы hex-строки. (%d <> %d) Строка: \n %s \nИгнорирование строки..."

// -----------------------------------------------------------------------------
// сообщения основной программы:
#define	HC_TEXT_ERR_SECTOR_OVER       "\nError #1"
#define	HC_TEXT_ERR_WR_SECTOR         "\nError #2"
#define	HC_TEXT_ERR_FILEEXT           "\nError #3"
#define	HC_TEXT_ERR_PCHAR_ADDR        "\nError #4"
#define	HC_TEXT_ERR_OPENFILE          "\nОшибка открытия файла. (Имя файла: '%s')"
#define	HC_TEXT_ERR_PSIZE             "\nНеверный размер указателя (%s). Должен быть от 2 до 4."
#define	HC_TEXT_INFO_PARAM_UNFOND     "\nЗапрашиваемый параметр в структуре конфигуратора не найден (%s)"

#define	HC_TEXT_INFO_FIELDS_NUM       "%s [Кол-во полей: %d]. "
#define	HC_TEXT_INFO_FORMAT_LE        " Формат хранения : little-endian\n"
#define	HC_TEXT_INFO_FORMAT_BE        " Формат хранения : big-endian\n"
#define	HC_TEXT_INFO_VALUE            "[значение: %s]\n"
#define	HC_TEXT_INFO_TYPE_INT32      " - INT32 "
#define	HC_TEXT_INFO_TYPE_INT16      " - INT16 "
#define	HC_TEXT_INFO_TYPE_BYTE       " - BYTE "
#define	HC_TEXT_INFO_TYPE_FLOAT      " - FLOAT "
#define	HC_TEXT_INFO_TYPE_PCHAR      " - STRING "
#define	HC_TEXT_INFO_NULL             "\nСтруктура конфигуратора не найдена!\n"
#define	HC_TEXT_INFO_WRITE_PCHAR_ERR  "\nОшибка! Размер новой строки превышает размер текущей строки.\n"
#define	HC_TEXT_INFO_WRITE_PCHAR      "\nЗамена строки в файле. '%s' -> '%s'"

#define	HC_TEXT_CRC_DUP               "\nРезультат вычисления CRC равен 0! Файл уже содержит CRC код. (используйте -r)"
#define	HC_TEXT_CRC_FORCE_MSG         "\n --force флаг обнаружен. Пишем 0 в конец файла!... но зачем?"
#define	HC_TEXT_CRC_NEED_FORCE        "\nДля принудительной записи 0 в конец файла используйте --crc-force или -f"
#define	HC_TEXT_CRC_ALG_INFO          "\nРезультат вычисления CRC: 0x%04X (алгоритм: %s)\nРазмер файла: %d"
#define	HC_TEXT_CRC_PLACE_ERR         "\nПроблема при размещении CRC. Адрес вышел за границы 64Kb области (адрес = 0x%04X)"

#define	HC_TEXT_FILE_INFO             "Входной файл : '%s' -> Файл результата : '%s'\n"
#define	HC_TEXT_FILE_INVALID_EXT      "\nОшибка! Расширение входного файла не поддерживается (расширение: %s)"

#define	HC_TEXT_INFO_IN_PREF          "\nВход> "
#define	HC_TEXT_INFO_OUT_PREF         "\n Кол-во измененных полей: %d\nВыход> "

#define	HC_TEXT_FILE_WRITE_MSG        "\nЗапись файла '%s'..."
#define	HC_TEXT_FILES_COPY_MSG        "\nСкопированно файлов: %d"


#endif	/* HEXCONFIGTEXTMSG_H */

