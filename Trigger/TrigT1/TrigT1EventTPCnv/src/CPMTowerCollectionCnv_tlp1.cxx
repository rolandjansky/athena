/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/CPMTowerCollectionCnv_tlp1.h"

CPMTowerCollectionCnv_tlp1::CPMTowerCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_CPMTowerCnv );
}

void CPMTowerCollectionCnv_tlp1::setPStorage( CPMTowerCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_CPMTowerCollection );

  m_CPMTowerCnv.setPStorage(&storage->m_CPMTowers);
}
