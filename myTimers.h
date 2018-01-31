/*
 * myTimers.h
 *
 */

#include "defines.c"

#ifndef MYTIMERS_H_
#define MYTIMERS_H_

float fadeTimer;

uint32_t colortick;

// TODO add colorSeq and colorFadeDiff initialization to 0
//signed  colorFadeDiff[MAX_SEQ_COLORS][3];                                                 // Array to store color Difference data
int colorSeq[MAX_SEQ_COLORS][3];                                                            // 2D array to store the color sequence numbers
//double fadeTimeMs[MAX_SEQ_COLORS][3];                                                     // QTY in MS needed to pass for the color to change TODO check with this or long for better resolution
//signed int fadeTimeMs[MAX_SEQ_COLORS][3];                                                 // QTY in MS needed to pass for the color to change


float currentRGBColor[3];                                                                   // Current Color
//double colorDiff[3];
float colorFadeDiff[MAX_SEQ_COLORS][3];
float colorFadeTimer[MAX_SEQ_COLORS+1];                                                     // Faded timer of each fade
uint32_t colorLocation;                                                                     // Color location in the fade difference array need 1 extra element for the last -> first transition
uint8_t disableDirection;
int direction;                                                                              // Direction of the Loop 0 - Forward, 1 - Backward
// Prototypes
void initTimers(int,int,int);
void initfadeClock(void);
void updateFadeTime(uint32_t);
void initFade(uint8_t);

#endif /* MYTIMERS_H_ */

