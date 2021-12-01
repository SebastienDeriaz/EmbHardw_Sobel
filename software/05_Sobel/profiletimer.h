#ifndef PROFILE_TIMER_H
#define PROFILE_TIMER_H

#include <system.h>
#include <io.h>
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
#define TIMER_PERIOD_MS 0.000001


extern alt_u32 profileTimerStartValue;
extern alt_u32 profileTimerStopValue;
extern alt_u32 profileTimerValue;
#define AVERAGE_VALUES 5
extern int averageIndex;
extern double profileTimerValues[AVERAGE_VALUES];

void startProfileTimer();
void stopProfileTimer();
double profileTimerms(); // Returns the timer value in ms
void printProfileTimer();


#endif //PROFILE_TIMER_H