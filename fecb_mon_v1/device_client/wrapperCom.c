#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <poll.h>

#include "wrapperCom.h"


#define ERROR  (-1)

int addr_type = 0;
int addr_len = 0;
int caddr_len = 0;
int req_port = 0;
int resp_port = 0;
int mon_port = 0;
int sockfd = 0;

enum { MaxProtBuf = 16384};

struct sockaddr_in servaddr;
struct sockaddr_in clientaddr;

/*extern void sig_child(int signo) ; */
static unsigned char *cmdBuf, *respBuf;
static unsigned char  *read_ptr ,*write_ptr ;


ssize_t writen(int fd, void *vptr, ssize_t nbytes)
{
  ssize_t l, nwritten, n = 0;
  extern unsigned char  *write_ptr ;


  write_ptr = ( unsigned char  *) vptr;
  while ( n < nbytes) {

     if (nbytes - n > MaxProtBuf) l = MaxProtBuf; else l = nbytes - n;

       so_block(fd,0); 
        nwritten = write(fd,write_ptr+n,l);
    // nwritten = send(fd,write_ptr+n,l, 0);
 //     ioctl (fd, I_FLUSH, FLUSHW); /* Flush the data to Socket */
      
       so_block(fd,1);
     

    if (nwritten <= 0) {
      if (errno == EINTR)
        nwritten = 0;
      else
        return -1;
    }
    n+=nwritten;
  }
  return (n);
  
}

ssize_t readn(int fd, void *vptr, ssize_t nbytes)
{
  ssize_t nread;
  ssize_t l, n = 0;
  extern unsigned char  *read_ptr ;

   read_ptr = ( unsigned char  *) vptr;
  while (n < nbytes) {
       if ((nbytes - n )> MaxProtBuf) l = MaxProtBuf; else l = nbytes - n;

   //    nread = read(fd,read_ptr+n,l);

 //       so_block(fd,0); 
        nread = recv(fd,read_ptr+n,l,MSG_DONTWAIT);
  //      so_block(fd,1); 
     
      // ssize_t n = recv(fd,read_ptr,nbytes,0); 
      //ioctl (fd, I_FLUSH, FLUSHR);  /*Flush the data from Socket  */

    if (nread < 0) {
      if (errno == EINTR || errno == EWOULDBLOCK) nread = 0;
        return -1;
    } else if (nread == 0) {
      break;
    }
     n+=nread;
  }
  return (n);
  
}

int allocComBuf() 
{
   extern unsigned char *cmdBuf, *respBuf;

   if ( (cmdBuf = (unsigned char *) calloc(1,parseCMSCmdSize)) == (unsigned char *)NULL ) 
    { fprintf(stderr, " allocComBuf : could not allocate memory\n");
      return ERROR ;
    }

   if ( (respBuf = (unsigned char *) calloc(1,devResponseSize) ) == (unsigned char *)NULL ) 
    { fprintf(stderr, " allocComBuf : could not allocate memory\n");
      return ERROR ;
    }
    return 0;
}

 int sendCmdPkt(int *fd, parseCMSCmd *cmdPKT)
 {
   ssize_t bytes= -1;

    if ( (bytes=writen(*fd, (unsigned char*)cmdPKT, parseCMSCmdSize)) != parseCMSCmdSize ) {
            fprintf(stderr, "### Error: sendSystemPkt() Command Failed [%d] !!\n",(int) bytes);
            // Check machine is alive or not by ping routine
            // close timer 0, queue reset, thread close ; Taking care of corrupt packet while reading
            // reset enable
            return 1;
    } else {
            fprintf(stderr, "sendSystemPkt() Number of Bytes [%d]\n",(int) bytes);
            return 0 ;
    }
 }

