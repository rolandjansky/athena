/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainerCnv_tlp1.h"
 
 
TrigTauClusterDetailsContainerCnv_tlp1::TrigTauClusterDetailsContainerCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigTauClusterDetailsContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_TauClusterDetailsCnv); 
}

void TrigTauClusterDetailsContainerCnv_tlp1::setPStorage( TrigTauClusterDetailsContainer_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigTauClusterDetailsContainers );
  m_TauClusterDetailsCnv.        setPStorage(&storage->m_TauClusterDetails);
}

