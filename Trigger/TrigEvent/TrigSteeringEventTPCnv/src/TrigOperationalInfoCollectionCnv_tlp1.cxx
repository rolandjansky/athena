/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollectionCnv_tlp1.h"
 
TrigOperationalInfoCollectionCnv_tlp1::TrigOperationalInfoCollectionCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigOperationalInfoCollectionCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_TrigOperationalInfoCnv); 
}

void TrigOperationalInfoCollectionCnv_tlp1::setPStorage( TrigOperationalInfoCollection_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigOperationalInfoCollection );
  
  m_TrigOperationalInfoCnv.setPStorage(&storage->m_TrigOperationalInfo);

}
