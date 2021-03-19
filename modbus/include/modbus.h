#define READ 0x23

#define R_TEMPERATURE 0xc1
#define R_POTEN 0xc2

typedef union CRCData{
    unsigned char string[2];
    short data;
}CRCData;

typedef struct ModBusMessage{
    char address;
    char function;
    char subcode;
    char *message;
    CRCData crc;
}ModbusMessage;


typedef struct ModBusString{
    unsigned char *message;
    int size;
}ModBusString;


ModBusString *createMessage(char address, char function, char subfunction, unsigned char *message, int messageSize);
