
/* Structure that contains identifier details used in example */
struct identifier
{
    /* Variable to hold device address */
    uint8_t dev_addr;

    /* Variable that contains file descriptor */
    int8_t fd;
};

struct bme280_data readTemperatureData(struct bme280_dev *dev);

void configure_bme280(struct bme280_dev *dev, struct identifier *id, char *i2c_path);
