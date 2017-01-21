@del *.hex
@del *.bin

@copy test_m64k.TestHex test_m64k.Hex /y

@echo Build test file
@srec_cat test_m64k.hex -intel -crc16-b-e -maximum-addr test_m64k.hex -intel 0x8005 -XMODEM -ENable=exec-start-addr -Line_Termination CRLF -Line_Length 44 -o test_m64k.test.hex -intel

@echo change field MHZ=8000000 to MHZ=8000001
@..\hexConfig.exe -i test_m64k.hex -o test_m64k.out.ch1.hex --MHZ=8000001 -b
@..\hexConfig.exe -i test_m64k.out.ch1.hex -o test_m64k.out.ch2.hex --MHZ=8000000 -b -r -c CRC16.L.0x8005.0x0000
@fc test_m64k.out.ch2.hex test_m64k.test.hex
@if %errorlevel% == 0 (echo LESS 64K TEST OK) else (echo LESS 64K TEST ERR)

@copy test_m64k.out.ch2.hex test_over64k.bin /y
@..\hexConfig.exe -i test_over64k.bin -o test_over64k.out.crc.hex -b -c CRC16.L.0x8005.0x0000
@srec_cat test_over64k.bin -binary -offset 0 -crc16-b-e -maximum-addr test_over64k.bin -binary 0x8005 -XMODEM -Line_Termination CRLF -Line_Length 44 -o test_over64k.test.hex -intel
@fc test_over64k.out.crc.hex test_over64k.test.hex
@if %errorlevel% == 0 (echo OVER 64K TEST OK) else (echo OVER 64K TEST ERR)

@srec_cat test_over64k.bin -binary -offset 0 -o test_over64k.f20.hex -intel
@srec_cat test_over64k.bin -binary -offset 0 -Line_Termination CRLF -Line_Length 44 -o test_over64k.f10.hex -intel
@..\hexConfig.exe -i test_over64k.f20.hex -o test_over64k.f.out.hex
@fc test_over64k.f10.hex test_over64k.f.out.hex
@if %errorlevel% == 0 (echo HEX REFORMAT TEST OK) else (echo HEX REFORMAT TEST ERR)

@del *.hex
@del *.bin


exit


