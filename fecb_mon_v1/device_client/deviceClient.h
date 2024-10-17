
#ifndef _DEVICE_CLIENT_H_
#define _DEVICE_CLIENT_H_

#define ERROR (-1)
#include "wrapperCom.h"
typedef enum {CMD_IDLE=0, GET_SID_DID=1, GET_DATA=2} command_state_type;

extern command_state_type cmd_state;

extern char formDeviceCmd(parseCMSCmd *cPtr);
extern void formDeviceResp(parseCMSCmd *, devResponse *);


#endif
