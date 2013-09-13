/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CPMHitsCollectionCnv_tlp1.h"

CPMHitsCollectionCnv_tlp1::CPMHitsCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CPMHitsCnv );
}

void CPMHitsCollectionCnv_tlp1::setPStorage( CPMHitsCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CPMHitsCollection );

  m_CPMHitsCnv.setPStorage(&storage->m_CPMHits);
}
