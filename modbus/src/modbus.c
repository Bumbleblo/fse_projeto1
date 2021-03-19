#include <stdlib.h>
#include <stdio.h>
#include "modbus.h"
#include "crc16.h"

ModBusString *createMessage(char address, char function, char subfunction, unsigned char *message, int messageSize){
    ModBusString *modbusString = (ModBusString*)malloc(sizeof(ModBusString));

    int messageTotalSize = 5 + messageSize;

    modbusString->message = (char*)malloc(sizeof(char)*(messageTotalSize));

    char *ptr = modbusString->message;

    *ptr++ = address;
    *ptr++ = function;
    *ptr++ = subfunction;

    int i;
    for(i = 0; i <messageSize; ++i)
        *ptr++ = message[i];

    CRCData crc;

    crc.data = calcula_CRC(modbusString->message, 3 + messageSize);

    printf("%d\n", crc.data);

    for(i = 0; i<2; ++i)
        printf("%x ", crc.string[i]);

    printf("\n");

    *ptr++ = crc.string[0];
    *ptr++ = crc.string[1];

    modbusString->size = messageTotalSize;

    return modbusString;
}
