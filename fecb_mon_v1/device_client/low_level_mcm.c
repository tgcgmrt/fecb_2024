/******* Add Check Sum function *********************
 * Add CS at the end of the buffer
 * CS does no include MCM Address                  */

#include "low_level_mcm.h"
#include  "fecb_mcm.h"


void add_cksum(unsigned char buff[])
{
 int length, i;
 char CS=0;

 length = buff[1] + (buff[2]<<8);
 for(i=1; i<length; i++) CS += *(buff+i);
 CS = ~CS+1;

 buff[length] = CS; 

}


void AppendAdd(unsigned char *buff)
{
   int i,length,ret=0;
   memset(rbuff.Msg,0x00,200);
  
    buff[0]=*buf_ptr0; 
    
    length = buff[1] + (buff[2]<<8);
    add_cksum(buff);  
    
    write(fifo_fd, buff, (length+1));   
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"); 
    for (i=0; i<(length+1); i++)
    {
      
      printf("0x%02X\t",(unsigned char)buff[i]);
      
    }
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    usleep(300000);
    ret=read(resp_fecbfd,rbuff.Msg,200);
    rbuff.Msg[199]='\0';
   printf("############# Read Call returned %d\n @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",ret);
   rbuff.Length = rbuff.Msg[1] + (rbuff.Msg[2]<<8);
   rbuff.Length+=1;
   if(rbuff.Length < 5 || rbuff.Length > 200)
   rbuff.Length=0; // added by RU on 13/01/2021

     if ( rbuff.Length <= 0 )
      { resp_code = 11; }
      else
     { resp_code = 10;   
     }
    // close(resp_fecbfd);
   	 
} 


void set_dig_mask64()
{
 int i;
 unsigned char buff[18];
 //char buff[]={0,0x11,0,0,1,0,9,0,4,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0};

 buff[0]=0;
 buff[1]=0x11;
 buff[2]=0;
 buff[3]=0;
 buff[4]=1;
 buff[5]=0;
 buff[6]=9;
 buff[7]=0;
 buff[8]=4;
  buff[9]=0xff;
 buff[10]=0xff;
 buff[11]=0xff;
 buff[12]=0xff;
 buff[13]=0xff;
 buff[14]=0xff;
 buff[15]=0xff;
 buff[16]=0xff;
 buff[17]=0;

 for(i=0; i<8; i++)
 buff[9+i] = exeCmd.Argument[i]; 
 AppendAdd(buff);
}

void set_dig_mask32()
{
 int i;
 unsigned char buff[14];
 //char buff[]={0,0xd,0,0,1,0,5,0,3,0xff,0xff,0xff,0xff,0};

 buff[0]=0;
 buff[1]=0xd;
 buff[2]=0;
 buff[3]=0;
 buff[4]=1;
 buff[5]=0;
 buff[6]=5;
 buff[7]=0;
 buff[8]=3;
 buff[9]=0xff;
 buff[10]=0xff;
 buff[11]=0xff;
 buff[12]=0xff;
 buff[13]=0;

 for(i=0; i<4; i++)
 buff[9+i] = exeCmd.Argument[i]; 
 AppendAdd(buff); 

}

void set_dig_mask16()
{
  int i;
 unsigned char buff[12];
  //char buff[]={0,0xb,0,0,1,0,3,0,2,0xff,0xff,0};
  buff[0]=0;
 buff[1]=0xb;
 buff[2]=0;
 buff[3]=0;
 buff[4]=1;
 buff[5]=0;
 buff[6]=3;
 buff[7]=0;
 buff[8]=2;
 buff[9]=0xff;
 buff[10]=0xff;
 buff[11]=0;

  for(i=0; i<2; i++)
  buff[9+i] = exeCmd.Argument[i];
  AppendAdd(buff);  
 
}

void set_anl_mask()
{
  int i;
  char buff[18];
  //char buff[]={0,0x11,0,0,1,0,9,0,1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0};
 buff[0]=0;
 buff[1]=0x11;
 buff[2]=0;
 buff[3]=0;
 buff[4]=1;
 buff[5]=0;
 buff[6]=9;
 buff[7]=0;
 buff[8]=1;
 buff[9]=0xff;
 buff[10]=0xff;
 buff[11]=0xff;
 buff[12]=0xff;
 buff[13]=0xff;
 buff[14]=0xff;
 buff[15]=0xff;
 buff[16]=0xff;
 buff[17]=0;

  for(i=0; i<8; i++)
  buff[9+i] = exeCmd.Argument[i]; 
  AppendAdd(buff); 
 // InitBuff(buff);
}

