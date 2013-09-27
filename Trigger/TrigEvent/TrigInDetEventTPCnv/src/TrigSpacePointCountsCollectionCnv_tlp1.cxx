/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollectionCnv_tlp1.h"


//constructor
TrigSpacePointCountsCollectionCnv_tlp1::TrigSpacePointCountsCollectionCnv_tlp1(){

  // add the "main" base class converter (ie. TrigSpacePointCountsCollectionCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverterForReading(&m_trigSpacePointCountsCnv_p1);
  addTPConverterForReading(&m_trigSpacePointCountsCnv_p2);
  addTPConverter(&m_trigSpacePointCountsCnv_p3);

}

void TrigSpacePointCountsCollectionCnv_tlp1::setPStorage( TrigSpacePointCountsCollection_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigSpacePointCountsCollectionVec );

   //for all other converters defined in the base class
   m_trigSpacePointCountsCnv_p1.setPStorage(&storage->m_trigSpacePointCountsVec);
   m_trigSpacePointCountsCnv_p2.setPStorage(&storage->m_trigSpacePointCountsVec_p2);
   m_trigSpacePointCountsCnv_p3.setPStorage(&storage->m_trigSpacePointCountsVec_p3);
}

