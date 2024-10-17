#include "define.h"
#include "fecb_mcm.h"
#include "response.h"

// added function for FE pow mon , bhavesh , 19.10.2021
double convertdB_fe( float pval)
{
   double p[] =  {-40.29, 326.1, -689.6, -46.67, 641.7 };
   double q[] =  { -11.24, 48.01, -101.2, 114, -54.67};
  
   double pow_db=0.0;


   //pval = -5.0 + ( 0.001 * 39.0625 * pval); /* Voltage Conversion */


   
   pow_db = (double) ( (p[0]* pow(pval, 4) + p[1] * pow(pval, 3)  + p[2] * pow(pval, 2)  + p[3]* pow(pval, 1) + p[4]) /
      				
      			    (pow(pval, 5) + q[0] * pow(pval,4) +  q[1] * pow(pval,3) + q[2] * pow(pval,2) + q[3] * pow(pval,1) + q[4])
                     		) ;
   
   return (double) pow_db;
}

void decodeFEBox(int BoxNum)
 {
         int t, sub_band, wal_stat,i;  
         float  vn1, vn2, vp1, vp2, vref1, vref2, vwf1, vwf2, tp_fe, tp_lna;
         int rf_stat=-999;
	 double pow_db_ch2,pow_db_ch1; 
        

	   if((int) stmpbuf[2]>100 && (int) stmpbuf[4]>100)
             {
               if((int) stmpbuf[6]<100)noise_cal = 11;    // T-T-F NG=11 EHical On
               else noise_cal = 40;              // T-T-T NG=10 LOW CAL OFF
	     }

	   if((int) stmpbuf[2]<100 && (int) stmpbuf[4]<100)
             {
               if((int) stmpbuf[6]>100)noise_cal = 41;   // F-F-T : LOW CAL ON
	       else noise_cal = 10;             // F-F-F : EHiCal OFF
             }

	   if((int) stmpbuf[2]<100 && (int) stmpbuf[4]>100)
             {
               if((int) stmpbuf[6]<100) noise_cal = 21;  // F-T-F : NG=21 HICAL ON
	       else noise_cal = 30;            //  F-T-T : NG=30 MDE CAL OFF
	     }

           if((int) stmpbuf[2]>100 && (int) stmpbuf[4]<100)
             {
               if((int) stmpbuf[6]>100) noise_cal = 31;  // T-F-T NG=31 MED CAL ON
	       else noise_cal = 20;              // T-F-F NG=20 High Cal OFF
             }

        //   sprintf(tbuf,"FE_NGCAL"); tbuf[8]='\0';
          strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"FE_M_NGCAL");

           switch (noise_cal)
           {
               case 10 : sprintf(tbuf,"EHi OFF"); break;
               case 11 : sprintf(tbuf,"EHi ON"); break;
               case 20 : sprintf(tbuf,"Hi  OFF"); break;
               case 21 : sprintf(tbuf,"Hi  ON"); break;
               case 30 : sprintf(tbuf,"MED OFF"); break;
               case 31 : sprintf(tbuf,"MED ON"); break;
               case 40 : sprintf(tbuf,"LOW OFF"); break;
               case 41 : sprintf(tbuf,"LOW ON"); break;
               default : sprintf(tbuf,"Cal Unknown");
           }
            tbuf[strlen(tbuf)-1]='\0';
            sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%s",tbuf);
               resp_msg_cntr++;

           // At present only one RF ON/OFF status bit is there in the EXISTING ONLINE
	   if((int) stmpbuf[8] < 100 && (int) stmpbuf[9] > 100)rf_stat = 1;
	   else rf_stat = 0;

            strcpy(&dev_Resp.resp_data.prmname[resp_msg_cntr][0], "RF_ON_M");
            sprintf(&dev_Resp.resp_data.prmvalue[resp_msg_cntr][0], "%d", rf_stat);
            resp_msg_cntr++;
                                   /*
            strcpy(&dev_Resp.resp_data.prmname[resp_msg_cntr][0], "RF_ON_M_CH2");
            sprintf(&dev_Resp.resp_data.prmvalue[resp_msg_cntr][0], "%d", rf_stat);
            resp_msg_cntr++;     */


	  if(BoxNum == 5 || BoxNum == 4 || BoxNum == 3 )               // Only single band
	  {
           if((int) stmpbuf[10] < 100 && (int) stmpbuf[12] < 100)
           sub_band = 0;            // FF 0 1060 MHz
           else if((int) stmpbuf[10] < 100 && (int) stmpbuf[12] > 100)
           sub_band = 1;       // FT 1 1280 MHz
           else if((int) stmpbuf[10] > 100 && (int) stmpbuf[12] < 100)
           sub_band = 2;       // TF 2 1390 MHz
           else if((int) stmpbuf[10] > 100 && (int) stmpbuf[12] > 100)
           sub_band = 3;       // TT 3 1420 MHz
     }

            strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_FILTER_M");
            sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr], "%d", sub_band);
            resp_msg_cntr++;
                                 /*
            strcpy(&dev_Resp.resp_data.prmname[resp_msg_cntr][0], "FE_FILTER_M_CH2");
            sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr], "%d", sub_band);
            resp_msg_cntr++;   */



	   if((int) stmpbuf[16] < 100 && (int) stmpbuf[17] < 100)wal_stat = 0;
	   else wal_stat = 1;
          strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_WALSH_SW");
          sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr], "%d", wal_stat);
          resp_msg_cntr++;

          vwf1 = ((float)stmpbuf[16]*10)/(float)256;
          strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "WALSH_VOLT1_M");
          sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr], "%f", (vwf1 -5));
          resp_msg_cntr++;


	  vwf2 = ((float)stmpbuf[17]*10)/(float)256;
          strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "WALSH_VOLT2_M");
          sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr], "%f", (vwf2- 5));
          resp_msg_cntr++;


	   vn1 = ((float)stmpbuf[18]*10)/(float)256;
	   vn2 = ((float)stmpbuf[19]*10)/(float)256;
	   vp1 = ((float)stmpbuf[20]*10)/(float)256;
	   vp2 = ((float)stmpbuf[21]*10)/(float)256;
	   vref1 = ((float)stmpbuf[22]*10)/(float)256;
	   vref2 = ((float)stmpbuf[23]*10)/(float)256;

           strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_VOLT_N1");
           sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f", (vp1 - 5.0));
           strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_VOLT_N2");
           sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f",  (vp2 - 5.0));
           strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_VOLT_P1");
           sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f",  (vn1 - 5.0));
           strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_VOLT_P2");
           sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f",  (vn2 - 5.0));
           strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_VOLT_R1");
           sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f", (vref1 - 5.0));
           strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_VOLT_R2");
           sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f", (vref2 - 5.0));