void set_mean_mode()
{
 char buff[12];
 //buff[0]={0,0xb,0,0,1,0,3,0,0,2,0,0};
 buff[0]=0;
 buff[1]=0xb;
 buff[2]=0;
 buff[3]=0;
 buff[4]=1;
 buff[5]=0;
 buff[6]=3;
 buff[7]=0;
 buff[8]=0;
 buff[9]=2;
 buff[10]=0;
 buff[11]=0;
 buff[10] = exeCmd.Argument[0]; AppendAdd(buff); 
 //InitBuff(buff);
}

void feed_cntrl_old()
{
  char buff[12];
  //buff[]={0,0xa,0,0,3,0,2,0,0,0,0};
  buff[0]=0;
  buff[1]=0xa;
  buff[2]=0;
  buff[3]=0;
  buff[4]=3;
  buff[5]=0;
  buff[6]=2;
  buff[7]=0;
  buff[8]=0;
  buff[9]=0;
  buff[10]=0;
  buff[8] = exeCmd.Argument[0];
  buff[9] = exeCmd.Argument[0]>>8;//buff[9]=0;
  AppendAdd(buff);
}

void feed_cntrl_new()
{
  int i;
  char buff[12];
  //buff[]={0,0xa,0,0,6,0,2,0,0,0,0};
  buff[0]=0;
  buff[1]=0xa;
  buff[2]=0;
  buff[3]=0;
  buff[4]=6;
  buff[5]=0;
  buff[6]=2;
  buff[7]=0;
  buff[8]=0;
  buff[9]=0;
  buff[10]=0;
  for(i=0; i<2; i++)
  buff[8+i] = exeCmd.Argument[i]; 
  AppendAdd(buff);
}

void fe_box_mon()
{
  char buff[12], temp_arg[1];
   int i;
  //buff[]={0,0xa,0,0,7,0,2,0,0,0,0};
  buff[0]=0;
  buff[1]=0xa;
  buff[2]=0;
  buff[3]=0;
  buff[4]=7;
  buff[5]=0;
  buff[6]=2;
  buff[7]=0;
  buff[8]=0;
  buff[9]=0;
  buff[10]=0;
  temp_arg[0]= (int)exeCmd.Argument[0]-1;
  buff[9] = temp_arg[0];
  /* printf("EXE CMD ARGUMENT in LOW LEVEL MCM ====>0x%02X, TEMP ARG= 0x%02X \n",exeCmd.Argument[0], temp_arg[0] );   
  printf("BUFF[9] LOW LEVEL MCM ====>0x%02X\n",buff[9] ); 
   for (i=0; i<12; i++)
   { printf("FE BOX MONITORING PKT in LOW LEVEL MCM ====>0x%02X\n", buff[i]); } */ 
 // InitBuff(buff);
  AppendAdd(buff);
}

  // exeCmd Far Memory Location hence not passing to function