//  Check the TCP/IP connection broken or Data ready
//
void closeSock(int sockfd)
{
    if(  shutdown(sockfd, SHUT_RDWR) == ERROR) {
           fprintf(stderr, " consumeCMS: terminateConsumer() Socket fd %d not closed \n",sockfd);
     }
     if ( close(sockfd) == ERROR) {
           fprintf(stderr, " consumeCMS: terminateConsumer() Socket fd %d not closed \n",sockfd);
     }
}
 int chkpollEvent(int *fd)
 {
   struct pollfd recvpoll;

   bzero(&recvpoll,sizeof(struct pollfd));
   recvpoll.fd = *fd;

#define _GNU_SOURCE
   recvpoll.events= POLLHUP|POLLNVAL ; // no need to set the flag?
 // recvpoll.events= POLLHUP|POLLRDHUP|POLLNVAL ; // no need to set the flag?

    poll(&recvpoll, (nfds_t)1,400);
    switch((short)recvpoll.revents)  {
          case POLLIN : fprintf(stderr, " POLLIN Data ready %d\n",(short) recvpoll.revents); break;
          case POLLERR : fprintf(stderr, " POLLERR %d\n",(short)recvpoll.revents);  break;
    }

   if  ( ( recvpoll.revents & POLLHUP ) || ( recvpoll.revents & POLLNVAL ) ){
          fprintf(stderr, " ### Error recvRespPkt() : poll - Signal occurred  POLLHUP|POLLNVAL  %d\n",(short)recvpoll.revents);
          return (int) recvpoll.revents;
 //       case POLLRDHUP : fprintf(stderr, " ### Error recvRespPkt() : poll - Signal occurred  POLLRDHUP  %d\n",(short)recvpoll.revents);
    } 

      return 0 ;
 }
 
 int recvCmdPkt(int *fd, parseCMSCmd *cmdPKT)
 {           
   ssize_t bytes= -1;
   int recvEvent;

   if( ( recvEvent = chkpollEvent(fd)) != 0 ) return recvEvent;
   bzero(cmdPKT,parseCMSCmdSize);

  if ( (bytes = readn(*fd, (unsigned char *)cmdPKT, parseCMSCmdSize)) != parseCMSCmdSize) {
      //fprintf(stderr, "WARNING : recvCmdPkt() Command not received [%d] !!\n",bytes); //SSK
      return 1;
  } else {
    fprintf(stderr, "recvCmdPkt() Number of Bytes [%ld] \n", bytes);
    printCmdPkt(cmdPKT);
    return 0 ;
  }

 }

 int sendRespPkt(int *fd, devResponse* devResp)
 {
   ssize_t bytes= -1;

     if( (bytes = writen(*fd,(unsigned char*)devResp, devResponseSize)) != devResponseSize )
     {
       fprintf(stderr, "### Error: sendRespPkt()  Failed [%d]!! \n",(int) bytes);
              return 1;
     }else {
        fprintf(stderr, "sendRespPkt() Command send %d [%d] !!\n",(int) bytes, (int)devResponseSize);
        return 0;
     }
 }

 int recvRespPkt(int *fd, devResponse *devResp)
 {           
   ssize_t bytes= -1;
   int recvEvent;

   if( ( recvEvent = chkpollEvent(fd)) != 0 ) return recvEvent;
   bzero(devResp,devResponseSize);

  if ( (bytes = readn(*fd, (unsigned char *)devResp, devResponseSize)) != devResponseSize ) {
         fprintf(stderr, "### Error : recvRespPkt() : Response not received [%d] !!\n", (int)bytes);
       // Check machine is alive or not by ping routine
      return 1;                
  } else {                   
    fprintf(stderr, " #### recvRespPkt() : Response received %d [%d] \n ", (int)bytes, (int)devResponseSize);
  // printDevResp(devResp);
    return 0;
  }                   

 }


void printBasicFld(BasicFlds *bptr) {
    fprintf(stderr, "\n\t seq [%d]  Subsystemid [%s]  \n\t ID [%s] CMDName [%s] \n\t TimeStamp [%s] \n",
       bptr->seq, bptr->subsysid, bptr->cmd_id, bptr->cmd_name, bptr->timestamp);

}

void printAlarmPkt(RespAlarm *alrm)
{
   fprintf(stderr, " Id %d %d %s \n", alrm->id, alrm->level, alrm->description);
}

