/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSLPreSelectorOut.hh"
#include <iostream>

namespace LVL1TGCTrigger {

TGCSLPreSelectorOut::TGCSLPreSelectorOut()
{
  for(int  i=1; i<=NumberOfPtLevel; i+=1)
    for( int j=0; j<NCandidateInSLPreSelector; j+=1){
      m_idSSC[i][j]=0;
      m_r[i][j]=0;
      m_phi[i][j]=0;
      m_dr[i][j]=99;
      m_dphi[i][j]=99;
      m_hit[i][j]=false;
      m_iVeto[i][j]=false;
    }      
}


void TGCSLPreSelectorOut::print() const
{
#ifdef TGCCOUT
  int i,j;
  std::cout<<std::endl<<"TGCSLPreSelectorOut::print() begin"<<std::endl;
  for( i=1; i<=NumberOfPtLevel; i+=1)
    for( j=0; j<NCandidateInSLPreSelector; j+=1)
      if(m_hit[i][j]) std::cout<<" No."<<j<<" Pt= "<<i<<" #SSC="<<m_idSSC[i][j]
			     <<" r= "<<m_r[i][j]<<" phi= "<<m_phi[i][j]
			     <<" veto= "<<m_iVeto[i][j] 
			     << std::endl;
  std::cout<<"TGCSLPreSelectorOut::print() end"<<std::endl;
#endif
}

} //end of namespace bracket
