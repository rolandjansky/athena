/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/IsoMuonFeatureContainerCnv_tlp1.h"


//constructor
IsoMuonFeatureContainerCnv_tlp1::IsoMuonFeatureContainerCnv_tlp1(){

  // add the "main" base class converter (ie. IsoMuonFeatureContainerCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_isoMuonFeatureCnv );


}

void IsoMuonFeatureContainerCnv_tlp1::setPStorage( IsoMuonFeatureContainer_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_isoMuonFeatureContainerVec );

   //for all other converters defined in the base class
   m_isoMuonFeatureCnv.setPStorage( &storage->m_isoMuonFeatureVec );

}

