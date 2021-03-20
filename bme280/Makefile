CC=gcc -c 

SRCDIR = src/
INCLUDEDIR = include/
BINDIR= bin/

SOURCES := $(wildcard $(SRCDIR)/*.c)

TARGET = bme280

all: clean 
	@echo "compiling bm280 module"
	@mkdir $(BINDIR)
	$(CC) $(SRCDIR)/bme280.c -Iinclude -o $(BINDIR)/bme280.o
	$(CC) $(SRCDIR)/linux_userspace.c -Iinclude -o $(BINDIR)/linux_userspace.o
	@echo "bm280 module compiled"

clean:
	@rm -rf bin
