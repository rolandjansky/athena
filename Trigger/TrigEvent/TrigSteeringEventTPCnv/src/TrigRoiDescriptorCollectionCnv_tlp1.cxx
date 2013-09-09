/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollectionCnv_tlp1.h"
 
TrigRoiDescriptorCollectionCnv_tlp1::TrigRoiDescriptorCollectionCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigRoiDescriptorCollectionCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_TrigRoiDescriptorCnv); 
}

void TrigRoiDescriptorCollectionCnv_tlp1::setPStorage( TrigRoiDescriptorCollection_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_TrigRoiDescriptorCollection );
  
  m_TrigRoiDescriptorCnv.setPStorage(&storage->m_TrigRoiDescriptor);

}

