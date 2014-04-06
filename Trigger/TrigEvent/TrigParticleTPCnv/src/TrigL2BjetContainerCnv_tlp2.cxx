/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticleTPCnv/TrigL2BjetContainerCnv_tlp2.h"
 
 
TrigL2BjetContainerCnv_tlp2::TrigL2BjetContainerCnv_tlp2() {

  // add the "main" base class converter (ie. TrigL2BjetContainerCnv)
  addMainTPConverter();              

  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter(&m_L2BjetCnv); 
  addTPConverter(&m_p4PtEtaPhiMCnv);
}
 
void TrigL2BjetContainerCnv_tlp2::setPStorage( TrigL2BjetContainer_tlp2 *storage ) {

  //for the base class converter
  setMainCnvPStorage(&storage->m_TrigL2BjetContainers);

  //for all other converters defined in the base class
  m_L2BjetCnv.      setPStorage(&storage->m_L2Bjet);
  m_p4PtEtaPhiMCnv. setPStorage(&storage->m_p4PtEtaPhiM);
}


