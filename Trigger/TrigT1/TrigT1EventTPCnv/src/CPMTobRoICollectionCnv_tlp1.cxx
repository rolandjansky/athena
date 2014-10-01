/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CPMTobRoICollectionCnv_tlp1.h"

CPMTobRoICollectionCnv_tlp1::CPMTobRoICollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CPMTobRoICnv );
}

void CPMTobRoICollectionCnv_tlp1::setPStorage( CPMTobRoICollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CPMTobRoICollection );

  m_CPMTobRoICnv.setPStorage(&storage->m_CPMTobRoI);
}
