/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CMXJetTobCollectionCnv_tlp1.h"

CMXJetTobCollectionCnv_tlp1::CMXJetTobCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CMXJetTobCnv );
}

void CMXJetTobCollectionCnv_tlp1::setPStorage( CMXJetTobCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CMXJetTobCollection );

  m_CMXJetTobCnv.setPStorage(&storage->m_CMXJetTob);
}
