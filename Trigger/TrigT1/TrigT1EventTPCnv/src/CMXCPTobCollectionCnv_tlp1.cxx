/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CMXCPTobCollectionCnv_tlp1.h"

CMXCPTobCollectionCnv_tlp1::CMXCPTobCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CMXCPTobCnv );
}

void CMXCPTobCollectionCnv_tlp1::setPStorage( CMXCPTobCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CMXCPTobCollection );

  m_CMXCPTobCnv.setPStorage(&storage->m_CMXCPTob);
}
