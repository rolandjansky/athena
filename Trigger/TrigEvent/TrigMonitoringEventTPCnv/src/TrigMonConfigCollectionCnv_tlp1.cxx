/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEventTPCnv/TrigMonConfigCollectionCnv_tlp1.h"
 
TrigMonConfigCollectionCnv_tlp1::TrigMonConfigCollectionCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigMonConfigCollectionCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_TrigMonConfigCnv); 
}

void TrigMonConfigCollectionCnv_tlp1::setPStorage( TrigMonConfigCollection_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigMonConfigCollection );
  
  m_TrigMonConfigCnv.setPStorage(&storage->m_TrigMonConfig);

}
