#include "define.h"
#include "fecb_mcm.h"

int MCMCARD1 = 5, MCMCARD2 = 4;

 // Initialize variables
double pow_db = 0.0;  // Initialize to a default value, e.g., 0.0

// Initialize arrays
char mask1[6] = {0};  // Initialize all elements to 0
char mask2[6] = {0};
char mask3[6] = {0};
char mask4[6] = {0};

// Initialize pointers and arrays
char *buf_ptr0 = 0;  // Initialize to nullptr
unsigned short bitD0[8] = {0};  // Initialize all elements to 0
unsigned short mcm4word[2] = {0};  // Initialize all elements to 0
unsigned short digWord[32] = {0};  // Initialize all elements to 
 fecbset fecbSet;
 execcmd exeCmd;
 serialBuff wbuff;
 serialBuff rbuff;

void Convert(int offset, char * mask_x)
{
  int j;
  unsigned long Argument0, Argument1;
  unsigned char tab1[3], tab2[3];
 
  for (j=0;j<2;j++) {tab1[j]=mask_x[j];tab2[j]=mask_x[j+2];}     //"ffff" mask format
  tab1[2] = tab2[2] = '\0';       //strtol() converts until '\n'

 #ifdef DEBUG
  printf(" mask_x[0] %c mask_x[2] %c mask_x[1] %c mask_x[3] %c \n",
           mask_x[0], mask_x[2], mask_x[1], mask_x[3]);

  printf(" tab1 %s tab2 %s \n", tab1, tab2);
 #endif

  exeCmd.Argument[0+offset] =(unsigned char) strtol((const char *)tab2, NULL, 16);     //3412 order   // Hex Conversion */
  exeCmd.Argument[1+offset] = (unsigned char) strtol((const char *)tab1, NULL, 16);

 
  #ifdef DEBUG
  printf("Argument0 = %x  Argument1 = %x \n", exeCmd.Argument[0+offset],exeCmd.Argument[1+offset]);
  #endif
}

void Mask32toArg(char * mask1, char * mask2)
{
int j;

  for(j=0;j<sizeof(exeCmd.Argument);j++)
  exeCmd.Argument[j]=0x00;

   Convert(0, mask1);
   Convert(2, mask2);
}

void Mask64toArg(char * mask1, char * mask2, char * mask3, char * mask4)
{
int j;

  for(j=0;j<sizeof(exeCmd.Argument);j++)
  exeCmd.Argument[j]=0x00;
  Convert(0, mask1); Convert(2, mask2);
  Convert(4, mask3); Convert(6, mask4);
}

void getbitDO(short word)
{
  int i, mask, t_bit;
  extern unsigned short bitD0[] ;

   for(i=7;i >= 0;)
   {
     mask = 1<<i;
     t_bit =  word & mask;            /* and operator */
     if( t_bit == 0 ) bitD0[i] = 0;
     else bitD0[i] = 1;
     i--;
   }
#ifdef DEBUG
   printf("BIT : %d  ",word);
   for(i=0;i<7;i++) {
     printf("%d",bitD0[i]);
   }
   printf("\n");
#endif
}

void mcm4setting()
{
     int i , j ,b ;
     for(i=0;i<32;i++) { digWord[i]=0; } 
          
      for (j=0; j<2; j++)
        { 
           
           // ----------New logic for MCM 4, SEP 20, 2017 ---------//
           // NG Cycle bit D0,1
         getbitDO(fecbSet.ngCycle); for (b=0;b<2;b++) { digWord[b + (j*16)] = bitD0[b]; }

       // NG Freq bit D2,3
         getbitDO(fecbSet.ngFreq); for (b=0;b<2;b++) { digWord[ 2 + b + (j*16)] = bitD0[b]; }

          // FE SEL D4
            digWord[4 + (j * 16) ] = fecbSet.rfcmSW;

           // Walsh Pattern D5,6,7
              getbitDO(fecbSet.walshPat); for (b=0;b<3;b++) { digWord[ 5 + b + (j*16)] = bitD0[b]; }

            // Walsh Freq D8,9
             getbitDO(fecbSet.walshFreq); for (b=0;b<2;b++) { digWord[ 8 + b + (j*16)] = bitD0[b]; }

           // Spare 10,11
           digWord[10 + (j*16)] = digWord[11 + (j*16)] = 0;

           // Walsh Group B12,13,14 freezed to value-1?
           digWord[12 + (j*16)] = digWord[13 + (j*16)] = digWord[14 + (j*16)] = 1;

        // bit D15 - EN
          if(j == 1) digWord[15 + (j*16)] = 1;
            else digWord[15 + (j*16)] = 0 ;

        } // for j end

           #ifdef DEBUG
           printf(" INITIALIZING WORDS");
           #endif
          mcm4word[0] = 0 ;  mcm4word[1] = 0;

          for(i = 0 ; i < 16; i++)
          {
             if(digWord[i+16]) mcm4word[0] = mcm4word[1] | (1 << i);
             if(digWord[i]) mcm4word[1] = mcm4word[0] | (1 << i );
          }

        //  printf(" MCM4WORD1 ");
          for(i=0 ; i<16;i++)   {
             if( (mcm4word[1] & ( 1 << i )) ) { b = 1 ; }
             else b = 0 ;
             //printf( "%d", b);
          }
         // printf("\n");

         // printf(" MCM4WORD0 ");
          for(i= 0 ; i<16;i++)  {
             if( (mcm4word[0] & ( 1 << i )) ) { b = 1 ; }
             else b = 0 ;
             //printf( "%d", b);
          }
        //  printf("\n");

        //  printf("################# %04xX %04xX ======> mcm4word 0 and 1 \n", (mcm4word[0] & (~(1 << 4)) ), (mcm4word[1] & (~(1 <<4))) );

}


