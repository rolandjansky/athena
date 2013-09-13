/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_tlp2.h"


//constructor
MuonFeatureContainerCnv_tlp2::MuonFeatureContainerCnv_tlp2(){

  // add the "main" base class converter (ie. MuonFeatureContainerCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_muonFeatureCnv );


}

void MuonFeatureContainerCnv_tlp2::setPStorage( MuonFeatureContainer_tlp2* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_muonFeatureContainerVec );

   //for all other converters defined in the base class
   m_muonFeatureCnv.setPStorage( &storage->m_muonFeatureVec );

}
