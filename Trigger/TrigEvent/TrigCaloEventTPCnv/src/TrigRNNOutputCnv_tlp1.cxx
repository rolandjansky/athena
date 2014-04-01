/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEventTPCnv/TrigRNNOutputCnv_tlp1.h"

TrigRNNOutputCnv_tlp1::TrigRNNOutputCnv_tlp1() {
  // add the "main" base class converter (ie. RingerRingsCnv)
  addMainTPConverter();
}

void TrigRNNOutputCnv_tlp1::setPStorage(TrigRNNOutput_tlp1* storage) {
   // for the base class converter
   setMainCnvPStorage(&storage->m_trigRNNOutput);
}
