FROM mitchallen/pi-cross-compile

RUN apt-get update && apt-get install git sudo -y

RUN git clone https://github.com/WiringPi/WiringPi.git && cd WiringPi && ./build

WORKDIR /build

ENTRYPOINT ["make"]

