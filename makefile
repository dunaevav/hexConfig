DIR := .
#DEF += -Wno-pointer-to-int-cast
INC := ./usr/include 
#LOPTION := -lrt -lpopt
LOPTION := -LC:\Tools\MinGW_\msys\1.0\lib -lpopt
GCC := gcc

CFILES := utils/hexutils.c \
	  utils/crc16utils.c \
	  utils/strutils.c \
	  utils/fileutils.c \
	  cmdLine/cmdLine.c \
	  hexConfig.c 
	  

OFILES = $(CFILES:%.c=%.o)
	
all: $(OFILES) 
	$(GCC) $(OFILES) $(LOPTION) -o hexConfig.exe
#	$(GCC) $(OFILES) $(LOPTION) -o hexConfig.elf


$(OFILES): %.o: %.c
	$(GCC) $(DEF) -I $(INC) -c $(DIR)/$< -o $@

clean:
	for m in $(OFILES); do rm -v $$m ; done
	rm hexConfig.exe
#	rm hexConfig.elf
	
.PHONY: all clean
.SUFFIXES:	.c .o .hex .dis .bin .elf


