/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEventTPCnv/RingerRingsContainerCnv_tlp1.h"

RingerRingsContainerCnv_tlp1::RingerRingsContainerCnv_tlp1() {
  // add the "main" base class converter
  addMainTPConverter();

  // Add all converters defined in this top level converter:
  addTPConverter(&m_ringerRingsCnv); 
}

void RingerRingsContainerCnv_tlp1::setPStorage(
         RingerRingsContainer_tlp1 *storage) {
  setMainCnvPStorage(&storage->m_RingerRingsContainers);
  m_ringerRingsCnv.setPStorage(&storage->m_RingerRings);
}

