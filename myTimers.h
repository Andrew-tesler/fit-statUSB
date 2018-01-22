/*
 * myTimers.h
 *
 */

#include "defines.c"

#ifndef MYTIMERS_H_
#define MYTIMERS_H_

uint32_t fadeTimer;
// TODO add colorSeq and colorFadeDiff initialization to 0
signed int colorFadeDiff[MAX_SEQ_COLORS][3];                                                // Array to store color Difference data
uint8_t colorSeq[MAX_SEQ_COLORS][3];                                                        // 2D array to store the color sequence numbers
double fadeTimeMs[MAX_SEQ_COLORS][3];                                                               // QTY in MS needed to pass for the color to change


// Prototypes
void initTimers(int,int,int);
void initfadeClock(void);
void updateFadeTime(uint32_t);
void initFade(uint8_t);

#endif /* MYTIMERS_H_ */

