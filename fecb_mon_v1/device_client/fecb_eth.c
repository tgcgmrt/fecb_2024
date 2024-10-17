/*   Forming intermediate response for all system commands  
     Before forming, Command Validation function should be called,
     Which will validate the command and System.
*/
#include "define.h"
#include "fecb_mcm.h"
#include "fecb_eth.h"
int form_inter_resp(parseCMSCmd *Wrpr_Cmd, devResponse *MCM_Resp)
{
  int loop;

  printf("\nEntering form_inter_resp");

  MCM_Resp->code = cmd_validation(Wrpr_Cmd,MCM_Resp);
  MCM_Resp->event = INTERMEDIATE;
  MCM_Resp->MCM_alarm.level=1;
  switch(MCM_Resp->code)
  {
    case 10 :  strcpy(MCM_Resp->message,"Command received successfully");
               printf("\nCommand received successfully");
               break;

    case 12 :  strcpy(MCM_Resp->message,"Command Syntax Error");
               printf("\nCommand Syntax Error");
               break;
    case 13 :  strcpy(MCM_Resp->message,"Command Incomplete");
               printf("\nCommand Incomplete");
               break;
    case 14 :  strcpy(MCM_Resp->message,"Command Invalid");
               printf("\nCommand Invalid");
               break;
    case 100:  strcpy(MCM_Resp->message,"Error Unknown");
               printf("\nError Unknown");
               break;
    default :  break;
  };

 // ******** Write in to Tx_BasicFlds *********** //
  if ( MCM_Resp->code != 10 )
  MCM_Resp->event = FINAL;

  MCM_Resp->resp_elem.seq = Wrpr_Cmd->cmdelem.seq;
  MCM_Resp->resp_elem.version = Wrpr_Cmd->cmdelem.version;
  strcpy(MCM_Resp->resp_elem.subsysid,Wrpr_Cmd->cmdelem.subsysid);
  strcpy(MCM_Resp->resp_elem.cmd_id,Wrpr_Cmd->cmdelem.cmd_id);
  strcpy(MCM_Resp->resp_elem.cmd_name,Wrpr_Cmd->cmdelem.cmd_name);
  strcpy(MCM_Resp->resp_elem.timestamp,Wrpr_Cmd->cmdelem.timestamp);
  MCM_Resp->resp_elem.priority = Wrpr_Cmd->cmdelem.priority;
  MCM_Resp->resp_elem.timeout = Wrpr_Cmd->cmdelem.timeout;

  // ******** Write in to Tx_DataPkt *********** //
  MCM_Resp->resp_data.numpkt = 0;
  for(loop=0; loop<MAXDATA; loop++)
   {
     strcpy(MCM_Resp->resp_data.prmname[loop],"\0");
     strcpy(MCM_Resp->resp_data.prmvalue[loop],"\0");
   }
  // ******** Write in to Tx_alarm *********** //
  MCM_Resp->MCM_alarm.id = 0;
  MCM_Resp->MCM_alarm.level = 0;
  strcpy(MCM_Resp->MCM_alarm.description,"\0");

  return MCM_Resp->code;
}



// ********* This function will validate command against various checks. ********* //
// **** Syntax error, Invalid, Incomplete command, Other error will be checked **** //

int cmd_validation(parseCMSCmd *Wrpr_Cmd, devResponse *MCM_Resp)
 {
   int cmpl=0, i, j;
  /* char tmpstr[256];

    strcpy(tmpstr,Wrpr_Cmd->cmdelem.cmd_name);
    strcat(tmpstr, Wrpr_Cmd->cmdelem.subsysid);
    printf("\n #### Command received: %s \n", tmpstr);

    strcpy(tmpstr, Wrpr_Cmd->cmdelem.timestamp); printf("time: %s \n", tmpstr);
    strcpy(tmpstr, Wrpr_Cmd->cmdelem.cmd_id);  printf("cmdId: %s \n", tmpstr);

    printf("NumPkt %d \n", Wrpr_Cmd->cmd_data.numpkt);

    for (i=0;i< Wrpr_Cmd->cmd_data.numpkt;i++)
    {
     strcpy(tmpstr,Wrpr_Cmd->cmd_data.prmname[i]); printf("Name %s ", tmpstr);
     strcpy(tmpstr,Wrpr_Cmd->cmd_data.prmvalue[i]); printf("value %s \n", tmpstr);
    } */

  

      if(!strcmp(Wrpr_Cmd->cmdelem.subsysid,SYSTEM_2))
       {
         //printf("\nSubsystem validated");

         if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_200))
          {
           return 10;
          }
         else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_201))
          {
           return 10;
          }
         else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_202))
          {
           
           return 10;
          }
         else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_203))
          {
           return 10;
          }
         else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_204))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_205))
          {
           return 10;
          }
         else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_206))
          {
           return 10;
          }
         else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_207))
          {
           return 10;
          }
         else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_208))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_209))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_210))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_211))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_212))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_213))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_214))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_215))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_216))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_217))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_218))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_219))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_220))
          {
           return 10;
          }
           else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_221))
           {
           return 10;
          }
           else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_222))
          {
           return 10;
          }
           else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_223))
          {
           return 10;
          }
          else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_224))   // SET WALSH
          {
           return 10;
          }
         else
          {
           return 12;     // Incase of Syntax Error in command //
          }
       }
      else
       {
        return 14;
       }

 }


