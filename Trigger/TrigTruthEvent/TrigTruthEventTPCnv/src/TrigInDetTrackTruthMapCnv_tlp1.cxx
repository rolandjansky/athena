/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_tlp1.h"


//constructor
TrigInDetTrackTruthMapCnv_tlp1::TrigInDetTrackTruthMapCnv_tlp1(){

  // add the "main" base class converter (ie. TrigInDetTrackTruthMapCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_trigInDetTrackCollectionCnv_p1 );
  addTPConverter( &m_trigInDetTrackCnv_p1 );
  addTPConverter( &m_trigInDetTrackFitParCnv_p1 );
  addTPConverter( &m_trigInDetTrackTruthCnv_p1 );
  addTPConverter( &m_hepMcParticleLinkCnv_p1 );
  addTPConverter( &m_trigIDHitStatsCnv_p1 );

}

void TrigInDetTrackTruthMapCnv_tlp1::setPStorage( TrigInDetTrackTruthMap_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigInDetTrackTruthMap_p1 );

   //for all other converters defined in the base class
   m_trigInDetTrackCollectionCnv_p1. setPStorage( &storage->m_trigInDetTrackCollection_p1 );
   m_trigInDetTrackCnv_p1.	     setPStorage( &storage->m_trigInDetTrack_p1	          );
   m_trigInDetTrackFitParCnv_p1.     setPStorage( &storage->m_trigInDetTrackFitPar_p1     );
   m_trigInDetTrackTruthCnv_p1.	     setPStorage( &storage->m_trigInDetTrackTruth_p1      );
   m_hepMcParticleLinkCnv_p1.	     setPStorage( &storage->m_hepMcParticleLink_p1	  );
   m_trigIDHitStatsCnv_p1.           setPStorage( &storage->m_trigIDHitStats_p1           ); 


}