void FECB_CTRL(int MCMADDR )
{
  int i, j;
  char buff[12];


   for(j=0;j<sizeof(wbuff.Msg);j++) { wbuff.Msg[j]=0x00; }

   wbuff.Length = 0;

   buf_ptr0 = (char *)&wbuff.Msg ;
   memset(buf_ptr0,0x00,200); 
   memset(buff,0x00,12);
   buf_ptr0[0] = MCMADDR;
 
  switch(exeCmd.CmdNum)
  {
       case 1 :  // NULL
                 //buff[]={0,8,0,0,0,0,0,0,0};
                 memset(buff,0x00,12);
                 buff[0]=0;
                 buff[1]=8;
                 buff[2]=0;
                 buff[3]=0;
                 buff[4]=0;
                 buff[5]=0;
                 buff[6]=0;
                 buff[7]=0;
                 buff[8]=0;
                 AppendAdd(buff);
                 break;
       case 2 : // IDL Mode
                 //buff[]={0,0xa,0,0,1,0,2,0,0,0,0};
                 buff[0]=0;
                 buff[1]=0xa;
                 buff[2]=0;
                 buff[3]=0;
                 buff[4]=1;
                 buff[5]=0;
                 buff[6]=2;
                 buff[7]=0;
                 buff[8]=0;
                 buff[9]=0;
                 buff[10]=0;
                 AppendAdd(buff);
                 break;
       case 3 : // Scan Mode
                 //buff[]={0,0xa,0,0,1,0,2,0,0,1,0};
                 buff[0]=0;
                 buff[1]=0xa;
                 buff[2]=0;
                 buff[3]=0;
                 buff[4]=1;
                 buff[5]=0;
                 buff[6]=2;
                 buff[7]=0;
                 buff[8]=0;
                 buff[9]=1;
                 buff[10]=0; 
                 AppendAdd(buff);
                 break;
       case 4 : // Mean Mode -- Unused.
                 set_mean_mode(); 
                 break;
       case 5 : // Set AnalogMask
                set_anl_mask();
                break;
       case 6 : // Set Digital 16 bit Mask
                set_dig_mask16();
                break;
       case 7 : // Set Digital32 bit Mask
                set_dig_mask32();
                break;
       case 8 : // Set Digital64 bit Mask
                set_dig_mask64();
                break;
       case 9 : // TX Read Anl Mask
                //buff[]={0,9,0,0,2,0,1,0,0,0};
                 buff[0]=0;
                 buff[1]=9;
                 buff[2]=0;
                 buff[3]=0;
                 buff[4]=2;
                 buff[5]=0;
                 buff[6]=1;
                 buff[7]=0;
                 buff[8]=0;
                 buff[9]=0; AppendAdd(buff);
                // InitBuff(buff);
                break;
      case 10 : // TX Read Dig Mask16
                 //buff[]={0,9,0,0,2,0,1,0,1,0};
                 buff[0]=0;
                 buff[1]=9;
                 buff[2]=0;
                 buff[3]=0;
                 buff[4]=2;
                 buff[5]=0;
                 buff[6]=1;
                 buff[7]=0;
                 buff[8]=1;
                 buff[9]=0; AppendAdd(buff);
                 //InitBuff(buff);
                 break;
      case 11 : //  TX Read Dig Mask32
                 //buff[]={0,9,0,0,2,0,1,0,2,0};
                 buff[0]=0;
                 buff[1]=9;
                 buff[2]=0;
                 buff[3]=0;
                 buff[4]=2;
                 buff[5]=0;
                 buff[6]=1;
                 buff[7]=0;
                 buff[8]=2;
                 buff[9]=0; AppendAdd(buff);
               //  InitBuff(buff);
                 break;
      case 12 : // TX Read Dig Mask64
                 //char buff[]={0,9,0,0,2,0,1,0,5,0};
                 buff[0]=0;
                 buff[1]=9;
                 buff[2]=0;
                 buff[3]=0;
                 buff[4]=2;
                 buff[5]=0;
                 buff[6]=1;
                 buff[7]=0;
                 buff[8]=5;
                 buff[9]=0; AppendAdd(buff);
               //  InitBuff(buff);
                 break;
      case 13 : // TX Read Version
                 //buff[]={0,9,0,0,2,0,1,0,3,0};
                 buff[0]=0;
                 buff[1]=9;
                 buff[2]=0;
                 buff[3]=0;
                 buff[4]=2;
                 buff[5]=0;
                 buff[6]=1;
                 buff[7]=0;
                 buff[8]=3;
                 buff[9]=0;  AppendAdd(buff);
                // InitBuff(buff);
                 break;
      case 14 : // TX Read Mode
                 //buff[]={0,9,0,0,2,0,1,0,4,0};
                 buff[0]=0;
                 buff[1]=9;
                 buff[2]=0;
                 buff[3]=0;
                 buff[4]=2;
                 buff[5]=0;
                 buff[6]=1;
                 buff[7]=0;
                 buff[8]=4;
                 buff[9]=0; AppendAdd(buff);
                // InitBuff(buff);
                 break;
      case 15 : // TX Reboot
                 //buff[]={0,8,0,0,5,0,0,0,0};
                 buff[0]=0;
                 buff[1]=8;
                 buff[2]=0;
                 buff[3]=0;
                 buff[4]=5;
                 buff[5]=0;
                 buff[6]=0;
                 buff[7]=0;
                 buff[8]=0; AppendAdd(buff);
                 //InitBuff(buff);
                 break;
      case 16 : // TX Feed Mcm
                feed_cntrl_old(&exeCmd.Argument);
                break;
      case 17 : // TX Feed Mcm
                feed_cntrl_new(&exeCmd.Argument);
                break;
      case 18 : //TX FE Box monitor
                fe_box_mon(&exeCmd.Argument);
                break;
      case 19 : //  TX Common Box Monitor
                //buff[]={0,9,0,0,7,0,1,0,1,0};
                 buff[0]=0;
                 buff[1]=9;
                 buff[2]=0;
                 buff[3]=0;
                 buff[4]=7;
                 buff[5]=0;
                 buff[6]=1;
                 buff[7]=0;
                 buff[8]=1;
                 buff[9]=0; AppendAdd(buff);
               //  InitBuff(buff);
                break;
   }
 
      if(resp_code = 10)
       {         
         if(rbuff.Length > 1 )
         decodeResponse();
         printf("FECB_CTRL finished\n");
         return ;
      }
     
}

//-----------------------------------------------//

