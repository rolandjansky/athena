/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
*/

// ====================================================================
/*
        TGCNSW.cc            
*/
// ====================================================================

#include <iostream>
#include <iomanip>

#include "TrigT1TGC/TGCNSW.h"
#include "TrigT1TGC/NSWTrigOut.h"

namespace LVL1TGCTrigger {

// ====================================================================
//
// class description
//
// ====================================================================

TGCNSW::TGCNSW(){
  for (int side=0; side < 2; side++) {
    for (int TP=0; TP < NumberOfNSWTriggerProcesser; TP++) {
      std::vector<int> vecTP;
      vecTP.push_back(TP);
      m_buffer[side][TP].reset(new NSWTrigOut(side, vecTP)); 
    }
  }
}


TGCNSW::~TGCNSW(){
}

TGCNSW::TGCNSW(const TGCNSW& right){
  for(int idx1=0; idx1<2; idx1++){
    for (int idx2=0; idx2<NumberOfNSWTriggerProcesser; idx2++){
      m_buffer[idx1][idx2] = 0; 
    }
  }
  *this= right;
}


const TGCNSW& TGCNSW::operator=(const TGCNSW& right){
  if (this != &right) {
    for(int  idx1=0; idx1<2; idx1++){
      for (int idx2=0; idx2<NumberOfNSWTriggerProcesser; idx2++){
	m_buffer[idx1][idx2].reset(new NSWTrigOut(*(right.m_buffer[idx1][idx2])));
      }
    }
  }
  return *this;
}

std::shared_ptr<const NSWTrigOut> TGCNSW::getOutput(TGCRegionType region ,int side,int TGC_TriggerSector) const{

  std::shared_ptr<NSWTrigOut> tirgNSW_output;
  tirgNSW_output.reset(new NSWTrigOut());
  tirgNSW_output->clear();
  if ( (side<0)||(side>1) ) return 0;
  if(region==ENDCAP){
    if ( TGC_TriggerSector<0 || TGC_TriggerSector>47 ) return 0;
    ////NSW Large Secor Info
    int NSW_TriggerSecot=(TGC_TriggerSector+1)/6*2;
    if(NSW_TriggerSecot==16){NSW_TriggerSecot=0;}
    if(NSW_TriggerSecot>=0 && NSW_TriggerSecot<=15){
      *tirgNSW_output+=*m_buffer[side][NSW_TriggerSecot];
    }

    ////NSW Small Sector Info
    if(TGC_TriggerSector%6==1 || TGC_TriggerSector%6==2){
      return tirgNSW_output;
    }
    NSW_TriggerSecot=(TGC_TriggerSector+1+(TGC_TriggerSector-1)/6*2)/4;
    if(NSW_TriggerSecot==0){NSW_TriggerSecot=15;}
    if(NSW_TriggerSecot>=0 && NSW_TriggerSecot<=15){
      *tirgNSW_output+=*m_buffer[side][NSW_TriggerSecot];
    }

    return tirgNSW_output;
  }

  if(region==FORWARD){
    if ( TGC_TriggerSector<0 || TGC_TriggerSector>23 ) return 0;
    ////NSW Large Secor Info
    int NSW_TriggerSecot=(TGC_TriggerSector+1)/3*2;
    if(NSW_TriggerSecot==16){NSW_TriggerSecot=0;}
    if(NSW_TriggerSecot>=0 && NSW_TriggerSecot<=15){
      *tirgNSW_output+=*m_buffer[side][NSW_TriggerSecot];
    }

    ///NSW Small Sector Info
    NSW_TriggerSecot=2*(TGC_TriggerSector+2)/3-1;
    if(NSW_TriggerSecot==-1){NSW_TriggerSecot=15;}
    if(NSW_TriggerSecot>=0 && NSW_TriggerSecot<=15){
      *tirgNSW_output+=*m_buffer[side][NSW_TriggerSecot];
    }
    return tirgNSW_output;
  } 


  return 0;  
}


void  TGCNSW::setOutput(int side, int NSWTriggerProcesser, int NSWeta_8bit, int NSWphi_6bit, int NSWDtheta_5bit){
  if ( (side<0)||(side>1) ) return;//side 0::Aside 1::Cside
  if ( (NSWTriggerProcesser<0) || (NSWTriggerProcesser>=NumberOfNSWTriggerProcesser) ) return;
  m_buffer[side][NSWTriggerProcesser]->setSide(side);
  m_buffer[side][NSWTriggerProcesser]->setNSWTriggerProcessor(NSWTriggerProcesser);
  m_buffer[side][NSWTriggerProcesser]->setEta(NSWeta_8bit);
  m_buffer[side][NSWTriggerProcesser]->setPhi(NSWphi_6bit);
  m_buffer[side][NSWTriggerProcesser]->setDtheta(NSWDtheta_5bit);
}

void  TGCNSW::eraseOutput(){
  for(int idx1=0; idx1<2; idx1++){
    for (int idx2=0; idx2<NumberOfNSWTriggerProcesser; idx2++){
      m_buffer[idx1][idx2]->clear(); 
    }
  }
}

void TGCNSW::print() const{
  for(int idx1=0; idx1<2; idx1++){
    for (int idx2=0; idx2<NumberOfNSWTriggerProcesser; idx2++){
      m_buffer[idx1][idx2]->print(); 
    }
  }
}
  

} //end of namespace bracket
