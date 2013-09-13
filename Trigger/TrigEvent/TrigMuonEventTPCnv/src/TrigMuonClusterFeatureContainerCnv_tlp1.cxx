/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureContainerCnv_tlp1.h"


//constructor
TrigMuonClusterFeatureContainerCnv_tlp1::TrigMuonClusterFeatureContainerCnv_tlp1(){

  // add the "main" base class converter (ie. TrigMuonClusterFeatureContainerCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_trigMuonClusterFeatureCnv );


}

void TrigMuonClusterFeatureContainerCnv_tlp1::setPStorage( TrigMuonClusterFeatureContainer_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigMuonClusterFeatureContainerVec );

   //for all other converters defined in the base class
   m_trigMuonClusterFeatureCnv.setPStorage( &storage->m_trigMuonClusterFeatureVec );

}