// Temperature Monitoring Aug 27, 2013 VBB  //
           tp_fe   = (float) (((float)stmpbuf[24]* 39.0625) - 5000.0)/80;
           tp_lna  = (float) (((float)stmpbuf[25]* 39.0625) - 5000.0)/80;

            strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "TEMP_FE_M");
            sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f", tp_fe);
         //   printf(" %d %s %s \n", (resp_msg_cntr-1),dev_Resp.resp_data.prmname[resp_msg_cntr-1],dev_Resp.resp_data.prmvalue[resp_msg_cntr-1]);

            strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "TEMP_LNA_M");
            sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f",  tp_lna);
          //  printf(" %d %s %s \n", (resp_msg_cntr-1),dev_Resp.resp_data.prmname[resp_msg_cntr-1],dev_Resp.resp_data.prmvalue[resp_msg_cntr-1]);

// FE Power Monitoring stmpbuf[27], stmpbuf[28] -- Check voltage and dB conversio?4/4/2014 Parikh //
	   vn1 = ((float)stmpbuf[27]*10)/(float)256 - 5.0;
 	   printf("###################################Value of Ch1 fe volt[%f]\n",vn1);
 	   pow_db = (double)convertdB_fe(vn1);
           strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_DET_M_CH1");
           //sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f",  (vn1 - 5.0));
	   sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%g", pow_db);
	   printf("###################################Value of Ch1 fe pow[%g]\n",pow_db);

	   vn2 = ((float)stmpbuf[26]*10)/(float)256 - 5.0;
	   printf("###################################Value of Ch2 fe volt[%f]\n",vn2);
	   pow_db = (double)convertdB_fe(vn2);
	   strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_DET_M_CH2");
           //sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f",  (vn2 - 5.0));
	   sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%g", pow_db);
	   printf("###################################Value of Ch2 fe pow[%g]\n",pow_db);	   

/*
            strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_DET_M_CH1");
            //sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f",  (vn1 - 5.0));
	    sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%g", pow_db_ch1);
	    printf("###################################Value of Ch2 fe pow[%g]\n",pow_db_ch1);
*/
	    //sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%g", pow_db);
/*
            strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "FE_DET_M_CH2");
            //sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%5.2f",  (vn2 - 5.0));
	    sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr++], "%g", pow_db_ch2);
	    printf("###################################Value of Ch2 fe pow[%g]\n",pow_db_ch2);
*/
          //  printf(" %d %s %s \n",(resp_msg_cntr-1), dev_Resp.resp_data.prmname[resp_msg_cntr-1],dev_Resp.resp_data.prmvalue[resp_msg_cntr-1]);

// decimal to voltage = -5+ ( 0.001*(float)pcBufRead[j]*39.0625); //
// printf("FEBOX Decode End>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	 return;
}

double convertdB ( float pval , int Box)
{
   double a[] =  { 3.613, -3589000000000000, 0, 122.1, -37.47 };
   double b[] =  { 4.353, -8.921, 2.454, 7.34, 2.077 };
   double c[] =  { 0.006815, 1.895, 0.000000000000000222, 1.898, 1.513 };
   double powerdB=0.0;


   pval = -5.0 + ( 0.001 * 39.0625 * pval); /* Voltage Conversion */

   if(Box) /* Common Box dBm conversion*/
   {
      powerdB = (double) ( a[0] * exp(-pow( ((pval - b[0])/c[0]),2) )+
                         a[1] * exp(-pow( ((pval - b[1])/c[1]),2) )+
                         a[3] * exp(-pow( ((pval - b[3])/c[3]),2) )+
                         a[4] * exp(-pow( ((pval - b[4])/c[4]),2) )
                       ) ;
   }
   return (double) powerdB ;
}


