#ifndef PROFILE_TIMER_H
#define PROFILE_TIMER_H

#include <stdbool.h>
#include "sys/alt_timestamp.h"
#include "alt_types.h"

#define TIMER_STATUS_REGISTER 0
#define TIMER_CONTROL_REGISTER 1<<2
#define TIMER_PERIOD_REGISTER_L 2<<2
#define TIMER_PERIOD_REGISTER_H 3<<2
#define TIMER_SNAPSHOT_REGISTER_L 4<<2
#define TIMER_SNAPSHOT_REGISTER_H 5<<2
#define BIT_START 1<<2
#define BIT_STOP 1<<3
//Embedded Peripherals IP User Guide, p.277, table 25
#define TIMER_ENABLE_IRQ 1<<0
#define TIMER_ENABLE_CONTINUOUS 1<<1
#define TIMER_START 1<<2

#define CLOCK_PERIOD_MS 0.00002


#define TIMERS_COUNT 5

#define TIMER_LOOP 0
#define TIMER_CONV_GRAYSCALE 1
#define TIMER_SOBEL_X 2
#define TIMER_SOBEL_Y 3
#define TIMER_SOBEL_THRESHOLD 4

extern alt_u32 profileTimerStartValue[TIMERS_COUNT];
#define AVERAGE_VALUES 5
extern int averageIndex[TIMERS_COUNT];
extern alt_u32 profileTimerValues[TIMERS_COUNT][AVERAGE_VALUES];

void startProfileTimer(int index);
void stopProfileTimer(int index);
double profileTimerms(int index); // Returns the timer value in ms
void printProfileTimers();


#endif //PROFILE_TIMER_H