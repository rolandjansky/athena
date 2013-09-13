/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/JetElementCollectionCnv_tlp1.h"

JetElementCollectionCnv_tlp1::JetElementCollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_JetElementCnv );
}

void JetElementCollectionCnv_tlp1::setPStorage( JetElementCollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_JetElementCollection );

  m_JetElementCnv.setPStorage(&storage->m_JetElements);
}
