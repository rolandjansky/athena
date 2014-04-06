/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticleTPCnv/TrigL2BphysContainerCnv_tlp1.h"
 
 
TrigL2BphysContainerCnv_tlp1::TrigL2BphysContainerCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigL2BphysContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverterForReading( &m_L2BphysCnv); 
  addTPConverter( &m_VertexCnv);    
  addTPConverter( &m_trigInDetTrackCnv);    
  addTPConverter( &m_trigInDetTrackFitParCnv);    
  addTPConverter( &m_L2BphysCnv_p2); 
  addTPConverter( &m_trigInDetTrackCollectionCnv);    
}

void TrigL2BphysContainerCnv_tlp1::setPStorage( TrigL2BphysContainer_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigL2BphysContainers );
  m_L2BphysCnv.               setPStorage(&storage->m_L2Bphys);
  m_VertexCnv.                setPStorage(&storage->m_trigVertex);
  m_trigInDetTrackCnv.        setPStorage(&storage->m_trigInDetTrack);
  m_trigInDetTrackFitParCnv.  setPStorage(&storage->m_trigInDetTrackFitPar);
  m_L2BphysCnv_p2.            setPStorage(&storage->m_L2Bphys_p2);
  m_trigInDetTrackCollectionCnv.  setPStorage(&storage->m_trigInDetTrackCollection);
}

