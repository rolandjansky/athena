/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CMXJetHitsCollectionCnv_tlp1.h"

CMXJetHitsCollectionCnv_tlp1::CMXJetHitsCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CMXJetHitsCnv );
}

void CMXJetHitsCollectionCnv_tlp1::setPStorage( CMXJetHitsCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CMXJetHitsCollection );

  m_CMXJetHitsCnv.setPStorage(&storage->m_CMXJetHits);
}
