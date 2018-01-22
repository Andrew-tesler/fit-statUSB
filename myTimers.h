/*
 * myTimers.h
 *
 */

#include "defines.c"

#ifndef MYTIMERS_H_
#define MYTIMERS_H_

uint32_t fadeTimer;
// TODO add colorSeq and colorFadeDiff initialization to 0
//signed  colorFadeDiff[MAX_SEQ_COLORS][3];                                                // Array to store color Difference data
int colorSeq[MAX_SEQ_COLORS][3];                                                        // 2D array to store the color sequence numbers
//double fadeTimeMs[MAX_SEQ_COLORS][3];                                                     // QTY in MS needed to pass for the color to change TODO check with this or long for better resolution
//signed int fadeTimeMs[MAX_SEQ_COLORS][3];                                                 // QTY in MS needed to pass for the color to change
double colorDiff[3];
signed int colorFadeDiff[3];

int currentRGBColor[3];                                                                     // Current Color



// Prototypes
void initTimers(int,int,int);
void initfadeClock(void);
void updateFadeTime(uint32_t);
void initFade(uint8_t);

#endif /* MYTIMERS_H_ */

