#include <stdio.h>
#include <unistd.h>    /* Symbolic Constants */
#include <fcntl.h>
#include <sys/types.h> /* Primitive System Data Types */
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <setjmp.h>
#include <signal.h>
#include <pthread.h>
#include <poll.h>

#include "define.h"
#include "wrapperCom.h"
#include "fecb_eth.h"

int reqFd ;
unsigned int time_interval;

jmp_buf set_pt ;  /* typedef int */

char *fecbfifo;
int fifo_fd;      /* FIF0 File Descriptor for writing FECB command, Which can be read by USb-RS485 program */ 
char *respfecb;
int resp_fecbfd;   

float temp;

int resp_msg_cntr;
int resp_code;
char mcm_addr;

char temp_ar[20], time_ar[20] ;

parseCMSCmd ser_Cmd;
devResponse dev_Resp, femonPkt;



 static void signal_handler( int ) ;

     static void signal_handler(int sig){
   
             unsigned int time_interval;
             switch (sig)     {
               case SIGHUP  :  
               case SIGINT  :  
               case SIGSEGV :   
               case SIGPIPE :
                               longjmp(set_pt,sig);
               default:       fprintf(stderr, " signal_handler() : Signal received %d \n",sig);
             }
     }


void main(int argc, char *argv[])
{
    
    parseCMSCmd *cmdptr;
    devResponse *respptr;
    int rc;
    static int return_from_longjmp_to_setjmp ;
    int catchRevent = 0;
    static int timeOutCounter=0;
    fecbfifo = FECB_FIFO; 
    respfecb = RESP_FECB;

      fifo_fd = open(fecbfifo,O_WRONLY|O_NONBLOCK); 
      if(fifo_fd == -1)
      {
          printf("Problem in Opening FECB FIFO\n");
          perror(argv[0]);
          exit(1);
          
      }
      else
      {
           printf("FECB FIFO ID in Command_process() ====> %d\n",fifo_fd);
      }

      resp_fecbfd = open(respfecb,O_RDWR|O_NONBLOCK); 
      if(resp_fecbfd == -1)
      {
         printf("Problem in Opening RESPONSE FECB FIFO \n");
         perror(argv[0]);
         exit(1);
      }
      else
      {
        printf("RESPONSE FECB FIFO ID in AppendAdd() ====> %d\n",resp_fecbfd);
      }
        
       /*  unlink("/tmp/fecmd_fifo");
         unlink("/tmp/resp_fecb");

                 
    if (mkfifo (fecbfifo, 0666) == -1)
       {
         printf("Problem in CREATING FECB FIFO \n"); 
         printf("Unable to create a fifo; errno=%d\n",errno);
       } 
       else
         {
           printf("Successfully in CREATING FECB COMMAND FIFO \n"); 
         }
   if (mkfifo (respfecb,0666) == -1)
   {
       printf("Problem in CREATING ### RESPONSE FECB FIFO \n");
       printf("Unable to create a RESPONSE FECB FIFO ; errno=%d\n",errno); 
   }
   else
     {
       printf("Successfully in CREATING FECB RESPONSE FIFO \n"); 
     }*/


     fprintf(stderr, " BasicFlds %ld \n", sizeof(BasicFlds));
     fprintf(stderr, " DataPkt %ld \n", sizeof(DataPkt));
     fprintf(stderr, " RespAlarm %ld \n", sizeof(RespAlarm));
     fprintf(stderr, " devResponse %ld \n", sizeof(devResponse));
     fprintf(stderr, " parseCMSCmd %ld \n", sizeof(parseCMSCmd));

  /* To catch the consumCMS termination so that  DeviceComm can be reset */
     (void) signal(SIGHUP,SIG_IGN); (void) signal(SIGINT,signal_handler);
     (void) signal(SIGQUIT,signal_handler); (void) signal(SIGILL,signal_handler);
     (void) signal(SIGTRAP,signal_handler); (void) signal(SIGIOT,signal_handler);
     (void) signal(SIGFPE,signal_handler);  (void) signal(SIGBUS,signal_handler);
     (void) signal(SIGSEGV,signal_handler); (void) signal(SIGPIPE,signal_handler);
     (void) signal(SIGALRM,signal_handler); (void) signal(SIGTERM,signal_handler);
     (void) signal(SIGUSR1,signal_handler); (void) signal(SIGUSR2,signal_handler);

     if ( (cmdptr = (parseCMSCmd *) calloc(1,parseCMSCmdSize)) == NULL ) {
        fprintf(stderr, " ### ERROR : deviceClient() : Memory for cmdptr could not allocate \n");
        exit(1);
     }        


     if ( (respptr = (devResponse *) calloc(1,devResponseSize)) == NULL ) {
        fprintf(stderr, " ### ERROR : deviceClient() : Memory for cmdptr could not allocate \n");
        exit(1);
     }        

     do {

        if( (reqFd = initClientConn(reqPORT)) == ERROR ) {
            fprintf(stderr, "### ERROR : deviceClient() reqFd %d: could not connect to RequestPort %d \n", reqFd, reqPORT);
        }else {
            fprintf(stderr, "deviceClient() : DO LOOP Successfully connected to RequestPort %d #####REQUEST=>FD %d\n", reqPORT,reqFd);
           
        }
        sleep(1);
    } while (reqFd < 0 );

  /* setjmp return 0 if success */
   if((return_from_longjmp_to_setjmp = setjmp(set_pt)) != 0 )
   {
          rc = return_from_longjmp_to_setjmp ;
          fprintf(stderr, " \n#### singal_handler() :: Catched  signal %d \n",rc);
          if (reqFd > 0) {
            closeSock(reqFd); // If Program receives SIGPIPE Signal, Device Client closes the Socket FD Date 29/04/2019 
          }
    
        do {
          if( (reqFd = initClientConn(reqPORT)) == ERROR ) 
          {
              fprintf(stderr, "### ERROR : deviceClient() reqFd %d: could not connect to RequestPort %d \n", reqFd,reqPORT);
          }
          else
          {
              fprintf(stderr, "deviceClient() : LONG JUMP LOOP Successfully connected to RequestPort %d \n", reqPORT);
          }
          sleep(1);
        } while (reqFd < 0 );
        timeOutCounter=0;
  }

  while(1) 
  {
           // Read event
      if ( (catchRevent = recvCmdPkt(&reqFd,cmdptr)) == 0 )
      {
         timeOutCounter=0;
         printCmdPkt(cmdptr);
        
         form_inter_resp(cmdptr,respptr);
        // usleep(10000);
         sendRespPkt(&reqFd,respptr);
         FE_form_final_resp(cmdptr,respptr);
         respptr= &dev_Resp;
         sendRespPkt(&reqFd,respptr);
      }
        //  printDevResp(respptr);
         // close(fifo_fd);
         //formDeviceResp(cmdptr, respptr);
      
      if (  (catchRevent&POLLHUP) || (catchRevent&POLLNVAL) || timeOutCounter > DOMON_TIMEOUT ) // TIMEOUT in sec
       {
                  fprintf(stderr," checkDevEvent : pollfd got singla %d \n",catchRevent);  
                  signal_handler(SIGPIPE);
       }

      if (catchRevent & POLLERR)
        {
            closeSock(reqFd);
            fprintf(stderr," checkDevEvent : pollfd got singla %d \n",catchRevent);
            signal_handler(SIGPIPE);
        }
 
      usleep(300000);
      timeOutCounter += 300000;
  }

}
