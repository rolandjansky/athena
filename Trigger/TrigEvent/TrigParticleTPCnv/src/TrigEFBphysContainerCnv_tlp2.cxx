/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #include "TrigParticleTPCnv/TrigEFBphysContainerCnv_tlp2.h"
 
 
 TrigEFBphysContainerCnv_tlp2::TrigEFBphysContainerCnv_tlp2()
 {
   // add the "main" base class converter (ie. TrigEFBphysContainerCnv)
  addMainTPConverter();              

  // Add all converters defined in this top level converter:
 // never change the order of adding converters!                             
  addTPConverter( &m_EFBphysCnv); 
 }
 
 void TrigEFBphysContainerCnv_tlp2::setPStorage( TrigEFBphysContainer_tlp2 *storage )
 {
  setMainCnvPStorage( &storage->m_TrigEFBphysContainers );
  m_EFBphysCnv.        setPStorage(&storage->m_EFBphys);
 }

