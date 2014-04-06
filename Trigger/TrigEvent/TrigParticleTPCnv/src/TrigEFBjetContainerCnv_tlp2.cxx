/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticleTPCnv/TrigEFBjetContainerCnv_tlp2.h"


TrigEFBjetContainerCnv_tlp2::TrigEFBjetContainerCnv_tlp2() {

  // add the "main" base class converter (ie. TrigEFBjetContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter(&m_EFBjetCnv);
  addTPConverter(&m_p4PtEtaPhiMCnv);
}

void TrigEFBjetContainerCnv_tlp2::setPStorage( TrigEFBjetContainer_tlp2 *storage ) {

  //for the base class converter
  setMainCnvPStorage(&storage->m_TrigEFBjetContainers);

  //for all other converters defined in the base class
  m_EFBjetCnv.      setPStorage(&storage->m_EFBjet);
  m_p4PtEtaPhiMCnv. setPStorage(&storage->m_p4PtEtaPhiM);
}

