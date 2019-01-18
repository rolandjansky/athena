/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCSSCControllerOut.cxx,v 1.5 2005-11-29 08:12:40 nkanaya Exp $
#include "TrigT1TGC/TGCSSCControllerOut.hh"
#include <iostream>

namespace LVL1TGCTrigger {

const int TGCSSCControllerOut::s_chamber[TotalNumTGCRegionType][TGCSSCControllerOut::MaxNumberOfSubSectorCluster]
 = {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
    { 0, 0, 0, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4 } 
   }; 
    
TGCSSCControllerOut::TGCSSCControllerOut(TGCRegionType region): 
  m_regionType(region) 
{ 
  for(int i=0; i<MaxNumberOfChamberInR; i++ ){
    for(int j=0; j<MaxNumberOfPhiInSSC; j++) {
      m_phi[i][j]   =0;
      m_dPhi[i][j]  = 0;
      m_ptPhi[i][j] =0;
      m_hitPhi[i][j] = false;
    }
  }

  for (int i=0; i<MaxNumberOfSubSectorCluster; i++ ){
    m_r[i]    = 0;
    m_dR[i]   = 0;
    m_ptR[i]  = 0; 
    m_hitR[i] = false;
  }
} 

bool TGCSSCControllerOut::hasHit(int ssc, bool ored) const
{
  for(int phiposInSSC = 0 ;phiposInSSC < MaxNumberOfPhiInSSC; phiposInSSC++){
    if(!ored){
      if(m_hitR[ssc]&&m_hitPhi[getChamberNumber(ssc)][phiposInSSC]) return true;
    } else if(hasChamberBoundary(ssc)){
      int idx =getOredChamberNumber(ssc);
      if( m_hitR[ssc] && (idx>=0) &&
         m_hitPhi[idx][phiposInSSC]) return true;
    }
  }
  return false;
}

bool TGCSSCControllerOut::hasHit(int ssc, int phiposInSSC, bool ored) const
{
  if(!ored){
    if(m_hitR[ssc]&&m_hitPhi[getChamberNumber(ssc)][phiposInSSC]) return true;
  } else if(hasChamberBoundary(ssc)){
    int idx =getOredChamberNumber(ssc);
    if( m_hitR[ssc] && (idx>=0) &&
       m_hitPhi[idx][phiposInSSC]) return true;
  }
  return false;
}

void TGCSSCControllerOut::print() const
{
#ifdef TGCCOUT
  int i,j,k;
  std::cout<<std::endl;
  std::cout<<"TGCSSCControllerOut::print() begin"<<std::endl;

  for( j=0; j<getNumberOfChamberInR(); j+=1)
    for( k=0; k<MaxNumberOfPhiInSSC; k++)
      if(m_hitPhi[j][k]) std::cout<<"#Chamber= "<< j << " Phi" << k
                        <<" phi= "<<m_phi[j][k]<<" dPhi= "<<m_dPhi[j][k]
                        <<" H/L= "<<m_ptPhi[j][k]<<std::endl;

  for( i=0; i<getNumberOfSubSectorCluster(); i+=1)
    if(m_hitR[i]) std::cout<<"#SSC= "<<i
                    <<" r= "<<m_r[i]<<" dR= "<<m_dR[i]
                    <<" H/L= "<<m_ptR[i]<<std::endl;

  std::cout<<"TGCSSCControllerOut::print() end"<<std::endl;
#endif
}

void TGCSSCControllerOut::clear()
{
  int i, j;
  for( i=0; i<getNumberOfChamberInR(); i+=1)
    for( j=0; j<MaxNumberOfPhiInSSC; j++)
      m_hitPhi[i][j]=false;

  for( i=0; i<getNumberOfSubSectorCluster(); i+=1)
    m_hitR[i]=false;
}


} //end of namespace bracket