void decodeCommonBox()
{
  int i,j, pt=0;
  int mcm_on, mcm_off, pow_on, pow_off;
  int BAND_1,BAND_2,BAND_3,BAND_4,BAND_5;
  //float fl;
  float Volt[54],cb_temp;                                    //Variable added for CB;
  //float volt,volt37,volt38,volt39,volt40;          //Variable added for FE
  
   long l;
   unsigned char *t = NULL;
   t = (unsigned char*)malloc(sizeof(unsigned char )*512);

#ifdef DEBUG
   printf(" Common Box decode >>>>>>>>>>>>>>>>>> resp_msg_cntr %d \n", resp_msg_cntr);
#endif

  for(i=0;i<=53;i++)
			{
 			  // printf ("MCM RESPONSE COPYING IN COMMON BOX STMPBUF[%d] ===> %x\n",i, stmpbuf[i] );			
                          Volt[i] = -5+0.001*(float)stmpbuf[i]*39.0625;
                          printf (" Voltage Data COMMON BOX Volt[%d] ===> %.3f\n",i, Volt[i] );
			}
			
 // Logic for MCM ON
		      if (Volt[50] >= 0)
                         {
                          //fprintf(fp,"\n MCM 5 ON \n ");
                          printf ("MCM 5 ON");
                          mcm_on= 1;
                          strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"MCM_ON");
       		   	  sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%d",mcm_on);
       	                  resp_msg_cntr++;
                          
		         }
		      else
                         {
                          //fprintf(fp,"\n MCM 5 OFF\n ");	
                          mcm_off= 0;
                          strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"MCM_OFF");
       		   sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%d",mcm_off);
       	           resp_msg_cntr++;
                          	
			  }

//Logic For Power Supply
		     if (Volt[20] > 2.5 && Volt[23] < -1.5 && Volt[24]>1.5)
			{
		         //fprintf(fp,"\n Power Supply Card Working Properly \n");
		          pow_on= 1;
                          strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"POWER_ON");
       		   sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%d",pow_on);
       	           resp_msg_cntr++;
                          
			}
		      /*else if (	Volt[24] < 1.5 )
			{
		         fprintf(fp,"\n Problem in +5V Supply \n");
			}
		      else if (	Volt[23] > -1.5 )
			{
		         fprintf(fp,"\n Problem in -5V Supply \n");
			}
		      else if (Volt[20] < 2.5)
			{
                         fprintf(fp,"\n Problem in +12V Supply \n");
			}
		       else if (Volt[25] < 3.5)
			{
                         fprintf(fp,"\n Problem in +9V Supply \n");
			}*/
		       else 
			{
		         //fprintf(fp,"\n Problem in Power Supply Card \n");
		          pow_off= 0;
                         strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"POWER_OFF");
       		  sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%d",pow_off);
       	          resp_msg_cntr++;
                          
			}

// Logic for Band selection (Channel 1)
		      if (Volt[28] >= 0 && Volt[29] <=-2.5 && Volt[34] >= 0 && Volt[35] <= -2.5 && Volt[36] >= 0 && Volt[37] <= -2.5)
		         {
                          //fprintf(fp,"\n Channel 1: Band 1 Selected \n");
                          sprintf(tbuf,"Band-1");
			}
		      else if (Volt[28] <= -2.5 && Volt[29] >= 0 && Volt[34] >= 0 && Volt[35] <= -2.5 && Volt[36] >= 0 && Volt[37] <= -2.5)
		         {
                          //fprintf(fp,"\n Channel 1: Band 2 Selected \n");
                          sprintf(tbuf,"Band-2");
			 }
		      else if (Volt[30] <= -2.5 && Volt[31] >= 0 && Volt[34] <= -2.5 && Volt[35] >= 0 && Volt[36] >= 0 && Volt[37] <= -2.5)
		         {
                         // fprintf(fp,"\n Channel 1: Band 3 Selected \n");
                         sprintf(tbuf,"Band-3");
                        }
		       else if (Volt[32] >= 0 && Volt[33] <= -2.5 && Volt[36] <=-2.5 && Volt[37] >= 0)
		         {
                          //fprintf(fp,"\n Channel 1: Band 4 Selected \n");
                          sprintf(tbuf,"Band-4");
                          }
                       else if (Volt[32] <= -2.5 && Volt[33] >= 0 && Volt[36] <=-2.5 && Volt[37] >= 0)
		         {
                          //fprintf(fp,"\n Channel 1: Band 5 Selected \n");
                          sprintf(tbuf,"Band-5");
                       }
		       else
			{
			 //fprintf(fp,"\n Channel 1: Band 5 Selected \n");
       	         sprintf(tbuf,"NoBand");
			}
			 strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "BAND_SELECT_M_CH1");
		         sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%s",tbuf);
		         printf ("\nBAND CH1 = %s\n", tbuf);
        		 resp_msg_cntr++; 
 
