/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMissingEtEventTPCnv/TrigMissingETContainerCnv_tlp1.h"
  
TrigMissingETContainerCnv_tlp1::TrigMissingETContainerCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigMissingETContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverterForReading( &m_TrigMissingETCnv_p1); 
  addTPConverter( &m_TrigMissingETCnv_p2); 

}

void TrigMissingETContainerCnv_tlp1::setPStorage( TrigMissingETContainer_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigMissingETContainer_p1 );
  
  m_TrigMissingETCnv_p2.setPStorage(&storage->m_TrigMissingET_p2);

}
