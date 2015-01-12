/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include <iostream>
#include <cmath>
#include "TrigT1RPChardware/Matrix.h"
#include "TrigT1RPChardware/Pad.h"

using namespace std;

//--------------------------------------------------------------------//
Pad::Pad(int run, int event, CMAword debug,
    ubit16 subsys, ubit16 sect, ubit16 padad, ubit16 lowhig, bool oldSimulation){
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
padDebug=debug;
const ubit16 df=0;
m_run = run;
m_event=event;
m_oldSimulation = oldSimulation;
subsystem=subsys;
sector=sect;
padadd=padad;
lowhigh=lowhig;
//
nBunMax = NOBXS;
m_feet_on=false;
m_feet_thresholds[0]=0;m_feet_thresholds[1]=1;m_feet_thresholds[2]=2;
//
if(padDebug&1<<df) {
 cout<<" ==================== "<<endl;
 cout<<" =     Pad::Pad     = "<<endl;
 cout<<" ==================== "<<endl;
 cout<<" m_run= "<<m_run<<" m_event= "
     <<m_event<<" padDebug= "<<hex<<padDebug<<dec<<endl;
 cout<<" Pad parameters: Subsystem: "<<subsystem
     <<" Sector: "<<sector
     <<" Pad address: "<<padadd
     <<" LowHigh: "<<lowhigh
     <<" m_oldSimulation: " << m_oldSimulation
     <<endl;
}//end-of-if(*(lvl1dbg
//
reset();
setDefaultConfiguration();
}//end-of-pad::pad
//---------------------------------------------------------------------//
Pad::~Pad(){
const ubit16 df=1;
if(padDebug&1<<df) cout<<"Distructor of Pad called "<<endl;
}//end-of-Pad::~Pad
//----------------------------------------------------------------------//
void Pad::reset() {
ubit16 i,j,k,l,m;

//
// input to the PAD logic: initialize padIn
//
 for(m=0; m<2; m++) { 
  for(k=0; k<2; k++) {    // eta-phi matrix   
   for(j=0; j<2; j++) {   // matrix local address   
    for(l=0; l<nBunMax; l++) {  // loop on NOBXS bunches   
     for(i=0; i<2; i++) { // type of input: i=0 => threshold; i=1 => overlap
      padIn[m][k][j][l][i] = 0;
     }//end-of-for(i
    }//end-of-for(l
   }//end-of-for(j
  }//end-of-for(k
 }//end-of-for(m
//
for(l=0;l<nBunMax;l++) {
 for(k=0; k<8; k++) {   // reset padStep2 data per each bunch crossing
  padStep2[l][0][k]=0;
  padStep2[l][1][k]=0;
 }//end-of-for(k
 for(k=0; k<7; k++) {   // reset padOut data per each bunch crossing
  padOut[l][k]=0;
 }//end-of-for(k
}//end-of-for(l
}//end-of-Pad::reset
//----------------------------------------------------------------------//
void Pad::setDefaultConfiguration() {
const ubit16 df=2;
projectionLogic=2;
//
// load "hitOuterPlane"
//
thHitOuterPlane=0;
// put this in hitInOuterPlane
hitInOuterPlane[0]=0; hitInOuterPlane[1]=0; hitInOuterPlane[2]=0;
if(thHitOuterPlane) hitInOuterPlane[thHitOuterPlane-1]=1;
if(padDebug&1<<df) {
 cout<<" Pad: hitInOuterPlane= "<<hitInOuterPlane[0]<<" "
                                <<hitInOuterPlane[1]<<" "
                                <<hitInOuterPlane[2]<<" "
                                <<endl;
 cout<<" Pad: projectionLogic= "<<projectionLogic<<endl;
}//end-of-if(padDebug&1<<df)
}//end-of-Pad::setDefaultConfiguration
//----------------------------------------------------------------------//
void Pad::setProjectionLogic(ubit16 prologic) {
 if(prologic<4) {
  projectionLogic=prologic;
 } else {
  cout<<" Pad.setProjectionLogic; prologic= "<<prologic
  <<" not allowed; default value "<<projectionLogic<<" is retained "<<endl;
 }//end-of-if
}//end-of- Pad::setProjectionLogic(ubit16 prologic)
//----------------------------------------------------------------------//
void Pad::load(ubit16 lowhig, ubit16 etaphi, ubit16 locCMadd,
               ubit16 BX, ubit16 thres, ubit16 overl){
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
if(lowhig>1||etaphi>1||locCMadd>1||BX>=NOBXS) {
 cout<<" Pad::load : lowhig="<<lowhig<<" etaphi="<<etaphi
     <<" locCMadd="<<locCMadd<<" BX="<<BX<<" not valid"<<endl;
} else {
 padIn[lowhig][etaphi][locCMadd][BX][0] = thres;
 padIn[lowhig][etaphi][locCMadd][BX][1] = overl;
}//end-of-if(lowhig
}//end-of-Pad::load
//----------------------------------------------------------------------//
void Pad::execute(){
const ubit16 df=3;
if(padDebug&1<<df) cout<<" execute PAD "<<endl;
if(lowhigh) {
 threshold();
 overlap();
 makeOut();
 if (m_feet_on) makeFeet();
}//end-of-if(lowhigh
}//end-of-Pad::execute
//---------------------------------------------------------//
void Pad::threshold(){
const ubit16 df=4;
// 
// reminder: padIn[m][i][j][k][l] is   0   no trigger 
//                                1,2,3
//
ubit16 i, j, l, m;
ubit16 thisThreshold;
for(m=0; m<2; m++) {          // Low- High-Pt 
 for(j=0; j<2; j++)  {        // eta-phi matrix address
  for(i=0; i<2; i++)  {       // local matrix address
   for(l=0; l<nBunMax; l++) { // loop on nBunMax bunches
    if( padIn[m][j][i][l][0]) {
     if( m && hitInOuterPlane[padIn[m][j][i][l][0]-1] ) {
      padStep2[l][i][6] = 1;
     } else {
//define absolute threshold
      thisThreshold = 3*m + padIn[m][j][i][l][0];
// find highest threshold 
      if(thisThreshold > padStep2[l][j][i] ) {
       padStep2[l][j][i] = thisThreshold;
      }//end-of-if
      //if( !j && (thisThreshold > padStep2[l][j][2]) ) {
      if( thisThreshold > padStep2[l][j][2] ) {
       padStep2[l][j][2] = thisThreshold;       // highest threshold
       padStep2[l][j][3] = i;                   // CM with the highest thres.
       if(padDebug&1<<df) {
       cout<<" Pad::threshold; padStep2 eta-phi "<<j
           <<" localAdd "<<i<<" Bunch "<<l
           <<" threshold "
           <<padStep2[l][j][2]<<endl;
       }//end-of-if(padDebug&1<<df)
      }//end-of-if(padStep2
      padStep2[l][j][4]++;                      // number of RoIs with thresh.
     }//end-of-if( m && hitInOuterPlane 
    }//end-of-if(padIn
   }//end-of-for(l
  }//end-of-for(i  
 }//end-of-for(j
}//end-of-for(m
}//end-of-Pad::threshold
//---------------------------------------------------------//
void Pad::overlap(){
//const ubit16 df=5;
ubit16 i, j, l;
for(l=0; l<nBunMax; l++) {  // loop on nBunMax bunches
 for(j=0; j<2; j++)      {  // loop on Low- High-Pt systems
  for(i=0; i<2; i++)      {  // eta-phi matrix address: eta=0; phi=1
    // "external" overlapping strips
    //
    if(padIn[j][i][1][l][1]&2) padStep2[l][i][5] = 1;  // c1 ovl_sx
    if(padIn[j][i][0][l][1]&1) padStep2[l][i][5] = 1;  // c0 ovl_dx
  }//end-of-for(i
 }//end-of-for(j
}//end-of-for(l
}//end-of-Pad::overlap()
//---------------------------------------------------------//
void Pad::makeOut(){
//const ubit16 df=6;
switch(projectionLogic) {
 case 0:
  makeOutEta();
  break;
 case 1:
  makeOutPhi();
  break;
 case 2:
  makeOutEtaAndPhi();
  break;
 case 3:
  makeOutEtaOrPhi();
  break;
 default:
  cout<<" Pad: problems with projectionLogic= "<<projectionLogic<<endl;  
}//end-of-switch

}//end-of-Pad::makeOut()
//---------------------------------------------------------//
void Pad::makeOutEta(){
//const ubit16 df=6;
ubit16 l;
for(l=0; l<nBunMax; l++) {  //loop on Bunches
 if(padStep2[l][0][2]) {
//  RoI definition
    if (m_oldSimulation) {
        padOut[l][0] = 2*padStep2[l][0][3];
    }else{
        padOut[l][0] = padStep2[l][0][3];
    }
    padOut[l][1] = padStep2[l][0][2];
    padOut[l][2] = padStep2[l][0][6];
    if(padStep2[l][0][5]) padOut[l][4] = 1;
    padOut[l][5] = 1;
 }//end-of-if(padStep2[l][0][2]) {
}//end-of-for(l
}//end-of-makeOutEta()
//---------------------------------------------------------//
void Pad::makeOutPhi(){
//const ubit16 df=6;
ubit16 l;
for(l=0; l<nBunMax; l++) {  //loop on Bunches
 if(padStep2[l][1][2]) {
     // RoI definition
     if (m_oldSimulation) {
         padOut[l][0] = padStep2[l][1][3];
     }else{
         padOut[l][0] = 2*padStep2[l][1][3];
     }
     padOut[l][1] = padStep2[l][1][2];
     padOut[l][2] = padStep2[l][1][6];
     if(padStep2[l][1][5]) padOut[l][3] = 1;
     padOut[l][5] = 1;
 }//end-of-if(padStep2[l][1][2]) {
}//end-of-for(l
}//end-of-makeOutPhi()
//---------------------------------------------------------//
void Pad::makeOutEtaAndPhi(){
//const ubit16 df=6;
ubit16 l;
for(l=0; l<nBunMax; l++) {  //loop on Bunches
    if(padStep2[l][0][2] && padStep2[l][1][2]) {
        // RoI definition
        if (m_oldSimulation){
            padOut[l][0] = padStep2[l][1][3] + 2*padStep2[l][0][3]; //th 
        }else {
            padOut[l][0] = padStep2[l][0][3] + 2*padStep2[l][1][3]; 
        }
        if (padStep2[l][0][2]<padStep2[l][1][2]) {
            padOut[l][1] = padStep2[l][0][2]; // th
            padOut[l][2] = padStep2[l][0][6];// OPL
        } else  {
            padOut[l][1] = padStep2[l][1][2];  
            padOut[l][2] = padStep2[l][1][6];  
        }
     if(padStep2[l][1][5]) padOut[l][3] = 1; // ovl phi
     if(padStep2[l][0][5]) padOut[l][4] = 1; // ovl eta
     padOut[l][5] = padStep2[l][0][7]; // roi ambiguity

 }//end-of-if
}//end-of-for(l
}//end-of-Pad::makeOutEtaAndPhi()
//---------------------------------------------------------//
void Pad::makeOutEtaOrPhi(){
//const ubit16 df=6;
ubit16 l;
for(l=0; l<nBunMax; l++) {  //loop on Bunches
 if(padStep2[l][0][2] || padStep2[l][1][2]) { 
  if(padStep2[l][0][2] && !padStep2[l][1][2]) {
   makeOutEta();
  } else if(padStep2[l][1][2] && !padStep2[l][0][2]) {
   makeOutPhi();
  } else {
   makeOutEtaAndPhi();
  }//end-of-if( &&! )
 }//end-of-if(padStep2[l][0][2] || padStep2[l][1][2])
}//end-of-for(l
}//end-of-makeOuttaEOrPhi()
//---------------------------------------------------------//
void Pad::makeFeet(){
    ubit16 l;
    for(l=0; l<nBunMax; l++){
        if (padOut[l][1]>0) padOut[l][1]=m_feet_thresholds[padOut[l][1]-1]+1;
    }
}
//---------------------------------------------------------//
void Pad::display(int flag) {
//
// first dump the padStep2 registers
//
ubit16 i, j;
char padOutText[7][6];
strcpy(padOutText[0],"RoI");
strcpy(padOutText[1],"Ptcod");
strcpy(padOutText[2],"OPL");
strcpy(padOutText[3],"OvPhi");
strcpy(padOutText[4],"OvEta");
strcpy(padOutText[5],"RoIAm");
strcpy(padOutText[6],"BCDcn");
if(flag) {
 cout<<endl;
 cout<<" ************************************* "<<endl;
 cout<<" *                                   * "<<endl;
 cout<<" *       P A D    D I S P L A Y      * "<<endl;
 cout<<" *                                   * "<<endl;
 cout<<" ************************************* "<<endl;
 cout<<endl;
 cout<<" P A R A M E T E R S: "<<endl;
 cout<<"     Subsystem "<<subsystem<<endl;
 cout<<"     Sector    "<<sector<<endl;
 cout<<"     PAD addr  "<<padadd<<endl;
 cout<<"     Low-High  "<<lowhigh<<endl;
 cout<<"m_oldSimulation:" << m_oldSimulation<<endl;
 cout<<" feet_on:" << m_feet_on;
 if (m_feet_on) cout<<" feet_thresholds:" << m_feet_thresholds[0]<<","<< m_feet_thresholds[1]<<","<< m_feet_thresholds[2]<<endl;
 cout<<endl;
}
if(flag-1){
 cout<<" D U M P    padStep2  "<<endl;
 cout<<"                             ";
 cout<<"Projection Eta ";
 cout<<"     ";
 cout<<"Projection Phi "<<endl;
 cout<<"                             ";
 cout<<"---------------";
 cout<<"     ";
 cout<<"---------------"<<endl;
 cout<<"                             ";
 cout<<"C C t a n o O A";
 cout<<"     ";
 cout<<"C C t a n o O A"<<endl;
 cout<<"                             ";
 cout<<"M M h d u v P m";
 cout<<"     ";
 cout<<"M M h d u v P m"<<endl;
 cout<<"                             ";
 cout<<"0 1 r d m e L b";
 cout<<"     ";
 cout<<"0 1 r d m e L b"<<endl;
 cout<<"                             ";
 cout<<"    e r   r   i";
 cout<<"     ";
 cout<<"    e r   r   i"<<endl;
//
 
for(j=0; j<nBunMax; j++) {
 cout<<"  Bunch Crossing Number "<<j<<"   ";
 for(i=0; i<2; i++) { // Eta-Phi system
  cout<<" "<<padStep2[j][i][0]<<" "<<padStep2[j][i][1]
      <<" "<<padStep2[j][i][2]<<" "<<padStep2[j][i][3]
      <<" "<<padStep2[j][i][4]<<" "<<padStep2[j][i][5]
      <<" "<<padStep2[j][i][6]<<" "<<padStep2[j][i][7];
  if(i==0) cout<<"    ";
 }//end-of-for(i
 cout<<endl;
}//end-of-for(j
cout<<endl;
}//end-of-if
if(flag) {
//
// dump padOut
//
 cout<<" D U M P    PadOut  "<<endl;
 for(i=0; i<nBunMax; i++){
  cout<<" Pad: padOut Bunch="<<i<<":";
  for(j=0; j<7; j++) {
   cout<<" "<<padOutText[j]<<" "<<padOut[i][j];
  }//end-of-for(j
  cout<<endl;
 }//end-of-for(i
cout<<endl;
}//end-of-if(flag 
}//end-of-Pad::display
//---------------------------------------------------------//
//
// readout method
//
int Pad::get_PADID(){  //f
 return padadd;        //f
}                      //f
//---------------------------------------------------------//
ubit16 Pad::getRoI(ubit16 bunch) {
ubit16 output=0;
 if(bunch<nBunMax) {
  output=padOut[bunch][0];
 } else {
  cout<<" getRoI; Error: bunch address "<<bunch
      <<" not consistent with nBunMax= "<<nBunMax
      <<endl;
 }
return output;
}//end-of-getRoI
//---------------------------------------------------------//
ubit16 Pad::getPTCoding(ubit16 bunch) {
ubit16 output=0;
 if(bunch<nBunMax) {
  output=padOut[bunch][1];
 } else {
  cout<<" getPTCoding; Error: bunch address "<<bunch
      <<" not consistent with nBunMax= "<<nBunMax
      <<endl;
 }
return output;
}//end-of-getPTCoding
//---------------------------------------------------------//
ubit16 Pad::getOPL(ubit16 bunch) {
ubit16 output=0;
 if(bunch<nBunMax) {
  output=padOut[bunch][2];
 } else {
  cout<<" getOPL; Error: bunch address "<<bunch
      <<" not consistent with nBunMax= "<<nBunMax
      <<endl;
 }
return output;
}//end-of-getOPL
//---------------------------------------------------------//
ubit16 Pad::getOverlapPhi(ubit16 bunch) {
ubit16 output=0;
 if(bunch<nBunMax) {
  output=padOut[bunch][3];
 } else {
  cout<<" getOverlapPhi; Error: bunch address "<<bunch
      <<" not consistent with nBunMax= "<<nBunMax
      <<endl;
 }
return output;
}//end-of-getOverlapPhi
//---------------------------------------------------------//
ubit16 Pad::getOverlapEta(ubit16 bunch) {
ubit16 output=0;
 if(bunch<nBunMax) {
  output=padOut[bunch][4];
 } else {
  cout<<" getOverlapEta; Error: bunch address "<<bunch
      <<" not consistent with nBunMax= "<<nBunMax
      <<endl;
 }
return output;
}//end-of-getOverlapEta
//---------------------------------------------------------//
ubit16 Pad::getRoIAmbiguity(ubit16 bunch) {
ubit16 output=0;
 if(bunch<nBunMax) {
  output=padOut[bunch][5];
 } else {
  cout<<" getRoIAmbiguity; Error: bunch address "<<bunch
      <<" not consistent with nBunMax= "<<nBunMax
      <<endl;
 }
return output;
}//end-of-getRoIAmbiguity
//---------------------------------------------------------//
ubit16 Pad::getBCIDCounter(ubit16 bunch) {
ubit16 output=0;
 if(bunch<nBunMax) {
  output=padOut[bunch][6];
 } else {
  cout<<" getBCIDCounter; Error: bunch address "<<bunch
      <<" not consistent with nBunMax= "<<nBunMax
      <<endl;
 }
return output;
}//end-of-getBCIDCounter
//-----------
bool   Pad::isOldSimulation(){
    return m_oldSimulation;
}
//----------
//-----------
void   Pad::setOldSimulation(bool value){
    m_oldSimulation=value;
}
//----------