// Logic for Band selection (Channel 2)
		      if (Volt[0] >= 0 && Volt[1] <=-2.5 && Volt[6] >= 0 && Volt[7] <= -2.5 && Volt[8] >= 0 && Volt[9] <= -2.5)
		         {
                          //fprintf(fp,"\n Channel 2: Band 1 Selected \n");
                          sprintf(tbuf,"Band-1");
			}
		      else if (Volt[0] <= -2.5 && Volt[1] >= 0 && Volt[6] >= 0 && Volt[7] <= -2.5 && Volt[8] >= 0 && Volt[9] <= -2.5)
		         {
                          //fprintf(fp,"\n Channel 2: Band 2 Selected \n");
                          // sprintf(tbuf,"%s","BAND2");
                          sprintf(tbuf,"Band-2");
			}
		      else if (Volt[2] <= -2.5 && Volt[3] >= 0 && Volt[6] <= -2.5 && Volt[7] >= 0 && Volt[8] >= 0 && Volt[9] <= -2.5)
		         {
                          //fprintf(fp,"\n Channel 2: Band 3 Selected \n");
                          // sprintf(tbuf,"%s","BAND3");
                          sprintf(tbuf,"Band-3");
                        }
		       else if (Volt[4] >= 0 && Volt[5] <= -2.5 && Volt[8] <=-2.5 && Volt[9] >= 0)
		         {
                          //fprintf(fp,"\n Channel 2: Band 4 Selected \n");
                          //sprintf(tbuf,"%s","BAND4");
                          sprintf(tbuf,"Band-4");
                        }
                       else if (Volt[4] <= -2.5 && Volt[5] >= 0 && Volt[8] <=-2.5 && Volt[9] >= 0)
		         {
                          //fprintf(fp,"\n Channel 2: Band 4 Selected \n");
                          //sprintf(tbuf,"%s","BAND4");
                          sprintf(tbuf,"Band-5");
                        } 
                        
		       else
			{
			 //fprintf(fp,"\n No correct Band Selected in Channel \n");
			 //sprintf(tbuf,"%s","BAND5");
			 sprintf(tbuf,"NoBand");
			}
			 strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "BAND_SELECT_M_CH2");
		         sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%s",tbuf);
		         printf ("\nBAND CH2 = %s\n", tbuf);
        		 resp_msg_cntr++;

// Logic for Solar Attenuator (Channel 1)
		      if (Volt[38] >= 0 && Volt[39] <= -2.5 && Volt[42] >= 0 && Volt[43] <= -2.5 && Volt[44] <= -2.5 && Volt[45] >= 0 && Volt[46] <= -2.5  && Volt[53] >= 0)
		         {
                          //fprintf(fp,"\n Channel 1: 0 db Atennution \n");
                          sprintf(tbuf,"0");
			 }
		      else if (Volt[38] <= -2.5 && Volt[39] >= 0 && Volt[42] <= -2.5 && Volt[43] >= 0 && Volt[44] <= -2.5 && Volt[45] >= 0 && Volt[46] <= -2.5  && Volt[53] >= 0)
		         {
                          //fprintf(fp,"\n Channel 1: 14 db Atennution \n");
                          sprintf(tbuf,"14");
                        }
		      else if (Volt[38] >= 0 && Volt[39] <= -2.5 && Volt[42] >= 0 && Volt[43] <= -2.5 && Volt[44] >= 0 && Volt[45] <= -2.5 && Volt[46] >= 0 && Volt[53] <= -2.5)
		         {
                          //fprintf(fp,"\n Channel 1: 30 db Atennution \n");
                          sprintf(tbuf,"30");
                        }
		      else if (Volt[38] <= -2.5 && Volt[39] >=0 && Volt[42] <= -2.5 && Volt[43] >=0 && Volt[44] >= 0 && Volt[45] <= -2.5 && Volt[46] >= 0  && Volt[53] <= -2.5)
		         {
                          //fprintf(fp,"\n Channel 1: 44 db Atennution \n");
                          sprintf(tbuf,"44");
                        }
                        
                      else if (Volt[38] <=-2.5 && Volt[39] >= 0 && Volt[42] >= 0 && Volt[43] <= -2.5 && Volt[44] <= -2.5 && Volt[45] >= 0 && Volt[46] >= 0  && Volt[53] <= -2.5)
		         {
                          //fprintf(fp,"\n FE Termination \n");
                          sprintf(tbuf,"-1");
                          }
		       else
			{
                        // fprintf(fp,"\n No correct Attenuation selected in Channel 1 \n");
                         sprintf(tbuf,"NoAttn");
    			} 
			  strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"SOL_ATTEN_M_CH1");
                         sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%s",tbuf);
                         printf ("\nSOL ATTN CH1 Value=%s\n", tbuf);
   			  resp_msg_cntr++;

// Logic for Solar Attenuator (Channel 2)
		      if (Volt[10] >= 0 && Volt[11] <= -2.5 && Volt[14] >= 0 && Volt[15] <= -2.5 && Volt[16] <= -2.5 && Volt[17] >= 0 && Volt[18] <= -2.5  && Volt[25] >= 0)
		         {
                         // fprintf(fp,"\n Channel 2: 0 db Atennution \n");
                          sprintf(tbuf,"0");
                        }
		      else if (Volt[10] <= -2.5 && Volt[11] >= 0 && Volt[14] <= -2.5 && Volt[15] >= 0 && Volt[16] <= -2.5 && Volt[17] >= 0 && Volt[18] <= -2.5  && Volt[25] >= 0)
		         {
                         // fprintf(fp,"\n Channel 2: 14 db Atennution \n");
                          sprintf(tbuf,"14");
                        }
		      else if (Volt[10] >= 0 && Volt[11] <= -2.5 && Volt[14] >= 0 && Volt[15] <= -2.5 && Volt[16] >= 0 && Volt[17] <= -2.5 && Volt[18] >= 0 && Volt[25] <= -2.5)
		         {
                         // fprintf(fp,"\n Channel 2: 30 db Atennution \n");
                          sprintf(tbuf,"30");
                        }
		      else if (Volt[10] <= -2.5 && Volt[11] >=0 && Volt[14] <= -2.5 && Volt[15] >=0 && Volt[16] >= 0 && Volt[17] <= -2.5 && Volt[18] >= 0  && Volt[25] <= -2.5)
		         {
                          //fprintf(fp,"\n Channel 2: 44 db Atennution \n");
                          sprintf(tbuf,"44");
                        }
                       else if (Volt[10] <=-2.5 && Volt[11] >= 0 && Volt[14] >= 0 && Volt[15] <= -2.5 && Volt[16] <= -2.5 && Volt[17] >= 0 && Volt[18] >= 0  && Volt[25] <= -2.5)
		         {
                          //fprintf(fp,"\n Channel 2: FE Termination \n");
                          sprintf(tbuf,"-1");
                        }
		       else
			{
                         //fprintf(fp,"\n No correct Attenuation selected in Channel 2 \n");
                         sprintf(tbuf,"NoAttn");
                       }
			  strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"SOL_ATTEN_M_CH2");
                         sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%s",tbuf);
                         printf ("\nSOL ATTN CH2 Value=%s\n", tbuf);
   			  resp_msg_cntr++;

