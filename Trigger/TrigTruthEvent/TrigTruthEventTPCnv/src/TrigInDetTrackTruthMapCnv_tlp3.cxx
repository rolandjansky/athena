/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_tlp3.h"


//constructor
TrigInDetTrackTruthMapCnv_tlp3::TrigInDetTrackTruthMapCnv_tlp3(){

  // add the "main" base class converter (ie. TrigInDetTrackTruthMapCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters

  addTPConverter( &m_trigInDetTrackTruthCnv_p1 );
  addTPConverter( &m_hepMcParticleLinkCnv_p2 );
  addTPConverter( &m_trigIDHitStatsCnv_p1 );

}

void TrigInDetTrackTruthMapCnv_tlp3::setPStorage( TrigInDetTrackTruthMap_tlp3* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigInDetTrackTruthMap_p1 );

   //for all other converters defined in the base class

   m_trigInDetTrackTruthCnv_p1.      setPStorage( &storage->m_trigInDetTrackTruth_p1      );
   m_hepMcParticleLinkCnv_p2.        setPStorage( &storage->m_hepMcParticleLink_p2        );
   m_trigIDHitStatsCnv_p1.           setPStorage( &storage->m_trigIDHitStats_p1           );


}
