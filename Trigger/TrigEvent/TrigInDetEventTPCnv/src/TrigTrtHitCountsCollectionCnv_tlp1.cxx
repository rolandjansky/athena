/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollectionCnv_tlp1.h"


//constructor
TrigTrtHitCountsCollectionCnv_tlp1::TrigTrtHitCountsCollectionCnv_tlp1(){

  // add the "main" base class converter (ie. TrigTrtHitCountsCollectionCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_trigTrtHitCountsCnv );


}

void TrigTrtHitCountsCollectionCnv_tlp1::setPStorage( TrigTrtHitCountsCollection_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigTrtHitCountsCollectionVec );

   //for all other converters defined in the base class
   m_trigTrtHitCountsCnv.setPStorage( &storage->m_trigTrtHitCountsVec );

}

