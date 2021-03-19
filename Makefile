TARGET=projeto1
#CC     = gcc -c
CFLAGS = -Wall

#LINKER = gcc -o
LFLAGS = -Wall

SOURCES = $(wildcard *.c)
INCLUDES = $(wildcard *.h)
OBJECTS = $(wildcard: .c=.o)
MAKE=make

CC=/pitools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc

all:  main temperatura modbus
	$(CC) -o $(TARGET) main.o bme280/bin/bme280.o modbus/bin/crc16.o modbus/bin/modbus.o PID/bin/pid.o -Imodbus/include -IPID/include 

main:
	$(CC) -c  main.c -Ibme280/include -Imodbus/include -IPID/include -o main.o 

temperatura:
	cd bme280 && $(MAKE)

modbus:
	cd modbus && $(MAKE)

pid:
	cd PID && $(MAKE)

clean:
	rm -rf bme280/bin modbus/bin PID/bin
	rm *.o
