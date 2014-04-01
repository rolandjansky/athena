/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEventTPCnv/TrigT2JetContainerCnv_tlp1.h"
 
 
TrigT2JetContainerCnv_tlp1::TrigT2JetContainerCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigT2JetContainerCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverterForReading( &m_T2JetCnv); 
  addTPConverter( &m_T2JetCnv_p2); 
}

void TrigT2JetContainerCnv_tlp1::setPStorage( TrigT2JetContainer_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigT2JetContainers );
  m_T2JetCnv.        setPStorage(&storage->m_T2Jet);
  m_T2JetCnv_p2.     setPStorage(&storage->m_T2Jet_p2);
}
