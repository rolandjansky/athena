/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp3.h"
#include "IdDictParser/IdDictParser.h"


//constructor
TrigInDetTrackCollectionCnv_tlp3::TrigInDetTrackCollectionCnv_tlp3(){


  // add the "main" base class converter (ie. TrigInDetTrackCollectionCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_trigInDetTrackFitParCnv );
  addTPConverter( &m_trigInDetTrackCnv );

}

void TrigInDetTrackCollectionCnv_tlp3::setPStorage( TrigInDetTrackCollection_tlp3* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigInDetTrackCollections );

   //for all other converters defined in the base class
   m_trigInDetTrackFitParCnv.     setPStorage( &storage->m_trigInDetTrackFitPars );
   m_trigInDetTrackCnv.           setPStorage( &storage->m_trigInDetTracks );

}



TrigInDetTrackCollectionCnvARA_tlp3::TrigInDetTrackCollectionCnvARA_tlp3()
{
  IdDictParser* parser = new IdDictParser;
  IdDictMgr& idd = parser->parse ("IdDictParser/ATLAS_IDS.xml");
  int ret = m_pixId.initialize_from_dictionary (idd);
  if (ret == 0)
    m_trigInDetTrackCnv.setPixelID (&m_pixId);
}
