/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/PadTrigger.h"

#include "TrigT1NSWSimTools/PadData.h"
#include <iostream>
#include "TrigT1NSWSimTools/tdr_compat_enum.h"
namespace NSWL1 {

uint16_t PadTrigger::bctag() const {
    return m_pads.size() ? this->firstPad()->BC_Tag() : 0x0;
}

int PadTrigger::sideId() const {
  return m_pads.size() ? this->firstPad()->sideId() : -1;
} 

int PadTrigger::sectorId() const {
  return m_pads.size() ? this->firstPad()->sectorId() : -1;
}

int PadTrigger::triggerSectorNumber() const{
  return m_pads.size() ? this->firstPad()->triggerSectorNumber() : -1;
}


std::shared_ptr<PadData> PadTrigger::firstPad() const{ //returns the first non-null  pad from inner wedge
  if(this->m_pads.at(0)->padNumber()!=nullPadNumber) return this->m_pads.at(0);
  return this->m_pads.at(1);
}

std::shared_ptr<PadData> PadTrigger::firstPadInner() const{//returns the first non-null  pad from inner wedge
  return this->firstPad();
}

std::shared_ptr<PadData> PadTrigger::firstPadOuter() const{//returns the first non-null  pad from outer wedge
  if(this->m_pads.at(4)->padNumber()!=nullPadNumber) return this->m_pads.at(4);
  return this->m_pads.at(5);
}



} // NSWL1


