/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CMXCPHitsCollectionCnv_tlp1.h"

CMXCPHitsCollectionCnv_tlp1::CMXCPHitsCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CMXCPHitsCnv );
}

void CMXCPHitsCollectionCnv_tlp1::setPStorage( CMXCPHitsCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CMXCPHitsCollection );

  m_CMXCPHitsCnv.setPStorage(&storage->m_CMXCPHits);
}
