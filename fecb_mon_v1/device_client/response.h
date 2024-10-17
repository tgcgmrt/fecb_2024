#ifndef RESPONSE_H_INCLUDED
#define RESPONSE_H_INCLUDED
#include<math.h>
#define IDLE_MODE 1
#define SCAN_MODE 2
#define MEAN_MODE 3
#define LIMITS_MODE 4

int noise_cal;
char tbuf[64],abuf[64];

int Monparam[12];


float Monvoltbis[22];
unsigned char tmpbuf[200], stmpbuf[256];
void copyTmpBuf(char *, char *, int);
 void copyTmpBuffe(char *, char *, int);
 void copyTmpBufcb(char *, char *, int);
 double convertdB (float , int );
 void decodeFEBox(int);
 void decodeCommonBox( );

unsigned long t0, sec_tm;

#endif
