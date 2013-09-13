/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/TileMuFeatureContainerCnv_tlp1.h"


//constructor
TileMuFeatureContainerCnv_tlp1::TileMuFeatureContainerCnv_tlp1(){

  // add the "main" base class converter (ie. TileMuFeatureContainerCnv)

  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  
  addTPConverter( &m_TileMuFeatureCnv );
}

void TileMuFeatureContainerCnv_tlp1::setPStorage( TileMuFeatureContainer_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_TileMuFeatureContainerVec );

   //for all other converters defined in the base class
   m_TileMuFeatureCnv.setPStorage( &storage->m_TileMuFeatureVec );

}
