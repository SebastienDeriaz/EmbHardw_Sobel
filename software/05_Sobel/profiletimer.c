#include "profiletimer.h"


int averageIndex = 0;
double profileTimerValues[AVERAGE_VALUES] = {0};
alt_u32 profileTimerValue = 0;
alt_u32 profileTimerStopValue = 0;
alt_u32 profileTimerStartValue = 0;

void startProfileTimer() {
    // Stop the timer, reset, reset the status register, then start
    profileTimerStartValue = alt_timestamp_start();
}

void stopProfileTimer() {
    profileTimerStopValue = alt_timestamp();
    profileTimerValue = profileTimerStartValue - profileTimerStopValue;
}

double profileTimerms() { return (double)profileTimerValue * TIMER_PERIOD_MS; }

void printProfileTimer() {
    profileTimerValues[averageIndex++] = profileTimerms();
    if(averageIndex >= AVERAGE_VALUES) {
        averageIndex = 0;
    }
    double value = 0;
    bool ready = true;
    for(int i = 0;i<AVERAGE_VALUES;i++) {
        if(profileTimerValues[i] == 0) ready = false;
        value += profileTimerValues[i];
    }
    value /= AVERAGE_VALUES;
    if(ready) {
        printf("Loop time : %3.6f ms\n", value);
    }
    else {
        printf("Building average...\n", value);
    }
}