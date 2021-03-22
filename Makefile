TARGET=projeto1

BINDIR=bin
CC=gcc

all:  main bme280 modbus pid lcd log
	@echo "Linking"
	$(CC) -o $(TARGET) \
		main.o bin/linux_userspace.o bin/bme280.o bin/modbus.o bin/crc16.o bin/pid.o bin/lcd.o bin/log.o\
		-Imodbus/include -Ibme280/include -Ipid/include -Ilcd/include -Ilog/include\
		-lwiringPi -lrt -lcrypt -lm -DDEBUG

main: createbin
	@echo "Compiling project binary"
	$(CC) -c main.c -Ibme280/include -Imodbus/include -Ipid/include -Ilog/include -Ilcd/include-DDEBUG -o main.o -lwiringPi

bme280: createbin
	@echo "Compiling BME280 module"A
	$(CC) -c bme280/src/bme280.c -Ibme280/include -o $(BINDIR)/bme280.o
	$(CC) -c bme280/src/linux_userspace.c -Ibme280/include -o $(BINDIR)/linux_userspace.o

modbus: createbin
	@echo "Compiling MODBUS module"
	$(CC) -c modbus/src/crc16.c -Imodbus/include -o $(BINDIR)/crc16.o
	$(CC) -c modbus/src/modbus.c -Imodbus/include -o $(BINDIR)/modbus.o

pid: createbin
	@echo "Compiling PID module"
	$(CC) -c pid/src/pid.c -Ipid/include -o $(BINDIR)/pid.o

lcd: createbin
	@echo "Compiling LCD module"
	$(CC) -c lcd/src/lcd.c -Ilcd/include -o $(BINDIR)/lcd.o

log: createbin
	@echo "Compiling LOG module"
	$(CC) -c log/src/log.c -Ilog/include -o $(BINDIR)/log.o

createbin:
	@mkdir -p bin

clean-deps:
	@echo "Cleaning bin"
	@rm -rf bin

clean: clean-deps
	@echo "Clearning"
	@rm -rf $(TARGET)
	@echo "Done"

