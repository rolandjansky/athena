/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticleTPCnv/TrigTauContainerCnv_tlp1.h"
 
 
TrigTauContainerCnv_tlp1::TrigTauContainerCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigTauContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_TauCnv); 
  addTPConverter( &m_P4PtEtaPhiMCnv);
}

void TrigTauContainerCnv_tlp1::setPStorage( TrigTauContainer_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigTauContainers );
  m_TauCnv.        setPStorage(&storage->m_Tau);
  m_P4PtEtaPhiMCnv.   setPStorage(&storage->m_P4PtEtaPhiM);
}


