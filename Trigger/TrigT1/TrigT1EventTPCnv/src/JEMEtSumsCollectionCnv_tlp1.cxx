/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/JEMEtSumsCollectionCnv_tlp1.h"

JEMEtSumsCollectionCnv_tlp1::JEMEtSumsCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_JEMEtSumsCnv );
}

void JEMEtSumsCollectionCnv_tlp1::setPStorage( JEMEtSumsCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_JEMEtSumsCollection );

  m_JEMEtSumsCnv.setPStorage(&storage->m_JEMEtSums);
}
