/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #include "TrigParticleTPCnv/TrigL2BjetContainerCnv_tlp1.h"
 
 
 TrigL2BjetContainerCnv_tlp1::TrigL2BjetContainerCnv_tlp1()
 {
   // add the "main" base class converter (ie. TrigL2BjetContainerCnv)
  addMainTPConverter();              

  // Add all converters defined in this top level converter:
 // never change the order of adding converters!                             
  addTPConverter( &m_L2BjetCnv); 
 }
 
 void TrigL2BjetContainerCnv_tlp1::setPStorage( TrigL2BjetContainer_tlp1 *storage )
 {
  setMainCnvPStorage( &storage->m_TrigL2BjetContainers );
  m_L2BjetCnv.        setPStorage(&storage->m_L2Bjet);
 }


