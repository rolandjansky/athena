/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_Lvl1RpcConf_H
#define TrigT1RPChardware_Lvl1RpcConf_H

#include "TrigT1RPChardware/Lvl1Def.h"
#define MAX_NUMBER_OF_SECTYPES 3
class Lvl1RpcConf {

public:
Lvl1RpcConf ();  //constructor
~Lvl1RpcConf (); //distructor

void gimeMatrix (int sizebuff, int *listmat, int *listmat2,
                 int subsys, int proj,int sector,int lowhig,
        	 int address[2], int stripaddress);
int   lvl1debug(int i, int j);
int * lvl1debug(int i);
CMAword CMDebug();
CMAword PDDebug();
int SectorType(int subsystem, int sector);
int GroupType(int sectortype, int station, int chamber);
int PhiGroup(int sectortype, int station, int groupadd, int mode);
int NumberOfChambers(int projection, int sectortype, int station);
int NumberOfChannels(int proiection, int sectortype, int station, int chamber);
int MatSetAdd(int subsystem, int sector, int projection, 
              int pivot, int address0, int address1);
CMAword *Windows(int subsystem, int sector, int lowhigh, 
                 int projection, int pivot, 
	         int address0, int address1);
ubit16 *Delay(int subsystem, int sector, int lowhigh,
              int projection, int pivot, int address0, int address1);
ubit16 *Pulse(int subsystem, int sector, int lowhigh,
              int projection, int pivot, int address0, int address1);
int *LocalDirec(int subsystem, int sector, int lowhigh,
                int projection, int pivot, int address0, int address1);
int *K_Readout(int subsystem, int sector, int lowhigh,
               int projection, int pivot, int address0, int address1);
int *Majority(int subsystem, int sector, int lowhigh,
              int projection, int pivot, int address0, int address1);
CMAword *Overlap(int subsystem, int sector, int lowhigh,
                 int projection, int pivot, int address0, int address1);
sbit32 *Geometry(int subsystem, int sector, int lowhigh,
              int projection, int pivot, int address0, int address1);	     
int MatSetSectorType(int sectortype, int lowhigh, int addmatprg);
int MatSetProjection(int sectortype, int lowhigh, int addmatprg);
int MatSetLowHigh(int sectortype, int lowhigh, int addmatprg);
int MatSetAddress0(int sectortype, int lowhigh, int addmatprg);
int MatSetAddress1(int sectortype, int lowhigh, int addmatprg);
int NumberOfPhiGroups(int sectortype, int station);
////
//
//    splitter[i][j][k][l];
//             ^  ^  ^  ^
//             |  |  |  |
//             |  |  |  -- absolute connector address
//             |  |  ----- sectortype
//             |  -------- low-high pt
//             ----------- projection
//
//    splitPhi[j][k][l];
//             ^  ^  ^
//             |  |  |
//             |  |  -- Phi-Group Address
//             |  ----- sectortype
//             -------- low-high pt
//
// eta connector map:
//    chamConn = 1000*matrix_absolute_address + local_matrix_connector_address
//
// pad connector map:
//    chamConn = PadAddress
//
struct split {
 ubit16 chamConn;
 split   *next;
};
split *splitter[2][2][MAX_NUMBER_OF_SECTYPES][100];
split *splitPhi   [2][MAX_NUMBER_OF_SECTYPES][ 20];
ubit16 conmax, phiGroupmax;
ubit16 CMtoRPC[2][2][MAX_NUMBER_OF_SECTYPES][20][16];
//
////
void    listOfCM         (int subsys, int proj, int sector, int lowhig,
                          int modAddress, int stripAddress, ubit16 *CMlist);
void    listOfPads       (int subsys, int sector, int lowhig, int modAddress,
                          ubit16 *CMlist);
ubit16 absoluteConnector (int proj, int secType, int lowhig,
                          int modAddress, int stripAddress);
ubit16 moduleRPC (int proj, int lowhig, int secType,
                  int globalConnector);

private:
char datatext[5][50];
//
// Debug parameters for the full simulation program
//
int lvl1dbg[4][32];
CMAword matrixDebug;
CMAword padDebug;
//
ubit16 numberOfSectorTypes;
int all_sector_types[MAX_NUMBER_OF_SECTYPES];
int cham[2][MAX_NUMBER_OF_SECTYPES][3][20], 
    nbch[2][MAX_NUMBER_OF_SECTYPES][3];
int nphi_groups[MAX_NUMBER_OF_SECTYPES][3], 
    phi_groups[MAX_NUMBER_OF_SECTYPES][3][20][2];
int group_type[MAX_NUMBER_OF_SECTYPES][3][20];
//
//  nbch gives the number of rpc modules along eta or phi for a given
//  sector type and station type:
//  nbch[k][i][j]
//       ^  ^  ^
//       |  |  |
//       |  |  --- station index
//       |  ------ sector_type index
//       --------- projection index
//
//  
//
//  cham gives the number of eta/phi strips for each rpc chamber
//  cham[i][j][k][l]
//       ^  ^  ^  ^
//       |  |  |  |
//       |  |  |  - address of the rpc chamber, from 0 to 19 max
//       |  |  ---- station index: 0=inner, 1=middle, 2=outer
//       |  ------- sector index
//       ---------- projection index:0=eta view; 1=phi view;
//
//  nphi_grups gives for a given sector_type and station the number
//  of eta_chambers grouped to form the units considered for the
//  phi-trigger (projection=1).
//
//  nphi_groups[i][j]
//              ^  ^
//              |  |
//              |  --- station address
//              ------ sector_type address
//
//  phi_groups gives for a given sector_type, station and group address
//  (from 0 to nphi_groups-1) the address of the first and the last 
//  consecutive eta-chambers belonging to that group. As far as the pivot
//  plane is concerned this corresponds uniquely to the pad segmentation
//
//
//  phi_groups[i][j][k][l]
//             ^  ^  ^  ^
//             |  |  |  |
//             |  |  |  -- 0/1: address first/last eta-chamber in the group
//             |  |  ----- group address
//             |  -------- station address
//             ----------- sector_type address
//
//
//  group_type gives back the group type of
//  that chamber in the station "j" and sector "i"; if station=pivot this
//  corresponds to the pad address
//
//  group_type[i][j][k]
//             ^  ^  ^
//             |  |  |
//             |  |  -- chamber eta_address
//             |  ----- station address
//             -------- sector_type
//
//
struct matcable {
 char station[3];
 char type;
 int address;
 int chalow[2]; //low-pt: address of the chamber at the lowest |eta|
 int conlow[2]; //low-pt: local address of the connector at the lowest |eta|
 int connum[2]; //number of connectors
 int glolow[2]; //low-pt: global address of the connector at the lowest |eta|
 int glohig[2]; //low-pt: global address of the connector at the highest |eta|
};
matcable *rpcmod[2][MAX_NUMBER_OF_SECTYPES][20];
//
int sector_type[2][32];
//
//  sector_type[j][i] gives the sector type of sector with address "i" of
//                    subsystem "j"
//
// rpcmod[k][i][j]
//        ^  ^  ^
//        |  |  |
//        |  |  ------------ chamber address from 0 to 20(max) counted from |eta|=0
//        |  --------------- sector type: 0=large; 1=small; 2=feet;
//        ------------------ projection index: 0=eta view; 1=phi view;
//
int pinconn[2];
char sectorFileName[80];
char matSecFileName[80];
char matrixFileName[80];
//
//
// CMA programmed
//
// structure of the data set:
//
// line 1     sectype lowhigh projection address[0] address[1]
// line 2     threshold[0] majority[0]
// line 3-6   pairs of 8 integer giving the lowest-highest programmed channels
// line 7     threshold[1] majority[1]
// line 8-11  pairs of 8 integer giving the lowest-highest programmed channels
// line 12    threshold[2] majority[2]
// line 13-16 pairs of 8 integer giving the lowest-highest programmed channels
// .......
//
struct CMAprog {
     int sectype;        // sector type   
     int lowhigh;        // low-high pT
     int projection;     // projection
     int address[2];     // address
  ubit16 delay[2][2][8]; // channel delay
  ubit16 width[2][2][8]; // pulse width
     int localDirec[2];  // direction of local coincidence
     int k_readout;      // address of thresholds for readout pattern
     int threshold[3];   // threshold
     int majority[3];    // majority type
 CMAword prog[3][32][2]; // programmed matrix map: 2 data for 32 channels
                         // for 3 thresholds
 CMAword overlap[2];     // channels in overlap with other(s) CMA(s);
                         // overlap[0]= number of  low-|eta|/|phi| channels in ov.
      		         // overlap[1]= number of high-|eta|/|phi| channels in ov.
     sbit32 geom[32];    // geometry
};//end-of-struct CMA    //
///////////////////////////////////////////////////////////////////////////////
//
//
// CMAprog CMAs[i][j][k]
//              ^  ^  ^
//              |  |  |
//              |  |  -- absolute sequential address of the matrix*threshold
//              |  ----- Pt Type Address (0=low-pt  1=high-pt)
//              -------- Sector Type Address (from 0 to 63 max)
//
///////////////////////////////////////////////////////////////////////////////
CMAprog CMAs[MAX_NUMBER_OF_SECTYPES][2][20*3];
//
//
void readSectorParameters(int nsect);
void readMatrixAddresses(int addr);
void readMatrixParameters(int addr, int ptType, int absSequential);
void gimeMatrixMain (int sizebuff, int *listmat,
                     int subsys, int proj,int sector,int lowhig,
		     int address[2], int stripaddress);
void gimeMatrixScnd (int sizebuff, int *listmat,
                     int subsys, int sector,int lowhig,
		     int address[2]);
void cabling();
void cablingDisplay(ubit16 mode);
};//end-of-class Lvl1RpcConf
#endif
