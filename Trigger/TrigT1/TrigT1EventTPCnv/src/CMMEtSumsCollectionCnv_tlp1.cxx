/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CMMEtSumsCollectionCnv_tlp1.h"

CMMEtSumsCollectionCnv_tlp1::CMMEtSumsCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CMMEtSumsCnv );
}

void CMMEtSumsCollectionCnv_tlp1::setPStorage( CMMEtSumsCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CMMEtSumsCollection );

  m_CMMEtSumsCnv.setPStorage(&storage->m_CMMEtSums);
}
