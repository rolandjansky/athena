/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp1.h"


//constructor
TrigInDetTrackCollectionCnv_tlp1::TrigInDetTrackCollectionCnv_tlp1(){


  // add the "main" base class converter (ie. TrigInDetTrackCollectionCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverterForReading( &m_trigInDetTrackCnv );
  addTPConverterForReading( &m_trigInDetTrackFitParCnv );
  addTPConverter( &m_trigInDetTrackFitParCnv_p2 );
  addTPConverter( &m_trigInDetTrackCnv_p2 );

}

void TrigInDetTrackCollectionCnv_tlp1::setPStorage( TrigInDetTrackCollection_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigInDetTrackCollections );

   //for all other converters defined in the base class
   m_trigInDetTrackCnv.              setPStorage( &storage->m_trigInDetTracks );
   m_trigInDetTrackFitParCnv.        setPStorage( &storage->m_trigInDetTrackFitPars );
   m_trigInDetTrackFitParCnv_p2.     setPStorage( &storage->m_trigInDetTrackFitPars_p2 );
   m_trigInDetTrackCnv_p2.           setPStorage( &storage->m_trigInDetTracks_p2 );

}

