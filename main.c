#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <wiringPi.h>

#include "modbus.h"
#include "bme280.h"
#include "pid.h"
#include "linux_userspace.h"
#include "lcd.h"

#define ARDUINO_DEVICE 0x1
#define GPIO_R_PIN 23
#define GPIO_V_PIN 24

typedef union TemperatureData{
    float temperature;
    unsigned char bytes[4];
}TemperatureData;

void onResistor(double controle){

    int result  = (int)controle*10.24;

    #ifdef DEBUG
        printf("Resistor value: %d\n", result);
    #endif

    softPwmWrite(GPIO_R_PIN, result);

}

void offResistor(){
    softPwmWrite(GPIO_R_PIN, 0);
}

void onCooler(double controle){

    int result  = (int)controle*10.24;

    #ifdef DEBUG
        printf("Resistor value: %d\n", result);
    #endif

    softPwmWrite(GPIO_V_PIN, result);
}

void offCooler(){
    softPwmWrite(GPIO_V_PIN, 0);
}

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

int uart0_filestream;

void handle_exit(int sig){

    // close uart
    printf("Closing uart\n");
    close(uart0_filestream);

    // close gpio
    printf("Closing GPIO\n");
    offResistor();
    offCooller();

    exit(0);

}

int main(){

    // handles ctrl + c sigint signal
    signal(SIGINT, handle_exit);

    // wiring pi setup
    printf("Configurando WiringPi\n");
    if(wiringPiSetupGpio() == -1){
        fprintf(stderr, "WiringPi setup error\n");
        exit(1);
    }

    softPwmCreate(GPIO_R_PIN, 0, 1024);
    softPwmCreate(GPIO_V_PIN, 0, 1024);

    //configure PID
    pid_configura_constantes(5.0, 1.0, 5.0);
    double controle;

    //configure bme280
    printf("Configurando bme280\n");
    struct bme280_data leitura;
    struct bme280_dev bme280_device;
    struct identifier id;

    configure_bme280(&bme280_device, &id, "/dev/i2c-1");

    printf("argumentos enviados\n");
    int response = bme280_init(&bme280_device);

    if(response != BME280_OK){
        fprintf(stderr, "Failed to initialize bme280: %d\n", response);
        exit(1);
    }   
    printf("Device configurado\n");

    //configurando uart

    int count = 0;
    unsigned char registration[4];

    registration[0] = 9;
    registration[1] = 7;
    registration[2] = 3;
    registration[3] = 3;

    uart0_filestream = init_uart("/dev/serial0");

    ModBusString *px_buffer;
    unsigned char rx_buffer[100];

    //loop principal
    while(1){

        // Read reference temperature
        printf("Reading reference temperature (bme280)\n");
        
        leitura = readTemperatureData(&bme280_device);

        // Getting temperature MODBUS
        px_buffer = createMessage(
            ARDUINO_DEVICE,
            READ,
            R_TEMPERATURE,
            (void*)registration,
            4
        );

        count = write(uart0_filestream, px_buffer->message, px_buffer->size);

        if(count < 0){
            fprintf(stderr, "Write error\n");
        }

        sleep(1);

        count = read(uart0_filestream, rx_buffer, 3 + 2 + 4);

        if(count < 0){
            fprintf(stderr, "Read error\n");
        }


        int i;

        TemperatureData temp;

        for(i=3; i<7; ++i)
            temp.bytes[i-3] =rx_buffer[i];

        #ifdef DEBUG
            printf("Temperature: %f\n", temp.temperature);
        #endif 

        //Getting potence MODBUS
        printf("Reading potence reference (potenciomento)\n");
        
        // Getting potence MODBUS
        

        TemperatureData potence;

        px_buffer = createMessage(
            ARDUINO_DEVICE,
            READ,
            R_POTEN,
            (void*)registration,
            4
        );

        count = write(uart0_filestream, px_buffer->message, px_buffer->size);

        if(count < 0){
            fprintf(stderr, "Write error\n");
        }

        sleep(1);

        count = read(uart0_filestream, rx_buffer, 3 + 2 + 4);

        if(count < 0){
            fprintf(stderr, "Read error\n");
        }

        for(i=3; i<7; ++i)
            potence.bytes[i-3] = rx_buffer[i];


        #ifdef DEBUG
            printf("Potence: %f\n", potence.temperature);
        #endif


        #ifdef DEBUG
            int pos_debug;
            for(pos_debug = 0; pos_debug<count; ++pos_debug){
                printf("%x ", rx_buffer[pos_debug]);
            }
            printf("\n");
        #endif

        // update PID
        pid_atualiza_referencia(potence.temperature);
        controle = pid_controle(temp.temperature);

        printf("Control %lf\n", controle);

        if(controle > 0){
            onResistor(controle);
            offCooler();
        }else{
            if(controle < -20){
                onCooler(-1*controle);
            }else{
                offCooler();
            }

            offResistor();
        }

        showLCD((float)potence.temperature, (float)temp.temperature, (float)potence.temperature);

        #ifdef DEBUG
            printf("%lf\n", controle);
        #endif

    }

    return 0;
}
