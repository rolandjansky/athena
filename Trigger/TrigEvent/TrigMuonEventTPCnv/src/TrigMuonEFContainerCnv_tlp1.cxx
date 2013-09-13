/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/TrigMuonEFContainerCnv_tlp1.h"
 
 
TrigMuonEFContainerCnv_tlp1::TrigMuonEFContainerCnv_tlp1()
{
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!
  addMainTPConverter();              
  
  addTPConverter( &m_MuonEFCnv); 
  addTPConverter( &m_P4IPtCotThPhiMCnv);    
  
}

void TrigMuonEFContainerCnv_tlp1::setPStorage( TrigMuonEFContainer_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigMuonEFContainers );
  
  m_MuonEFCnv.        setPStorage(&storage->m_MuonEF);
  m_P4IPtCotThPhiMCnv.setPStorage(&storage->m_P4IPtCotThPhiM);
}
