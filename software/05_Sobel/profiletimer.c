#include "profiletimer.h"

unsigned long profileTimerValue = 0;
int averageIndex = 0;
double profileTimerValues[AVERAGE_VALUES] = {0};

void startProfileTimer() {
    // Stop the timer, reset, reset the status register, then start
    IOWR_16DIRECT(PROFILETIMER_BASE, TIMER_CONTROL_REGISTER, BIT_STOP);
    IOWR_16DIRECT(PROFILETIMER_BASE, TIMER_PERIOD_REGISTER_L, 0xFFFF);
    IOWR_16DIRECT(PROFILETIMER_BASE, TIMER_PERIOD_REGISTER_H, 0xFFFF);
    IOWR_16DIRECT(PROFILETIMER_BASE, TIMER_CONTROL_REGISTER, BIT_START);
}

void stopProfileTimer() {
    IOWR_16DIRECT(PROFILETIMER_BASE, TIMER_SNAPSHOT_REGISTER_L,
                  0);  // Request a read
    profileTimerValue = 0xFFFFFFFF -
                        ((unsigned long)IORD_16DIRECT(PROFILETIMER_BASE,
                                                      TIMER_SNAPSHOT_REGISTER_H)
                         << 16) +
                        ((unsigned long)IORD_16DIRECT(
                            PROFILETIMER_BASE, TIMER_SNAPSHOT_REGISTER_L));
    IOWR_16DIRECT(PROFILETIMER_BASE, TIMER_CONTROL_REGISTER, BIT_STOP);
}

double profileTimerms() { return (double)profileTimerValue * CLOCK_PERIOD_MS; }

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