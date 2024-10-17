#ifndef FECB_MCM_H_INCLUDED
#define FECB_MCM_H_INCLUDED

extern char mcm_addr;
extern double pow_db;
extern char mask1[6], mask2[6], mask3[6], mask4[6];

extern char mask1[6], mask2[6], mask3[6], mask4[6];
void Convert(int offset, char * mask_x);
void Mask32toArg(char * mask1, char * mask2);
void Mask64toArg(char * mask1, char * mask2, char * mask3, char * mask4);
int fecbCtrlCmd(parseCMSCmd *Wrpr_Cmd);
extern void FECB_CTRL(int MCMADDR );
 typedef struct
 {
    int cmdCode;
    int freq_set[2];
    int solar_attn[2]; 
    int polSwap, noiseCal, ngCycle,ngFreq;
    int walshGrp, walshSW,walshPat,walshFreq;
    int rfcmSW, ngSW;
 } fecbset;

 typedef struct
 {
    int CmdNum ;
    int McmPow ;
    int feBoxNo[2];
    unsigned char Argument[256];
 } execcmd;

 typedef struct
 {
    int Length;
    char Msg[200];
 } serialBuff ;


extern execcmd exeCmd;
extern serialBuff wbuff;
extern serialBuff rbuff;
extern char *buf_ptr0;
extern unsigned short bitD0[8], mcm4word[2];
extern unsigned short digWord[32];
extern fecbset fecbSet;


#endif
