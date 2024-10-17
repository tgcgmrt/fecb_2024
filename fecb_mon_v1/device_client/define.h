#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED

#include<stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>


#include <sys/types.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <netdb.h>
#include <setjmp.h>
#include <signal.h>
#include<time.h>

#define DOMON_TIMEOUT  60000000  // 1.5m

#define CMS_IP "192.168.3.35"

//----------------------Case:1 Serial+Ethernet Rabbit Card Testing in FELAB with GUI Server(C21)-----------------------------------
//----------------------USB-RS485 Cable connected form Laptop to FE TEST Setup Box------------------------------------------------
#define WRPR_IP  "192.168.70.120"      // **WRPR_IP  for FELAB testing Give C21 IP during Serial Testing in FELAB with c21 GUI SERVER
#define WRPR_PORT  3005                 //In Terminal 1.sudo ./mcmtest  2../deviceClient 3.server on GUI

#define SERVER_ADDR    "192.168.70.120"  // **SERVER_ADDR for FELAB testing Give C21 IP during Serial Testing in FELAB 
#define SERVER_PORT    "3005"           // c21 - jive - fecb - 192.168.8.251(Laptop IP--ifconfig) 
				        // GUI - seturfsys -- dashboard domon after 30 seconds   ---TESTED OK in FELAB 26/09/2024
//-------------------------------------------------------------------------------------------------------------------------------

/*
//-------------------Case:2 Serial+Ethernet Rabbit Card Testing in FELAB with local deviceServer(Laptop)-----------------------------------
//----------------------USB-RS485 Cable connected form Laptop to FE TEST Setup Box-----------------------------------------------
#define WRPR_IP  "192.168.8.251"  //**WRPR_IP for FELAB testing Give Laptop IP during Serial Testing in FELAB with local deviceserver(Laptop)
#define WRPR_PORT  3005  

#define SERVER_ADDR    "192.168.8.251"  // **SERVER_ADDR for FELAB testing Give Laptop IP during Serial Testing in FELAB
#define SERVER_PORT    "3005"           // In Terminal 1.sudo ./mcmtest 2../server 3../deviceClient
                              //Commands from Laptop deviceserver ---0 Null 4 seturfsys 3 Domon ---check response in DevicClient window
//-------------------------------------------------------------------------------------------------------------------------------
*/

#define RESP_PORT  5000
#define RESP_IP  "192.168.3.35"

#define EVNT_PORT 5001
#define EVNT_IP  "192.168.3.35"

#define MON_PORT  19999
#define MON_IP   "192.168.3.35"

#define SUBSYS_ID "SimulatedSys"   // SimulatedSys if not defined
#define DEVICE_IP "192.168.37.2" 

typedef enum { reqPORT, resPORT, evntPORT, monPORT } serverPorts;

/**** FIFO Definition for FECB ***/

#define FECB_FIFO "/tmp/fecmd_fifo"

#define RESP_FECB "/tmp/resp_fecb"

extern int reqFd ;

extern unsigned int time_interval;

extern jmp_buf set_pt ;  /* typedef int */

extern char *fecbfifo;
extern int fifo_fd;      /* FIF0 File Descriptor for writing FECB command, Which can be read by USb-RS485 program */ 

extern char *respfecb;
extern int resp_fecbfd;   

extern float temp;

extern int resp_msg_cntr;
extern int resp_code;
extern char mcm_addr;

#define ERROR      (-1)

#define MAXDATA       	128
#define DATALEN       	64
#define MSGLEN		128

typedef enum { WRAPPER=10, INTERMEDIATE=11, FINAL=12, ASYNCH=13, ALARM=100 } EventEnum ;
typedef enum { SUCCESS=10, FAILED=11, SYNTX=12, INCOMPLETE=13, INVALID=14, UNKNOWNERR=100 } CodeEnum ;


extern char temp_ar[20], time_ar[20] ;


#pragma pack(push,1)
typedef struct
 {
    short  id;
    short  level;
    char description[MSGLEN];
 } RespAlarm;

typedef struct
 {
    short numpkt;
    char prmname[MAXDATA][DATALEN];
    char prmvalue[MAXDATA][DATALEN];
 } DataPkt;

// Basic Fields Information resued by XMLIB to form XML ACK/NACK/RESPONSE //
typedef struct
 {
    int  seq;                  // Sequence number of Command
    float version;
    short priority ;
    short timeout ;
    char  subsysid[DATALEN];
    char  cmd_name[DATALEN];        // Command name at user level
    char  timestamp[DATALEN];
    char  cmd_id[8];           // Command ID Device Specific
 } BasicFlds;

typedef struct
 {
    short code;
    short event ;
    char message[MSGLEN];

    RespAlarm MCM_alarm;
    DataPkt resp_data;
    BasicFlds resp_elem;
 } devResponse;

typedef struct
 {
    BasicFlds cmdelem;
    DataPkt cmd_data;
 } parseCMSCmd;
#pragma pack(pop)

#define parseCMSCmdSize  sizeof(parseCMSCmd)
#define devResponseSize  sizeof(devResponse)

extern parseCMSCmd ser_Cmd;
extern devResponse dev_Resp, femonPkt;

#endif