void printDataPkt(DataPkt *dpkt) {
   int i;
    fprintf(stderr, "Number of Arg [%d] \n", dpkt->numpkt);
  
    for(i=0;i<dpkt->numpkt;i++) {
         fprintf(stderr, "%d name %s Value %s \n", dpkt->numpkt, dpkt->prmname[i], dpkt->prmvalue[i]);
    }
   
}

void printDevResp(devResponse *drptr)
{
         printBasicFld(&drptr->resp_elem);
         printDataPkt(&drptr->resp_data);
         printAlarmPkt(&drptr->MCM_alarm);
         fprintf(stderr, " Code %d  Event %d Message %s \n", drptr->code, drptr->event, drptr->message);

}
void printCmdPkt(parseCMSCmd *cmdptr)
{
         printBasicFld(&cmdptr->cmdelem);
         printDataPkt(&cmdptr->cmd_data);
}

/* flag , 1 non block/no delay ; 0 block */
/* opts = (opts | O_NONBLOCK);*/

//
// client
//
int so_block(int sockfd,int flag)
{ int flg;
     flg = fcntl(sockfd, F_GETFL, &flg);
     if (flg < 0) return flg;
     if (flag){ flg |= O_NDELAY; }
     else { flg &= ~O_NDELAY; }
     return fcntl(sockfd, F_SETFL, flg);
}

int initClientConn(int clientPort)
{
   int sock=ERROR;
   char *envptr;
   char hostname[128];
   struct hostent *host;
   struct sockaddr_in server_addr; int reuse=1;

   switch (clientPort)
   {
     case reqPORT :
        if( ( envptr = ( char * ) getenv("WRPR_PORT")) != NULL ) {
              resp_port = atoi(envptr);
        } else {   resp_port = WRPR_PORT; }
        if( ( envptr = ( char * ) getenv("WRPR_IP")) != NULL ) {
              strcpy(hostname,envptr);
        } else {   strcpy(hostname,WRPR_IP); } 
       break ;

     case resPORT :
        if( ( envptr = ( char * ) getenv("RESP_PORT")) != NULL ) {
              resp_port = atoi(envptr);
        } else {   resp_port = RESP_PORT; }
        if( ( envptr = ( char * ) getenv("RESP_IP")) != NULL ) {
              strcpy(hostname,envptr);
        } else {   strcpy(hostname,RESP_IP); } 
       break ;

     case evntPORT :
        if( ( envptr = ( char * ) getenv("EVNT_PORT")) != NULL ) {
              resp_port = atoi(envptr);
        } else {   resp_port = EVNT_PORT; }
        if( ( envptr = ( char * ) getenv("EVNT_IP")) != NULL ) {
              strcpy(hostname,envptr);
        } else {   strcpy(hostname,EVNT_IP); }
       break ;

     case monPORT :
        if( ( envptr = ( char * ) getenv("MON_PORT")) != NULL ) {
              resp_port = atoi(envptr);
        } else {   resp_port = MON_PORT; }
        if( ( envptr = ( char * ) getenv("MON_IP")) != NULL ) {
              strcpy(hostname,envptr);
        } else {   strcpy(hostname,MON_IP); } 
       break ;

 
     default :
        fprintf(stderr, "### ERROR : initClientConn() : Invalid Port %d \n",  clientPort); return ERROR;
   }

   host = gethostbyname(hostname);

   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR ) {
       perror("initRespPortCMS() ");
       return ERROR ;
   }
 reuse=1;
 if(setsockopt(sock, IPPROTO_TCP,TCP_NODELAY, &reuse,sizeof(reuse)) == ERROR)
 { perror("setsockopt_TCP_NODELAY"); return ERROR; }   
