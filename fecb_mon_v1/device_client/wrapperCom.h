
#ifndef _WRAPPER_COM_H_
#define _WRAPPER_COM_H_

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

//-------  Unet --------//
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


#include "define.h"

extern struct sockaddr_in servaddr;
extern struct sockaddr_in clientaddr;

extern int addr_type, addr_len, caddr_len;
extern req_port, resp_port, mon_port;
extern int sockfd;

typedef enum { TIMEOUT=0, RESET=1, APPLN=2, TOTAL_WSTAT } enum_wrapperStat;

extern ssize_t writen(int fd, void *vptr, ssize_t n);
extern ssize_t readn(int fd, void *vptr, ssize_t n);

extern int chkpollEvent(int *);
extern int sendRespPkt(int *, devResponse *);
extern int recvRespPkt(int *, devResponse *);

extern int sendCmdPkt(int *, parseCMSCmd *);
extern int recvCmdPkt(int *, parseCMSCmd *);
void printDevResp(devResponse *);
void printCmdPkt(parseCMSCmd *);
extern int initServer(void);
extern int initClientConn(int );
extern int so_block(int ,int );
extern int allocComBuf();
void closeSock(int sockfd);
//
//  Form Command
//
#endif
