#ifndef PV_H
#define PV_H

#include <wiringPiSPI.h>
#include <wiringPi.h>

int get_pressure_velocity(int* pressureP, int* velocityP);
int Read3002_PV(int CS, int* ch0, int* ch1);

#endif