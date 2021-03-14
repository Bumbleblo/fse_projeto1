#include <stdio.h>
#include "bme280.h"

int main(){

    printf("Teste\n");

    struct bme280_dev dev;
    int8_t rslt = BME280_OK;
    uint8_t dev_addr = BME280_I2C_ADDR_PRIM;


    signed char user_i2c_read = 0x76;
    signed char user_i2c_write = 0x76;

    dev.intf_ptr = &dev_addr;
    dev.intf = BME280_I2C_INTF;
    dev.read = &user_i2c_read;
    dev.write = &user_i2c_write;
    dev.delay_ms = user_delay_ms;

    rslt = bme280_init(&dev);



    return 0;
}
