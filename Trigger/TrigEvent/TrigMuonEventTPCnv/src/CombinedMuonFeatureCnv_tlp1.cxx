/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/CombinedMuonFeatureCnv_tlp1.h"


//constructor
CombinedMuonFeatureCnv_tlp1::CombinedMuonFeatureCnv_tlp1(){

  // add the "main" base class converter (ie. CombinedMuonFeatureCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_mfCnv );
  addTPConverter( &m_IDTkCnv );
  addTPConverter( &m_IDTkFitParCnv );

}

void CombinedMuonFeatureCnv_tlp1::setPStorage( CombinedMuonFeature_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_combMu );

   //for all other converters defined in the base class
   m_mfCnv.            setPStorage( &storage->m_muFastTrack );
   m_IDTkCnv.          setPStorage( &storage->m_inDetTracks );
   m_IDTkFitParCnv.    setPStorage( &storage->m_inDetTrackFitPar );

}

