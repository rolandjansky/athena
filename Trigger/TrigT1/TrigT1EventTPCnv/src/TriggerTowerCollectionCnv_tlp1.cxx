/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/TriggerTowerCollectionCnv_tlp1.h"

TriggerTowerCollectionCnv_tlp1::TriggerTowerCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_TriggerTowerCnv );
}

void TriggerTowerCollectionCnv_tlp1::setPStorage( TriggerTowerCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_TriggerTowerCollection );

  m_TriggerTowerCnv.setPStorage(&storage->m_TriggerTowers);
}
