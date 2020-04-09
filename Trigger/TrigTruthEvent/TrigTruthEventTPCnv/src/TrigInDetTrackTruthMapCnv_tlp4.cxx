/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMapCnv_tlp4.h"


//constructor
TrigInDetTrackTruthMapCnv_tlp4::TrigInDetTrackTruthMapCnv_tlp4(){

  // add the "main" base class converter (ie. TrigInDetTrackTruthMapCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters

  addTPConverter( &m_trigInDetTrackTruthCnv_p1 );
  addTPConverter( &m_hepMcParticleLinkCnv_p3 );
  addTPConverter( &m_trigIDHitStatsCnv_p1 );

}

void TrigInDetTrackTruthMapCnv_tlp4::setPStorage( TrigInDetTrackTruthMap_tlp4* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigInDetTrackTruthMap_p1 );

   //for all other converters defined in the base class

   m_trigInDetTrackTruthCnv_p1.      setPStorage( &storage->m_trigInDetTrackTruth_p1      );
   m_hepMcParticleLinkCnv_p3.        setPStorage( &storage->m_hepMcParticleLink_p3        );
   m_trigIDHitStatsCnv_p1.           setPStorage( &storage->m_trigIDHitStats_p1           );


}
