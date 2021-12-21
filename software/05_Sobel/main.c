#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <system.h>

#include "camera.h"
#include "dipswitch.h"
#include "grayscale.h"
#include "i2c.h"
#include "lcd_simple.h"
#include "profiletimer.h"
#include "sobel.h"
#include "vga.h"

int main() {
    void *buffer1, *buffer2, *buffer3, *buffer4;
    unsigned short *image;
    unsigned char *grayscale;
    unsigned char current_mode;
    unsigned char mode;
    init_LCD();
    init_camera();
    vga_set_swap(VGA_QuarterScreen | VGA_Grayscale);
    printf("Hello from Nios II!\n");
    cam_get_profiling();
    buffer1 = (void *)malloc(cam_get_xsize() * cam_get_ysize());
    buffer2 = (void *)malloc(cam_get_xsize() * cam_get_ysize());
    buffer3 = (void *)malloc(cam_get_xsize() * cam_get_ysize());
    buffer4 = (void *)malloc(cam_get_xsize() * cam_get_ysize());
    cam_set_image_pointer(0, buffer1);
    cam_set_image_pointer(1, buffer2);
    cam_set_image_pointer(2, buffer3);
    cam_set_image_pointer(3, buffer4);
    enable_continues_mode();
    init_sobel_arrays(cam_get_xsize() >> 1, cam_get_ysize());
    do {
        if (new_image_available() != 0) {
            if (current_image_valid() != 0) {
                startProfileTimer(TIMER_LOOP);
                current_mode = DIPSW_get_value();
                mode = current_mode &
                       (DIPSW_SW1_MASK | DIPSW_SW3_MASK | DIPSW_SW2_MASK);
                image = (unsigned short *)current_image_pointer();
                switch (mode) {
                    case 0:
                        transfer_LCD_with_dma(&image[16520],
                                              cam_get_xsize() >> 1,
                                              cam_get_ysize(), 0);
                        if ((current_mode & DIPSW_SW8_MASK) != 0) {
                            vga_set_swap(VGA_QuarterScreen);
                            vga_set_pointer(image);
                        }
                        break;
                    case 1:
                        startProfileTimer(TIMER_CONV_GRAYSCALE);
                        conv_grayscale((void *)image, cam_get_xsize() >> 1,
                                       cam_get_ysize(), false);
                        stopProfileTimer(TIMER_CONV_GRAYSCALE);
                        grayscale = get_grayscale_picture();
                        transfer_LCD_with_dma(&grayscale[16520],
                                              cam_get_xsize() >> 1,
                                              cam_get_ysize(), 1);
                        if ((current_mode & DIPSW_SW8_MASK) != 0) {
                            vga_set_swap(VGA_QuarterScreen | VGA_Grayscale);
                            vga_set_pointer(grayscale);
                        }
                        break;
                    case 2:
                        startProfileTimer(TIMER_CONV_GRAYSCALE);
                        conv_grayscale((void *)image, cam_get_xsize() >> 1,
                                       cam_get_ysize(), false);
                        stopProfileTimer(TIMER_CONV_GRAYSCALE);
                        grayscale = get_grayscale_picture();
                        startProfileTimer(TIMER_SOBEL_X);
                        sobel_x_with_rgb(grayscale);
                        stopProfileTimer(TIMER_SOBEL_X);
                        image = GetSobel_rgb();
                        transfer_LCD_with_dma(&image[16520],
                                              cam_get_xsize() >> 1,
                                              cam_get_ysize(), 0);
                        if ((current_mode & DIPSW_SW8_MASK) != 0) {
                            vga_set_swap(VGA_QuarterScreen);
                            vga_set_pointer(image);
                        }
                        break;
                    case 3:
                        startProfileTimer(TIMER_CONV_GRAYSCALE);
                        conv_grayscale((void *)image, cam_get_xsize() >> 1,
                                       cam_get_ysize(), false);
                        stopProfileTimer(TIMER_CONV_GRAYSCALE);
                        grayscale = get_grayscale_picture();
                        startProfileTimer(TIMER_SOBEL_X);
#if INLINING < 2
                        sobel_x(grayscale);
#endif
                        stopProfileTimer(TIMER_SOBEL_X);
                        startProfileTimer(TIMER_SOBEL_Y);
                        sobel_y_with_rgb(grayscale);
                        stopProfileTimer(TIMER_SOBEL_Y);
                        image = GetSobel_rgb();
                        transfer_LCD_with_dma(&image[16520],
                                              cam_get_xsize() >> 1,
                                              cam_get_ysize(), 0);
                        if ((current_mode & DIPSW_SW8_MASK) != 0) {
                            vga_set_swap(VGA_QuarterScreen);
                            vga_set_pointer(image);
                        }
                        break;
                    case 4:
                    case 5:
                        startProfileTimer(TIMER_CONV_GRAYSCALE);
                        conv_grayscale((void *)image, cam_get_xsize() >> 1,
                                       cam_get_ysize(), mode == 5);
                        stopProfileTimer(TIMER_CONV_GRAYSCALE);

                        grayscale = get_grayscale_picture();

#if INLINING < 2
                        startProfileTimer(TIMER_SOBEL_X);
                        sobel_x(grayscale);
                        stopProfileTimer(TIMER_SOBEL_X);

                        startProfileTimer(TIMER_SOBEL_Y);
                        sobel_y(grayscale);
                        stopProfileTimer(TIMER_SOBEL_Y);

                        startProfileTimer(TIMER_SOBEL_THRESHOLD);
                        sobel_threshold(128);
                        stopProfileTimer(TIMER_SOBEL_THRESHOLD);
#else
                        startProfileTimer(TIMER_SOBEL_X);
                        sobel_complete(grayscale, 128, mode == 5);
                        stopProfileTimer(TIMER_SOBEL_X);
#endif

                        grayscale = GetSobelResult();
                        transfer_LCD_with_dma(&grayscale[16520],
                                              cam_get_xsize() >> 1,
                                              cam_get_ysize(), 1);
                        if ((current_mode & DIPSW_SW8_MASK) != 0) {
                            vga_set_swap(VGA_QuarterScreen | VGA_Grayscale);
                            vga_set_pointer(grayscale);
                        }
                        break;
                }
                printf("mode : %d\n", mode);
                stopProfileTimer(TIMER_LOOP);
                printProfileTimers();
            }
        }
    } while (1);
    return 0;
}
