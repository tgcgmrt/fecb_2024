#include"low_level_mcm.h"
#include "usb_rs485.h"
#define DEBUG
/*--------------Add Check Sum function---------------------------------*/
void add_cksum(unsigned char buff[])
{
 int length, i;
 char CS=0;

 length = buff[1] + (buff[2]<<8);
 for(i=1; i<length; i++) CS += *(buff+i);
 CS = ~CS+1;
// length = length+1;
 buff[length] = CS;
/*   #ifdef DEBUG 
   printf("I am inside Cheksum function :\n");
   fprintf(stderr, "I m in Add_chksum%.2x\t\n", (unsigned char)(buff[length])); 
  #endif */
}
/*------------ Write & read function----------------------------------*/
void write_read(unsigned char *buff,unsigned char *buf_ptr0,serialBuff ruff)
{
  
   long bytes=0;
   int length,i,j,k;
   char CS=0;
   float volt;
   time_t tim;
   time(&tim);
  unsigned char  *pcBufRead = NULL, *testBuf = NULL;
   length = buff[1] + (buff[2]<<8);
 
    buff[0]=*buf_ptr0;              
    add_cksum(buff); 
 
    #ifdef DEBUG 
                  for (i=0; i<(length+1); i++)
                  printf("serial PKT in USB_RS485 ====>0x%02X\n",(unsigned char) buff[i]); 
    #endif
    
    DWORD dwBytesWritten = 0,dwBytesInQueue = 0 ;
    DWORD dwRxSize = 2;
    FT_STATUS ftStatus;
    FT_HANDLE ftHandle;
    EVENT_HANDLE eh;
  
          if((ftStatus = FT_OpenEx(DEVICE_ID, FT_OPEN_BY_SERIAL_NUMBER, &ftHandle)) != FT_OK)
                  {
			
			printf("Error FT_OpenEx(%d)\n", ftStatus);
			
		  }
                else
                   {
                      fprintf(stderr," Device opened successfully\n");
                   }   
	
                    ftStatus = FT_SetTimeouts(ftHandle, 1000, 1000);
                       if (ftStatus == FT_OK)
                       {
                           fprintf(stderr," Device SET timeout ok \n");
                       }
                       else 
                       {
                          fprintf(stderr," Device SET timeout FAiled \n");
                       }

                  FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX);  // clean the Tx and RX buffers...

                    ftStatus = FT_SetRts(ftHandle);
                    if (ftStatus == FT_OK) 
                       {
                         // FT_SetRts OK
                          fprintf(stderr,"FT_SetRTS(%d)\n", ftStatus);
                       }
                      else 
                      {
                                // FT_SetRts failed
                                 fprintf(stderr,"ERROR FT_SetRTS(%d)\n", ftStatus);
                      }
       
		   ftStatus = FT_SetBaudRate(ftHandle, 9600); 
                    if (ftStatus == FT_OK) 
                     {
                     // FT_SetBaudRate OK
                     fprintf(stderr,"FT_SetBaudRate(%d)\n", ftStatus);
                    }
                   else 
                   {
                        // FT_SetBaudRate Failed
                        fprintf(stderr,"Error FT_SetBaudRate(%d)\n", ftStatus);

                   }
       
                  // Set 8 data bits, 1 stop bit and no parity
                 ftStatus = FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1,FT_PARITY_MARK);
                  if (ftStatus == FT_OK) 
                    {
                     
                      fprintf(stderr,"SET FT_SetDataCharacteristics(%d)\n", ftStatus);
                    }
                 else 
                    {
                       fprintf(stderr,"Error FT_SetDataCharacteristics(%d)\n", ftStatus);
                    }
                                  
		/* Write */
		if((ftStatus = FT_Write(ftHandle, buff, 1, &dwBytesWritten)) != FT_OK)
                       {
			 printf("Error FT_Write(%d)\n", ftStatus);
			}      
                         /*
                        if((buff[0]==5))
                          {
                            usleep(0);
                            fprintf(stderr,"###### NO DELAY for RABBIT MCM 5\n");
                          }
                         else
                         {
                           usleep(2000);
                           fprintf(stderr,"###### DELAY for OLD MCM \n");
                         }    */   
                      usleep(2000);
                     
                     dwBytesWritten = 0; 
                    ftStatus = FT_SetBaudRate(ftHandle, 9600);

                   ftStatus = FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1,FT_PARITY_SPACE);
                  if (ftStatus == FT_OK)
                    {
                      // FT_SetDataCharacteristics OK
                         fprintf(stderr,"SET FT_SetDataCharacteristics(%d)\n", ftStatus);
                    }
                    else {
                            // FT_SetDataCharacteristics Failed
                          fprintf(stderr,"Error FT_SetDataCharacteristics(%d)\n", ftStatus);
                      
                         }
                                             
                      if((ftStatus = FT_Write(ftHandle, (buff+1),length, &dwBytesWritten)) != FT_OK) {
                        printf("Error FT_Write(%d)\n", ftStatus);
                         }
                     /*    else
                          {
                                for(i=0;i<=dwBytesWritten;i++)
                                printf("We have successfully written buffer %x\n", (unsigned char)buff[i]);
                           }*/
                             
                  ftStatus = FT_ClrRts(ftHandle);
                 if (ftStatus == FT_OK)
                  {
                         // FT_SetRts OK
                         fprintf(stderr,"FT_ClearRTS(%d)\n", ftStatus);
                  }
                  else
                   {
                       // FT_SetRts failed
                     fprintf(stderr,"ERROR FT_ClearRTS(%d)\n", ftStatus);
                   }
                          
                       pcBufRead = (char*)malloc(sizeof(char )*512);
                       testBuf = (char*)malloc(sizeof(char )*512);
                       memset(pcBufRead, 0x00, 512); 
                       memset(testBuf, 0x00,512); 
                       
              	        ftStatus = FT_Read(ftHandle, pcBufRead,dwRxSize, &dwBytesInQueue);
                        if (ftStatus == FT_OK) 
                         {
                             if(dwBytesInQueue == 0 ) 
                             {
                                 fprintf(stderr,"############### MCM DEVICE TIMEOUT ###########\n\n\n\n\n\n"); 
                                          
                             } 
			     else
                               { 
                                  printf("######### RESPONSE FROM THE SYSTEM###########\n");
                                  printf("MCM ADDRESS IS=>%d\n",(int)pcBufRead[0]);                                              
                                  printf("PACKET LENGTH IS=>%d\n",(int)pcBufRead[1]);
                                                     rbuff.Msg[0]= pcBufRead[0];
                                                     rbuff.Msg[1]= pcBufRead[1];
                                                   #ifdef DEBUG
                                                      printf("##### RBUFF ====>MCM ADDRESS IS=>%d\n",(int)rbuff.Msg[0]);                                              
                                                      printf("$$$$$$ RBUFF PACKET LENGTH IS=>%d\n",(int)rbuff.Msg[1]);
                                                   #endif
                                               
                                }
			    }              
			    else 
                           {
                                fprintf(stderr,"ERROR FT_TIMEOUT(%d)\n", ftStatus);
				printf("Device TIMEOUT\n"); 
			    } 
                          

                               sprintf(testBuf,"%x",pcBufRead[1]);
  
                               dwRxSize = (DWORD)strtoul( testBuf, NULL, 16); 
                               rbuff.Length= (dwRxSize+1); 
                              /*    if ( rbuff.Length <= 0 )
                               { resp_code = 11; }
                               else { resp_code = 10; } */
   
                                 if (dwRxSize == 0 ) 
                                      goto again; 

                                  dwRxSize = dwRxSize-1;                         

                                     dwBytesInQueue = 0;  
                                     ftStatus = FT_Read(ftHandle, pcBufRead,dwRxSize, &dwBytesInQueue);
                                     for(j = 0; j <rbuff.Length ; j++) 
                                      {                     
                                        rbuff.Msg[j+2] = pcBufRead[j];                       
	                              }

                                     if (ftStatus == FT_OK) 
                                       {
	                        	 /* if(dwRxSize == 82)
                                           {  
                                               fprintf(f,"==========> FE Monitoring Data =================>\n"); 
                    
                                                for(j=17;j<(dwRxSize);j++)
                                               {
                                                      k= j-17;
                                                      volt = -5+0.001*(float)pcBufRead[j]*39.0625;
                                                      fprintf(stderr,"Channel[%d]=>Raw data is 0x%02X\t Voltage =>%.3f\n",k,pcBufRead[j],volt);
                  
                                               }
                                                     fprintf(f,"==========> FE Monitoring Done ===========>\n\n\n"); 
                                           }
                                           else if(dwRxSize == 46)
                                            {  
                    
                                                for(j=14;j<(dwRxSize);j++)
                                                  {
                                                         k= j-14;
                                                         volt = -5+0.001*(float)pcBufRead[j]*39.0625;
                                                         fprintf(f,"Channel[%d]=>Raw data is 0x%02X\t Voltage =>%.3f\n",k,pcBufRead[j],volt);
                    
                                                         fflush(f);
                                                  }
                                                   fprintf(f,"\n"); 
                                                      fflush(f);
                                             }
		                           else if(dwRxSize == 67)
                                            {  
                    
                                                   for(j=12;j<(dwRxSize);j++)
                                                    {
                                                            k= j-12;
                                                            volt = -5+0.001*(float)pcBufRead[j]*39.0625;
                    
                                                           fprintf(fp,"%.3f\t",volt);
                                                            fflush(fp);
                                                     }
                  
                                              }
                                              else
                                                {*/
                     
                                                    /*  for(j = 0; j < dwRxSize; j++) 
                                                      {
		                                        printf("MCM RESPONSE BUFFER [%d] = 0x%02X\n", j, pcBufRead[j]);
                       
                     
		                                      } */
                                                  printf("\t\t*************************************************\n");
                                                  printf("\t\t############ MCM COMMAND SUCCESSFUL #############\n");
                                                  printf("\t\t*************************************************\n");
                                                  printf("\n");
                                                  if ( rbuff.Length <= 0 )
                                                    { resp_code = 11; }
                                                     else { resp_code = 10; }                    

               
                                               // }
		                            }
			          else 
                                    {
                                        fprintf(stderr,"ERROR FT_TIMEOUT(%d)\n", ftStatus);
				          
			            }

                  
                        again:
                           free(pcBufRead);
                           free(testBuf);                             	
		        FT_Close(ftHandle);
       
}

