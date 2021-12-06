/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/


#include "TrigT1TGC/TGCBIS78.h"
#include "TrigT1TGC/BIS78TrigOut.h"
#include <iostream> // will be removed

namespace LVL1TGCTrigger {

TGCBIS78::TGCBIS78()
{
  for (int PadBoard=0; PadBoard < NumberOfBIS78PadBoard; PadBoard++) {
    std::vector<int> vecPadBoard;
    vecPadBoard.push_back(PadBoard);
    m_buffer[PadBoard].reset(new BIS78TrigOut(vecPadBoard)); 
  }
}


TGCBIS78::~TGCBIS78()
{
}

TGCBIS78::TGCBIS78(const TGCBIS78& right)
{
  for (int idx=0; idx<NumberOfBIS78PadBoard; idx++){
    m_buffer[idx] = 0; 
  }
  *this= right;
}


const TGCBIS78& TGCBIS78::operator=(const TGCBIS78& right)
{
  if (this != &right) {
    for (int idx=0; idx<NumberOfBIS78PadBoard; idx++){
      m_buffer[idx].reset(new BIS78TrigOut(*(right.m_buffer[idx])));
    }
  }
  return *this;
}

std::shared_ptr<const BIS78TrigOut> TGCBIS78::getOutput(TGCRegionType region ,int TGC_TriggerSector) const
{
  std::shared_ptr<BIS78TrigOut> trigBIS78_output;
  trigBIS78_output.reset(new BIS78TrigOut());
  trigBIS78_output->clear();
  if(region==ENDCAP){
    if ( TGC_TriggerSector<0 || TGC_TriggerSector>47 ) return 0;
    int BIS78_TriggerSect=(TGC_TriggerSector - 2)/6;
    if(TGC_TriggerSector < 2) BIS78_TriggerSect=7;
    if(BIS78_TriggerSect>=0 && BIS78_TriggerSect<=7){
      *trigBIS78_output+=*m_buffer[BIS78_TriggerSect];
    }

    return trigBIS78_output;
  }

  return 0;  
}


void TGCBIS78::setOutput(int BIS78PadBoard, uint8_t BIS78eta_6bit, uint8_t BIS78phi_6bit, uint8_t BIS78Deta_3bit, uint8_t BIS78Dphi_3bit, uint8_t BIS78flag3over3eta_1bit, uint8_t BIS78flag3over3phi_1bit) {

  if ( (BIS78PadBoard<0) || (BIS78PadBoard>=NumberOfBIS78PadBoard) ) return;
  m_buffer[BIS78PadBoard]->setBIS78PadBoard(BIS78PadBoard);
  m_buffer[BIS78PadBoard]->setEta(BIS78eta_6bit);
  m_buffer[BIS78PadBoard]->setPhi(BIS78phi_6bit);
  m_buffer[BIS78PadBoard]->setDeta(BIS78Deta_3bit);
  m_buffer[BIS78PadBoard]->setDphi(BIS78Dphi_3bit);
  m_buffer[BIS78PadBoard]->setflag3over3eta(BIS78flag3over3eta_1bit);
  m_buffer[BIS78PadBoard]->setflag3over3phi(BIS78flag3over3phi_1bit);
}


void TGCBIS78::eraseOutput()
{
  for (int idx=0; idx<NumberOfBIS78PadBoard; idx++){
    m_buffer[idx]->clear(); 
  }
}

void TGCBIS78::print() const
{
  for (int idx=0; idx<NumberOfBIS78PadBoard; idx++){
    m_buffer[idx]->print(); 
  }
}
  

}   // end of namespace