// Logic for Swap and Unswap
		     if (Volt[48] >= 0 && Volt[49] <= -2.5)
			{
                          //fprintf(fp,"\n Unswapped \n");
                          sprintf(tbuf,"0");                     
			}
		     else
                        {	
		          //fprintf(fp,"\n Swapped\n");
		           sprintf(tbuf,"1");
		        }
		           strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "RF_SWAP");
		           sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%s",tbuf);
		           printf ("\nSWAP Value=%s\n", tbuf);
        		   resp_msg_cntr++;
			

// Logic for Noise
			if (Volt[51] <= 1)
			   {
			    //fprintf(fp,"\n Noise OFF\n ");
			    sprintf(tbuf,"OFF");
			   }
			else
			   {
			    //fprintf(fp,"\n Noise ON\n");
			    sprintf(tbuf,"ON");
			   }
			    strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "NOISE");
			    sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%s",tbuf);
		            printf ("\nNoise Value=%s\n", tbuf);
        		    resp_msg_cntr++;
			    
			    
	 

// Logic for Walsh
/*

 		     if (Volt[26] < 1 && Volt[53] > 3.5 && Volt[45] > 3.5 && Volt[46] < 1)
			{
                          //fprintf(fp,"\n Walsh 1 and Walsh 2 ON \n");
                          sprintf(tbuf,"W1ON_W2ON");
			}
		     else if (Volt[26] > 3.5 && Volt[53] < 1 && Volt[45] < 1 && Volt[46] > 3.5)
                        {	
		         // fprintf(fp,"\n Walsh 1 and Walsh 2 OFF\n");
		         sprintf(tbuf,"W1OFF_W2OFF");
			}
		     else if (Volt[26] < 1 && Volt[53] > 3.5 && Volt[45] < 1 && Volt[46] > 3.5)
                        {	
		          //fprintf(fp,"\n Walsh 1 ON and Walsh 2 OFF\n");
		          sprintf(tbuf,"W1ON_W2OFF");
			}
		     else if (Volt[26] > 3.5 && Volt[53] < 1 && Volt[45] > 3.5 && Volt[46] < 1)
                        {	
		          //fprintf(fp,"\n Walsh 1  OFF and Walsh 2 ON\n");
		          sprintf(tbuf,"W1OFF_W2ON");
			}
		     else
			{
				
			}
			  strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr], "WALSH_STAT");
			  sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%s",tbuf);
		         // strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr],tbuf);
        		  resp_msg_cntr++; 
 */
		
		     // fprintf(fp,"\n RF Power CH2   ==> %.3f\t\n",Volt[19]);
		      strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"RF_POW_M_CH2");
                     sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%.3f",Volt[19]);
                     resp_msg_cntr++;
		     // fprintf(fp,"\n RF Power CH1   ==> %.3f\t\n",Volt[47]);
		      strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"RF_POW_M_CH1");
                     sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%.3f",Volt[47]);
                     resp_msg_cntr++;

                     // fprintf(fp,"\n Box Temprature ==> %.3f\t\n",(1000*Volt[52])/80);
                     cb_temp=(1000*Volt[52])/80;
                     strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"CB_TEMP");
         	      sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%5.2f", cb_temp );
         	      printf ("Common Box Temp=%5.2f", cb_temp);
                     resp_msg_cntr++ ; 
               }


void copyTmpBuf(char *tmpbuf, char *rptr, int siz)
{
   int i;
   
    for(i=0;i< sizeof(tmpbuf);i++) tmpbuf[i]=0;
        
    for (i = 0; i < siz; i++)
    {
       tmpbuf[i] = rptr[i];
     //  printf ("MCM RESPONSE COPYING IN Front End BOX STMBUF[%d] ===> %x\n",i, tmpbuf[i] );
      #ifdef DEBUG
         printf (" %x ", tmpbuf[i] );
      #endif
    }
    printf("\n");
}
void copyTmpBuffe(char *tmpbuf, char *rptr, int siz)
{
   int i;
   
    for(i=0;i< sizeof(tmpbuf);i++) tmpbuf[i]=0;
        
    for (i = 0; i < siz; i++)
    {
       tmpbuf[i] = rptr[15+i];
       printf ("MCM RESPONSE COPYING IN Front End BOX STMBUF[%d] ===> %x\n",i, tmpbuf[i] );
      #ifdef DEBUG
         printf (" %x ", tmpbuf[i] );
      #endif
    }
    printf("\n");
}
void copyTmpBufcb(char *tmpbuf, char *rptr, int siz)
{
   int i;
   
    for(i=0;i< sizeof(tmpbuf);i++) tmpbuf[i]=0;
        
    for (i = 0; i < siz; i++)
    {
       tmpbuf[i] = rptr[14+i];
       printf ("MCM RESPONSE COPYING IN COMMON BOX STMBUF[%d] ===> %x\n",i, tmpbuf[i] );
      #ifdef DEBUG
         printf (" %x ", tmpbuf[i] );
      #endif
    }
    printf("\n");
}


