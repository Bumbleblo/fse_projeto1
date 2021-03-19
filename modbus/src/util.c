
    int reference_temperature = 10;

    int uart0_filestream = init_uart("/dev/serial0");

    unsigned char message[4];

    message[0] = 9;
    message[1] = 7;
    message[2] = 3;
    message[3] = 3;

    ModBusString *m;
   
    m = createMessage(
        ARDUINO_DEVICE,
        READ,
        R_POTEN,
        (void*)&message[0],
        4
    );

    int i;
    printf("Sending message| size %d\n", m->size);
    for(i = 0; i<m->size; ++i){
        printf("%x ", m->message[i]);
    }

    int count = write(uart0_filestream, m->message, m->size);

    if(count < 0){
        fprintf(stderr, "Write error");
    }

    printf("\n");

    sleep(2);

    unsigned char rx_buffer[100];

    printf("Receive  message\n");
    count = read(uart0_filestream, rx_buffer, m->size);

    if(count < 0){
        printf("Read error\n");
    }

    for(i=0; i<count; ++i){
        printf("%x ", rx_buffer[i]);
    }
