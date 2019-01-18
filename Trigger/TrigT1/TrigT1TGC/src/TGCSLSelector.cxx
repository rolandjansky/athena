/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSLSelector.hh"
#include "TrigT1TGC/TGCSectorLogic.hh"
#include "TrigT1TGC/TGCSLSelectorOut.hh"
#include "TrigT1TGC/TGCSLPreSelectorOut.hh"

namespace LVL1TGCTrigger {

// select Highest Pt tracks from all Pt Level. 
TGCSLSelectorOut* TGCSLSelector::select(TGCSLPreSelectorOut* PSOut,
					TGCSLSelectorOut* out)
{
  int pt,i,j,nCan=0;
  int ptStart=NumberOfPtLevel;
  for( i=0;(i<NumberOfPtLevel)&&(nCan<NCandidateInSLSelector); i+=1){
    pt=ptStart-i;
    for( j=0;(j<NCandidateInSLPreSelector)&&
	   (nCan<NCandidateInSLSelector); j+=1) {

      if(PSOut->getHit(pt,j)){
	out->setPtLevel(nCan,pt);
	int R= 2*PSOut->getIdSSC(pt,j)+PSOut->getR(pt,j) - (m_sectorLogic->getRegion()==Endcap ? 1 : 0);
	out->setR(nCan,R);
	out->setPhi(nCan,PSOut->getPhi(pt,j));
	out->setDR(nCan,2*PSOut->getDR(pt,j));
	out->setDPhi(nCan,PSOut->getDPhi(pt,j));
	out->setInnerVeto(nCan,PSOut->getInnerVeto(pt,j));
	nCan++;
      }

    }
  }
  return out;
}
TGCSLSelector::TGCSLSelector( const TGCSectorLogic* sL): 
  m_sectorLogic(sL) 
{ 
} 

} //end of namespace bracket