int  mcmPow(int mcmSW)
{
    int FEDAT_ON, FEDAT_OFF;
    int i,b;

    exeCmd.McmPow = mcmSW;
    printf("#################### Inside MCMPow execCMD is %d\n",exeCmd.McmPow);
    switch(mcmSW)
    {
       case 0 : // MCM OFF
               //printf("####### I am in MCMPOW OFF CASE\n");
               sprintf(mask1,"%04x",mcm4word[1] & (~(unsigned short)( 1 << 4)) );
               sprintf(mask2,"%04x",mcm4word[0] & (~(unsigned short)( 1 << 4)) );
 #ifdef DEBUG
               printf("MCMC OFF mask1 %s ", mask1);
               printf("MCM OFF  mask2: %s \n", mask2);
 #endif
               Mask32toArg(mask1, mask2);
               exeCmd.CmdNum = 7;
               FECB_CTRL(MCMCARD2);  
               usleep(10000);         //set dig mask 32 bits
               break;

       case 1 : 
               sprintf(mask1,"%04x",mcm4word[1] & (~(unsigned short)( 1 << 4) ) );
               sprintf(mask2,"%04x",mcm4word[0] & (~(unsigned short)( 1 << 4) ) );
#ifdef DEBUG
               printf("MCM OFF mask1: %s ", mask1);
               printf("MCM OFF mask2: %s \n", mask2);
#endif
               Mask32toArg(mask1, mask2);
               exeCmd.CmdNum = 7;
               FECB_CTRL(MCMCARD2);           //set dig mask 32 bits
                 usleep(10000); 
            
               sprintf(mask1,"%04x",mcm4word[1] | ( 1 << 4) );
               sprintf(mask2,"%04x",mcm4word[0] | ( 1 << 4) );
#ifdef DEBUG
               printf("MCM ON mask1: %s ", mask1);
               printf("MCM ON mask2: %s \n", mask2);
#endif
               Mask32toArg(mask1, mask2);
               exeCmd.CmdNum = 7;
               FECB_CTRL(MCMCARD2);   sleep(1); 
               usleep(10000); 
               //set dig mask 32 bits
               break;
   }

}