/*
  reuse=parseCMSCmdSize + 1024;
 if( setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &reuse, sizeof(reuse)) == ERROR)
 { perror("setsockopt_SO_KEEPALIVE"); return ERROR; }
 reuse=devResponseSize + 1024;
 if( setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &reuse, sizeof(reuse)) == ERROR)
 { perror("setsockopt_SO_KEEPALIVE"); return ERROR; }
*/


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(resp_port); // as client
    server_addr.sin_addr =*((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == ERROR ) {
         closeSock(sock);
         perror("initClientConn() ");
         return ERROR ;
    }
    fprintf(stderr, " initClientConn() : hostname- %s resp_port %d Connection Successful.\n",hostname, resp_port);
    return sock ;
}

int initServer( )
{
  extern int sockfd;
  extern struct sockaddr_in servaddr;
  extern int req_port;
  int reuse = 1 ;
  struct linger wrplinger = { 0 , 0 };
  char *envptr;

   
   if( ( envptr = ( char * ) getenv("WRPR_PORT")) != NULL ) {
         req_port= atoi(envptr);
   } else {   req_port = WRPR_PORT; }

  fprintf(stderr, " init_server() WRPR_PORT %d \n",req_port);

/*
  gethostname(hostname, sizeof(hostname));
  if ((wrpsrv_host = gethostbyname(hostname)) == NULL) return ;
  addr_type = wrpsrv_host->h_addrtype; addr_len = wrpsrv_host->h_length;
  wrpsrv_net_addr = *(u_long *)wrpsrv_host->h_addr;
*/

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(req_port);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   
  bzero(&(servaddr.sin_zero),8);
 /*servaddr.sin_addr.s_addr = wrpsrv_net_addr ; */

  if( (sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    fprintf(stderr, " \n#### Error : init_server() Unable to open socket !.\n");
    return ERROR;
  }


/* get & clear the errors on socket 
 if( getsockopt(sockfd, SOL_SOCKET, SO_ERROR, NULL ,  &optval) == ERROR) 
 { perror("setsockopt-SO_ERROR"); return ERROR; }  */
/* Disabling SO_LINGER OPtion as on some OS it may cause dangeour 17/10/2011 JPK  */

 if( setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &wrplinger, sizeof(wrplinger)) == ERROR)
 { perror("setsockopt-SO_LINGER"); return ERROR; } 

 if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == ERROR)
 { perror("setsockopt-SO_REUSEADDR"); return ERROR; }

/*
 if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) == ERROR) 
 { perror("setsockopt-SO_REUSEPORT"); return ERROR; } 
 if( setsockopt(sockfd, SOL_SOCKET, SO_DEBUG, &reuse, sizeof(reuse)) == ERROR)
 { perror("setsockopt_SO_DEBUG"); return ERROR; }
KEEP ALIVE function can be engage the port till 2 hours hence avoiding this 
option 17/10/11 JPK
*/
 if( setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &reuse, sizeof(reuse)) == ERROR)
 { perror("setsockopt_SO_KEEPALIVE"); return ERROR; }

/*
 reuse=parseCMSCmdSize + 1024;
 if( setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &reuse, sizeof(reuse)) == ERROR)
 { perror("setsockopt_SO_KEEPALIVE"); return ERROR; }
 reuse=devResponseSize + 1024;
 if( setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &reuse, sizeof(reuse)) == ERROR)
 { perror("setsockopt_SO_KEEPALIVE"); return ERROR; }
*/

 reuse=1;
 if(setsockopt(sockfd, IPPROTO_TCP,TCP_NODELAY, (char *)&reuse,sizeof(reuse)) == ERROR)
 { perror("setsockopt_TCP_NODELAY"); return ERROR; }   


/*
 if( setsockopt(sockfd, SOL_SOCKET, 15, &reuse, sizeof(reuse)) == ERROR)
 { perror("setsockopt_15"); return ERROR; } */

  if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr )) < 0) {
    fprintf(stderr,"init_server() : Unable to bind to WRPR_PORT %d\n", req_port);
    closeSock(sockfd);
    return ERROR;
  }

  listen(sockfd,10); // How many clients can connect
  fprintf(stdout," init_server() : WRAPPER SERVER LISTENING ON PORT %d\n", req_port);

  return 0 ;
}
