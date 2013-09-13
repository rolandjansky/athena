/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainerCnv_tlp1.h"


//constructor
TileTrackMuFeatureContainerCnv_tlp1::TileTrackMuFeatureContainerCnv_tlp1(){

  // add the "main" base class converter (ie. TileTrackMuFeatureContainerCnv)

  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters

  //addTPConverter( &m_TileTrackMuFeatureCnv );
  addTPConverterForReading( &m_TileTrackMuFeatureCnv );
  addTPConverter( &m_TileTrackMuFeatureCnv_p2);

}

void TileTrackMuFeatureContainerCnv_tlp1::setPStorage( TileTrackMuFeatureContainer_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_TileTrackMuFeatureContainerVec );

   //for all other converters defined in the base class
   m_TileTrackMuFeatureCnv.setPStorage( &storage->m_TileTrackMuFeatureVec );
   m_TileTrackMuFeatureCnv_p2.setPStorage( &storage->m_TileTrackMuFeatureVec_p2);
}

