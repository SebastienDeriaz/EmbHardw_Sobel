/*
 * sobel.c
 *
 *  Created on: Sep 12, 2015
 *      Author: theo
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <system.h>

#include "io.h"
#include "optimisationsettings.h"

const char gx_array[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
const char gy_array[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

short *sobel_x_result;
short *sobel_y_result;
unsigned short *sobel_rgb565;
unsigned char *sobel_result;
int sobel_width;
int sobel_height;

void init_sobel_arrays(int width, int height) {
    int loop;
    sobel_width = width;
    sobel_height = height;
    if (sobel_x_result != NULL) free(sobel_x_result);
    sobel_x_result = (short *)malloc(width * height * sizeof(short));
    if (sobel_y_result != NULL) free(sobel_y_result);
    sobel_y_result = (short *)malloc(width * height * sizeof(short));
    if (sobel_result != NULL) free(sobel_result);
    sobel_result =
        (unsigned char *)malloc(width * height * sizeof(unsigned char));
    if (sobel_rgb565 != NULL) free(sobel_rgb565);
    sobel_rgb565 =
        (unsigned short *)malloc(width * height * sizeof(unsigned short));
    for (loop = 0; loop < width * height; loop++) {
        sobel_x_result[loop] = 0;
        sobel_y_result[loop] = 0;
        sobel_result[loop] = 0;
        sobel_rgb565[loop] = 0;
    }
}

short sobel_mac(unsigned char *pixels, int x, int y, const char *filter,
                unsigned int width) {
    short dy, dx;
    short result = 0;
#if LOOP_UNROLLING_VERSION == 0
    for (dy = -1; dy < 2; dy++) {
        for (dx = -1; dx < 2; dx++) {
            result += filter[(dy + 1) * 3 + (dx + 1)] *
                      pixels[(y + dy) * width + (x + dx)];
        }
    }
#elif LOOP_UNROLLING_VERSION == 1
    for (dy = -1; dy < 2; dy++) {
        result += filter[(dy + 1) * 3] * pixels[(y + dy) * width + (x - 1)];
        result += filter[(dy + 1) * 3 + 1] * pixels[(y + dy) * width + (x)];
        result += filter[(dy + 1) * 3 + 2] * pixels[(y + dy) * width + (x + 1)];
    }
#elif LOOP_UNROLLING_VERSION == 2
    result += filter[0] * pixels[(y - 1) * width + (x - 1)];
    result += filter[1] * pixels[(y - 1) * width + x];
    result += filter[2] * pixels[(y - 1) * width + (x + 1)];
    result += filter[3] * pixels[y * width + (x - 1)];
    result += filter[4] * pixels[y * width + (x)];
    result += filter[5] * pixels[y * width + (x + 1)];
    result += filter[6] * pixels[(y + 1) * width + (x - 1)];
    result += filter[7] * pixels[(y + 1) * width + (x)];
    result += filter[8] * pixels[(y + 1) * width + (x + 1)];
#endif
    return result;
}

#if INLINING < 2
void sobel_x(unsigned char *source) {
    int x, y;

    for (y = 1; y < (sobel_height - 1); y++) {
        for (x = 1; x < (sobel_width - 1); x++) {
#if INLINING == 0
            sobel_x_result[y * sobel_width + x] =
                sobel_mac(source, x, y, gx_array, sobel_width);
#elif INLINING == 1
            short *result = &sobel_x_result[y * sobel_width + x];
            *result = 0;
            *result += gx_array[0][0] * source[(y - 1) * sobel_width + (x - 1)];
            *result += gx_array[0][1] * source[(y - 1) * sobel_width + x];
            *result += gx_array[0][2] * source[(y - 1) * sobel_width + (x + 1)];
            *result += gx_array[1][0] * source[y * sobel_width + (x - 1)];
            *result += gx_array[1][1] * source[y * sobel_width + (x)];
            *result += gx_array[1][2] * source[y * sobel_width + (x + 1)];
            *result += gx_array[2][0] * source[(y + 1) * sobel_width + (x - 1)];
            *result += gx_array[2][1] * source[(y + 1) * sobel_width + (x)];
            *result += gx_array[2][2] * source[(y + 1) * sobel_width + (x + 1)];
#endif
        }
    }
}
#endif
void sobel_x_with_rgb(unsigned char *source) {
    int x, y;
    short result;

    for (y = 1; y < (sobel_height - 1); y++) {
        for (x = 1; x < (sobel_width - 1); x++) {
            result = sobel_mac(source, x, y, gx_array, sobel_width);
            sobel_x_result[y * sobel_width + x] = result;
            if (result < 0) {
                sobel_rgb565[y * sobel_width + x] = ((-result) >> 2) << 5;
            } else {
                sobel_rgb565[y * sobel_width + x] = ((result >> 3) & 0x1F)
                                                    << 11;
            }
        }
    }
}
#if INLINING < 2
void sobel_y(unsigned char *source) {
    int x, y;

    for (y = 1; y < (sobel_height - 1); y++) {
        for (x = 1; x < (sobel_width - 1); x++) {
#if INLINING == 0
            sobel_y_result[y * sobel_width + x] =
                sobel_mac(source, x, y, gy_array, sobel_width);
#elif INLINING == 1
            short *result = &sobel_y_result[y * sobel_width + x];
            *result = 0;
            *result += gy_array[0][0] * source[(y - 1) * sobel_width + (x - 1)];
            *result += gy_array[0][1] * source[(y - 1) * sobel_width + x];
            *result += gy_array[0][2] * source[(y - 1) * sobel_width + (x + 1)];
            *result += gy_array[1][0] * source[y * sobel_width + (x - 1)];
            *result += gy_array[1][1] * source[y * sobel_width + (x)];
            *result += gy_array[1][2] * source[y * sobel_width + (x + 1)];
            *result += gy_array[2][0] * source[(y + 1) * sobel_width + (x - 1)];
            *result += gy_array[2][1] * source[(y + 1) * sobel_width + (x)];
            *result += gy_array[2][2] * source[(y + 1) * sobel_width + (x + 1)];
#endif
        }
    }
}
#else
void sobel_complete(unsigned char *src, short threshold) {
    int x, y, z;
    short result_x, result_y;
    short sum, value;
#if INLINING == 2
    for (y = 1; y < (sobel_height - 1); y++) {
        for (x = 1; x < (sobel_width - 1); x++) {
            result_x = 0;
            result_y = 0;
            // x
            result_x += gx_array[0][0] * src[(y - 1) * sobel_width + (x - 1)];
            result_x += gx_array[0][1] * src[(y - 1) * sobel_width + x];
            result_x += gx_array[0][2] * src[(y - 1) * sobel_width + (x + 1)];
            result_x += gx_array[1][0] * src[y * sobel_width + (x - 1)];
            result_x += gx_array[1][1] * src[y * sobel_width + (x)];
            result_x += gx_array[1][2] * src[y * sobel_width + (x + 1)];
            result_x += gx_array[2][0] * src[(y + 1) * sobel_width + (x - 1)];
            result_x += gx_array[2][1] * src[(y + 1) * sobel_width + (x)];
            result_x += gx_array[2][2] * src[(y + 1) * sobel_width + (x + 1)];
            // y
            result_y = 0;
            result_y += gy_array[0][0] * src[(y - 1) * sobel_width + (x - 1)];
            result_y += gy_array[0][1] * src[(y - 1) * sobel_width + x];
            result_y += gy_array[0][2] * src[(y - 1) * sobel_width + (x + 1)];
            result_y += gy_array[1][0] * src[y * sobel_width + (x - 1)];
            result_y += gy_array[1][1] * src[y * sobel_width + (x)];
            result_y += gy_array[1][2] * src[y * sobel_width + (x + 1)];
            result_y += gy_array[2][0] * src[(y + 1) * sobel_width + (x - 1)];
            result_y += gy_array[2][1] * src[(y + 1) * sobel_width + (x)];
            result_y += gy_array[2][2] * src[(y + 1) * sobel_width + (x + 1)];

            arrayindex = (y * sobel_width) + x;
            value = result_x;
            sum = (value < 0) ? -value : value;
            value = result_y;
            sum += (value < 0) ? -value : value;
            sobel_result[arrayindex] = (sum > threshold) ? 0xFF : 0;
        }
    }
#else
    for (z = 0; z < (sobel_height - 2) * (sobel_width - 2); z++) {
        result_x = 0;
        result_y = 0;
#if INLINING == 3
        // x
        result_x += gx_array[0][0] * src[z];
        result_x += gx_array[0][1] * src[z + 1];
        result_x += gx_array[0][2] * src[z + 2];
        result_x += gx_array[1][0] * src[z + sobel_width];
        result_x += gx_array[1][1] * src[z + sobel_width + 1];
        result_x += gx_array[1][2] * src[z + sobel_width + 2];
        result_x += gx_array[2][0] * src[z + 2 * sobel_width];
        result_x += gx_array[2][1] * src[z + 2 * sobel_width + 1];
        result_x += gx_array[2][2] * src[z + 2 * sobel_width + 2];
        // y
        result_y += gy_array[0][0] * src[z];
        result_y += gy_array[0][1] * src[z + 1];
        result_y += gy_array[0][2] * src[z + 2];
        result_y += gy_array[1][0] * src[z + sobel_width];
        result_y += gy_array[1][1] * src[z + sobel_width + 1];
        result_y += gy_array[1][2] * src[z + sobel_width + 2];
        result_y += gy_array[2][0] * src[z + 2 * sobel_width];
        result_y += gy_array[2][1] * src[z + 2 * sobel_width + 1];
        result_y += gy_array[2][2] * src[z + 2 * sobel_width + 2];
#elif INLINING == 4
        // x
        result_x += gx_array[0][0] * src[z];
        // result_x += gx_array[0][1] * src[z + 1];
        result_x += gx_array[0][2] * src[z + 2];
        result_x += gx_array[1][0] * src[z + sobel_width];
        // result_x += gx_array[1][1] * src[z + sobel_width + 1];
        result_x += gx_array[1][2] * src[z + sobel_width + 2];
        result_x += gx_array[2][0] * src[z + 2 * sobel_width];
        // result_x += gx_array[2][1] * src[z + 2 * sobel_width + 1];
        result_x += gx_array[2][2] * src[z + 2 * sobel_width + 2];
        // y
        result_y += gy_array[0][0] * src[z];
        result_y += gy_array[0][1] * src[z + 1];
        result_y += gy_array[0][2] * src[z + 2];
        // result_y += gy_array[1][0] * src[z + sobel_width];
        // result_y += gy_array[1][1] * src[z + sobel_width + 1];
        // result_y += gy_array[1][2] * src[z + sobel_width + 2];
        result_y += gy_array[2][0] * src[z + 2 * sobel_width];
        result_y += gy_array[2][1] * src[z + 2 * sobel_width + 1];
        result_y += gy_array[2][2] * src[z + 2 * sobel_width + 2];
#elif INLINING == 5 && SOBEL_CUSTOM == 0
        // x
        const char gx_array[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
        const char gy_array[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

        unsigned char *offset = &src[z];
        unsigned int sobel_width_2 = 2 * sobel_width;
        // result_x += gx_array[0][0] * offset[0];
        result_x -= offset[0];
        // result_x += gx_array[0][2] * offset[2];
        result_x += offset[2];
        // result_x += gx_array[1][0] * offset[sobel_width];
        result_x -= (unsigned short)(offset[sobel_width]) << 1;
        // result_x += gx_array[1][2] * offset[sobel_width + 2];
        result_x += (unsigned short)(offset[sobel_width + 2]) << 1;
        // result_x += gx_array[2][0] * offset[2 * sobel_width];
        result_x -= offset[sobel_width_2];
        // result_x += gx_array[2][2] * offset[2 * sobel_width + 2];
        result_x += offset[sobel_width_2 + 2];
        // y
        // result_y += gy_array[0][0] * offset[0];
        result_y += offset[0];
        // result_y += gy_array[0][1] * offset[1];
        result_y += (unsigned short)(offset[1]) << 1;
        // result_y += gy_array[0][2] * offset[2];
        result_y += offset[2];
        // result_y += gy_array[2][0] * offset[2 * sobel_width];
        result_y -= offset[sobel_width_2];
        // result_y += gy_array[2][1] * offset[2 * sobel_width + 1];
        result_y -= (unsigned short)(offset[sobel_width_2 + 1]) << 1;
        // result_y += gy_array[2][2] * offset[2 * sobel_width + 2];
        result_y -= offset[sobel_width_2 + 2];
#elif SOBEL_CUSTOM == 1
        unsigned char *offset = &src[z];
        unsigned int sobel_width_2 = 2 * sobel_width;
        uint32_t dataa, datab;
        // offset[0], offset[1], offset[2], offset[sobel_width],
        // offset[sobel_width+2], offset[sobel_width_2], offset[sobel_width_2 +
        // 1], offset[sobel_width_2+2]
        //
        // dataa -> offset[0], offset[1], offset[2], offset[sobel_width]
        // dataa -> offset[sobel_width+2], offset[sobel_width_2],
        // offset[sobel_width_2 + 1], offset[sobel_width_2+2]

        dataa = (uint32_t)(offset[0] << 24) | (uint32_t)(offset[1] << 16) |
                (uint32_t)(offset[2] << 8) | (uint32_t)(offset[sobel_width]);
        datab = offset[sobel_width + 2] << 24 | offset[sobel_width_2] << 16 |
                offset[sobel_width_2 + 1] << 8 | offset[sobel_width_2 + 2];
        // dataa = (*((uint32_t *)offset) & 0xFFFFFF00);// |
        // offset[sobel_width]; datab = (offset[sobel_width + 2] << 24) |
        //        (*((uint32_t *)&offset[sobel_width_2]) >> 8);
        

        //printf("dataa : %08X\n", dataa);
        //printf("datab : %08X\n", datab);

        sobel_result[z] = ALT_CI_SOBEL_0(dataa, datab);
#endif

#if SOBEL_CUSTOM == 0
        value = result_x;
        sum = (value < 0) ? -value : value;
        value = result_y;
        sum += (value < 0) ? -value : value;
        sobel_result[z] = (sum > threshold) ? 0xFF : 0;
#endif
    }
#endif
}

#endif

void sobel_y_with_rgb(unsigned char *source) {
    int x, y;
    short result;

    for (y = 1; y < (sobel_height - 1); y++) {
        for (x = 1; x < (sobel_width - 1); x++) {
            result = sobel_mac(source, x, y, gy_array, sobel_width);
            sobel_y_result[y * sobel_width + x] = result;
            if (result < 0) {
                sobel_rgb565[y * sobel_width + x] = ((-result) >> 2) << 5;
            } else {
                sobel_rgb565[y * sobel_width + x] = ((result >> 3) & 0x1F)
                                                    << 11;
            }
        }
    }
}

#if INLINING < 2
void sobel_threshold(short threshold) {
    int x, y, arrayindex;
    short sum, value;
    for (y = 1; y < (sobel_height - 1); y++) {
        for (x = 1; x < (sobel_width - 1); x++) {
            arrayindex = (y * sobel_width) + x;
            value = sobel_x_result[arrayindex];
            sum = (value < 0) ? -value : value;
            value = sobel_y_result[arrayindex];
            sum += (value < 0) ? -value : value;
            sobel_result[arrayindex] = (sum > threshold) ? 0xFF : 0;
        }
    }
}
#endif

unsigned short *GetSobel_rgb() { return sobel_rgb565; }

unsigned char *GetSobelResult() { return sobel_result; }