int fecbCtrlCmd(parseCMSCmd *Wrpr_Cmd)
{
   int add_fe[2], i , j ,b ;
   int data1[2], data2[2], data3[2];
   char mask[4][8];

   int freq_band[]= {50,  150, 235, 325, 610,  1060, 1170, 1280, 1390, 1420, -1};
   int p_data11[] = {0x0, 0x1, 0x8, 0xa, 0x10, 0x14, 0x14, 0x14, 0x14, 0x14};
   int p_data32[] = {0x0, 0x0, 0x0, 0x0, 0x0,  0x00, 0x40, 0x20, 0x60, 0x90};
   int p_add[]    = {0x1, 0x2, 0x3, 0x4, 0x5,  0x6,  0x6,  0x6,  0x6, 0x6};

   int freq_uband[]= {50, 150,  235, 290, 350,  410,  470,  325, 600,   685, 725,   770,  850, 1060, 1170, 1280, 1390, 1420 , -1};
   int p_udata11[] = {0x0, 0x1, 0x8, 0xa, 0xa,  0xa,  0xa,  0xa, 0x10, 0x10, 0x10, 0x10, 0x10, 0x14, 0x14, 0x14, 0x14, 0x14 };
   int p_udata32[] = {0x0, 0x0, 0x0,0x60, 0x20, 0x40, 0x0,  0x80, 0x60 ,0x20, 0x80, 0x40, 0x00, 0x00, 0x40, 0x20, 0x60, 0x90};
   int p_uadd []   = {0x1, 0x2, 0x3, 0x4, 0x4,  0x4,  0x4,  0x4,  0x5 ,  0x5, 0x5,  0x5,   0x5,  0x6,  0x6,  0x6,  0x6, 0x6};

   int sol_attr[] = {   0,   14,   30,   44,  -1};
   int p_data12[] = {0x80, 0xe0, 0x00, 0x60, 0xa0};
   int p_data21[] = {0x01, 0x01, 0x00, 0x00, 0x00};

   int ns_cal[]   = {   0,    1,    2,    3, -1};
   char *N_CAL[]   = {"E-HI","HI","MED","LOW","RF-OFF"};
   int p_data31[] = {0x06, 0x05, 0x0a, 0x09, 0x15};
 
   typedef enum { INIT=0, FNULL=1, SETRFSYS=2,FESDOMON=3, SEL_FEBOX=4, SEL_UFEBOX=5, RFSWAP=6, RFATTN=7, RFON=8, CBTERM=9, \
                  RFTERM=10, RFNGSET=11, RFNGCAL=12 ,WALSHPATERN=13, SETWALSHGRP=14, SETURFSYS=15, RFCM_SW=16, RAWMON=17, RESTORE=18, \
                  SETTIME=19, REBOOTMCM=20,WALSHFREQ=21,NOISEFREQ=22,SETRFNOISE=23,SETWALSH=24,TCMD=25 } FesEnum;

   #ifdef DEBUG
     printf("\n\nfecb -->   %d\n", fecbSet.cmdCode);
   #endif


   switch (fecbSet.cmdCode)
   {
        case FNULL :  mcm4setting();
                      mcmPow(1); sleep(1);
                      FECB_CTRL(MCMCARD2);                     
                      exeCmd.CmdNum = 1;
                      FECB_CTRL(MCMCARD1);
                      break ;

        case WALSHPATERN:  mcm4setting();
                           mcmPow(1);
                           break;
        case WALSHFREQ:    mcm4setting();
                           mcmPow(1);
                           break;

        case NOISEFREQ:    mcm4setting();
                           mcmPow(1);
                           break;

        case SETRFNOISE:   mcm4setting();
                           mcmPow(1);
                           break;

        case SETWALSHGRP : mcm4setting();   // Actual implementation is not here. In future we can add. 
                           mcmPow(1);
                           break;
        case SETWALSH : mcm4setting();   // Actual implementation is not here. In future we can add. 
                           mcmPow(1);
                           break;

        case CBTERM :    for (j=0; j<2; j++)
                                     {   
                                        for(i=0;( freq_uband[i] != (-1) && fecbSet.freq_set[j] != freq_uband[i]); i++);
                                         if( freq_uband[i] == - 1 )
                                        { return ((-1) * SETURFSYS); }
                                        data1[0] = p_udata11[i];
                                        data3[1] = p_udata32[i];
                                        add_fe[j] = p_uadd[i];
                                        exeCmd.feBoxNo[j] = (int) p_uadd[i]; 
                                      //  printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
                                         #ifdef DEBUG
                                          printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
                                           #endif

                                           //Control word for setting SOLAR ATTENUATOR in the common box
                                          for (i=0; sol_attr[i]!=-1 && fecbSet.solar_attn[j]!=sol_attr[i]; i++) {;}
                                          data1[1] = p_data12[i];  // p_data11 + p_data12
                                          data2[0] = p_data21[i];

                                          // If polarization swap
                                         if (fecbSet.polSwap != 0) 
                                         {data2[1] = 0x02; }
                                         else 
                                         {data2[1]=0x0;}     // p_data21 + p_data22 , p_data22 is not declared

                                          //  control word for setting NOISE CAL LEVEL in front end box.
                                          for (i=0; fecbSet.noiseCal != ns_cal[i]; i++) {;}
                                           data3[0] = p_data31[i];  // p_data32 +  p_data31

                                          if (fecbSet.freq_set[1] == 0) exeCmd.feBoxNo[++j] = 0;                  
                                      }

                                   mcm4setting();
                                   mcmPow(1);sleep(1);
                                   data1[0] = data1[0] + data1[1];
                                   data2[0] = data2[0] + data2[1];
                                   data3[0] = data3[0] + data3[1];
           
                               bzero(mask1,sizeof(mask1));bzero(mask2,sizeof(mask2));bzero(mask3,sizeof(mask3));bzero(mask4,sizeof(mask4));
             //  Frequency, Solar Attenuator, Nose cal and Swap control
                                  sprintf(mask1, "07%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                                  sprintf(mask3, "08%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                  //printf("07%02x ",data1[0]); printf("00%02x ",data1[0]); printf("08%02x ",data2[0]); printf("00%02x \n",data2[0]);
#ifdef DEBUG
                  printf("mask1: %s ", mask1);
                  printf("mask2: %s ", mask2);
                  printf("mask3: %s ", mask3);
                  printf("mask4: %s \n", mask4);
#endif
                  Mask64toArg(mask1, mask2, mask3, mask4);
                  exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

                 if (fecbSet.freq_set[1] == 0) j = 0;
                 else j = 1;
                 sprintf(mask1, "09%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                 sprintf(mask3, "0A%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                 //printf("09%02x ",data1[j]); printf("00%02x ",data1[j]); printf("0A%02x ",data2[j]); printf("00%02x \n",data2[j]);
#ifdef DEBUG
                 printf("mask1: %s ", mask1);
                 printf("mask2: %s ", mask2);
                 printf("mask3: %s ", mask3);
                 printf("mask4: %s \n", mask4);
#endif
                 Mask64toArg(mask1, mask2, mask3, mask4);
                 exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

                                break;
                 
        case RFATTN :   for (j=0; j<2; j++)
                                     {   
                                        for(i=0;( freq_uband[i] != (-1) && fecbSet.freq_set[j] != freq_uband[i]); i++);
                                         if( freq_uband[i] == - 1 )
                                        { return ((-1) * SETURFSYS); }
                                        data1[0] = p_udata11[i];
                                        data3[1] = p_udata32[i];
                                        add_fe[j] = p_uadd[i];
                                        exeCmd.feBoxNo[j] = (int) p_uadd[i]; 
                                      //  printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
                                         #ifdef DEBUG
                                          printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
                                           #endif

                                           //Control word for setting SOLAR ATTENUATOR in the common box
                                          for (i=0; sol_attr[i]!=-1 && fecbSet.solar_attn[j]!=sol_attr[i]; i++) {;}
                                          data1[1] = p_data12[i];  // p_data11 + p_data12
                                          data2[0] = p_data21[i];

                                          // If polarization swap
                                         if (fecbSet.polSwap != 0) 
                                         {data2[1] = 0x02; }
                                         else 
                                         {data2[1]=0x0;}     // p_data21 + p_data22 , p_data22 is not declared

                                          //  control word for setting NOISE CAL LEVEL in front end box.
                                          for (i=0; fecbSet.noiseCal != ns_cal[i]; i++) {;}
                                           data3[0] = p_data31[i];  // p_data32 +  p_data31

                                          if (fecbSet.freq_set[1] == 0) exeCmd.feBoxNo[++j] = 0;                  
                                      }

              mcm4setting();
              mcmPow(1);sleep(1);
              data1[0] = data1[0] + data1[1];
              data2[0] = data2[0] + data2[1];
              data3[0] = data3[0] + data3[1];
           
                  bzero(mask1,sizeof(mask1));bzero(mask2,sizeof(mask2));bzero(mask3,sizeof(mask3));bzero(mask4,sizeof(mask4));
             //  Frequency, Solar Attenuator, Nose cal and Swap control
                  sprintf(mask1, "07%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                  sprintf(mask3, "08%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                  //printf("07%02x ",data1[0]); printf("00%02x ",data1[0]); printf("08%02x ",data2[0]); printf("00%02x \n",data2[0]);
#ifdef DEBUG
                  printf("mask1: %s ", mask1);
                  printf("mask2: %s ", mask2);
                  printf("mask3: %s ", mask3);
                  printf("mask4: %s \n", mask4);
#endif
                  Mask64toArg(mask1, mask2, mask3, mask4);
                  exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

                 if (fecbSet.freq_set[1] == 0) j = 0;
                 else j = 1;
                 sprintf(mask1, "09%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                 sprintf(mask3, "0A%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                 //printf("09%02x ",data1[j]); printf("00%02x ",data1[j]); printf("0A%02x ",data2[j]); printf("00%02x \n",data2[j]);
#ifdef DEBUG
                 printf("mask1: %s ", mask1);
                 printf("mask2: %s ", mask2);
                 printf("mask3: %s ", mask3);
                 printf("mask4: %s \n", mask4);
#endif
                 Mask64toArg(mask1, mask2, mask3, mask4);
                 exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

               
                 break;
                 
        case RFNGCAL:   for (j=0; j<2; j++)
                                     {   
                                        for(i=0;( freq_uband[i] != (-1) && fecbSet.freq_set[j] != freq_uband[i]); i++);
                                         if( freq_uband[i] == - 1 )
                                        { return ((-1) * SETURFSYS); }
                                        data1[0] = p_udata11[i];
                                        data3[1] = p_udata32[i];
                                        add_fe[j] = p_uadd[i];
                                        exeCmd.feBoxNo[j] = (int) p_uadd[i]; 
                                      //  printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
                                         #ifdef DEBUG
                                          printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
                                           #endif

                                           //Control word for setting SOLAR ATTENUATOR in the common box
                                          for (i=0; sol_attr[i]!=-1 && fecbSet.solar_attn[j]!=sol_attr[i]; i++) {;}
                                          data1[1] = p_data12[i];  // p_data11 + p_data12
                                          data2[0] = p_data21[i];

                                          // If polarization swap
                                         if (fecbSet.polSwap != 0) 
                                         {data2[1] = 0x02; }
                                         else 
                                         {data2[1]=0x0;}     // p_data21 + p_data22 , p_data22 is not declared

                                          //  control word for setting NOISE CAL LEVEL in front end box.
                                          for (i=0; fecbSet.noiseCal != ns_cal[i]; i++) {;}
                                           data3[0] = p_data31[i];  // p_data32 +  p_data31

                                          if (fecbSet.freq_set[1] == 0) exeCmd.feBoxNo[++j] = 0;                  
                                      }

              mcm4setting();
              mcmPow(1);sleep(1);
              data1[0] = data1[0] + data1[1];
              data2[0] = data2[0] + data2[1];
              data3[0] = data3[0] + data3[1];
           
                  bzero(mask1,sizeof(mask1));bzero(mask2,sizeof(mask2));bzero(mask3,sizeof(mask3));bzero(mask4,sizeof(mask4));
             //  Frequency, Solar Attenuator, Nose cal and Swap control
                  sprintf(mask1, "07%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                  sprintf(mask3, "08%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                  //printf("07%02x ",data1[0]); printf("00%02x ",data1[0]); printf("08%02x ",data2[0]); printf("00%02x \n",data2[0]);
#ifdef DEBUG
                  printf("mask1: %s ", mask1);
                  printf("mask2: %s ", mask2);
                  printf("mask3: %s ", mask3);
                  printf("mask4: %s \n", mask4);
#endif
                  Mask64toArg(mask1, mask2, mask3, mask4);
                  exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

                 if (fecbSet.freq_set[1] == 0) j = 0;
                 else j = 1;
                 sprintf(mask1, "09%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                 sprintf(mask3, "0A%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                 //printf("09%02x ",data1[j]); printf("00%02x ",data1[j]); printf("0A%02x ",data2[j]); printf("00%02x \n",data2[j]);
#ifdef DEBUG
                 printf("mask1: %s ", mask1);
                 printf("mask2: %s ", mask2);
                 printf("mask3: %s ", mask3);
                 printf("mask4: %s \n", mask4);
#endif
                 Mask64toArg(mask1, mask2, mask3, mask4);
                 exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

               
                 break;
                 
        case RFTERM :    for (j=0; j<2; j++)
                                     {   
                                        for(i=0;( freq_uband[i] != (-1) && fecbSet.freq_set[j] != freq_uband[i]); i++);
                                         if( freq_uband[i] == - 1 )
                                        { return ((-1) * SETURFSYS); }
                                        data1[0] = p_udata11[i];
                                        data3[1] = p_udata32[i];
                                        add_fe[j] = p_uadd[i];
                                        exeCmd.feBoxNo[j] = (int) p_uadd[i]; 
                                      //  printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
                                         #ifdef DEBUG
                                          printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
                                           #endif

                                           //Control word for setting SOLAR ATTENUATOR in the common box
                                          for (i=0; sol_attr[i]!=-1 && fecbSet.solar_attn[j]!=sol_attr[i]; i++) {;}
                                          data1[1] = p_data12[i];  // p_data11 + p_data12
                                          data2[0] = p_data21[i];

                                          // If polarization swap
                                         if (fecbSet.polSwap != 0) 
                                         {data2[1] = 0x02; }
                                         else 
                                         {data2[1]=0x0;}     // p_data21 + p_data22 , p_data22 is not declared

                                          //  control word for setting NOISE CAL LEVEL in front end box.
                                          for (i=0; fecbSet.noiseCal != ns_cal[i]; i++) {;}
                                           data3[0] = p_data31[i];  // p_data32 +  p_data31

                                          if (fecbSet.freq_set[1] == 0) exeCmd.feBoxNo[++j] = 0;                  
                                      }

              mcm4setting();
              mcmPow(1);sleep(1);
              data1[0] = data1[0] + data1[1];
              data2[0] = data2[0] + data2[1];
              data3[0] = data3[0] + data3[1];
           
                  bzero(mask1,sizeof(mask1));bzero(mask2,sizeof(mask2));bzero(mask3,sizeof(mask3));bzero(mask4,sizeof(mask4));
             //  Frequency, Solar Attenuator, Nose cal and Swap control
                  sprintf(mask1, "07%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                  sprintf(mask3, "08%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                  //printf("07%02x ",data1[0]); printf("00%02x ",data1[0]); printf("08%02x ",data2[0]); printf("00%02x \n",data2[0]);
#ifdef DEBUG
                  printf("mask1: %s ", mask1);
                  printf("mask2: %s ", mask2);
                  printf("mask3: %s ", mask3);
                  printf("mask4: %s \n", mask4);
#endif
                  Mask64toArg(mask1, mask2, mask3, mask4);
                  exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

                 if (fecbSet.freq_set[1] == 0) j = 0;
                 else j = 1;
                 sprintf(mask1, "09%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                 sprintf(mask3, "0A%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                 //printf("09%02x ",data1[j]); printf("00%02x ",data1[j]); printf("0A%02x ",data2[j]); printf("00%02x \n",data2[j]);
#ifdef DEBUG
                 printf("mask1: %s ", mask1);
                 printf("mask2: %s ", mask2);
                 printf("mask3: %s ", mask3);
                 printf("mask4: %s \n", mask4);
#endif
                 Mask64toArg(mask1, mask2, mask3, mask4);
                 exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

               
                 break;

        case RFSWAP:    for (j=0; j<2; j++)
                                     {   
                                        for(i=0;( freq_uband[i] != (-1) && fecbSet.freq_set[j] != freq_uband[i]); i++);
                                         if( freq_uband[i] == - 1 )
                                        { return ((-1) * SETURFSYS); }
                                        data1[0] = p_udata11[i];
                                        data3[1] = p_udata32[i];
                                        add_fe[j] = p_uadd[i];
                                        exeCmd.feBoxNo[j] = (int) p_uadd[i]; 
                                      //  printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
                                         #ifdef DEBUG
                                          printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
                                           #endif

                                           //Control word for setting SOLAR ATTENUATOR in the common box
                                          for (i=0; sol_attr[i]!=-1 && fecbSet.solar_attn[j]!=sol_attr[i]; i++) {;}
                                          data1[1] = p_data12[i];  // p_data11 + p_data12
                                          data2[0] = p_data21[i];

                                          // If polarization swap
                                         if (fecbSet.polSwap != 0) 
                                         {data2[1] = 0x02; }
                                         else 
                                         {data2[1]=0x0;}     // p_data21 + p_data22 , p_data22 is not declared

                                          //  control word for setting NOISE CAL LEVEL in front end box.
                                          for (i=0; fecbSet.noiseCal != ns_cal[i]; i++) {;}
                                           data3[0] = p_data31[i];  // p_data32 +  p_data31

                                          if (fecbSet.freq_set[1] == 0) exeCmd.feBoxNo[++j] = 0;                  
                                      }

              mcm4setting();
              mcmPow(1);sleep(1);
              data1[0] = data1[0] + data1[1];
              data2[0] = data2[0] + data2[1];
              data3[0] = data3[0] + data3[1];
           
                  bzero(mask1,sizeof(mask1));bzero(mask2,sizeof(mask2));bzero(mask3,sizeof(mask3));bzero(mask4,sizeof(mask4));
             //  Frequency, Solar Attenuator, Nose cal and Swap control
                  sprintf(mask1, "07%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                  sprintf(mask3, "08%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                  //printf("07%02x ",data1[0]); printf("00%02x ",data1[0]); printf("08%02x ",data2[0]); printf("00%02x \n",data2[0]);
#ifdef DEBUG
                  printf("mask1: %s ", mask1);
                  printf("mask2: %s ", mask2);
                  printf("mask3: %s ", mask3);
                  printf("mask4: %s \n", mask4);
#endif
                  Mask64toArg(mask1, mask2, mask3, mask4);
                  exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

                 if (fecbSet.freq_set[1] == 0) j = 0;
                 else j = 1;
                 sprintf(mask1, "09%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                 sprintf(mask3, "0A%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                 //printf("09%02x ",data1[j]); printf("00%02x ",data1[j]); printf("0A%02x ",data2[j]); printf("00%02x \n",data2[j]);
#ifdef DEBUG
                 printf("mask1: %s ", mask1);
                 printf("mask2: %s ", mask2);
                 printf("mask3: %s ", mask3);
                 printf("mask4: %s \n", mask4);
#endif
                 Mask64toArg(mask1, mask2, mask3, mask4);
                 exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits
               
                 break;
        case SETRFSYS :                // Set RF, upgraded RF
	                for (j=0; j<2; j++)
                         {

                          // Frequency Setting
                            for(i=0;( freq_band[i] != (-1) && fecbSet.freq_set[j] != freq_band[i]); i++);
                             if( freq_band[i] == - 1 )
                                 { return ( (-1) * SETRFSYS );
                                 }
                                 data1[0] = p_data11[i];
                                 data3[1] = p_data32[i];
                                 add_fe[j] = p_add[i];
                                 exeCmd.feBoxNo[j] = (int) p_add[i];    
                         
                      
 #ifdef DEBUG
                printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
 #endif
          //Control word for setting SOLAR ATTENUATOR in the common box
            for (i=0; sol_attr[i]!=-1 && fecbSet.solar_attn[j]!=sol_attr[i]; i++) {;}
                data1[1] = p_data12[i];  // p_data11 + p_data12
                data2[0] = p_data21[i];

           // If polarization swap
           if (fecbSet.polSwap != 0) {data2[1] = 0x02; }else {data2[1]=0x0;}     // p_data21 + p_data22 , p_data22 is not declared

          //  control word for setting NOISE CAL LEVEL in front end box.
            for (i=0; fecbSet.noiseCal != ns_cal[i]; i++) {;}
            data3[0] = p_data31[i];  // p_data32 +  p_data31

            if (fecbSet.freq_set[1] == 0) exeCmd.feBoxNo[++j] = 0;

            }
              data1[0] = data1[0] + data1[1];
              data2[0] = data2[0] + data2[1];
              data3[0] = data3[0] + data3[1];
             
              mcm4setting();
              mcmPow(1);sleep(1);
                  bzero(mask1,sizeof(mask1));bzero(mask2,sizeof(mask2));bzero(mask3,sizeof(mask3));bzero(mask4,sizeof(mask4));
             //  Frequency, Solar Attenuator, Nose cal and Swap control
                  sprintf(mask1, "07%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                  sprintf(mask3, "08%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                  //printf("07%02x ",data1[0]); printf("00%02x ",data1[0]); printf("08%02x ",data2[0]); printf("00%02x \n",data2[0]);
#ifdef DEBUG
                  printf("mask1: %s ", mask1);
                  printf("mask2: %s ", mask2);
                  printf("mask3: %s ", mask3);
                  printf("mask4: %s \n", mask4);
#endif
                  Mask64toArg(mask1, mask2, mask3, mask4);
                  exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

                 if (fecbSet.freq_set[1] == 0) j = 0;
                 else j = 1;
                 sprintf(mask1, "09%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                 sprintf(mask3, "0A%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                 //printf("09%02x ",data1[j]); printf("00%02x ",data1[j]); printf("0A%02x ",data2[j]); printf("00%02x \n",data2[j]);
#ifdef DEBUG
                 printf("mask1: %s ", mask1);
                 printf("mask2: %s ", mask2);
                 printf("mask3: %s ", mask3);
                 printf("mask4: %s \n", mask4);
#endif         
               //  sleep(1);
                 Mask64toArg(mask1, mask2, mask3, mask4);
                 exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits
                 
               
                 for (i=1; i<add_fe[0]; i++) {
                   sprintf(mask1, "%02x16", i); sprintf(mask2, "0016");
#ifdef DEBUG
                   printf("mask1: %s ", mask1);
                   printf("mask2: %s \n", mask2);
#endif
                  // sleep(1);
                   Mask32toArg(mask1, mask2);
                   exeCmd.CmdNum = 7; FECB_CTRL(MCMCARD1);           //set dig mask 32 bits
                 }

                 for (i=add_fe[0]+1; i<7; i++) {
                   sprintf(mask1, "%02x16", i); sprintf(mask2, "0016");
#ifdef DEBUG
                   printf("mask1: %s ", mask1);
                   printf("mask2: %s \n", mask2);
#endif
                   //sleep(1);
                   Mask32toArg(mask1, mask2);
                   exeCmd.CmdNum = 7; FECB_CTRL(MCMCARD1);           //set dig mask 32 bits
                 } 

                 for (j=0;j<1;j++) {
                   sprintf(mask1, "%02x%02x",add_fe[j],data3[0]);   //21/3 jk
                   sprintf(mask2, "00%02x",data3[0]);
#ifdef DEBUG
                   printf("mask1: %s ", mask1);
                   printf("mask2: %s \n", mask2);
#endif
                  // sleep(1);
                   Mask32toArg(mask1, mask2);
                   exeCmd.CmdNum = 7; FECB_CTRL(MCMCARD1);           //set dig mask 32 bits
                   if (fecbSet.freq_set[1] == 0) j++;
                 }

	        //  MCM OFF
                 mcmPow(0);
          
                break;

       case RFNGSET :
                        mcm4setting();
                        mcmPow(1); 
        
        /*  // NG Cycle bit D0,1
             for(i = 0;i<2;i++)
             { getbitDO((int)fecbSet.ngCycle); for (b=0;b<2;b++) { digWord[b + (i*16)] = bitD0[b]; }
             }

             for(i= 0 ;i<2;i++) {
             if(digWord[i+16]) mcm4word[0] = mcm4word[1] | (1 << i );
             if(digWord[i]) mcm4word[1] = mcm4word[0] | (1 << i );
             }

               sprintf(mask1,"%04x",mcm4word[1] | ( 1 << 4) );
               sprintf(mask2,"%04x",mcm4word[0] | ( 1 << 4) );
#ifdef DEBUG
               printf("mask1: %s ", mask1);
               printf("mask2: %s \n", mask2);
#endif
               Mask32toArg(mask1, mask2);
               exeCmd.CmdNum = 7; FECB_CTRL(MCMCARD1);           //set dig mask 32 bits
            //  Copy(-1, Mcm2, SPEINDX); */

              break;

       case RFCM_SW :   mcm4setting();
                        mcmPow(1);                        
                        break;

       case REBOOTMCM :
                        exeCmd.CmdNum = 15; FECB_CTRL(MCMCARD1);
                         break;

       case FESDOMON  :
                         if(exeCmd.McmPow == 0 )
                         { 
                            printf("####################Inside FESDOMON execCMD is %d\n",exeCmd.McmPow);
                            mcm4setting();
                            mcmPow(1);sleep(1);
                         }
                             exeCmd.CmdNum = 19;      // Common box monitor
                             FECB_CTRL(MCMCARD1);                   
                         for (i=0;i<1;i++)
                          {
                           if (exeCmd.feBoxNo[i] != 0) 
                               {
 #ifdef DEBUG
                                  printf(" \n============ FEBOX NO %d =====\n", exeCmd.feBoxNo[i]);
 #endif
                                  exeCmd.Argument[0] = exeCmd.feBoxNo[i];  // FeBox Number!
                                  exeCmd.CmdNum = 18;                                            //FE Box Mon
                                  FECB_CTRL(MCMCARD1); 
                     
                               }
                           }
                       break;

          case SETURFSYS :       

                 for (j=0; j<2; j++)
                 {   
                    for(i=0;( freq_uband[i] != (-1) && fecbSet.freq_set[j] != freq_uband[i]); i++);
                     if( freq_uband[i] == - 1 )
                    { return ((-1) * SETURFSYS); }

                    data1[0] = p_udata11[i];
                    data3[1] = p_udata32[i];
                    add_fe[j] = p_uadd[i];
                    exeCmd.feBoxNo[j] = (int) p_uadd[i]; 

                    #ifdef DEBUG
                    printf("\n ===> FEBox Number CH[%d] = %d \n", j, exeCmd.feBoxNo[j]);
                    #endif

                       //Control word for setting SOLAR ATTENUATOR in the common box
                      for (i=0; sol_attr[i]!=-1 && fecbSet.solar_attn[j]!=sol_attr[i]; i++) {;}

                      data1[1] = p_data12[i];  // p_data11 + p_data12
                      data2[0] = p_data21[i];

                      // If polarization swap
                     if (fecbSet.polSwap != 0) 
                     {data2[1] = 0x02; }
                     else 
                     {data2[1] = 0x0;}     // p_data21 + p_data22 , p_data22 is not declared

                      //  control word for setting NOISE CAL LEVEL in front end box.
                      for (i=0; fecbSet.noiseCal != ns_cal[i]; i++) {;}

                      data3[0] = p_data31[i];  // p_data32 +  p_data31

                      if (fecbSet.freq_set[1] == 0) exeCmd.feBoxNo[++j] = 0;                  
                  }

                   data1[0] = data1[0] + data1[1];
                   data2[0] = data2[0] + data2[1];
                   data3[0] = data3[0] + data3[1];

                 mcm4setting();
                 mcmPow(1);sleep(1);
           
                  bzero(mask1,sizeof(mask1));bzero(mask2,sizeof(mask2));bzero(mask3,sizeof(mask3));bzero(mask4,sizeof(mask4));
              //  Frequency, Solar Attenuator, Nose cal and Swap control
                  sprintf(mask1, "09%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                  sprintf(mask3, "0A%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                  //printf("07%02x ",data1[0]); printf("00%02x ",data1[0]); printf("08%02x ",data2[0]); printf("00%02x \n",data2[0]);
#ifdef DEBUG
                  printf("mask1: %s ", mask1);
                  printf("mask2: %s ", mask2);
                  printf("mask3: %s ", mask3);
                  printf("mask4: %s \n", mask4);
#endif
                  Mask64toArg(mask1, mask2, mask3, mask4);
                  exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

                 if (fecbSet.freq_set[1] == 0) j = 0;
                 else j = 1;
                 sprintf(mask1, "07%02x",data1[0]); sprintf(mask2, "00%02x",data1[0]);
                 sprintf(mask3, "08%02x",data2[0]); sprintf(mask4, "00%02x",data2[0]);
                 //printf("09%02x ",data1[j]); printf("00%02x ",data1[j]); printf("0A%02x ",data2[j]); printf("00%02x \n",data2[j]);
#ifdef DEBUG
                 printf("mask1: %s ", mask1);
                 printf("mask2: %s ", mask2);
                 printf("mask3: %s ", mask3);
                 printf("mask4: %s \n", mask4);
#endif
                 Mask64toArg(mask1, mask2, mask3, mask4);
                 exeCmd.CmdNum = 8; FECB_CTRL(MCMCARD1);  // set dig mask 64 bits

               
                 for (i=1; i<add_fe[0]; i++) {
                   sprintf(mask1, "%02x16", i); sprintf(mask2, "0016");
#ifdef DEBUG
                   printf("mask1: %s ", mask1);
                   printf("mask2: %s \n", mask2);
#endif
                   Mask32toArg(mask1, mask2);
                   exeCmd.CmdNum = 7; FECB_CTRL(MCMCARD1);           //set dig mask 32 bits
                 }

                 for (i=add_fe[0]+1; i<7; i++) {
                   sprintf(mask1, "%02x16", i); sprintf(mask2, "0016");
#ifdef DEBUG
                   printf("mask1: %s ", mask1);
                   printf("mask2: %s \n", mask2);
#endif
                   Mask32toArg(mask1, mask2);
                   exeCmd.CmdNum = 7; FECB_CTRL(MCMCARD1);           //set dig mask 32 bits
                 } 

                 for (j=0;j<1;j++) {
                   sprintf(mask1, "%02x%02x",add_fe[j],data3[0]);   //21/3 jk
                   sprintf(mask2, "00%02x",data3[0]);
#ifdef DEBUG
                   printf("=>mask1: %s ", mask1);
                   printf("=>mask2: %s\n", mask2);
#endif
                   Mask32toArg(mask1, mask2);
                   exeCmd.CmdNum = 7; FECB_CTRL(MCMCARD1);           //set dig mask 32 bits
                   if (fecbSet.freq_set[1] == 0) j++;
                 }

	        //  MCM OFF Not needed for time being Power OFF: mcmPow(0);
          
                break;
   }

}
