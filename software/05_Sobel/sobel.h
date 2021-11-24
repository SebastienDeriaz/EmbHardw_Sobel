/*
 * sobel.h
 *
 *  Created on: Sep 12, 2015
 *      Author: theo
 */

#ifndef SOBEL_H_
#define SOBEL_H_
#include "optimisationsettings.h"


void init_sobel_arrays(int width , int height);
void sobel_x_with_rgb( unsigned char *source );
#if INLINING >= 2
void sobel_complete(unsigned char* src, short threshold);
#else
void sobel_x( unsigned char *source );
void sobel_y( unsigned char *source );
#endif
void sobel_y_with_rgb( unsigned char *source );
void sobel_threshold(short threshold);
unsigned short *GetSobel_rgb();
unsigned char *GetSobelResult();



#endif /* SOBEL_H_ */
