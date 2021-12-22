/*
 * grayscale.c
 *
 *  Created on: Aug 21, 2015
 *      Author: theo
 */

#include <io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <system.h>

#include "optimisationsettings.h"

unsigned char *grayscale_array;
int grayscale_width = 0;
int grayscape_height = 0;

void conv_grayscale(void *picture, int width, int height,
                    bool use_custom_instructions) {
    int x, y;
    unsigned short gray;
    unsigned short *pixels = (unsigned short *)picture, rgb;
    unsigned short R, G, B;
    grayscale_width = width;
    grayscape_height = height;
    if (grayscale_array != NULL) free(grayscale_array);
    grayscale_array = (unsigned char *)malloc(width * height);

#if GRAYSCALE_VERSION == 0
    // Version de base
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            rgb = pixels[y * width + x];
            gray = (((rgb >> 11) & 0x1F) << 3) * 21;  // red part
            gray += (((rgb >> 5) & 0x3F) << 2) * 72;  // green part
            gray += (((rgb >> 0) & 0x1F) << 3) * 7;   // blue part
            gray /= 100;
            IOWR_8DIRECT(grayscale_array, y * width + x, gray);
        }
    }
#elif GRAYSCALE_VERSION == 1
    // Version slides
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            rgb = pixels[y * width + x];
            gray = ((rgb >> 8) & 0xF8) * 77;    // red
            gray += ((rgb >> 3) & 0xFC) * 151;  // green
            gray += (rgb << 3 & 0xF8) * 28;     // blue
            gray = gray >> 8;
            IOWR_8DIRECT(grayscale_array, y * width + x, gray);
        }
    }
#elif GRAYSCALE_VERSION == 2
    // Version semi-optimisée
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            rgb = pixels[y * width + x];
            gray = ((rgb >> 8) & 0xF8) * 77;    // red
            gray += ((rgb >> 3) & 0xFC) * 151;  // green
            gray += (rgb & 0x1F) * 224;         // blue
            gray = gray >> 8;
            IOWR_8DIRECT(grayscale_array, y * width + x, gray);
        }
    }
#elif GRAYSCALE_VERSION == 3
    // Version optimisée
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            rgb = pixels[y * width + x];
            gray = (rgb >> 2) & 0x3E00;   // red
            gray += (rgb << 4) & 0x7E00;  // green
            gray += (rgb << 8) & 0x1F00;  // blue
            gray = gray >> 8;
            IOWR_8DIRECT(grayscale_array, y * width + x, gray);
        }
    }
#elif GRAYSCALE_VERSION == 4
    // Custom instructions
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            gray = ALT_CI_GRAYSCALE_0(pixels[y * width + x], 0);
            IOWR_8DIRECT(grayscale_array, y * width + x, gray);
        }
    }
#elif GRAYSCALE_VERSION == 5
    // single for loop with or without custom instructions (depends on parameter)
    int z;
    if (use_custom_instructions) {
        for (z = 0; z < height * width; z++) {
            gray = ALT_CI_GRAYSCALE_0(pixels[z], 0);
            IOWR_8DIRECT(grayscale_array, z, gray);
        }
    } else {
        for (z = 0; z < height * width; z++) {
            rgb = pixels[z];
            gray = (rgb >> 2) & 0x3E00;   // red
            gray += (rgb << 4) & 0x7E00;  // green
            gray += (rgb << 8) & 0x1F00;  // blue
            gray = gray >> 8;
            IOWR_8DIRECT(grayscale_array, z, gray);
        }
    }
#endif
}

int get_grayscale_width() { return grayscale_width; }

int get_grayscale_height() { return grayscape_height; }

unsigned char *get_grayscale_picture() { return grayscale_array; }