/* Final Response function will in turn call FE_command process function */

// ******** Forming final response ******** //

void FE_form_final_resp(parseCMSCmd *Wrpr_Cmd, devResponse *MCM_Resp)
{

  MCM_Resp->event = command_process_FE(Wrpr_Cmd, MCM_Resp);
  MCM_Resp->code = resp_code; // previously 'code' was here?
  MCM_Resp->resp_data.numpkt = resp_msg_cntr;

  printf("\n ===> form_final_resp  :: EVENT %d CODE %d NUMPKT %d \n", MCM_Resp->event, MCM_Resp->code, MCM_Resp->resp_data.numpkt);
}

/* Forming Alarm Response */
/* void form_alarm(int Alrm_id, int Alrm_lvl)
{
  MCM_Resp.code = 10;
  MCM_Resp.event = 100;

  MCM_Resp.MCM_alarm.id = Alrm_id;
  MCM_Resp.MCM_alarm.level = Alrm_lvl;
  strcpy(Wrpr_Cmd.cmdelem.cmd_name,"domon");
  command_process_FE();
  //sock_write(&Socket, &MCM_Resp, MCM_Resp_Size);

} */


/* Fill Set Data function is called to fill the setting parameter in DOMON response structure */

void fillSetpara(parseCMSCmd *Wrpr_Cmd, devResponse *MCM_Resp)
{

       strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr],"BAND_SELECT_CH1");
       sprintf(MCM_Resp->resp_data.prmvalue[resp_msg_cntr],"%d",fecbSet.freq_set[0]);
       resp_msg_cntr++;

       strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr],"BAND_SELECT_CH2");
       sprintf(MCM_Resp->resp_data.prmvalue[resp_msg_cntr],"%d",fecbSet.freq_set[1]);
       resp_msg_cntr++;

       strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr],"SOL_ATTEN_CH1");
       sprintf(MCM_Resp->resp_data.prmvalue[resp_msg_cntr],"%d", fecbSet.solar_attn[0] );
       resp_msg_cntr++;

       strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr],"SOL_ATTEN_CH2");
       sprintf(MCM_Resp->resp_data.prmvalue[resp_msg_cntr],"%d", fecbSet.solar_attn[1] );
       resp_msg_cntr++;

       strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr], "SET_SWAP");
       sprintf(MCM_Resp->resp_data.prmvalue[resp_msg_cntr],"%d", fecbSet.polSwap);
       resp_msg_cntr++;

       strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr], "FE_WALSH_GROUP");
        sprintf(MCM_Resp->resp_data.prmvalue[resp_msg_cntr], "%d", fecbSet.walshGrp);
        resp_msg_cntr++;

       strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr], "FE_WALSH_SW");
       sprintf(MCM_Resp->resp_data.prmvalue[resp_msg_cntr], "%d", fecbSet.walshSW);
       resp_msg_cntr++;

        strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr], " FE_NGCAL");
       sprintf(MCM_Resp->resp_data.prmvalue[resp_msg_cntr], "%d",fecbSet.noiseCal);
       resp_msg_cntr++;

        strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr], " FE_NG_CYCLE ");
       sprintf(MCM_Resp->resp_data.prmvalue[resp_msg_cntr], "%d",fecbSet.ngCycle);
       resp_msg_cntr++;

       strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr], " RFCM_SW ");
       sprintf(MCM_Resp->resp_data.prmvalue[resp_msg_cntr], "%d",fecbSet.rfcmSW);
       resp_msg_cntr++;

       strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr], "NGSET");
       sprintf(MCM_Resp->resp_data.prmvalue[resp_msg_cntr], "%d",fecbSet.ngSW);
       resp_msg_cntr++;

}


