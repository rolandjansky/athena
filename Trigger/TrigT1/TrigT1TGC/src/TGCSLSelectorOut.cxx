/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSLSelectorOut.hh"
#include <iostream>

namespace LVL1TGCTrigger {

TGCSLSelectorOut::TGCSLSelectorOut():nCandidate(0)
{
  for( int i=0; i<NCandidateInSLSelector; i+=1){
    ptLevel[i]=0;
    r[i]=0;
    phi[i]=0;
    dr[i]=99;
    dphi[i]=99;
    iVeto[i]=false;
  }
}

void TGCSLSelectorOut::setPtLevel(int order, int ptLevelIn)
{
  if(order<NCandidateInSLSelector){
    ptLevel[order]=ptLevelIn;
    nCandidate++;
  }
}

void TGCSLSelectorOut::print() const
{
#ifdef TGCCOUT
  for(int i=0; i<nCandidate; i+=1){
    std::cout << i << " PtLevel=" << ptLevel[i]
	      << " R=" << r[i] 
	      << " Phi=" << phi[i]
	      << "InnerVetor=" << iVeto[i]
	      << std::endl;
  }
#endif
}


} //end of namespace bracket
