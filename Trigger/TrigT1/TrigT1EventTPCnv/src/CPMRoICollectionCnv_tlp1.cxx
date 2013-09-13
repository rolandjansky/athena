/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CPMRoICollectionCnv_tlp1.h"

CPMRoICollectionCnv_tlp1::CPMRoICollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CPMRoICnv );
}

void CPMRoICollectionCnv_tlp1::setPStorage( CPMRoICollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CPMRoICollection );

  m_CPMRoICnv.setPStorage(&storage->m_CPMRoI);
}
