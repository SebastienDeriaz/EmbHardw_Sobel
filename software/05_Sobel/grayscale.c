/*
 * grayscale.c
 *
 *  Created on: Aug 21, 2015
 *      Author: theo
 */

#include <io.h>
#include <stdlib.h>
#include <system.h>

#include "optimisationsettings.h"

unsigned char *grayscale_array;
int grayscale_width = 0;
int grayscape_height = 0;

void conv_grayscale(void *picture, int width, int height) {
    int x, y;
    unsigned short gray;
    unsigned short *pixels = (unsigned short *)picture, rgb;
    grayscale_width = width;
    grayscape_height = height;
    if (grayscale_array != NULL) free(grayscale_array);
    grayscale_array = (unsigned char *)malloc(width * height);
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            rgb = pixels[y * width + x];
            
#if GRAYSCALE_VERSION == 0
            gray = (((rgb >> 11) & 0x1F) << 3) * 21;  // red part
            gray += (((rgb >> 5) & 0x3F) << 2) * 72;  // green part
            gray += (((rgb >> 0) & 0x1F) << 3) * 7;   // blue part
            gray /= 100;
#elif GRAYSCALE_VERSION == 1
            gray = ((rgb >> 8) & 0xF8) * 77;    // red
            gray += ((rgb >> 3) & 0xFC) * 151;  // green
            gray += (rgb << 3 & 0xF8) * 28;     // blue
            gray = gray >> 8;
#elif GRAYSCALE_VERSION == 2
            gray = ((rgb >> 8) & 0xF8) * 77;    // red
            gray += ((rgb >> 3) & 0xFC) * 151;  // green
            gray += (rgb & 0x1F) * 224;         // blue
            gray = gray >> 8;

#elif GRAYSCALE_VERSION == 3
            gray = (rgb >> 2) & 0x3E00;
            gray += (rgb << 4) & 0x7E00;  // green
            gray += (rgb << 8) & 0x1F00;  // blue
            gray = gray >> 8;
#else

#endif
            IOWR_8DIRECT(grayscale_array, y * width + x, gray);
        }
    }
}

int get_grayscale_width() { return grayscale_width; }

int get_grayscale_height() { return grayscape_height; }

unsigned char *get_grayscale_picture() { return grayscale_array; }
