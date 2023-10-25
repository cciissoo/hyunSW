#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <wiringSerial.h>
#include "US_Header.h"
#include "LCD_Header.h"
#include "Button_Header.h"
#include "Setup.h"
#include "Types.h"
#include "Touch_Header.h"
#include "Pressure_Header.h"
#include <unistd.h>

enum STATE { __CLOSED, OPENING, OPENED, CLOSING };

int state0(int* state, int* fd_comm, int* var_button, int* var_comm, int* fd_motor);
int state1(int* fd_motor, int* state, double* var_dist, int* var_button);
int state2(int* state, int* fd_comm, int* fd_motor, int* var_button, int* var_comm, int* var_pressure);
int state3(int* state, int* fd_motor, int* var_touch, int* var_button);

int main(void) {
    int fd_lcd = init();
    int state = __CLOSED;
    int fd_comm;
    if ((fd_comm = serialOpen("/dev/ttyUSB0", 115200)) < 0)
    {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
        return 1;
    }
    int fd_motor;
    if ((fd_motor = serialOpen("/dev/ttyACM0", 9600)) < 0)
    {
        fprintf(stderr, "Unable to open serial device : %s\n", strerror(errno));
        return 1;
    }

    serialPutchar(fd_motor, 0);
    serialPutchar(fd_comm, 0);
    sleep(3000);
    int var_button = 0;
    int var_comm = 0;
    int var_pressure = 0;
    int var_touch = 0;
    double var_dist = 0.0;

    while (1) {
        printf("state = %d\n", state);
        switch (state) {
        case __CLOSED:
            state0(&state, &fd_comm, &var_button, &var_comm, &fd_motor);
            break;
        case OPENING:
            state1(&fd_motor, &state, &var_dist, &var_button);
            break;
        case OPENED:
            state2(&state, &fd_comm, &fd_motor, &var_button, &var_comm, &var_pressure);
            break;
        case CLOSING:
            state3(&state, &fd_motor, &var_touch, &var_button);
            break;
        default:
            break;
        }
    }
    return 0;
}

int state0(int* state, int* fd_comm, int* var_button, int* var_comm, int* fd_motor)
{
    if (serialDataAvail(*fd_comm))
    {
        *var_comm = serialGetchar(*fd_comm);
    }

    *var_button = get_button_state();

    if (*var_button == 2 || *var_comm == '1')
    {
        serialPutchar(*fd_motor, '1');
        *state = 1;
    }

    else if (*var_button == 1)
    {
        double a = get_door_dist();
        printf("dist = %.2f\n", a);
        if (a > 10.0) {
            serialPutchar(*fd_motor, '4');
            *state = 2;
        }
        /*
        if(get_door_dist() > 10.0)
        {
            serialPutchar(*fd_motor, '4');
            *state = 2;
        }
        */
    }

    *var_comm = 0;
    *var_button = 0;
    *var_comm = 0;

    return 1;
}

int state1(int* fd_motor, int* state, double* var_dist, int* var_button)
{
    char tmp = 0;

    if (serialDataAvail(*fd_motor))
    {
        tmp = serialGetchar(*fd_motor);
    }
    *var_dist = get_door_dist();

    if (*var_dist <= 10.0)
    {
        serialPutchar(*fd_motor, '0');
        *state = 2;
    }

    else if (tmp == 1)
    {
        *state = 2;
    }

    *var_dist = 0.0;
    *var_button = 0;

    return 1;
}

int state2(int* state, int* fd_comm, int* fd_motor, int* var_button, int* var_comm, int* var_pressure)
{
    if (serialDataAvail(*fd_comm))
    {
        *var_comm = serialGetchar(*fd_comm);
    }

    *var_button = get_button_state();


    if (*var_button == 2 || *var_comm == '2')
    {
        serialPutchar(*fd_motor, '2');
        *state = 3;
    }

    else if (*var_pressure == 1)
    {

    }

    return 1;
}

int state3(int* state, int* fd_motor, int* var_touch, int* var_button)
{
    char tmp = 0;
    if (serialDataAvail(*fd_motor))
    {
        tmp = serialGetchar(*fd_motor);
    }

    *var_touch = get_door_touched();

    if (*var_touch == 1)
    {
        serialPutchar(*fd_motor, '0');
        *state = 2;
    }

    else if (tmp == 1)
    {
        *state = 0;
    }

    *var_touch = 0;
    *var_button = 0;

    return 1;
}