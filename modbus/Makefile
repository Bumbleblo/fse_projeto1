CC=gcc -c

SRCDIR = src/
INCLUDEDIR = include/
BINDIR= bin/

SOURCES := $(wildcard $(SRCDIR)/*.c)

TARGET = modbus

all: clean 
	@echo "compiling modbus module"
	@mkdir $(BINDIR)
	$(CC) src/modbus.c -Iinclude -o bin/modbus.o
	$(CC) src/crc16.c -Iinclude -o bin/crc16.o
	@echo "modbus module compiled"

clean:
	@rm -rf bin
