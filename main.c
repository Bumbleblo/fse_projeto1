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
#include "log.h"

#define ARDUINO_DEVICE 0x1
#define GPIO_R_PIN 23
#define GPIO_V_PIN 24

typedef union TemperatureData{
    float temperature;
    unsigned char bytes[4];
}TemperatureData;

typedef struct ModbusTemperatureData{
    TemperatureData data;
    int ok;
} ModbusTemperatureData;

// global variables
int uart0_filestream;

// control
void onResistor(double);
void onCooler(double);
void offCooler();
void offResistor();
ModbusTemperatureData getMODBUSInfo(int pin);

// Setup functions
void handle_exit(int);
void setupGPIO();
void setupBME280(struct bme280_dev*, struct identifier*, char*);
int setupUart(char *path);

int main(){

    // handles ctrl + c sigint signal
    signal(SIGINT, handle_exit);

    // setup lcd
    setupLCD();

    // setup logger
    printf("Setup logger\n");
    initLog("report.csv");

    // setup gpio
    printf("Configurando WiringPi\n");
    setupGPIO();

    // setup pid
    printf("Configurando constantes PID\n");
    pid_configura_constantes(5.0, 1.0, 5.0);
    double controle;

    // setup bme280
    printf("Configurando BME280\n");
    struct bme280_data externalTemp;
    struct bme280_dev bme280_device;
    struct identifier id;

    setupBME280(&bme280_device, &id, "/dev/i2c-1");

    // setup uart
    printf("Setup UART\n");
    uart0_filestream = setupUart("/dev/serial0");

    //loop principal
    while(1){

        // Read external temperature
        printf("Reading reference temperature (bme280)\n");
        externalTemp = readTemperatureData(&bme280_device);

        // Getting temperature MODBUS
        ModbusTemperatureData internalTempInfo = getMODBUSInfo(R_TEMPERATURE);

        if(!internalTempInfo.ok){
            fprintf(stderr, "Fail to read resisto\n");
            continue;
        }

        TemperatureData internalTemp = internalTempInfo.data;

        #ifdef DEBUG
            printf("Temperature: %f\n", internalTemp.temperature);
        #endif 

        //Getting potence MODBUS
        printf("Reading potence reference (potenciomento)\n");
        ModbusTemperatureData potenceInfo = getMODBUSInfo(R_POTEN);

        if(!potenceInfo.ok){
            fprintf(stderr, "Fail do read potence\n");
            continue;
        }

        TemperatureData potence = potenceInfo.data;
        
        #ifdef DEBUG
            printf("Potence: %f\n", potence.temperature);
        #endif

        // update PID
        pid_atualiza_referencia(potence.temperature);
        controle = pid_controle(internalTemp.temperature);

        #ifdef DEBUG
            printf("Control %lf\n", controle);
        #endif

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

        // show information on lcd
        showLCD(
            (float)potence.temperature,
            (float)internalTemp.temperature,
            (float)externalTemp.temperature
        );

        // log information on CSV file
        logStatus(
            (float)internalTemp.temperature,
            (float)externalTemp.temperature,
            (float)potence.temperature, 
            (float)controle
        );

    }

    return 0;
}

// configure GPIO and R and V pin
void setupGPIO()
{

    // wiring pi setup
    if(wiringPiSetupGpio() == -1){
        fprintf(stderr, "WiringPi setup error\n");
        exit(1);
    }

    softPwmCreate(GPIO_R_PIN, 0, 1024);
    softPwmCreate(GPIO_V_PIN, 0, 1024);

}

// handle program when SIGINT is sended
void handle_exit(int sig){

    // close uart
    printf("Closing uart\n");
    close(uart0_filestream);

    // close gpio
    printf("Closing GPIO\n");
    offResistor();
    sleep(1);

    offCooler();
    sleep(1);


    printf("Closing LOG\n");
    closeLog();


    exit(0);

}

// setup UART
int setupUart(char *path){

    //global uart config

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

// Setup BME280
void setupBME280(struct bme280_dev *dev, struct identifier *id, char *path)
{

    configure_bme280(dev, id, path);

    int code = bme280_init(dev);

    if(code != BME280_OK){
        fprintf(stderr, "Failed to initialize bme280: %d\n", code);
        exit(1);
    }
}

ModbusTemperatureData getMODBUSInfo(int pin)
{

    int count = 0, i;
    unsigned char registration[] = {9, 7, 3, 3};
    ModBusString *px_buffer;
    ModbusTemperatureData response;
    unsigned char rx_buffer[100];

    response.ok = 1;

    px_buffer = createMessage(
            ARDUINO_DEVICE,
            READ,
            pin,
            (void*)registration,
            4
    );

    count = write(uart0_filestream, px_buffer->message, px_buffer->size);

    if(count < 0){
        fprintf(stderr, "Write error\n");
        response.ok = 0;
    }

    sleep(0.5);

    count = read(uart0_filestream, rx_buffer, 3 + 2 + 4);

    if(count < 0){
        fprintf(stderr, "Read error\n");
        response.ok = 0;
    }

    for(i=3; i<7; ++i)
        response.data.bytes[i-3] =rx_buffer[i];

    return response;
}
