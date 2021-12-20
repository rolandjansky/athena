/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCPatchPanelOut.h"
#include "TrigT1TGC/TGCPatchPanel.h"
#include "TrigT1TGC/TGCHitPattern.h"
#include <cstdlib>
#include <iostream>

namespace LVL1TGCTrigger {

TGCPatchPanelOut::TGCPatchPanelOut()
{
  for(int  i=0; i<NumberOfConnectorPerPPOut; i+=1){
    m_signalPattern[i] = 0;
  }
}

TGCPatchPanelOut& TGCPatchPanelOut::operator=(const TGCPatchPanelOut& right)
{
  if (this != &right) {
    m_bid = right.m_bid;
    m_origin = right.m_origin;

    for(int i=0; i<NumberOfConnectorPerPPOut; i+=1){
      if (m_signalPattern[i]!=0) delete m_signalPattern[i];
      m_signalPattern[i] = new TGCHitPattern ( *(right.m_signalPattern[i]) );
    }
  }
  return *this;
}

void TGCPatchPanelOut::deleteHitPattern(int i)
{
  if(m_signalPattern[i]!=0) delete m_signalPattern[i];
  m_signalPattern[i]=0;
}

void TGCPatchPanelOut::print() const
{
#ifdef TGCCOUT
  std::cout <<"PatchPanelOut: bid= "<<m_bid<<" PPID= "<<m_origin->getId()
       <<" PPType= "<<m_origin->getType()<<" PPRegion= "<<m_origin->getRegion()<<std::endl;
#endif
  int i;
  for( i=0; i<NumberOfConnectorPerPPOut; i++){
    if(m_signalPattern[i]!=0){
#ifdef TGCCOUT
      std::cout << "Connector"<<i<<std::endl;
#endif
      m_signalPattern[i]->print();
    }
  }
}



} //end of namespace bracket
