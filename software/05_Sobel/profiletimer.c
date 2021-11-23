#include "profiletimer.h"

int averageIndex[TIMERS_COUNT] = {0};
alt_u32 profileTimerValues[TIMERS_COUNT][AVERAGE_VALUES] = {0};
alt_u32 profileTimerStartValue[TIMERS_COUNT] = {0};

void startProfileTimer(int index) {
    if(index == 0) {
        alt_timestamp_start();
    }
    // Stop the timer, reset, reset the status register, then start
    profileTimerStartValue[index] = alt_timestamp();
}

void stopProfileTimer(int index) {
    alt_u32 stop = alt_timestamp();
    alt_u32 delta = stop - profileTimerStartValue[index];
    profileTimerValues[index][averageIndex[index]++] = delta;
    if (averageIndex[index] >= AVERAGE_VALUES) {
        averageIndex[index] = 0;
    }
}

double profileTimerms(int index) {
    double msValue = 0;
    bool ready = true;
    for (int i = 0; i < AVERAGE_VALUES; i++) {
        if (profileTimerValues[index][i] == 0) ready = false;
        msValue += (double)profileTimerValues[index][i];
    }
    if (ready)
        return msValue / AVERAGE_VALUES * CLOCK_PERIOD_MS;
    else
        return 0.0;
}

void printProfileTimers() {
    double value = 0;

    char* text = "";
    for (int index = 0; index < TIMERS_COUNT; index++) {
        value = profileTimerms(index);
        if (value == 0.0) {
            printf("Building average...\n");
        } else {
            switch (index) {
                case TIMER_LOOP:
                    text = "Loop time : ";
                    break;
                case TIMER_CONV_GRAYSCALE:
                    text = "conv_grayscale : ";
                    break;
                case TIMER_SOBEL_X:
                    text = "sobel_x : ";
                    break;
                case TIMER_SOBEL_Y:
                    text = "sobel_y : ";
                    break;
                case TIMER_SOBEL_THRESHOLD:
                    text = "sobel_threshold : ";
                    break;
            }
            printf("%s%3.3f ms\n", text, value);
        }
    }
}