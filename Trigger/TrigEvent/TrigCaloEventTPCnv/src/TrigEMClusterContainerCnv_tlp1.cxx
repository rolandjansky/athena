/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEventTPCnv/TrigEMClusterContainerCnv_tlp1.h"



TrigEMClusterContainerCnv_tlp1::TrigEMClusterContainerCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigEMClusterContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverterForReading( &m_EMClusterCnv); 
  addTPConverterForReading( &m_CaloClusterCnv);    
  addTPConverter( &m_EMClusterCnv_p2); 
  addTPConverter( &m_CaloClusterCnv_p2);    
}

void TrigEMClusterContainerCnv_tlp1::setPStorage( TrigEMClusterContainer_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigEMClusterContainers );
  m_EMClusterCnv.     setPStorage(&storage->m_EMCluster);
  m_CaloClusterCnv.   setPStorage(&storage->m_CaloCluster);
  m_EMClusterCnv_p2.  setPStorage(&storage->m_EMCluster_p2);
  m_CaloClusterCnv_p2.setPStorage(&storage->m_CaloCluster_p2);
}
