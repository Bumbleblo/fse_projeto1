TARGET = projeto1
#CC     = gcc -c
CFLAGS = -Wall

LINKER = gcc -o
LFLAGS = -Wall

SOURCES = $(wildcard *.c)
INCLUDES = $(wildcard *.h)
OBJECTS = $(wildcard: .c=.o)
MAKE= make

CC=/pitools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc

#main:
#	$(CC) main.c -Ibme280/include -o main.o

#temperatura:
#	cd bme280 && $(MAKE)

#compile:  main temperatura
#	$(LINKER) $(TARGET) main.o bme280/bin/bme280.o

#clean:
#	rm -rf bme280/bin
#	rm *.o
# 	rm $(TARGET)

all:
	$(CC) linux_userspace.c bme280/src/bme280.c -Ibme280/include/
