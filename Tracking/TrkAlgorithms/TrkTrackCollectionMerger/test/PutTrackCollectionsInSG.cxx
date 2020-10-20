/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PutTrackCollectionsInSG.cxx
 *
 * @brief Implementation file for the PutTrackCollectionsInSG class 
 *
 * @author Shaun Roe
 **/

#include "PutTrackCollectionsInSG.h"
#include "TrkTrack/TrackCollection.h"
#include <memory>


PutTrackCollectionsInSG::PutTrackCollectionsInSG(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator)
{
  //nop
}

//Initialize
StatusCode 
PutTrackCollectionsInSG::initialize() {
  ATH_MSG_INFO("Calling initialize");
  return StatusCode::SUCCESS;
}

//Execute
StatusCode 
PutTrackCollectionsInSG::execute()  {
  auto collection1 = std::make_unique<TrackCollection>(SG::VIEW_ELEMENTS);
  //note that the default c'tor should normally *not* be used and results in an invalid Track
  //however, it suffices for unit testing purposes
  Trk::Track *t1 =  new Trk::Track();
  collection1->push_back(t1);
  auto collection2 = std::make_unique<TrackCollection>(SG::VIEW_ELEMENTS);
  Trk::Track *t2 =  new Trk::Track();
  collection2->push_back(t2);
  //
  std::string collectionKey1("StoreGateSvc+TrackCollectionKey1");
  SG::WriteHandle<TrackCollection> sgTrackCollectionWriter1(collectionKey1);
  ATH_MSG_INFO("Record track collection1 in SG");
  ATH_CHECK(sgTrackCollectionWriter1.record(std::move(collection1)));	
  //
  std::string collectionKey2("StoreGateSvc+TrackCollectionKey2");
  SG::WriteHandle<TrackCollection> sgTrackCollectionWriter2(collectionKey2);
  ATH_MSG_INFO("Record track collection2 in SG");
  ATH_CHECK(sgTrackCollectionWriter2.record(std::move(collection2)));
  //
  return StatusCode::SUCCESS;
}

//Finalize
StatusCode
PutTrackCollectionsInSG::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}