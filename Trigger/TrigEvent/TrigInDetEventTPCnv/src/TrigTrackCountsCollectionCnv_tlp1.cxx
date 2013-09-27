/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigTrackCountsCollectionCnv_tlp1.h"


//constructor
TrigTrackCountsCollectionCnv_tlp1::TrigTrackCountsCollectionCnv_tlp1(){

  // add the "main" base class converter (ie. TrigTrackCountsCollectionCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverterForReading(&m_trigTrackCountsCnv_p1);
  addTPConverter(&m_trigTrackCountsCnv_p2);
}

void TrigTrackCountsCollectionCnv_tlp1::setPStorage( TrigTrackCountsCollection_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigTrackCountsCollectionVec );

   //for all other converters defined in the base class
   m_trigTrackCountsCnv_p1.setPStorage( &storage->m_trigTrackCountsVec );
   m_trigTrackCountsCnv_p2.setPStorage( &storage->m_trigTrackCountsVec_p2 );

}