// ******** Processing the FE commands ******** //
int command_process_FE(parseCMSCmd *Wrpr_Cmd, devResponse *MCM_Resp)
{
  int arg_fpscnt=0;
  int i, j ;
  fecbSet.ngCycle=0,fecbSet.rfcmSW = 0,fecbSet.ngFreq=0,fecbSet.walshPat = 0,fecbSet.walshFreq=0; // Added on Date 15/02/2023
  char *fevar[] =  { "band_select_ch1" ,"band_select_ch2" ,"rf_swap" ,"sol_atten_ch1" ,"sol_atten_ch2" ,"fe_ngcal" ,"fe_walsh_sw" ,"fe_walsh_grp" ,"fe_ngcycle" ,"rfcm_sw", "ngset","noisefreq","walshpattern","walshfreq"};
  typedef enum { BAND_SELECT_CH1, BAND_SELECT_CH2, RF_SWAP, SOL_ATTEN_CH1, SOL_ATTEN_CH2, FE_NGCAL, FE_WALSH_SW, FE_WALSH_GRP, FE_NG_CYCLE, RFCM_SW, NGSET,NOISEFREQ,WALSHPATTERN,WALSHFREQ, TSET};

//  printf("\nEntering Command_Process_FE");


  resp_msg_cntr = 0; // Reset the response
  resp_code = 0;
 // bzero(&fecbSet,sizeof(fecbset));

  if(! strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_200))      // Init
  {
    /* PortInit();
    ADC_Init();
    Set_RTC(Wrpr_Cmd.cmdelem.timestamp); */
    fecbSet.cmdCode = 0;
  }

  else if(! strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_201))      // Reset
  {       fecbSet.cmdCode = 1;        /*
    printf("\n\n****************************\n MCM Reset !!\n****************************\n");
    sock_close(&Socket);
    fecbSet.cmdCode = 1;
    exit(0);  */
  }

  else if(! strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_202))
  {

    fecbSet.cmdCode = 2;
   }

  else if(! strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_203))           // domon
  {

   resp_msg_cntr=0 ;

   strcpy(MCM_Resp->resp_data.prmname[resp_msg_cntr],"time_of_day");

   time_t rawtime = time(NULL);
   struct tm *ptm = localtime(&rawtime);

    printf("The time is: %02d:%02d:%02d\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    sprintf(time_ar,"%d:%d:%d",ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    strcpy(MCM_Resp->resp_data.prmvalue[resp_msg_cntr++],time_ar);
   
  fecbSet.cmdCode = 3; // fecbCtrlCmd();
  fecbCtrlCmd(Wrpr_Cmd);
  fillSetpara(Wrpr_Cmd,MCM_Resp);
  resp_code=10;    // domon successful for timebeing
  return 99;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_204))
  {
  fecbSet.cmdCode = 4;
  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_205))
  {
  fecbSet.cmdCode = 5;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_206))
  {
  fecbSet.cmdCode = 6;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_207))
  {

  fecbSet.cmdCode = 7;
  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_208))
  {
  fecbSet.cmdCode = 8;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_209))
  {
  fecbSet.cmdCode = 9;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_210))
  {
  fecbSet.cmdCode = 10;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_211))
  {
  fecbSet.cmdCode = 11;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_212))
  {
  fecbSet.cmdCode = 12;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_213))
  {
  fecbSet.cmdCode = 13;


  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_214))
  {
  fecbSet.cmdCode = 14;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_215))
  {
  fecbSet.cmdCode = 15;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_216))
  {
  fecbSet.cmdCode = 16;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_217))
  {
  fecbSet.cmdCode = 17;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_218))
  {
  fecbSet.cmdCode = 18;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_219))  // Settime will set RTC //
  {
    //Set_RTC(Wrpr_Cmd.cmdelem.timestamp);
    fecbSet.cmdCode = 19;
    return 12;

  }

  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_220))
  {
    fecbSet.cmdCode = 20;
  }
  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_221))
  {
    fecbSet.cmdCode = 21;
  }
  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_222))
  {
    fecbSet.cmdCode = 22;
  }
  else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_223))
  {
    fecbSet.cmdCode = 23;
  }
   else if(!strcmp(Wrpr_Cmd->cmdelem.cmd_name,Cmd_224))  // SET WALSH
  {
    fecbSet.cmdCode = 24;
  }
  else
  {
    printf("Nothing Matched, Received : %s",Wrpr_Cmd->cmdelem.cmd_name);
    return 12;
  }

