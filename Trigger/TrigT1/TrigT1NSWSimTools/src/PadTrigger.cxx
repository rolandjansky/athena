/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/SectorTriggerCandidate.h"

#include "TrigT1NSWSimTools/PadData.h"
#include <iostream>
#include "TrigT1NSWSimTools/tdr_compat_enum.h"
namespace NSWL1 {

uint16_t PadTrigger::bctag() const {
    return m_pads.size() ? m_pads[0]->BC_Tag() : 0x0;
}

int PadTrigger::sideId() const {
  return m_pads.size() ? m_pads[0]->sideId() : -1;
} 

int PadTrigger::sectorId() const {
  return m_pads.size() ? m_pads[0]->sectorId() : -1;
}


} // NSWL1


