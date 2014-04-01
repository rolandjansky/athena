/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEventTPCnv/RingerRingsCnv_tlp1.h"

RingerRingsCnv_tlp1::RingerRingsCnv_tlp1() {
  // add the "main" base class converter (ie. RingerRingsCnv)
  addMainTPConverter();
}

void RingerRingsCnv_tlp1::setPStorage(RingerRings_tlp1* storage) {
   // for the base class converter
   setMainCnvPStorage(&storage->m_ringerRings);
}

