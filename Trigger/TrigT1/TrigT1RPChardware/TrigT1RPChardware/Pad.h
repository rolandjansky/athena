/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_Pad_H
#define TrigT1RPChardware_Pad_H

#include "NOBXS.h"
#include "TrigT1RPChardware/Lvl1Def.h"

class Pad {
public:
Pad(int run, int event, CMAword debug,
         ubit16 subsys, ubit16 sect, ubit16 padad, ubit16 lowhig,
         bool oldSimulation);
//    ubit16 padInput[2][2][2][NOBXS][2]); 
~Pad();
void reset();
void setDefaultConfiguration();
void setProjectionLogic(ubit16 prologic);
void load(ubit16 lowhig, ubit16 etaphi, ubit16 CMadd,
          ubit16 BX, ubit16 thres, ubit16 overl);
void execute();
void display(int flag);
//
// readout method
//
int get_PADID();
CMAword padDebug;
ubit16 getRoI(ubit16 bunch);
ubit16 getPTCoding(ubit16 bunch);
ubit16 getOPL(ubit16 bunch);
ubit16 getOverlapPhi(ubit16 bunch);
ubit16 getOverlapEta(ubit16 bunch);
ubit16 getRoIAmbiguity(ubit16 bunch);
ubit16 getBCIDCounter(ubit16 bunch);
bool   isOldSimulation(); //M.C. 3/3/2010
void   setOldSimulation(bool value); 

// M.Corradi Jan 9, 2015
    void setFeetOn(bool feet) {m_feet_on = feet;};
void setFeetThresholds(unsigned short int ith, unsigned short int th)
        {m_feet_thresholds[ith]=th;};
        
private:
int m_run;
int m_event;
bool m_oldSimulation; //M.C. 3/3/2010
bool m_feet_on;
unsigned short int m_feet_thresholds[3];
    
    
//
// pad parameters
//
// Load the "Hit in Outer Plane" flag
//
ubit16 m_nBunMax;               // Maximum number of Bunch crossings to be cons.
ubit16 m_thHitOuterPlane;       // gives the address from 1 to 3 of the
                              // threshold used for Outer Plane Confirmation
                              // 0 = no hit In Outer Plane required
//
// store this information in hitInOuterPlane
//
ubit16 m_hitInOuterPlane[3];  //  hitInOuterPlane[i] i=0,1,2 threshold address
                            // = 0 => no Outer Plane coincidence required
                            // > 0 =>    Outer Plane coincidence required
//
//pad attributes: subsystem, sector, padadd, low/high-pt;
//
ubit16 m_subsystem;  //subsystem 0,1
ubit16 m_sector;     //sector address
ubit16 m_padadd;     //pad address
ubit16 m_lowhigh;    //lowhigh_pt type, 0=low; 1=high
//
ubit16 m_projectionLogic;


//  in the data and with the new cabling (m_oldSimulation==false)
//  the local RoI definition is
//  roi = phi_id*2+eta_id
//  and does not correspond to a simple geometrical order
//
// the drawing below reports the old roi numbering convention
// which is consistent with the simulation with RPCcablingSim (m_oldSimulation==true)
//
// RoI numbering
//
//      CM0              CM1
// |               |                 |                               
// |               |                 |                                    
// |   ROI 1       |     ROI 3       |   CM1                         
// |               |                 |                               
// +---------------+------------------
// |               |                 |                               ^ phi
// |   ROI 0       |     ROI 2       |   CM0                         |       
// |               |                 |                               |
// |               |                 |                               | 
// ==================== SECTOR GEOMETRICAL AXIS =====================> eta 
// |               |                 |                               |
// |               |                 |                               |     
// |   ROI 0       |     ROI 2       |   CM0                         |
// |               |                 |                               |
// +---------------+------------------                               V phi
// |               |                 |                               
// |   ROI 1       |     ROI 3       |   CM1                                 
// |               |                 |
// |               |                 |
//     CM0               CM1
//
// padIn[i][j][k][l][m]
//       ^  ^  ^  ^  ^
//       |  |  |  |  |
//       |  |  |  |  --- 0=threshold; 1=overlap
//       |  |  |  ------ bunch-crossing identifier (in NOBXS bunches range)
//       |  |  --------- matrix local address (0,1)
//       |  ------------ eta-phi matrix: 0=eta; 1=phi.
//       --------------- low-pt=0, high-pt=1;
//
ubit16 m_padIn[2][2][2][NOBXS][2];
//
// PAD LOGIC STEP n. 2
//
// padStep2[i][j][k]
//          ^  ^  ^
//          |  |  |
//          |  |  ----- results summary data as follows:
//          |  |  j=    0,1  highest threshold satisfied by CM with local 
//          |  |             address "j"
//          |  |  j=      2  highest satisfied threshold by pad logic
//          |  |  j=      3  CM local address with highest threshold
//          |  |  j=      4  number of CMs with any threshold satisfied
//          |  |  j=      5  overlap flag
//          |  |  j=      6  hit in outer plane
//          |  |  j=      7  Roi ambiguity
//          |  |
//          |  ----- eta-phi
//          -------- bunch-crossing identifier (in NOBXS bunches range)
//        
//
ubit16 m_padStep2[NOBXS][2][8];
//
// PAD LOGIC OUTPUT
//
//  padOut[i][j]
//         ^  ^
//         |  |
//         |  ---- pad logic output: 
//         |  j=0     RoI address in the pad region
//         |  j=1     pT coding (three low-pT thresh. + three high-pt thresh.)
//         |          1,2,3 increasing low-pt thresholds
//         |          4,5,6 increasing hig-pt thresholds
//         |  j=2     OPL (hit in outer plane)
//         |  j=3     Overlap phi flag to be solved by MUCTPI
//         |  j=4     Overlap eta flag to be solved by Sector Logic
//         |  j=5     RoI Ambiguity
//         |  j=6     BCID counter
//         |
//         ------- bunch-crossing identifier (in NOBXS bunches range)
//         
//
//
ubit16 m_padOut[NOBXS][7];
//
void threshold();
void overlap();
void makeOut();
void makeOutEta();
void makeOutPhi();
void makeOutEtaAndPhi();
void makeOutEtaOrPhi();
void makeFeet();
    
};//end-of-class Pad
#endif
