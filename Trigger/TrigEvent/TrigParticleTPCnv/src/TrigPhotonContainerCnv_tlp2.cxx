/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticleTPCnv/TrigPhotonContainerCnv_tlp2.h"
 
 
TrigPhotonContainerCnv_tlp2::TrigPhotonContainerCnv_tlp2()
{
  // add the "main" base class converter (ie. TrigPhotonContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_PhotonCnv); 
  addTPConverter( &m_P4PtEtaPhiMCnv);
  
}

void TrigPhotonContainerCnv_tlp2::setPStorage( TrigPhotonContainer_tlp2 *storage )
{
  setMainCnvPStorage( &storage->m_TrigPhotonContainers );
  m_PhotonCnv.        setPStorage(&storage->m_Photon);
  m_P4PtEtaPhiMCnv.   setPStorage(&storage->m_P4PtEtaPhiM);
}

