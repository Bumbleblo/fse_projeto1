CC = gcc -c 

SRCDIR = src/
INCLUDEDIR = include/
BINDIR= bin/

SOURCES := $(wildcard $(SRCDIR)/*.c)

TARGET = bme280

all: clean 
	@echo "compiling bm280 module"
	@mkdir $(BINDIR)
	$(CC) $(SOURCES) -I$(INCLUDEDIR) -o $(BINDIR)/$(TARGET).o
	@echo "bm280 module compiled"

clean:
	@rm -rf bin
