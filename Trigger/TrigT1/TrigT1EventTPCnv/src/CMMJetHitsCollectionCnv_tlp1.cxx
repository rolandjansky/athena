/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CMMJetHitsCollectionCnv_tlp1.h"

CMMJetHitsCollectionCnv_tlp1::CMMJetHitsCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CMMJetHitsCnv );
}

void CMMJetHitsCollectionCnv_tlp1::setPStorage( CMMJetHitsCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CMMJetHitsCollection );

  m_CMMJetHitsCnv.setPStorage(&storage->m_CMMJetHits);
}
