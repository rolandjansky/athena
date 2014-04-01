/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEventTPCnv/TrigRNNOutputContainerCnv_tlp1.h"

TrigRNNOutputContainerCnv_tlp1::TrigRNNOutputContainerCnv_tlp1() {
  // add the "main" base class converter
  addMainTPConverter();

  // Add all converters defined in this top level converter:
  addTPConverter(&m_trigRNNOutputCnv); 
}

void TrigRNNOutputContainerCnv_tlp1::setPStorage(
         TrigRNNOutputContainer_tlp1 *storage) {
  setMainCnvPStorage(&storage->m_TrigRNNOutputContainers);
  m_trigRNNOutputCnv.setPStorage(&storage->m_TrigRNNOutput);
}
