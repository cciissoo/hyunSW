#include "pressure_velocity_ver_0.1.h"
#include <stdio.h>

// SPI 설정
extern const int channel = 0; // SPI 채널 0 사용
extern const int speed = 1000000; // SPI 속도 1 MHz
extern const int mode = 0; // SPI 모드 0
extern const int CS_3202 = 17; // WiringPi CS 핀 번호

extern const int VMAX_v = 4905;
extern const int VMIN_v = 0;
extern const int VOutOfRange_v = -1;
extern const int VMode = 20;

extern const int PMAX_v = 4905;
extern const int PMIN_v = 0;
extern const int POutOfRange_v = -1;
extern const int PMode = 50;

int Read3002_PV(int CS, int* ch0, int* ch1) {
    unsigned char buffer[3];
    int msb;
    int lsb;

    digitalWrite(CS, LOW);

    buffer[0] = 0x01; // Start bit
    buffer[1] = 0x80; // Single-ended mode, ch0
    buffer[2] = 0x00;

    wiringPiSPIDataRW(channel, buffer, 3);

    msb = buffer[1] & 0x0F;
    lsb = buffer[2];

    *ch0 = (msb << 8) | lsb;

    digitalWrite(CS, HIGH);
    delay(1);
    digitalWrite(CS, LOW);

    buffer[0] = 0x01; // Start bit
    buffer[1] = 0xC0; // Single-ended mode, ch1
    buffer[2] = 0x00;

    wiringPiSPIDataRW(channel, buffer, 3);

    msb = buffer[1] & 0x0F;
    lsb = buffer[2];

    *ch1 = (msb << 8) | lsb;

    digitalWrite(CS, HIGH);

    return 1;
}

int get_pressure_velocity(int* pressureP, int* velocityP) {
    if (wiringPiSPISetup(channel, speed) == -1) {
        //printf("SPI 설정 실패\n");
        return 0;
    }

    //wiringPiSetup();
    wiringPiSetupGpio();
    pinMode(CS_3202, OUTPUT);
    digitalWrite(CS_3202, HIGH);

    Read3002_PV(CS_3202, pressureP, velocityP);

    //디버그 코드 : 압력, 속도의 spi 수신 값을 본다
    //printf("P : %d, V : %d\n", *pressureP, *velocityP);

    if (*velocityP < VMIN_v)
    {
        *velocityP = VOutOfRange_v;
    }
    if (*velocityP > VMAX_v)
    {
        *velocityP = VOutOfRange_v;
    }

    if (*velocityP == VOutOfRange_v)
    {
        printf("velocity is out of range\n");
    }
    else
    {
        *velocityP = (int)((float)(*velocityP) / (float)(VMAX_v - VMIN_v) * VMode + VMIN_v);
    }

    if (*pressureP < PMIN_v)
    {
        *pressureP = POutOfRange_v;
    }
    if (*pressureP > PMAX_v)
    {
        *pressureP = POutOfRange_v;
    }

    if (*pressureP == POutOfRange_v)
    {
        printf("pressure is out of range\n");
    }
    else
    {
        *pressureP = (int)((float)(*pressureP) / (float)(PMAX_v - PMIN_v) * PMode + PMIN_v);
    }

    //printf("CH0 = %d\n", *pressure);
    //printf("CH1 = %d\n\n", *velocity);

    return 1;
}