void decodeResponse()
{
  printf ("\n IM HERE in decodeREsponse");
  int i,j, rawDataLen, remRespLen, nCmdResps=0, cnt=0, cmdRespLen, nbLog, log1Len, log2Len, tempSize;
  //unsigned char cmdStat, lowCmd, lowSubCmd;
  int cmdStat, lowCmd, lowSubCmd;
  int mcmnb = 5, bothMcms = 0;

  //char *rbuf_ptr ;
  int rcmd = 0, RxCmdNum = -1;
  int FEboxNum;
  char tmp1, tmp2;
  char *cmdName[] = { "setMCMAddr", "NullCmd", "SetIdleMode", "SetScanMode", "SetMeanMode", "setAnalogMask", "setDigMask16Bits", "setDigMask32Bits",\
		      "setDigMask64Bits", "ReadAnalogMask", "ReadDigMask16Bits", "ReadDigMask32Bits", "ReadDigMask64Bits", "ReadVersion", "ReadMode",\
		      "Reboot", "FEControlOld", "FEControlNew", "FE-BoxMon", "CommonBoxMon", "PowerDown" };

   //rbuf_ptr = &rbuff.Msg;
   rawDataLen = remRespLen = rbuff.Length ;
  /* for(j = 0; j <rbuff.Length ; j++) 
       {
		 printf("MCM RESPONSE BUFFER [%d] = 0x%02X\n", j, rbuff.Msg[j]);                      
                     
		}*/

 // printf("++++++++++ START %d \n", resp_msg_cntr);

/*  strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"time");      // VAR0
  sec_tm = read_rtc();
  mktm(rtc, sec_tm);
 // sprintf(time_ar,"%d:%d:%d",rtc.tm_hour, rtc.tm_min, rtc.tm_sec);
  sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%d:%d:%d", rtc.tm_hour, rtc.tm_min, rtc.tm_sec);
   resp_msg_cntr++ ;*/


  // fprintf(stderr, " Response Message Length is %d \n", rawDataLen);

        // Decoding Logic
        while( remRespLen > 0 )
        {
                RxCmdNum = (-1);

		if((int)rbuff.Msg[cnt] != 5)
		{ cnt++; remRespLen--; continue; }

		nCmdResps++;
		if(nCmdResps > 1)   // Next Packet
		{ if(mcmnb != (int)rbuff.Msg[cnt])
		  {	bothMcms++;            }//To check whether both MCMs are used or only one
		}

// 0 is MCM CArd ADDRESS
		//mcmnb = atoi((char *)rbuff.Msg[cnt++]);
  //    printf("mcmnb = %d, cnt = %d \n", (int)rbuff.Msg[cnt], cnt);
      mcmnb = (int)rbuff.Msg[cnt++];

#ifdef DEBUG
  	printf("No Of Response %d, MCMCard %d\n",  nCmdResps, mcmnb);
#endif

                strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"totalresp");
                 sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%d",nCmdResps); resp_msg_cntr++;  // VAR1


                 strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"mcmcard");
                 sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%d",mcmnb); resp_msg_cntr++;  // VAR2
                  strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"time");

                     time_t rawtime = time(NULL);
                    struct tm *ptm = localtime(&rawtime);

                    // printf("The time is: %02d:%02d:%02d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
                      sprintf(time_ar,"%d:%d:%d",ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
                    strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr++],time_ar);

// 1 and 2 is PACKET SIZE
		//cmdRespLen = atoi((char *)rbuff.Msg[cnt]) + (atoi((char *)rbuff.Msg[cnt+1])<<8);

      cmdRespLen = (int)rbuff.Msg[cnt] + (((int)rbuff.Msg[cnt+1]) << 8) ;
#ifdef DEBUG
      printf(" [%d] %d + %d = CMDRESPLEN %d \n", cnt, (int)rbuff.Msg[cnt] ,(int) rbuff.Msg[cnt+1], cmdRespLen);
#endif
      cnt += 2;	// cnt points to byte "0" -- next to "response packet size" bytes
		cnt += 1; 	// cnt points to cmd status
// 4 Packet Command Status
		cmdStat = (int)rbuff.Msg[cnt];
#ifdef DEBUG
      printf("cmdStat = %d, cnt = %d \n", cmdStat, cnt);
#endif
      strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"cmdstat"); // VAR3

		switch (cmdStat)
		{
		   case 0 : strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"MCM Command OK"); resp_code=10; break;
		   case 1 : strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"MCM TimeOut");  break;
		   case 2 : strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"Command Check Sum Error");  break;
		   case 4 : strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"Command unknown");  break;
		   case 8 : strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"Packet too large");  break;
		   case 16 : strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"Answer Message truncated");break;
         default : strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"cmstate_blank");
      }
                 resp_msg_cntr++;
                 // Command Failed send diagnostic
                 if( cmdStat != 0 )
                 {   return ; }
                 // Points to "Number of Logical Packets" bytes
                 cnt++;
                 //cnt++;    //Added by RDB.....................
                 nbLog = (int)rbuff.Msg[cnt];
