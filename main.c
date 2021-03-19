#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "modbus.h"
#include "bme280.h"
#include "pid.h"

#define DEBUG 1
#define ARDUINO_DEVICE 0x1

int init_uart(char *path){

    //global uart config
    int uart0_filestream;

    uart0_filestream = open(path, O_RDWR|O_NOCTTY|O_NDELAY);

    if(uart0_filestream == -1){
        fprintf(stderr, "Error - Cannot open uart\n");

        exit(1);
    }

    struct termios options;

    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    return uart0_filestream;

}

void init_bme280(){
}

int main(){

    pid_configura_constantes(0, 0, 0);
    double controle;
    double leitura;

    while(1){

        bme280_driver = init_bme280();
        leitura = readTemperature(bme280_driver);
        controle = pid_controle(leitura);

        // aqui preciso agir nos atuadores
        sleep(1);
    }

    return 0;
}
