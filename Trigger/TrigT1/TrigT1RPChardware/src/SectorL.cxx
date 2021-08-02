/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include "TrigT1RPChardware/Matrix.h"
#include "TrigT1RPChardware/Pad.h"
#include "TrigT1RPChardware/SectorL.h"

using namespace std;

//--------------------------------------------------------------------//
SectorL::SectorL(int run, int event, CMAword /*debug*/,
                 ubit16 subsys, ubit16 sect)  
		 : BaseObject(Hardware,"SectorLogic") {
ubit16 i, j, k;
m_padData=7;
m_maxNumPads=10;
m_nBunMax=NOBXS;

for(i=0; i<m_nBunMax; i++) {
    m_numOfPads[i]=0;
}
 
m_run=run;
m_event=event;
m_subsystem=subsys;
m_sector=sect;    
        
for(i=0; i<m_nBunMax; i++) {
 m_numberOfRoIs[i]=0;
 m_sectorOutput[i]=0;
 for(k=0; k<10; k++) {
  m_sectorOut[i][k]=0;
 }
 for(k=0; k<2; k++) {
  m_pTArray[i][k][0]=0;
  m_pTArray[i][k][1]=0;
 }//end-of-for(k
 for(j=0; j<m_maxNumPads; j++){ 
  for(k=0; k<m_padData; k++) {
   m_sectorInput[i][j][k]=0;
  }//end-of-for(k
 }
}
}//end-of-SectorL::SectorL
//--------------------------------------------------------------------//
SectorL::~SectorL(){
}//end-of-SectorL::~SectorL
//-------------------------------------------------------------------//
void SectorL::load(ubit16 padAdd, ubit16 BX, 
                   ubit16 RoIAdd, ubit16 pT, ubit16 OPL,
		   ubit16 overlapPhi, ubit16 overlapEta, 
		   ubit16 RoIAmbiguity, ubit16 BCIDcounter) {

if( padAdd<10 && RoIAdd<4 && pT<7 && OPL<2 && overlapPhi<2 && 
    overlapEta<2 && RoIAmbiguity<2 ) {
 m_sectorInput[BX][m_numOfPads[BX]][0]=padAdd;
 m_sectorInput[BX][m_numOfPads[BX]][1]=RoIAdd;
 m_sectorInput[BX][m_numOfPads[BX]][2]=pT;
 m_sectorInput[BX][m_numOfPads[BX]][3]=OPL;
 m_sectorInput[BX][m_numOfPads[BX]][4]=overlapPhi;
 m_sectorInput[BX][m_numOfPads[BX]][5]=overlapEta;
 m_sectorInput[BX][m_numOfPads[BX]][6]=RoIAmbiguity;
 m_sectorInput[BX][m_numOfPads[BX]][7]=BCIDcounter;
 m_numOfPads[BX]++;
} else {
 throw std::out_of_range("problems loading Pad Output Data");
}
}//end-of-SectorL::load
//-------------------------------------------------------------------//
void SectorL::execute() {
//
// temporary code to fill at least partially the output
// buffer of the sector logic 
//
ubit16 i, j, k;
for(i=0; i<m_nBunMax; i++) {
 for(j=0; j<m_numOfPads[i]; j++) {
  if(m_sectorInput[i][j][2]) m_numberOfRoIs[i]++;
//  if(m_sectorInput[i][j][2]) {
//   cout<<" Bunch "<<i<<" PadInd "<<j<<" PT "<<m_sectorInput[i][j][2]
//       <<endl;
//  }//end-of-if(m_sectorInput[i][j][2]
  for(k=0; k<2; k++) {
   if(m_sectorInput[i][j][2]>m_pTArray[i][k][0]) {
    storePT(i,j,k);
    break;
   }//end-of-if(m_sector
  }//end-of-for(k
 }//end-of-for(j
}//end-of-for(i
//
//for(i=0; i<m_nBunMax; i++) {
// cout<<" pTArray[0] pT= "<<m_pTArray[i][0][0]
//     <<" padInd= "<<m_pTArray[i][0][1]<<endl
//     <<" pTArray[1] pT= "<<m_pTArray[i][1][0]
//     <<" padInd= "<<m_pTArray[i][1][1]<<endl
//     <<" number of RoIs= "<<m_numberOfRoIs[i]<<endl;
//}
//
// now fill the output buffer
//
for(i=0; i<m_nBunMax; i++) {

  // TEMP: use m_sectorOut[i][8] and [9] to store OVL1 and OVL2

  m_sectorOut[i][8] = 0;
  m_sectorOut[i][9] = 0;

 if(m_numberOfRoIs[i]>2) m_sectorOut[i][0]=1;
 if(m_numberOfRoIs[i]) {
  ubit16 padInd=m_pTArray[i][0][1];
  m_sectorOut[i][1] = 4*m_sectorInput[i][padInd][0]
                    +   m_sectorInput[i][padInd][1]
		    + 1;
  m_sectorOut[i][3] = m_sectorInput[i][padInd][2]; 
  m_sectorOut[i][8] = m_sectorInput[i][padInd][4]+2*m_sectorInput[i][padInd][5];

  if(m_numberOfRoIs[i]>1) {
   ubit16 padInd=m_pTArray[i][1][1];
   m_sectorOut[i][2] = 4*m_sectorInput[i][padInd][0]
                     +   m_sectorInput[i][padInd][1]
		     + 1;
   m_sectorOut[i][4] = m_sectorInput[i][padInd][2];
   m_sectorOut[i][9] = m_sectorInput[i][padInd][4]+2*m_sectorInput[i][padInd][5];
   
  }//end-of-if(m_numberOfRoIs[i]>1)
 }//end-of-if(m_numberOfRoIs[i])
 m_sectorOut[i][5] = 0;//>1 Candidate ROI1
 m_sectorOut[i][6] = 0;//>1 Candidate ROI2
 m_sectorOut[i][7] = 0;//BCID
 // m_sectorOut[i][8] = 0;//candidate1 sign
 //m_sectorOut[i][9] = 0;//candidate2 sign 
 if(!m_sectorOut[i][3]) m_sectorOut[i][3]=7;
 if(!m_sectorOut[i][4]) m_sectorOut[i][4]=7;

// cout<<" Bunch ID "<<i<<endl
//     <<" >2 Candidates in a sector "<<m_sectorOut[i][0]<<endl
//     <<" ROI 1 "<<m_sectorOut[i][1]<<endl
//     <<" ROI 2 "<<m_sectorOut[i][2]<<endl
//     <<" PT1 "<<m_sectorOut[i][3]<<endl
//     <<" PT2 "<<m_sectorOut[i][4]<<endl
//     <<" >1 Candidate ROI1 "<<m_sectorOut[i][5]<<endl
//     <<" >1 Candidate ROI2 "<<m_sectorOut[i][6]<<endl
//     <<" BCID counter "<<m_sectorOut[i][7]<<endl
//     <<" Candidate1 sign "<<m_sectorOut[i][8]<<endl
//     <<" Candidate2 sign "<<m_sectorOut[i][9]<<endl;
//
// now fille the Sector Logic Output word
// 
m_sectorOutput[i]=                    m_sectorOut[i][0];
m_sectorOutput[i]=m_sectorOutput[i] | (m_sectorOut[i][1]<< 1); // ROI1
m_sectorOutput[i]=m_sectorOutput[i] | (m_sectorOut[i][2]<<10); // ROI2
m_sectorOutput[i]=m_sectorOutput[i] | (m_sectorOut[i][3]<<19); // PT1
m_sectorOutput[i]=m_sectorOutput[i] | (m_sectorOut[i][4]<<22); // PT2
m_sectorOutput[i]=m_sectorOutput[i] | (m_sectorOut[i][5]<<25); // more than 1 _1
m_sectorOutput[i]=m_sectorOutput[i] | (m_sectorOut[i][6]<<26); // more than 1 _2
m_sectorOutput[i]=m_sectorOutput[i] | (m_sectorOut[i][7]<<27); // BX
m_sectorOutput[i]=m_sectorOutput[i] | (m_sectorOut[i][8]<<8); // OVL1
m_sectorOutput[i]=m_sectorOutput[i] | (m_sectorOut[i][9]<<17);// OVL2
}
}//end-of-SectorL::execute
//-------------------------------------------------------------------//
void SectorL::storePT(ubit16 bunch, ubit16 padInd, ubit16 modeInd) {
 if(!modeInd) {
  m_pTArray[bunch][1][0] = m_pTArray[bunch][0][0]; // copy pT
  m_pTArray[bunch][1][1] = m_pTArray[bunch][0][1]; // copy padAdd
  m_pTArray[bunch][0][0] = m_sectorInput[bunch][padInd][2];
  m_pTArray[bunch][0][1] = padInd;
 } else {
  m_pTArray[bunch][1][0] = m_sectorInput[bunch][padInd][2];
  m_pTArray[bunch][1][1] = padInd;
 }
}
//-------------------------------------------------------------------//
CMAword SectorL::output(ubit16 i) {
if(i<m_nBunMax) {
 return m_sectorOutput[i];
} else {
 return 0xffffffff;
}
}
