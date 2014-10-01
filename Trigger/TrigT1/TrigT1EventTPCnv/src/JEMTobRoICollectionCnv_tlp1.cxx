/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1EventTPCnv/JEMTobRoICollectionCnv_tlp1.h"

JEMTobRoICollectionCnv_tlp1::JEMTobRoICollectionCnv_tlp1()
{
  addMainTPConverter();

  addTPConverter( &m_JEMTobRoICnv );
}

void JEMTobRoICollectionCnv_tlp1::setPStorage( JEMTobRoICollection_tlp1 *storage )
{

  setMainCnvPStorage( &storage->m_JEMTobRoICollection );

  m_JEMTobRoICnv.setPStorage(&storage->m_JEMTobRoI);
}
