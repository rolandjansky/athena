/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_tlp1.h"


//constructor
CombinedMuonFeatureContainerCnv_tlp1::CombinedMuonFeatureContainerCnv_tlp1(){

  // add the "main" base class converter (ie. CombinedMuonFeatureContainerCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverterForReading( &m_combinedMuonFeatureCnv); 
  addTPConverter( &m_combinedMuonFeatureCnv_p2 );


}

void CombinedMuonFeatureContainerCnv_tlp1::setPStorage( CombinedMuonFeatureContainer_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_combinedMuonFeatureContainerVec );

   //for all other converters defined in the base class
   m_combinedMuonFeatureCnv.setPStorage( &storage->m_combinedMuonFeatureVec );
   m_combinedMuonFeatureCnv_p2.setPStorage( &storage->m_combinedMuonFeatureVec_p2 );

}
