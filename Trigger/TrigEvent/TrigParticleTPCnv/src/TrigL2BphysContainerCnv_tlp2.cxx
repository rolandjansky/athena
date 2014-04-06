/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticleTPCnv/TrigL2BphysContainerCnv_tlp2.h"
 
 
TrigL2BphysContainerCnv_tlp2::TrigL2BphysContainerCnv_tlp2()
{
  // add the "main" base class converter (ie. TrigL2BphysContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_L2BphysCnv); 

}

void TrigL2BphysContainerCnv_tlp2::setPStorage( TrigL2BphysContainer_tlp2 *storage )
{
  setMainCnvPStorage( &storage->m_TrigL2BphysContainers );
  m_L2BphysCnv.               setPStorage(&storage->m_L2Bphys);
}

