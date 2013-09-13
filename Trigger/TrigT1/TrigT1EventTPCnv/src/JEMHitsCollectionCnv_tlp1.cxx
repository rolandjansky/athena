/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/JEMHitsCollectionCnv_tlp1.h"

JEMHitsCollectionCnv_tlp1::JEMHitsCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_JEMHitsCnv );
}

void JEMHitsCollectionCnv_tlp1::setPStorage( JEMHitsCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_JEMHitsCollection );

  m_JEMHitsCnv.setPStorage(&storage->m_JEMHits);
}
