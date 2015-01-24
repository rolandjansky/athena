/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//$Id: TGCRPhiCoincidenceOut.cxx,v 1.3 2003-12-13 16:01:32 mishino Exp $
#include "TrigT1TGC/TGCRPhiCoincidenceOut.hh"
#include <iostream>
#include <assert.h>

namespace LVL1TGCTrigger {

TGCRPhiCoincidenceOut::TGCRPhiCoincidenceOut():
  idSSC(-1),
  phi(-1),
  r(-1),
  dR(0),
  dPhi(0),
  innerVeto(false)
{
  for( int i=1; i <= NumberOfPtLevel; i+=1) hit[i]=false;
}

bool TGCRPhiCoincidenceOut::hasHit() const
{
  for(int i=1; i<=NumberOfPtLevel; i+=1)
    if(hit[i]) return true;
  return false;
}

void TGCRPhiCoincidenceOut::clear()
{
  idSSC = -1;
  phi = -1;
  r = -1;
  for( int i=1; i <= NumberOfPtLevel; i+=1) hit[i]=false;
  innerVeto = false;
}

bool TGCRPhiCoincidenceOut::isSuperior(const TGCRPhiCoincidenceOut* right) const {
  //selection rule  1. large pT   2. small eta  3. small phi
  for(int pt=NumberOfPtLevel; pt>=1; pt--){
    if(this->getHit(pt)&&!right->getHit(pt)) return true;
    else if(!this->getHit(pt)&&right->getHit(pt)) return false;
    else if(this->getHit(pt)&&right->getHit(pt)) {
      if(this->getR()<right->getR()) return true;
      else if(this->getR()>right->getR()) return false;
      else {
        if(this->getPhi()<=right->getPhi()) return true;
        else return false;
      }
    }
  }
  return true;
}

void TGCRPhiCoincidenceOut::print() const
{
#ifdef TGCCOUT
  std::cout <<"TGCRPhiCoincidenceOut::print()" << std::endl;
  std::cout <<" phi= " << phi << " r= " << r << std::endl;
  std::cout <<" Pt= ";

  int i;
  for( i=1; i<=NumberOfPtLevel; i+=1){
    if(hit[i]) std::cout<<" "<<i;
  }
  std::cout<<std::endl;

  std::cout <<" Veto= ";
  if (innerVeto) std::cout<<" 1";
  else           std::cout<<" 0";
  std::cout<<std::endl;

#endif
}


} //end of namespace bracket
