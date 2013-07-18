/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Matrix_H
#define Matrix_H

#include "TrigT1RPChardware/NOBXS.h"
#include <cstdlib>
#include <cstring>
#include "TrigT1RPChardware/Lvl1Def.h"
#include "MuonCablingTools/BaseObject.h"
#include "TrigT1RPChardware/RpcBook.h" // rpcbook

class Matrix : public BaseObject {
public:
//****************************************************************************//
//                                                                            //
//          Coincidence Matrix data for ReadOut                               //
//          ===================================                               //
//                                                                            //
//          These data are used by MatrixReadOut                              //
//                                                                            //
// 1) raw data for readout; for more details see also below in input[][]...   //
//                                                                            //
CMAword rodat   [2][2][8*NOBXS][2];     // input signals to CMA; for readout  //
//                                                                            //
// 2) trigger results output; for more details see below                      //
CMAword k_readout[8*NOBXS];             // k-pattern for readout              //
//                                                                            //
// 3) highest triggered threshold                                             //
ubit16  highestthRO[8*NOBXS];  // highest triggered threshold (for ReadOut)//
//                                                                            //
// 4) trigger in overlap regions                                              //
ubit16 overlapRO[8*NOBXS];            // overlap flag (for ReadOut)           //
//                                                                            //
//****************************************************************************//
//
Matrix(int run, int event, CMAword debug,
       int subsys, int proj, int sect, int padadd, 
       int lowhig, int add[2], int locadd);
~Matrix();
void deleteRPCdata();
void reset();
void initRegisters();
void initPointers();
void initRPCpointers();
void initDat();
void putData (int sidemat, int layer, int stripaddress, float time);
void putPatt (const Matrix *p);
void setDefaultConfiguration();
void setRunEvent(int runNum, int eventNum);
void setBCzero (ubit16 offset);
void setDeadTime(ubit16 deadt);
void setDeadTime(ubit16 iside, ubit16 ilayer, ubit16 deadt);
void setDeadTime(ubit16 iside, ubit16 ilayer, ubit16 igroup, ubit16 deadt);
void setDelay(ubit16 iside, ubit16 ilayer, ubit16 delay);
void setDelay(ubit16 iside, ubit16 ilayer, ubit16 igroup, ubit16 delay);
void setPulseWidth(ubit16 length);
void setPulseWidth(ubit16 iside, ubit16 ilayer, ubit16 length);
void setPulseWidth(ubit16 iside, ubit16 ilayer, ubit16 igroup, ubit16 length);
void setMask0(ubit16 iside, ubit16 ilayer, ubit16 ichannel);
void setMask1(ubit16 ithreshold, ubit16 iside, ubit16 imajority, ubit16 ichannel);
void setMask1(ubit16 ithreshold, ubit16 iside, ubit16 imajority);
void setMask1(ubit16 ithreshold, ubit16 iside);
void setMaskReadOut(ubit16 iside, ubit16 ilayer, ubit16 ichannel);
void setMaskReadOut (ubit16 iside, ubit16 ilayer, ubit16 ichannel, ubit16 nchannels); 
void setConfig(int *l, ubit16 *p1, int *k, CMAword *p2, int *q, CMAword *o, 
               sbit32 *g);
void setLocalDirection(ubit16 add, int content);
void setKReadOut(int kToReadout);
void setOverlaThres(int overthres);
void setMajority(ubit16 add, int content);
void setRoad(ubit16 addThres, ubit16 addChn, ubit16 add64, CMAword content);
void setRoad(ubit16 addThres, ubit16 addChn, char road[17]);
void setMatOverlap(ubit16 add, CMAword content);
void setTrigDeadTime(ubit16 deadt);
void setTrigDeadTime(ubit16 igroup, ubit16 deadt);
void setDiagonal(ubit16 add, sbit32 content);
////////////////////////////////////////////////////////////////////////////////
int     getMajority(ubit16 add);
CMAword getRoad(ubit16 addThres, ubit16 addChn, ubit16 add64);
CMAword getMatOverlap(ubit16 add);
void execute();
int getSubsystem();
int getProjection();
int getSector();
int getPad();
int getLowHigh();
int getAddress0();
int getAddress1();
int getLocalAdd();
ubit16 getParams();
ubit16 getOutputThres(ubit16 bunch);
ubit16 getOutputOverl(ubit16 bunch);
sbit16 getBunchPhase();
sbit16 getBunchOffset();
void display();
void dispWind();
void dispWind(ubit16 thres);
void dispDefaultConfiguration();
ubit16 char2int(const char *str, CMAword the32[2]);
void histo(rpcbook *p);  // rpcbook
//
private:
int m_run;
int m_event;
//
////////////////////////////////////////////////////////////////////////////////
//
// debug flags
//
CMAword matrixDebug;
//
//Matrix attributes: subsystem, projection, sector, pad, low/high-pt, address;
//
int subsystem;  //subsystem 0,1
int projection; //projection 0=eta view; 1=phi view
int sector;     //sector address
int pad;        //pad address in this sector
int lowhigh;    //lowhigh_pt type, 0=low; 1=high
int address[2]; //address[0]=eta chamber segment address;
                //address[1]=phi chamber segment address;
int localadd;   //local Matrix address in the pad
//
// dynamic structure for the RPC raw data belonging to this matrix
//
struct rpcdata {
  sbit16    layer; // layer address: 0,1;
  sbit16 stripadd; // channel address: 0,..,31 side x; 0,..,63 side y;
 float          time; // digit time (high accuracy in case of MC events);
  sbit16       BC; // raw Bunch crossing identifier;
  sbit16      DLL; // raw DLL identifier;
  sbit16   masked; // masked bit, if masked==1;
  sbit16  maskto1; // mask_to_1 flag: set to 1 this channel
  sbit16     mark; // flag to mark hit; for internal use only
  sbit16 deadtime; // deadtime
  sbit16    delay; // delay time for this hit, in DLL counts
 rpcdata       *next; // pointer to next digit;
};
rpcdata *datarpc[2]; // pointers to the rpc raw data, one for each
//                   // side of the CMA: 0 for side-x and 1 for side-y;
//
// Coincidence Matrix parameters:
//                          nthres
//                          lowtohigh
//                          toreadout
//
////////////////////////////////////////////////////////////////////////////////
// 
// CMAword rodat[i][j][k][l]
// CMAword input[i][j][k][l];
//               ^  ^  ^  ^
//               |  |  |  |
//               |  |  |  - 64 bits needed
//               |  |  ---- absolute clock bin
//               |  ------- layer address
//               ---------- side address
//
// CMAword prepr[i][j][k][l][m];
//               ^  ^  ^  ^  ^
//               |  |  |  |  |
//               |  |  |  |  - 64 bits needed
//               |  |  |  ---- absolute clock bin
//               |  |  ------- layer address
//               |  ---------- side address
//               ------------- threshold address
//
// CMAword mjori[i][j][k][l][m];
//               ^  ^  ^  ^  ^
//               |  |  |  |  |
//               |  |  |  |  - 64 bits needed
//               |  |  |  ---- absolute clock bin
//               |  |  ------- majority address (0 for 1/2; 1 for 2/2)
//               |  ---------- side address
//               ------------- threshold address
//
// CMAword trigg[i][j][k];
//               ^  ^  ^  
//               |  |  |  
//               |  |  ---- 64 bits needed
//               |  ------- absolute clock bin
//               ---------- threshold address
//
//
////////////////////////////////////////////////////////////////////////////////
//
// CMAword rodat   [2][2][8*NOBXS][2];
   CMAword input   [2][2][8*NOBXS][2]      // input signals to CMA
          ,prepr[3][2][2][8*NOBXS][2]      // prepro pattern in CMA
          ,mjori[3][2][2][8*NOBXS][2]      // majority pattern in CMA
          ,trigg[3][8*NOBXS]               // trigger pattern in CMA
          ;                                // to high-pt CM
//
ubit16 trigger[3][NOBXS];        // triggered thresholds 0=no_trigger; 1=trigger
CMAword triggerOverlap[NOBXS][2];// trigger occured in an overlapping region:
                                 // 0=legt; 1=right; 
CMAword triggerOverlapRO[8*NOBXS][2]; // trigger occured in an overlapping 
                                      // region: 0=legt; 1=right;
//
////////////////////////////////////////////////////////////////////////////////
//                               //
// CMA output                    //
// ==========                    //
//                               //
CMAword k_pattern[8*NOBXS];      // k-pattern for trigger
//CMAword k_readout[8*NOBXS]   ; // k-pattern for readout
ubit16 highestth[NOBXS];         // highest triggered threshold: 
//                               //                       0=no trigger at all;
//                               //                       1="0" threshold;
//			         //                       2="1" threshold;
//                               //                       3="2" threshold
ubit16 overlap[NOBXS];           // overlap flag: 
//                               //          0 = no trigger in overlap regions
//                               //          1 = trigger in overlap low channels
//                               //          2 = trigger in overlap hig channels
//                               //          3 = trigger in overlap low and high
//ubit16 highestthRO[8*NOBXS];   // highest triggered threshold (for ReadOut)
//ubit16 overlapRO[8*NOBXS];       // overlap flag (for ReadOut)
sbit16 BCID;                     // Bunch Crossing IDentifier
////////////////////////////////////////////////////////////////////////////////
 ubit16 *chdly;    // channel delay
 ubit16 *width;    // pulse width
    int *locDi;    // local coincidence direction
    int *kRead;    // address of pattern threshold for readout
CMAword *roads;    // programmed 3 roads
    int *major;    // programmed 3 majorities
CMAword *overl;    // overlapping channels list
 sbit32 *geome;    // geometry
////////////////////////////////////////////////////////////////////////////////
static const ubit16 nthres;     // number of thresholds possible
static const ubit16 nchan[2];   // CMA x-y dimensions
static const float BCtime;      // Bunch-Crossing separation (25 ns)
static const ubit16 NDLLCYC;    // Number of DLL Cycles in one Bunch Crossing
static const float DLLtime;     // Bunch-Crossing IDentifier
static const sbit16 NBunch;     // number of Bunch-Crossings to be considered
static const sbit16 nclock;     // length of the CMA buffers
                                // crossing in the CMA buffers
static const sbit16 timeGroupA; // number of channel in a group for 
                                // timing setting
static const sbit16 timeGroupB; // number of channel in a group for 
                                // timing setting
static const sbit16 wordlen;    // number of bits for a single word in CMAword 
                                // (to be fixed to 32);
sbit16 thisBC;                  // real Bunch Crossing ID 
                                //   (from real data for example...)
ubit16 BCzero;                  // offset for bunch crossing address
///////////////////////// MATRIX CONFIGURATION REGISTERS //////////////////////
ubit16 lowtohigh;    // address of lowpt thres. to fill high-pt CMA
ubit16 toreadout;    // address of the threshold to be sent to readout
ubit16 overlapthres; // address of the threshold to be cosidered in overlap
ubit16 majorities[3];// majorities levels
ubit16 localDirec[2];// Direction of majority, from innermost layer to outermost
                     // layer, in radius. Values & configurations:
                     // = 1 ==> layer 0 channel n --> layer 1 channel n
                     // = 2 ==> layer 0 channel n --> layer 1 channel n-1
                     // = 4 ==> layer 0 channel n --> layer 1 channel n+1
                     // localDirec[0] ==> side 0 (=pivot)
                     // localDirec[1] ==> side 1
sbit16 BunchPhase;
sbit16 BunchOffset;
//
CMAword trigRoad[3][32][2];
//
//               Coincidence trigger roads for each threshold 
//
//               CMAword trigRoad[i][j][k]
//                                ^  ^  ^
//                                |  |  |
//                                |  |  --- half address of a 64-bits word
//                                |  ------- pivot channel address
//                                ---------- threshold address
//
ubit16 channDeadT[2][2][8];
//
//               delays
//               
//               channDeadT[i][j][k]
//                          ^  ^  ^
//                          |  |  |
//                          |  |  |
//                          |  |  -- group index: from 0 to 3 for side=0 (I)
//                          |  |                 from 0 to 7 for side=1 (J)
//                          |  ----- layer address: 0 or 1
//                          -------- side address : 0 (I) or 1 (J)
//
//
ubit16 channDelay[2][2][4];
//
//               delays
//               
//               channDelay[i][j][k]
//                          ^  ^  ^
//                          |  |  |
//                          |  |  |
//                          |  |  -- group index: from 0 to 1 for side=0 (I)
//                          |  |                 from 0 to 3 for side=1 (J)
//                          |  ----- layer address: 0 or 1
//                          -------- side address : 0 (I) or 1 (J)
//
//
ubit16 channMask0[2][2][64];
//
//               masking-to-0
//               
//               channMask0[i][j][k]
//                          ^  ^  ^
//                          |  |  |
//                          |  |  |
//                          |  |  -- channel address
//                          |  ----- layer address: 0 or 1
//                          -------- side address : 0 (I) or 1 (J)
//
//
CMAword channMask1[3][2][2][2];
//
//               masking-to-1 of OR and AND registers
//               
//            channMask1[i][j][k][l]
//                       ^  ^  ^  ^
//                       |  |  |  |
//                       |  |  |  |
//                       |  |  |  -- channel address, in bits
//                       |  |  ----- majority address: 0 (1/2) or 1 (2/2)
//                       |  -------- side address : 0 (I) or 1 (J)                      |
//                       ----------- threshold address 
//
CMAword channReadOutMask[2][2][2];
//
//               masking-to-0 of ReadOut RPC registers
//               
//      channReadOutMask[2][2][2];
//                       ^  ^  ^
//                       |  |  |
//                       |  |  |
//                       |  |  -- channel address, in bits, from 0 to 63
//                       |  ----- layer address: 0 or 1
//                       |------- side address : 0 (I) or 1 (J)                      |
//
//
ubit16 pulseWidth[2][2][8];
//
//               Pulse widths
//               
//               pulseWidth[i][j][k]
//                          ^  ^  ^
//                          |  |  |
//                          |  |  |
//                          |  |  -- group index: from 0 to 3 for side=0 (I)
//                          |  |                 from 0 to 7 for side=1 (J)
//                          |  ----- layer address: 0 or 1
//                          -------- side address : 0 (I) or 1 (J)
//
//
CMAword matOverlap[2]; // overlap registers
ubit16  trigDeadTime[4]; // trigger dead time
sbit32  diagonal[32];  // diagonal map (= pattern for pt=infinite)
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// READOUT parameters                                                        //
//                                                                           //
static const sbit16 ROOffset;     // ROOffset: alignment of CM input and 
                                  //           k-pattern        
//                                // channel time; units: DLL cycles         //
///////////////////////////////////////////////////////////////////////////////
void storeDeadtime();
void masking();
void delay();
void load();
void copyDataToReadOut();
void prepro();
void coincide();
void maskTo1();
void deadTime();
void pulse_width();
void declus();
void majori();
void reduce(ubit16 ia, ubit16 ja, ubit16 ka, ubit16 la, 
            ubit16 nup, ubit16 first);
void shift(CMAword *buffi, CMAword *buffo, ubit16 i);
void makeOut();
void makeTestPattern(ubit16 mode, ubit16 ktimes);
void makeOutPattern();
ubit16 config(ubit16 i, ubit16 *arr);
void   set_to_0 (CMAword *p, sbit16 channel);
void   set_to_1 (CMAword *p, sbit16 channel);
ubit16 bitstatus(const CMAword *p, ubit16 channel);
void inds(ubit16 *i, ubit16 channel);
CMAword intPow (const ubit16 base, const ubit16 expo);
void wind();
void show_attributes();
void disp_CMAreg(ubit16 id);
void dispRegister(const CMAword *p, ubit16 side);
void dispTrigger(const CMAword *p);
void dispBinary(const CMAword *p, __osstream *strdisp);
//
};//end-of-class matrix
#endif
