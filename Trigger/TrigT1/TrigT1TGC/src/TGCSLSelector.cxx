/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSLSelector.h"
#include "TrigT1TGC/TGCSectorLogic.h"
#include "TrigT1TGC/TGCSLSelectorOut.h"
#include "TrigT1TGC/TGCSLPreSelectorOut.h"

namespace LVL1TGCTrigger {

TGCSLSelector::TGCSLSelector(const TGCSectorLogic* sl)
 : m_sectorLogic(sl)
{}

TGCSLSelector::~TGCSLSelector()
{}

// select Highest Pt tracks from all Pt Level. 
bool TGCSLSelector::select(TGCSLPreSelectorOut* in, std::shared_ptr<TGCSLSelectorOut> out)
{
  int nCan=0;
  int ptStart=NumberOfPtLevel;
  for(int i=0; (i<NumberOfPtLevel)&&(nCan<TGCSLSelectorOut::s_NCandidateInSLSelector); i++) {
    int pt = ptStart - i;
    for(int j=0; (j<TGCSLPreSelectorOut::NCandidateInSLPreSelector)&&
	         (nCan<TGCSLSelectorOut::s_NCandidateInSLSelector); j++) {

      if(in->getHit(pt, j)){
	out->setPtLevel(nCan,pt);
	int R = 2 * in->getIdSSC(pt,j) + in->getR(pt,j) - (m_sectorLogic->getRegion()==Endcap ? 1 : 0);
	out->setR(nCan, R);
	out->setPhi(nCan, in->getPhi(pt,j));
	out->setDR(nCan, 2 * in->getDR(pt,j));
	out->setDPhi(nCan, in->getDPhi(pt,j));
	out->setInnerVeto(nCan, in->getInnerVeto(pt,j));
	nCan++;
      }

    }
  }
  return (nCan>0) ? true : false;
}

}   // end of namespace
