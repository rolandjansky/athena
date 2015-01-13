/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSectorLogic.hh"
#include "TrigT1TGC/TGCSLPreSelector.hh"
#include "TrigT1TGC/TGCSLPreSelectorOut.hh"
#include "TrigT1TGC/TGCRPhiCoincidenceOut.hh"
#include <iostream>

namespace LVL1TGCTrigger {

TGCSLPreSelector::TGCSLPreSelector(const TGCSLPreSelector& right)
{
  sectorLogic = right.sectorLogic;
  for( int i=0; i<MaxNumberOfSubSectorCluster; i+=1) coincidenceOut[i]=0;
  for( int i=0; i<sectorLogic->getNumberOfSubSectorCluster(); i+=1){
    coincidenceOut[i] = new TGCRPhiCoincidenceOut();
    *coincidenceOut[i]=*right.coincidenceOut[i];
  }
}

TGCSLPreSelector& TGCSLPreSelector::operator=(const TGCSLPreSelector& right)
{
  if(this!=&right){
    sectorLogic = right.sectorLogic;
    init();
    for( int i=0; i<sectorLogic->getNumberOfSubSectorCluster(); i+=1){
      coincidenceOut[i] = new TGCRPhiCoincidenceOut();
      *coincidenceOut[i]=*right.coincidenceOut[i];
    }
  }
  return *this;
}

TGCSLPreSelector::TGCSLPreSelector(const TGCSectorLogic* sL): 
  sectorLogic(sL) 
{
  for( int i=0; i<MaxNumberOfSubSectorCluster; i+=1) coincidenceOut[i]=0;
}

TGCSLPreSelector::~TGCSLPreSelector()
{
  for( int i=0; i<sectorLogic->getNumberOfSubSectorCluster(); i+=1){
    if(coincidenceOut[i]!=0) delete  coincidenceOut[i];
    coincidenceOut[i]=0;
  }
  sectorLogic=0;
}

void TGCSLPreSelector::init()
{
  for (int i=0; i<MaxNumberOfSubSectorCluster; ++i) {
    if(coincidenceOut[i]!=0) delete  coincidenceOut[i];
    coincidenceOut[i]=0;
  }
}

void TGCSLPreSelector::input(TGCRPhiCoincidenceOut* rPhiOut)
{
  if(rPhiOut!=0){
#ifdef TGCDEBUG
    std::cout << "TGCSLPreSelector inputs the TGCRPhiCoincidenceOut" << std::endl;
    rPhiOut->print();
#endif
    if(rPhiOut->hasHit()){
      coincidenceOut[rPhiOut->getIdSSC()]=rPhiOut;
#ifdef TGCDEBUG
      std::cout<<"input: sscid="<<rPhiOut->getIdSSC()<<std::endl;
#endif
    }else{
      coincidenceOut[rPhiOut->getIdSSC()]=0;
      // delete RPhiCoincidenceOut
      delete  rPhiOut;
    }
  }
}

TGCSLPreSelectorOut* TGCSLPreSelector::select()
{
  TGCSLPreSelectorOut* out = new TGCSLPreSelectorOut;
  int nCan[NumberOfPtLevel+1];

  for(int i=1; i<=NumberOfPtLevel; i+=1) nCan[i]=0;

  int pt,ssc;
  for( ssc=0;ssc<sectorLogic->getNumberOfSubSectorCluster(); ssc+=1){
    if(coincidenceOut[ssc]!=0){
      for( pt=1; pt<=NumberOfPtLevel; pt+=1){
	if((coincidenceOut[ssc]->getHit(pt))
	   &&(nCan[pt]<NCandidateInSLPreSelector)){
	  out->setIdSSC(pt, nCan[pt], ssc);
	  out->setR(pt, nCan[pt], coincidenceOut[ssc]->getR());
	  out->setPhi(pt, nCan[pt], coincidenceOut[ssc]->getPhi());
	  out->setDR(pt, nCan[pt], coincidenceOut[ssc]->getDR());
	  out->setDPhi(pt, nCan[pt], coincidenceOut[ssc]->getDPhi());
	  out->setInnerVeto(pt, nCan[pt], coincidenceOut[ssc]->getInnerVeto());
	  nCan[pt]+=1;
	}
      }
      delete coincidenceOut[ssc];
      coincidenceOut[ssc]=0;
    }
  }
  return out;
}

void TGCSLPreSelector::dumpInput() const
{
#ifdef TGCCOUT
  int ssc;
  std::cout<<std::endl<<"TGCSLPreSelector::dumpInput() begin"<<std::endl;
  for( ssc=0; ssc<sectorLogic->getNumberOfSubSectorCluster(); ssc+=1)
    if(coincidenceOut[ssc]!=0){
      std::cout<<" #SSC= "<<ssc<<" "<<std::endl;
      coincidenceOut[ssc]->print();
    }
  std::cout<<"TGCSLPreSelector::dumpInput() end"<<std::endl;
#endif
}

} //end of namespace bracket
