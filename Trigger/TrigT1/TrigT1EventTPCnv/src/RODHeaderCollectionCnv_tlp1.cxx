/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/RODHeaderCollectionCnv_tlp1.h"

RODHeaderCollectionCnv_tlp1::RODHeaderCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_RODHeaderCnv );
}

void RODHeaderCollectionCnv_tlp1::setPStorage( RODHeaderCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_RODHeaderCollection );

  m_RODHeaderCnv.setPStorage(&storage->m_RODHeaders);
}
