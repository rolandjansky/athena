/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_Lvl1RpcTrig_H
#define TrigT1RPChardware_Lvl1RpcTrig_H
#include "TrigT1RPChardware/NOBXS.h"
#include "TrigT1RPChardware/Lvl1RpcConf.h"
#include "TrigT1RPChardware/Matrix.h"
#include "TrigT1RPChardware/MatrixReadOut.h"
#include "TrigT1RPChardware/Pad.h"
#include "TrigT1RPChardware/PadReadOut.h"
#include "TrigT1RPChardware/SectorL.h"
#include "TrigT1RPChardware/RXReadOut.h"
#include "TrigT1RPChardware/RPCROD.h"
#include "TrigT1RPChardware/Lvl1Def.h"
#include "TrigT1RPChardware/RpcBook.h" // rpcbook

class Lvl1RpcTrig {
//this class defines the RPC data in the trigger program structure
public:
Lvl1RpcTrig (Lvl1RpcConf *p, int run, int event);
~Lvl1RpcTrig();
int filterIsOk (int addcha[2], int sbs, int sect, int stat, int lay, 
                int proj, int addloc);
int insert (int cham[2], int sbs, int sect, int stat, int lay, int proj, 
            int addloc, int addabs, float time, float xh[3], float xd[3]);
void testMatrix(int sidemat, int layer, int stripaddress, float time);
//                           
void delete_data();
int  scanning();
void setCMAs();
void resetCMAs();
void deleteCMAs();
void executeCMAs();
void listCMAs();
void setCMRs();
void resetCMRs();
void makeCMAReadOutFile();
void deleteCMRs();
//
void setPADs();
void resetPADs();
void deletePADs();
void executePADs();
void listPADs();
void setPDRs();
void resetPDRs();
void deletePDRs();
//
void setSECTOR();
void resetSECTORs();
void deleteSECTORs();
void setRXRs();
void resetRXRs();
void deleteRXRs();
//
void setRODs();
void resetRODs();
void deleteRODs();
//
void histoCMAs(rpcbook *p); // rpcbook
//
private:
int m_run, m_event;
ubit16 nSubSys;
ubit16 nSector;
ubit16 nPadMax;
ubit16 nPtType;
ubit16 nProjec;
ubit16 nMatMax;
ubit16 nBunMax;
struct rpcstrip {
 rpcstrip *next;       //pointer to the next digi
 int     local;        //local strip address
 int     global;       //global strip address
 float   time;         //time
 float   coordhit[3];  //hit space coordinates
 float   coordigi[3];  //digi space coordinates
}; //end-of-struct-rpcstrip
struct rpcchamber { 
 rpcchamber *next;       //pointer to next chamber
 int   address[2];       //chamber eta/phi addresses
 int       subsys;       //subsystem address
 int       sector;       //sector address
 int      station;       //station address
 int        layer;       //layer address
 rpcstrip *etastrip;     //pointer to etastrips
 rpcstrip *phistrip;     //pointer to phistrips   
};//end-of-struct-rpcchamber
rpcchamber *rpcpointer;
Lvl1RpcConf *rpcconfig;
Matrix         *CMA[2][32][10][2][2][2];
MatrixReadOut  *CMR[2][32][10][2][2][2];
Pad            *PDL[2][32][10][2];
PadReadOut     *PDR[2][32][10];
SectorL        *SCL[2][32];
RXReadOut      *RXR[2][32];
RPCROD         *ROD[2][16];
//
// Matrix *CMA[2][32][10][2][2][2];
//
// pointers to lists of Matrix objects that belongs the subsystem "i",
// the projection "j", the trigger sector "k" and the low/high pt
// flavour "l"
//
//  CMA[i][j][k][l][m][n]
//      ^  ^  ^  ^  ^  ^
//      |  |  |  |  |  |
//      |  |  |  |  |  -------- local Matrix address fro 0 to 1 max
//      |  |  |  |  ----------- projection address: 0 (reads eta) 1 (reads phi)
//      |  |  |  -------------- low-pt(=0)/high-pt(=1) Matrix type
//      |  |  ----------------- pad address: fro 0 to 15 max 
//      |  -------------------- sector address: fro 0 to 31 (see def. below)
//      ----------------------- subsystem address: 0(eta negative) 1(eta pos)
//
//
//
// MatrixReadOut *CMR[2][32][10][2][2][2];
//
// pointers to lists of Matrix-ReadOut objects that belongs the subsystem "i",
// the projection "j", the trigger sector "k" and the low/high pt
// flavour "l"
//
//  CMR[i][j][k][l][m][n]
//      ^  ^  ^  ^  ^  ^
//      |  |  |  |  |  |
//      |  |  |  |  |  -------- local Matrix address fro 0 to 1 max
//      |  |  |  |  ----------- projection address: 0 (reads eta) 1 (reads phi)
//      |  |  |  -------------- low-pt(=0)/high-pt(=1) Matrix type
//      |  |  ----------------- pad address: fro 0 to 15 max 
//      |  -------------------- sector address: fro 0 to 31 (see def. below)
//      ----------------------- subsystem address: 0(eta negative) 1(eta pos)
//
//
//
//
// pad *PDL[2][32][10][2];
//
//
//  PDL[i][j][k][l]
//      ^  ^  ^  ^  
//      |  |  |  |  
//      |  |  |  | 
//      |  |  |  |  
//      |  |  |  -------------- low-pt(=0)/high-pt(=1) pad type
//      |  |  ----------------- pad address: fro 0 to 10 max 
//      |  -------------------- sector address: fro 0 to 31 (see def. below)
//      ----------------------- subsystem address: 0(eta negative) 1(eta pos)
//
//
//
// Sector *SCL[2][32];
//
//
//  SCL[i][j]
//      ^  ^   
//      |  |  
//      |  |  
//      |  |  
//      |  |  
//      |  |
//      |  -------------------- sector address: fro 0 to 31 (see def. below)
//      ----------------------- subsystem address: 0(eta negative) 1(eta pos)
//
//
//  
//------------------------------------------------------------------//
//
// Read-Out
//
//------------------------------------------------------------------//
void applyCabling(int subsys, int proj, int sector, 
                  int lowhig, int address[2], int layer, 
	          int station, int stripaddress, float time);
void scan_cabling(int subsys, int proj, int sector, 
                  int lowhig, int address[2], int layer, 
	          int station, int stripaddress, float time);
void fillMatrix(CMAword matrixDebug,
                int subsys, int sector, int padadd, int lowhig, int proj,
                int localadd, int address[2], int sidemat, int layer, 
                int stripaddress, float time);
void fillMatrixHighPt(int i, int j, int k, int m, int n);
int check_road(Matrix *p, int sectype, int lowhigh, int addmatprg);
};
#endif
