/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_tlp1.h"


//constructor
MuonFeatureContainerCnv_tlp1::MuonFeatureContainerCnv_tlp1(){

  // add the "main" base class converter (ie. MuonFeatureContainerCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverterForReading( &m_muonFeatureCnv );
  addTPConverter( &m_muonFeatureCnv_p2 );

}

void MuonFeatureContainerCnv_tlp1::setPStorage( MuonFeatureContainer_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_muonFeatureContainerVec );

   //for all other converters defined in the base class
   m_muonFeatureCnv.setPStorage( &storage->m_muonFeatureVec );
   m_muonFeatureCnv_p2.setPStorage( &storage->m_muonFeatureVec_p2 );

}
