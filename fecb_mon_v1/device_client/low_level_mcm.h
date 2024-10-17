#ifndef LOW_LEVEL_MCM_H_INCLUDED
#define LOW_LEVEL_MCM_H_INCLUDED

#include"define.h"

void add_cksum(unsigned char buff[]);


void set_dig_mask64();
void set_dig_mask32();
void set_dig_mask16();
void set_anl_mask();
void set_mean_mode();
void feed_cntrl_old();
void feed_cntrl_new();
void fe_box_mon();
void FECB_CTRL(int MCMADDR );

#endif