/* #ifdef DEBUG
  for (j = 0 ; j < Wrpr_Cmd->cmd_data.numpkt;j ++)
  {
    printf(" Parameter name %s\n",Wrpr_Cmd->cmd_data.prmname[j]);
    printf(" Parameter name %s\n",Wrpr_Cmd->cmd_data.prmvalue[j]);
  }
#endif */   
  for (j = 0 ; j < Wrpr_Cmd->cmd_data.numpkt;j ++)
  { for (i = 0 ; i < TSET; i++ ) {

     if(! strcasecmp(Wrpr_Cmd->cmd_data.prmname[j], fevar[i]))
     { 
         
        switch(i)
        {
             case BAND_SELECT_CH1 :  fecbSet.freq_set[0] = (int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                    
                                     break;
             case BAND_SELECT_CH2 :  fecbSet.freq_set[1] = (int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]); 
                                    
                                     break;
             case SOL_ATTEN_CH1   :  fecbSet.solar_attn[0] = (int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                    
                                     break;
             case SOL_ATTEN_CH2   :  fecbSet.solar_attn[1] = (int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                     
                                     break;
             case RF_SWAP         :  fecbSet.polSwap = (int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                    
                                     break;
             case FE_NGCAL        :  fecbSet.noiseCal = (int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                    
                                     break;
             case FE_WALSH_GRP    :  fecbSet.walshGrp = (int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                     
                                     break;
             case FE_WALSH_SW     :  fecbSet.walshSW = (int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                     if( fecbSet.walshSW == 1 ) fecbSet.walshSW =0;  // Enable, -ve logic
                                     else fecbSet.walshSW = 8;  // Disable, -ve logic
                                     
                                     break;
             case FE_NG_CYCLE     :  fecbSet.ngCycle  = (int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                     switch(fecbSet.ngCycle)
                                     {
                                          case 0 : fecbSet.ngCycle = 0 ; break;
                                          case 25 : fecbSet.ngCycle = 1 ; break;
                                          case 50 : fecbSet.ngCycle = 2 ; break;
                                          case 100 : fecbSet.ngCycle = 3 ; break;
                                     }
                                    
                                     break;

             case RFCM_SW         :  fecbSet.rfcmSW = (int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                    
                                     break;

             case NGSET           :  fecbSet.ngSW = (int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                    
                                     break;

             case NOISEFREQ     :  fecbSet.ngFreq =(int) atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                    printf("######### NOISE FREQUENCY in FECB_ETH is %d\n",fecbSet.ngFreq);
                                    if( fecbSet.ngFreq > 3 || fecbSet.ngFreq  < 0 )
                                    fecbSet.ngFreq = 0;
                                     break;

             case WALSHPATTERN   :  fecbSet.walshPat =(int)atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                    printf("######### WALSH PATTERN in FECB_ETH is %d\n",fecbSet.walshPat);
                                    if( fecbSet.walshPat > 7 || fecbSet.walshPat  < 0 )
                                     fecbSet.walshPat = 0;
                                     break;

             case WALSHFREQ  :  fecbSet.walshFreq =(int)atoi(Wrpr_Cmd->cmd_data.prmvalue[j]);
                                 printf("######### WALSH FREQUENCY in FECB_ETH is %d\n",fecbSet.walshFreq);
                                 /*   if( fecbSet.walshFreq > 3 || fecbSet.walshFreq  < 0 )
                                    fecbSet.walshFreq = 0;*/
                                    break;
       }
     }
          

  } }
 

/*     if( fecbSet.ngFreq > 3 || fecbSet.ngFreq  < 0 )
         fecbSet.ngFreq = 0;

     if( fecbSet.walshPat > 7 || fecbSet.walshPat  < 0 )
         fecbSet.walshPat = 0;

     //if( fecbSet.walshFreq > 3 || fecbSet.walshFreq  < 0 )
         fecbSet.walshFreq = 1;*/
  fecbCtrlCmd(Wrpr_Cmd);
  return 12;
}




