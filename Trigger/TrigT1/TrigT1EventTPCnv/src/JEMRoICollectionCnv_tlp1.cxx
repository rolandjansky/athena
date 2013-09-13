/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/JEMRoICollectionCnv_tlp1.h"

JEMRoICollectionCnv_tlp1::JEMRoICollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_JEMRoICnv );
}

void JEMRoICollectionCnv_tlp1::setPStorage( JEMRoICollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_JEMRoICollection );

  m_JEMRoICnv.setPStorage(&storage->m_JEMRoI);
}
