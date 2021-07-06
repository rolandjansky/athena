/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSLSelectorOut.h"
#include <iostream>

namespace LVL1TGCTrigger {

TGCSLSelectorOut::TGCSLSelectorOut()
 : m_nCandidate(0)
{
  for( int i=0; i<s_NCandidateInSLSelector; i+=1){
    m_ptLevel[i]=0;
    m_r[i]=0;
    m_phi[i]=0;
    m_dr[i]=99;
    m_dphi[i]=99;
    m_iVeto[i]=false;
  }
}

void TGCSLSelectorOut::reset()
{
  for(int i=0; i<s_NCandidateInSLSelector; i++) {
    m_ptLevel[i]=0;
    m_r[i]=0;
    m_phi[i]=0;
    m_dr[i]=99;
    m_dphi[i]=99;
    m_iVeto[i]=false;
  }
  m_nCandidate=0;
}

void TGCSLSelectorOut::setPtLevel(int order, int ptLevelIn)
{
  if(order<s_NCandidateInSLSelector){
    m_ptLevel[order]=ptLevelIn;
    m_nCandidate++;
  }
}

void TGCSLSelectorOut::print() const
{
#ifdef TGCCOUT
  for(int i=0; i<m_nCandidate; i+=1){
    std::cout << i << " PtLevel=" << m_ptLevel[i]
	      << " R=" << m_r[i] 
	      << " Phi=" << m_phi[i]
	      << "InnerVetor=" << m_iVeto[i]
	      << std::endl;
  }
#endif
}


} //end of namespace bracket
