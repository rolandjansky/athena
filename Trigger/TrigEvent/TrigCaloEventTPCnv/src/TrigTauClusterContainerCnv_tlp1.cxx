/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_tlp1.h"
 
 
TrigTauClusterContainerCnv_tlp1::TrigTauClusterContainerCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigTauClusterContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverterForReading( &m_TauClusterCnv); 
  addTPConverterForReading( &m_CaloClusterCnv);    
  addTPConverter( &m_TauClusterCnv_p2); 
  addTPConverter( &m_CaloClusterCnv_p2);    
}

void TrigTauClusterContainerCnv_tlp1::setPStorage( TrigTauClusterContainer_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigTauClusterContainers );
  m_TauClusterCnv.     setPStorage(&storage->m_TauCluster);
  m_CaloClusterCnv.    setPStorage(&storage->m_CaloCluster);
  m_TauClusterCnv_p2.  setPStorage(&storage->m_TauCluster_p2);
  m_CaloClusterCnv_p2. setPStorage(&storage->m_CaloCluster_p2);
}
