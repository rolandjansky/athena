/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPChardware/Pad.h"

#include <cmath>
#include <fstream>
#include <iostream>

#include "TrigT1RPChardware/Matrix.h"

using namespace std;

//--------------------------------------------------------------------//
Pad::Pad(int run, int event, CMAword debug, ubit16 subsys, ubit16 sect, ubit16 padad, ubit16 lowhig, bool oldSimulation) {
    //
    // input parameters: run     run number
    //                   event   event number
    //                   subsys  subsystem address (0=eta<0; 1=eta>0)
    //                   sect    address of the sector of this pad
    //                   padad   local address of this pad
    //                   lowhig  type of pad: lowpt or highpt pad
    //            OldSimulation  true means using RPCcablingSim, false for MuonRPC_Cabling (M.Corradi 2/3/2010)
    //
    //                   CMA data are loaded in the PAD with the method "load"
    //
    padDebug = debug;
    // padDebug=0x3f; all debug ON

    const ubit16 df = 0;
    m_run = run;
    m_event = event;
    m_oldSimulation = oldSimulation;
    m_subsystem = subsys;
    m_sector = sect;
    m_padadd = padad;
    m_lowhigh = lowhig;
    //
    m_nBunMax = NOBXS;
    m_feet_on = false;
    m_feet_thresholds[0] = 0;
    m_feet_thresholds[1] = 1;
    m_feet_thresholds[2] = 2;
    //
    if (padDebug & 1 << df) {
        cout << " ==================== " << endl;
        cout << " =     Pad::Pad     = " << endl;
        cout << " ==================== " << endl;
        cout << " m_run= " << m_run << " m_event= " << m_event << " padDebug= " << hex << padDebug << dec << endl;
        cout << " Pad parameters: Subsystem: " << m_subsystem << " Sector: " << m_sector << " Pad address: " << m_padadd
             << " LowHigh: " << m_lowhigh << " m_oldSimulation: " << m_oldSimulation << endl;
    }  // end-of-if(*(lvl1dbg
    //
    reset();
    setDefaultConfiguration();
}  // end-of-pad::pad
//---------------------------------------------------------------------//
Pad::~Pad() {
    const ubit16 df = 1;
    if (padDebug & 1 << df) cout << "Distructor of Pad called " << endl;
}  // end-of-Pad::~Pad
//----------------------------------------------------------------------//
void Pad::reset() {
    ubit16 i, j, k, l, m;

    //
    // input to the PAD logic: initialize m_padIn
    //
    for (m = 0; m < 2; m++) {
        for (k = 0; k < 2; k++) {                  // eta-phi matrix
            for (j = 0; j < 2; j++) {              // matrix local address
                for (l = 0; l < m_nBunMax; l++) {  // loop on NOBXS bunches
                    for (i = 0; i < 2; i++) {      // type of input: i=0 => threshold; i=1 => overlap
                        m_padIn[m][k][j][l][i] = 0;
                    }  // end-of-for(i
                }      // end-of-for(l
            }          // end-of-for(j
        }              // end-of-for(k
    }                  // end-of-for(m
    //
    for (l = 0; l < m_nBunMax; l++) {
        for (k = 0; k < 8; k++) {  // reset m_padStep2 data per each bunch crossing
            m_padStep2[l][0][k] = 0;
            m_padStep2[l][1][k] = 0;
        }                          // end-of-for(k
        for (k = 0; k < 7; k++) {  // reset m_padOut data per each bunch crossing
            m_padOut[l][k] = 0;
        }  // end-of-for(k
    }      // end-of-for(l
}  // end-of-Pad::reset
//----------------------------------------------------------------------//
void Pad::setDefaultConfiguration() {
    const ubit16 df = 2;
    m_projectionLogic = 2;
    //
    // load "hitOuterPlane"
    //
    m_thHitOuterPlane = 0;
    // put this in m_hitInOuterPlane
    m_hitInOuterPlane[0] = 0;
    m_hitInOuterPlane[1] = 0;
    m_hitInOuterPlane[2] = 0;
    if (m_thHitOuterPlane) m_hitInOuterPlane[m_thHitOuterPlane - 1] = 1;
    if (padDebug & 1 << df) {
        cout << " Pad: m_hitInOuterPlane= " << m_hitInOuterPlane[0] << " " << m_hitInOuterPlane[1] << " " << m_hitInOuterPlane[2] << " "
             << endl;
        cout << " Pad: projectionLogic= " << m_projectionLogic << endl;
    }  // end-of-if(padDebug&1<<df)
}  // end-of-Pad::setDefaultConfiguration
//----------------------------------------------------------------------//
void Pad::setProjectionLogic(ubit16 prologic) {
    if (prologic < 4) {
        m_projectionLogic = prologic;
    } else {
        cout << " Pad.setProjectionLogic; prologic= " << prologic << " not allowed; default value " << m_projectionLogic << " is retained "
             << endl;
    }  // end-of-if
}  // end-of- Pad::setProjectionLogic(ubit16 prologic)
//----------------------------------------------------------------------//
void Pad::load(ubit16 lowhig, ubit16 etaphi, ubit16 locCMadd, ubit16 BX, ubit16 thres, ubit16 overl) {
    //
    // input data from Coincidence Matrix:
    //                                 lowhig  =  type of Matrix (low/high pt);
    //                                 etaphi  =  projection: eta=0, phi=1;
    //                                 locCMadd=  local CM address (0,1)
    //                                 BX      = "address" of this bunch
    //                                 thresOver= type of data: threshold or overlap
    //                                 thres    = highest threshold
    //                                 over     = overlap
    //
    if (lowhig > 1 || etaphi > 1 || locCMadd > 1 || BX >= NOBXS) {
        cout << " Pad::load : lowhig=" << lowhig << " etaphi=" << etaphi << " locCMadd=" << locCMadd << " BX=" << BX << " not valid"
             << endl;
    } else {
        m_padIn[lowhig][etaphi][locCMadd][BX][0] = thres;
        m_padIn[lowhig][etaphi][locCMadd][BX][1] = overl;
    }  // end-of-if(lowhig
}  // end-of-Pad::load
//----------------------------------------------------------------------//
void Pad::execute() {
    const ubit16 df = 3;
    if (padDebug & 1 << df) cout << " execute PAD " << endl;
    if (m_lowhigh) {
        threshold();
        overlap();
        makeOut();
        if (m_feet_on) makeFeet();
        if (padDebug & 1 << df) display(0);
    }  // end-of-if(m_lowhigh
}  // end-of-Pad::execute
//---------------------------------------------------------//
void Pad::threshold() {
    const ubit16 df = 4;
    //
    // reminder: m_padIn[m][i][j][k][l] is   0   no trigger
    //                                1,2,3
    //
    ubit16 i, j, l, m;
    ubit16 thisThreshold;
    for (m = 0; m < 2; m++) {                      // Low- High-Pt
        for (j = 0; j < 2; j++) {                  // eta-phi matrix address
            for (i = 0; i < 2; i++) {              // local matrix address
                for (l = 0; l < m_nBunMax; l++) {  // loop on m_nBunMax bunches
                    if (m_padIn[m][j][i][l][0]) {
                        if (m && m_hitInOuterPlane[m_padIn[m][j][i][l][0] - 1]) {
                            m_padStep2[l][i][6] = 1;
                        } else {
                            // define absolute threshold
                            thisThreshold = 3 * m + m_padIn[m][j][i][l][0];
                            // find highest threshold
                            if (thisThreshold > m_padStep2[l][j][i]) {
                                if (m_padStep2[l][j][i] == 0) m_padStep2[l][j][7]++;  // num of RoI with threshold
                                m_padStep2[l][j][i] = thisThreshold;
                            }  // end-of-if
                            // if( !j && (thisThreshold > m_padStep2[l][j][2]) ) {
                            if (thisThreshold > m_padStep2[l][j][2]) {
                                m_padStep2[l][j][2] = thisThreshold;  // highest threshold
                                m_padStep2[l][j][3] = i;              // CM with the highest thres.
                                if (padDebug & 1 << df) {
                                    cout << " Pad::threshold; padStep2 eta-phi " << j << " localAdd " << i << " Bunch " << l
                                         << " threshold " << m_padStep2[l][j][2] << endl;
                                }                   // end-of-if(padDebug&1<<df)
                            }                       // end-of-if(m_padStep2
                            m_padStep2[l][j][4]++;  // number of CMs with thresh.
                        }                           // end-of-if( m && m_hitInOuterPlane
                    }                               // end-of-if(m_padIn
                }                                   // end-of-for(l
            }                                       // end-of-for(i
        }                                           // end-of-for(j
    }                                               // end-of-for(m
}  // end-of-Pad::threshold
//---------------------------------------------------------//
void Pad::overlap() {
    // const ubit16 df=5;
    ubit16 i, j, l;
    for (l = 0; l < m_nBunMax; l++) {  // loop on m_nBunMax bunches
        for (j = 0; j < 2; j++) {      // loop on Low- High-Pt systems
            for (i = 0; i < 2; i++) {  // eta-phi matrix address: eta=0; phi=1
                // "external" overlapping strips
                //
                if (m_padIn[j][i][1][l][1] & 2) m_padStep2[l][i][5] = 1;  // c1 ovl_sx
                if (m_padIn[j][i][0][l][1] & 1) m_padStep2[l][i][5] = 1;  // c0 ovl_dx
            }                                                             // end-of-for(i
        }                                                                 // end-of-for(j
    }                                                                     // end-of-for(l
}  // end-of-Pad::overlap()
//---------------------------------------------------------//
void Pad::makeOut() {
    // const ubit16 df=6;
    switch (m_projectionLogic) {
        case 0: makeOutEta(); break;
        case 1: makeOutPhi(); break;
        case 2: makeOutEtaAndPhi(); break;
        case 3: makeOutEtaOrPhi(); break;
        default: cout << " Pad: problems with projectionLogic= " << m_projectionLogic << endl;
    }  // end-of-switch

}  // end-of-Pad::makeOut()
//---------------------------------------------------------//
void Pad::makeOutEta() {
    // const ubit16 df=6;
    ubit16 l;
    for (l = 0; l < m_nBunMax; l++) {  // loop on Bunches
        if (m_padStep2[l][0][2]) {
            //  RoI definition
            if (m_oldSimulation) {
                m_padOut[l][0] = 2 * m_padStep2[l][0][3];
            } else {
                m_padOut[l][0] = m_padStep2[l][0][3];
            }
            m_padOut[l][1] = m_padStep2[l][0][2];
            m_padOut[l][2] = m_padStep2[l][0][6];
            if (m_padStep2[l][0][5]) m_padOut[l][4] = 1;
            m_padOut[l][5] = 1;
        }  // end-of-if(m_padStep2[l][0][2]) {
    }      // end-of-for(l
}  // end-of-makeOutEta()
//---------------------------------------------------------//
void Pad::makeOutPhi() {
    // const ubit16 df=6;
    ubit16 l;
    for (l = 0; l < m_nBunMax; l++) {  // loop on Bunches
        if (m_padStep2[l][1][2]) {
            // RoI definition
            if (m_oldSimulation) {
                m_padOut[l][0] = m_padStep2[l][1][3];
            } else {
                m_padOut[l][0] = 2 * m_padStep2[l][1][3];
            }
            m_padOut[l][1] = m_padStep2[l][1][2];
            m_padOut[l][2] = m_padStep2[l][1][6];
            if (m_padStep2[l][1][5]) m_padOut[l][3] = 1;
            m_padOut[l][5] = 1;
        }  // end-of-if(m_padStep2[l][1][2]) {
    }      // end-of-for(l
}  // end-of-makeOutPhi()
//---------------------------------------------------------//
void Pad::makeOutEtaAndPhi() {
    // const ubit16 df=6;
    ubit16 l;
    for (l = 0; l < m_nBunMax; l++) {  // loop on Bunches
        if (m_padStep2[l][0][2] && m_padStep2[l][1][2]) {
            // RoI definition
            if (m_oldSimulation) {
                m_padOut[l][0] = m_padStep2[l][1][3] + 2 * m_padStep2[l][0][3];  // th
            } else {
                m_padOut[l][0] = m_padStep2[l][0][3] + 2 * m_padStep2[l][1][3];
            }
            if (m_padStep2[l][0][2] < m_padStep2[l][1][2]) {
                m_padOut[l][1] = m_padStep2[l][0][2];  // th
                m_padOut[l][2] = m_padStep2[l][0][6];  // OPL
            } else {
                m_padOut[l][1] = m_padStep2[l][1][2];
                m_padOut[l][2] = m_padStep2[l][1][6];
            }
            if (m_padStep2[l][1][5]) m_padOut[l][3] = 1;                            // ovl phi
            if (m_padStep2[l][0][5]) m_padOut[l][4] = 1;                            // ovl eta
            if (m_padStep2[l][0][7] + m_padStep2[l][1][7] > 2) m_padOut[l][5] = 1;  // roi ambiguity
        }                                                                           // end-of-if
    }                                                                               // end-of-for(l
}  // end-of-Pad::makeOutEtaAndPhi()
//---------------------------------------------------------//
void Pad::makeOutEtaOrPhi() {
    // const ubit16 df=6;
    ubit16 l;
    for (l = 0; l < m_nBunMax; l++) {  // loop on Bunches
        if (m_padStep2[l][0][2] || m_padStep2[l][1][2]) {
            if (m_padStep2[l][0][2] && !m_padStep2[l][1][2]) {
                makeOutEta();
            } else if (m_padStep2[l][1][2] && !m_padStep2[l][0][2]) {
                makeOutPhi();
            } else {
                makeOutEtaAndPhi();
            }  // end-of-if( &&! )
        }      // end-of-if(m_padStep2[l][0][2] || m_padStep2[l][1][2])
    }          // end-of-for(l
}  // end-of-makeOuttaEOrPhi()
//---------------------------------------------------------//
void Pad::makeFeet() {
    ubit16 l;
    for (l = 0; l < m_nBunMax; l++) {
        if (m_padOut[l][1] > 0) m_padOut[l][1] = m_feet_thresholds[m_padOut[l][1] - 1] + 1;
    }
}
//---------------------------------------------------------//
void Pad::display(int flag) {
    //
    // first dump the m_padStep2 registers
    //
    ubit16 i, j;
    char padOutText[7][6];
    strcpy(padOutText[0], "RoI");
    strcpy(padOutText[1], "Ptcod");
    strcpy(padOutText[2], "OPL");
    strcpy(padOutText[3], "OvPhi");
    strcpy(padOutText[4], "OvEta");
    strcpy(padOutText[5], "RoIAm");
    strcpy(padOutText[6], "BCDcn");
    if (flag) {
        cout << endl;
        cout << " ************************************* " << endl;
        cout << " *                                   * " << endl;
        cout << " *       P A D    D I S P L A Y      * " << endl;
        cout << " *                                   * " << endl;
        cout << " ************************************* " << endl;
        cout << endl;
        cout << " P A R A M E T E R S: " << endl;
        cout << "     Subsystem " << m_subsystem << endl;
        cout << "     Sector    " << m_sector << endl;
        cout << "     PAD addr  " << m_padadd << endl;
        cout << "     Low-High  " << m_lowhigh << endl;
        cout << "m_oldSimulation:" << m_oldSimulation << endl;
        cout << " feet_on:" << m_feet_on;
        if (m_feet_on)
            cout << " feet_thresholds:" << m_feet_thresholds[0] << "," << m_feet_thresholds[1] << "," << m_feet_thresholds[2] << endl;
        cout << endl;
    }
    if (flag - 1) {
        cout << " D U M P    padStep2  " << endl;
        cout << "                             ";
        cout << "Projection Eta ";
        cout << "     ";
        cout << "Projection Phi " << endl;
        cout << "                             ";
        cout << "---------------";
        cout << "     ";
        cout << "---------------" << endl;
        cout << "                             ";
        cout << "C C t a n o O A";
        cout << "     ";
        cout << "C C t a n o O A" << endl;
        cout << "                             ";
        cout << "M M h d u v P m";
        cout << "     ";
        cout << "M M h d u v P m" << endl;
        cout << "                             ";
        cout << "0 1 r d m e L b";
        cout << "     ";
        cout << "0 1 r d m e L b" << endl;
        cout << "                             ";
        cout << "    e r   r   i";
        cout << "     ";
        cout << "    e r   r   i" << endl;
        //

        for (j = 0; j < m_nBunMax; j++) {
            cout << "  Bunch Crossing Number " << j << "   ";
            for (i = 0; i < 2; i++) {  // Eta-Phi system
                cout << " " << m_padStep2[j][i][0] << " " << m_padStep2[j][i][1] << " " << m_padStep2[j][i][2] << " " << m_padStep2[j][i][3]
                     << " " << m_padStep2[j][i][4] << " " << m_padStep2[j][i][5] << " " << m_padStep2[j][i][6] << " "
                     << m_padStep2[j][i][7];
                if (i == 0) cout << "    ";
            }  // end-of-for(i
            cout << endl;
        }  // end-of-for(j
        cout << endl;
    }  // end-of-if
    if (flag) {
        //
        // dump m_padOut
        //
        cout << " D U M P    PadOut  " << endl;
        for (i = 0; i < m_nBunMax; i++) {
            cout << " Pad: padOut Bunch=" << i << ":";
            for (j = 0; j < 7; j++) { cout << " " << padOutText[j] << " " << m_padOut[i][j]; }  // end-of-for(j
            cout << endl;
        }  // end-of-for(i
        cout << endl;
    }  // end-of-if(flag
}  // end-of-Pad::display
//---------------------------------------------------------//
//
// readout method
//
int Pad::get_PADID() {  // f
    return m_padadd;    // f
}  // f
//---------------------------------------------------------//
ubit16 Pad::getRoI(ubit16 bunch) {
    ubit16 output = 0;
    if (bunch < m_nBunMax) {
        output = m_padOut[bunch][0];
    } else {
        cout << " getRoI; Error: bunch address " << bunch << " not consistent with m_nBunMax= " << m_nBunMax << endl;
    }
    return output;
}  // end-of-getRoI
//---------------------------------------------------------//
ubit16 Pad::getPTCoding(ubit16 bunch) {
    ubit16 output = 0;
    if (bunch < m_nBunMax) {
        output = m_padOut[bunch][1];
    } else {
        cout << " getPTCoding; Error: bunch address " << bunch << " not consistent with m_nBunMax= " << m_nBunMax << endl;
    }
    return output;
}  // end-of-getPTCoding
//---------------------------------------------------------//
ubit16 Pad::getOPL(ubit16 bunch) {
    ubit16 output = 0;
    if (bunch < m_nBunMax) {
        output = m_padOut[bunch][2];
    } else {
        cout << " getOPL; Error: bunch address " << bunch << " not consistent with m_nBunMax= " << m_nBunMax << endl;
    }
    return output;
}  // end-of-getOPL
//---------------------------------------------------------//
ubit16 Pad::getOverlapPhi(ubit16 bunch) {
    ubit16 output = 0;
    if (bunch < m_nBunMax) {
        output = m_padOut[bunch][3];
    } else {
        cout << " getOverlapPhi; Error: bunch address " << bunch << " not consistent with m_nBunMax= " << m_nBunMax << endl;
    }
    return output;
}  // end-of-getOverlapPhi
//---------------------------------------------------------//
ubit16 Pad::getOverlapEta(ubit16 bunch) {
    ubit16 output = 0;
    if (bunch < m_nBunMax) {
        output = m_padOut[bunch][4];
    } else {
        cout << " getOverlapEta; Error: bunch address " << bunch << " not consistent with m_nBunMax= " << m_nBunMax << endl;
    }
    return output;
}  // end-of-getOverlapEta
//---------------------------------------------------------//
ubit16 Pad::getRoIAmbiguity(ubit16 bunch) {
    ubit16 output = 0;
    if (bunch < m_nBunMax) {
        output = m_padOut[bunch][5];
    } else {
        cout << " getRoIAmbiguity; Error: bunch address " << bunch << " not consistent with m_nBunMax= " << m_nBunMax << endl;
    }
    return output;
}  // end-of-getRoIAmbiguity
//---------------------------------------------------------//
ubit16 Pad::getBCIDCounter(ubit16 bunch) {
    ubit16 output = 0;
    if (bunch < m_nBunMax) {
        output = m_padOut[bunch][6];
    } else {
        cout << " getBCIDCounter; Error: bunch address " << bunch << " not consistent with m_nBunMax= " << m_nBunMax << endl;
    }
    return output;
}  // end-of-getBCIDCounter
//-----------
bool Pad::isOldSimulation() { return m_oldSimulation; }
//----------
//-----------
void Pad::setOldSimulation(bool value) { m_oldSimulation = value; }
//----------
