/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/MuonFeatureDetailsContainerCnv_tlp1.h"


//constructor
MuonFeatureDetailsContainerCnv_tlp1::MuonFeatureDetailsContainerCnv_tlp1(){

  // add the "main" base class converter (ie. MuonFeatureContainerCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_muonFeatureDetailsCnv );


}

void MuonFeatureDetailsContainerCnv_tlp1::setPStorage( MuonFeatureDetailsContainer_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_muonFeatureDetailsContainerVec );

   //for all other converters defined in the base class
   m_muonFeatureDetailsCnv.setPStorage( &storage->m_muonFeatureDetailsVec );

}
