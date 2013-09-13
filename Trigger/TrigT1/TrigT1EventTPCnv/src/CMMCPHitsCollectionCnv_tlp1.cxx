/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CMMCPHitsCollectionCnv_tlp1.h"

CMMCPHitsCollectionCnv_tlp1::CMMCPHitsCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CMMCPHitsCnv );
}

void CMMCPHitsCollectionCnv_tlp1::setPStorage( CMMCPHitsCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CMMCPHitsCollection );

  m_CMMCPHitsCnv.setPStorage(&storage->m_CMMCPHits);
}