#ifdef DEBUG
                 printf("*********** nbLog = %d, cnt = %d \n", nbLog, cnt);
                 printf(" Number of logical packets in the response message : %d \n", nbLog);
#endif
                 strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"totallogpkt");  // VAR4
                  sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%d",nbLog);
                  resp_msg_cntr++;
 #ifdef DEBUG
                 printf("totallogpkt %d \n",nbLog);
 #endif
                 if(nbLog == 1) RxCmdNum = 1;
                 cnt++;
                 if( nbLog > 0 )
                 {
                    tempSize = 0;
                    // 6, 7 First Logical Packet Size
                    log1Len = (int)rbuff.Msg[cnt] + ( ((int)rbuff.Msg[cnt+1]) << 8) ;
  //                  printf("<<<<<<<<<<<<< log1Len = %d, cnt = %d \n", log1Len, cnt);
                    tempSize+=2;
                    cnt += 2 ; // Pointing to the byte next to "logical 1 packet Size byte". 8, 9

                    // if log1Len == 4 Ideal; BLANK-8, 9
                     tempSize+=2;
                     cnt += 2 ; // Pointing to MCM Mode byte of logical packet one
//                     printf("tempSize = %d \n", tempSize);
                    // 10 Location for 1- scan, 2- Mean, 3-Limit?
            if(log1Len > tempSize)
               {

                       strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"mcmmode");      // VAR5
                       switch( (int)rbuff.Msg[cnt])
                       {
		         case 1 : strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"Scan mode");
                                   resp_msg_cntr++; cnt++; tempSize++;
                                   break;
		         case 2 : strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"Mean mode");
                                   resp_msg_cntr++; cnt += 2; tempSize += 2;
                                   break;
		         case 3 : strcpy(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"Limits mode");
                                   resp_msg_cntr++; cnt++; tempSize++;
                                   break;
               default : break;
                }

                       strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"anamask"); // 8 bit analog Mask   VAR6
                        for(i=0;i<8;i++) {
	                           dev_Resp.resp_data.prmvalue[resp_msg_cntr][i] = (char) rbuff.Msg[cnt];
                              cnt++;
                        }
                              dev_Resp.resp_data.prmvalue[resp_msg_cntr][i] = '\0'; resp_msg_cntr++;
                            tempSize += 8;
  #ifdef DEBUG
	                printf("Scan data (length: %d) \n", log1Len - tempSize);
  #endif
                       strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],"scandata"); // 64 data points  VAR7
	                for (i = 0; i < (log1Len - tempSize) && (i < 63);  i++) { // Size -13 or -14 i.e. tempSize
	                   dev_Resp.resp_data.prmvalue[resp_msg_cntr][i] = (char) rbuff.Msg[cnt];
 	                   fprintf(stderr, "[ %x ]", dev_Resp.resp_data.prmvalue[resp_msg_cntr][i]);
	                   cnt++; tempSize++;
                   }   dev_Resp.resp_data.prmvalue[resp_msg_cntr][i]='\0';  resp_msg_cntr++;

                      }
                  }

		  if (nbLog == 2)
		  {
                        // Second Packet Length
			tempSize = 0;
//         printf( "  [%d] %d + [%d] %d log2Len %d \n", cnt, (int) rbuff.Msg[cnt], cnt+1, (int)rbuff.Msg[cnt+1], log2Len);
			log2Len = (int) rbuff.Msg[cnt] + ( ((int)rbuff.Msg[cnt+1]) <<8) ;
			cnt += 2;	//Pointing to the byte next to packet-size-byte
			tempSize += 2;

                        // Low and Lower Sub Command
			lowCmd = (int)rbuff.Msg[cnt];
			lowSubCmd = (int) rbuff.Msg[cnt+1]; // SecPAckStart + 3
 #ifdef DEBUG
         printf(" LOWCMD %d LSUBCMD %d \n", lowCmd, lowSubCmd);
 #endif
       	cnt += 2;
			tempSize += 2;
         printf(" LOWCMD %d LSUBCMD %d \n", lowCmd, lowSubCmd);
         printf( "LOG2LEN %d tempSize %d \n", log2Len, tempSize);

                        switch( lowCmd )
                        {
                            case 1 :  switch(lowSubCmd)   // SetProcess
                                      {
                                       case 0 :  rcmd = (int) rbuff.Msg[cnt];
                                                 if( rcmd = 0 ) RxCmdNum = 2;
                                                 else if ( rcmd = 1 ) RxCmdNum = 3;
                                                 else if ( rcmd = 2 ) RxCmdNum = 4;
                                                 sprintf(tmpbuf,"%s","true"); // tmpbuf shall not be null;
                                                 cnt++; tempSize++;
                                                 break;
                                       case 1 :  RxCmdNum = 5;
                                                 copyTmpBuf(tmpbuf,rbuff.Msg,8);
                                                 cnt += 8; tempSize += 8 ;
                                                 break;
                                       case 2 :  RxCmdNum = 6;
                                                 copyTmpBuf(tmpbuf,rbuff.Msg,2);
                                                 cnt += 2; tempSize += 2 ;
                                                 break;
                                       case 3 :  RxCmdNum = 7;
                                                 copyTmpBuf(tmpbuf,rbuff.Msg,4);
                                                 cnt += 4; tempSize += 4 ;
                                                 break;
                                       case 4 :  RxCmdNum = 8;
                                                 copyTmpBuf(tmpbuf,rbuff.Msg,8);                                               
                                                 cnt += 8; tempSize += 8 ;
                                                 break;
                                       default : break;
                                      }
                                  break;

                            case 2 :  switch(lowSubCmd)   // ReadProcess
                                      {
                                       case 0 :  RxCmdNum = 9;
                                                 copyTmpBuf(tmpbuf,rbuff.Msg,8);
                                                 cnt += 8; tempSize += 8 ;
                                                 break;
                                       case 1 :  RxCmdNum = 10;
                                                 copyTmpBuf(tmpbuf,rbuff.Msg,2);
                                                 cnt += 2; tempSize += 2 ;
                                                 break;
                                       case 2 :  RxCmdNum = 11;
                                                 copyTmpBuf(tmpbuf,rbuff.Msg,4);
                                                 cnt += 4; tempSize += 4 ;
                                                 break;
                                       case 5 :  RxCmdNum = 12;
                                                 copyTmpBuf(tmpbuf,rbuff.Msg,8);
                                                 cnt += 8; tempSize += 8 ;
                                                 break;
                                       case 3 :  RxCmdNum = 13;
                                                 tmp1 = ( (int) rbuff.Msg[cnt] & 0xf ) + 0x30;
                                                 tmp2 = ( ((int)rbuff.Msg[cnt] >> 4) & 0xf ) + 0x30;
                                                 sprintf(tmpbuf,"%c.%c",tmp2,tmp1);
                                                 cnt++; tempSize++;
                                                 break;
                                       case 4 :  RxCmdNum = 14;
                                                 switch( (int) rbuff.Msg[cnt] )
                                                 {
		                                   case 0 : sprintf(tmpbuf,"%s","IDLE");
                                                             break;
		                                   case 1 : sprintf(tmpbuf,"%s","SCAN");
                                                             break;
		                                   case 2 : sprintf(tmpbuf,"%s","MEAN");
                                                             break;
		                                   default : sprintf(tmpbuf,"%s","LIMITS");
                                                             break;
                                                 }
                                                 cnt++; tempSize++;
                                                 break;
                                      }
                                      break;

                            case 5 : RxCmdNum = 15;
                                   break;
                            case 8 : RxCmdNum = 20;
                                   break;

                            case 3 : RxCmdNum = 16;
                                    copyTmpBuf(tmpbuf,rbuff.Msg,2);
                                    cnt += 2; tempSize += 2 ;
                                   break;
                            case 6 : RxCmdNum = 17;
                                    copyTmpBuf(tmpbuf,rbuff.Msg,2);
                                    cnt += 2; tempSize += 2 ;
                                   break;

                            case 7 : // Main CB and FE Monitor
                                   printf("\n=> IN Case 7 CommonBox Monitor");
		                    if(lowSubCmd)
                                     {  RxCmdNum = 19;  // CommonBox
#ifdef DEBUG
                                        printf("\n=> Packet Buf CommonBox %d, cnt %d \n", (log2Len - tempSize), cnt);
#endif
                                        printf("\n=> IN RESPONSE.C CASE 7 Packet Buf CommonBox %d, cnt %d \n", (log2Len - tempSize), cnt);
                                        copyTmpBufcb(stmpbuf,rbuff.Msg,(log2Len -tempSize));
                                        stmpbuf[(log2Len-tempSize)+1]='\0';
                                        decodeCommonBox();
                                     } else {
                                        RxCmdNum = 18; // FE BOX Mon
                                        FEboxNum = (int)rbuff.Msg[cnt++]; cnt++; tempSize++;
                                        printf("\n=> RESPONSE.C Case 18 Packet Buf FE BOX %d, cnt %d \n", (log2Len - tempSize), cnt);
                                        copyTmpBuffe(stmpbuf,rbuff.Msg,(log2Len -tempSize));
#ifdef DEBUG
                                         printf("\n=> Packet Buf FE Box %d, cnt %d \n", (log2Len - tempSize), cnt);
#endif
                                         stmpbuf[(log2Len - tempSize) + 1]='\0';
                                        // printf("\n=> DECODE FE BOX Number =======> %d \n",FEboxNum);
                                         decodeFEBox(FEboxNum);
                                     }
                                        cnt += (log2Len -tempSize) ; tempSize += (log2Len - tempSize)  ;

                                   break;

                            default : break;
                        }
                  }

                  if(RxCmdNum >  0 && RxCmdNum != 15 && RxCmdNum != 20 ) {
                   //    sprintf(dev_Resp.resp_data.prmvalue[resp_msg_cntr],"%s",tmpbuf);  // copying first tmpbuf of Data  VAR8
                        sprintf(tmpbuf,"%s",cmdName[RxCmdNum]);
                  //     strcpy(dev_Resp.resp_data.prmname[resp_msg_cntr],tmpbuf); // Command Name
                       printf(" %s \n",tmpbuf);
                    //    resp_msg_cntr++;
                  }
			cnt++;		// Added for checksum -- If next response is there, pointing to that
			remRespLen -= (cmdRespLen+1);
 #ifdef DEBUG
  			printf("Command length: %d, rem. len; %d\n", cmdRespLen, remRespLen);
			printf("\n----------------------------////////\n");
 #endif
       }
       resp_code = 10;
  printf("Decode Resp. End >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");

 }


