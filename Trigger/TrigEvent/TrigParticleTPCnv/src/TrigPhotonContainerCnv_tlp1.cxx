/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticleTPCnv/TrigPhotonContainerCnv_tlp1.h"
 
 
TrigPhotonContainerCnv_tlp1::TrigPhotonContainerCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigPhotonContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_PhotonCnv); 
  addTPConverter( &m_EMclusterCnv);    
  addTPConverter( &m_CaloClusterCnv);    
  addTPConverter( &m_P4PtEtaPhiMCnv);
}

void TrigPhotonContainerCnv_tlp1::setPStorage( TrigPhotonContainer_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigPhotonContainers );
  m_PhotonCnv.        setPStorage(&storage->m_Photon);
  m_EMclusterCnv.     setPStorage(&storage->m_EMcluster);
  m_CaloClusterCnv.   setPStorage(&storage->m_CaloCluster);
  m_P4PtEtaPhiMCnv.   setPStorage(&storage->m_P4PtEtaPhiM);
}

