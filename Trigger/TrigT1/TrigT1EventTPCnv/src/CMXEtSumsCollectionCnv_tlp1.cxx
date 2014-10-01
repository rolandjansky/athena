/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CMXEtSumsCollectionCnv_tlp1.h"

CMXEtSumsCollectionCnv_tlp1::CMXEtSumsCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CMXEtSumsCnv );
}

void CMXEtSumsCollectionCnv_tlp1::setPStorage( CMXEtSumsCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CMXEtSumsCollection );

  m_CMXEtSumsCnv.setPStorage(&storage->m_CMXEtSums);
}
