/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/TileTrackMuFeatureCnv_tlp1.h"


//constructor
TileTrackMuFeatureCnv_tlp1::TileTrackMuFeatureCnv_tlp1(){

  // add the "main" base class converter (ie. TileTrackMuFeatureCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
 
  addTPConverter( &m_TileMuCnv);
  addTPConverter( &m_IDTkCnv);
  addTPConverter( &m_IDTkFitParCnv ); 
}

void TileTrackMuFeatureCnv_tlp1::setPStorage( TileTrackMuFeature_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_TileTrackMu );

   //for all other converters defined in the base class
   m_TileMuCnv.	       setPStorage( &storage->m_TileMu );	
   m_IDTkCnv.          setPStorage( &storage->m_Track );
   m_IDTkFitParCnv.    setPStorage( &storage->m_TrackFitPar );
}
