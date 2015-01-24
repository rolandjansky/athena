/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCPatchPanelOut.hh"
#include "TrigT1TGC/TGCPatchPanel.hh"
#include "TrigT1TGC/TGCHitPattern.hh"
#include <cstdlib>
#include <iostream>

namespace LVL1TGCTrigger {

TGCPatchPanelOut::TGCPatchPanelOut()
  :bid(0), origin(0)
{
  for(int  i=0; i<NumberOfConnectorPerPPOut; i+=1){
    signalPattern[i] = 0;
  }
}

TGCPatchPanelOut::~TGCPatchPanelOut()
{
  origin=0;
  for(int i=0; i<NumberOfConnectorPerPPOut; i+=1){
    if(signalPattern[i]!=0) delete signalPattern[i];
      signalPattern[i] = 0;
  }
}

TGCPatchPanelOut::TGCPatchPanelOut(const TGCPatchPanelOut& right)
  :bid(0), origin(0)
{
  for(int  i=0; i<NumberOfConnectorPerPPOut; i+=1){
    signalPattern[i] = 0;
  }
  *this = right;
}

TGCPatchPanelOut& TGCPatchPanelOut::operator=(const TGCPatchPanelOut& right)
{
  if (this != &right) {
    bid = right.bid;
    origin = right.origin;

    for(int i=0; i<NumberOfConnectorPerPPOut; i+=1){
      if (signalPattern[i]!=0) delete signalPattern[i];
      signalPattern[i] = new TGCHitPattern ( *(right.signalPattern[i]) );
    }
  }
  return *this;
}

void TGCPatchPanelOut::deleteHitPattern(int i)
{
  if(signalPattern[i]!=0) delete signalPattern[i];
  signalPattern[i]=0;
}

void TGCPatchPanelOut::print() const
{
#ifdef TGCCOUT
  std::cout <<"PatchPanelOut: bid= "<<bid<<" PPID= "<<origin->getId()
       <<" PPType= "<<origin->getType()<<" PPRegion= "<<origin->getRegion()<<std::endl;
#endif
  int i;
  for( i=0; i<NumberOfConnectorPerPPOut; i++){
    if(signalPattern[i]!=0){
#ifdef TGCCOUT
      std::cout << "Connector"<<i<<std::endl;
#endif
      signalPattern[i]->print();
    }
  }
}



} //end of namespace bracket
