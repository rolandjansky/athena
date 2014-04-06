/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #include "TrigParticleTPCnv/TrigEFBphysContainerCnv_tlp1.h"
 
 
 TrigEFBphysContainerCnv_tlp1::TrigEFBphysContainerCnv_tlp1()
 {
   // add the "main" base class converter (ie. TrigEFBphysContainerCnv)
  addMainTPConverter();              

  // Add all converters defined in this top level converter:
 // never change the order of adding converters!                             
  addTPConverterForReading( &m_EFBphysCnv); 
  addTPConverter( &m_EFBphysCnv_p2); 
  addTPConverter( &m_TrackParticleContainerCnv); 
 }
 
 void TrigEFBphysContainerCnv_tlp1::setPStorage( TrigEFBphysContainer_tlp1 *storage )
 {
  setMainCnvPStorage( &storage->m_TrigEFBphysContainers );
  m_EFBphysCnv.        setPStorage(&storage->m_EFBphys);
  m_EFBphysCnv_p2.     setPStorage(&storage->m_EFBphys_p2);
  m_TrackParticleContainerCnv. setPStorage(&storage->m_TrackParticleContainer);
 }

