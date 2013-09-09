/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitoringEventTPCnv/TrigMonEventCollectionCnv_tlp1.h"
 
TrigMonEventCollectionCnv_tlp1::TrigMonEventCollectionCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigMonEventCollectionCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_TrigMonEventCnv); 
}

void TrigMonEventCollectionCnv_tlp1::setPStorage( TrigMonEventCollection_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigMonEventCollection );
  
  m_TrigMonEventCnv.setPStorage(&storage->m_TrigMonEvent);

}
