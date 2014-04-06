/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticleTPCnv/TrigEFBjetContainerCnv_tlp1.h"


TrigEFBjetContainerCnv_tlp1::TrigEFBjetContainerCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigEFBjetContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_EFBjetCnv); 
}

void TrigEFBjetContainerCnv_tlp1::setPStorage( TrigEFBjetContainer_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigEFBjetContainers );
  m_EFBjetCnv.        setPStorage(&storage->m_EFBjet);
}

