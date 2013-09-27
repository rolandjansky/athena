/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigVertexCountsCollectionCnv_tlp1.h"


//constructor
TrigVertexCountsCollectionCnv_tlp1::TrigVertexCountsCollectionCnv_tlp1(){

  // add the "main" base class converter (ie. TrigVertexCountsCollectionCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter(&m_trigVertexCountsCnv_p1);

}

void TrigVertexCountsCollectionCnv_tlp1::setPStorage( TrigVertexCountsCollection_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigVertexCountsCollectionVec );

   //for all other converters defined in the base class
   m_trigVertexCountsCnv_p1.setPStorage( &storage->m_trigVertexCountsVec );
}



