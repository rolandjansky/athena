/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentTrackMaker/MuonCombiTrackBuilder.h"

#include "MuonRecToolInterfaces/IMuonCombiTrackMaker.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "StoreGate/DataHandle.h"
#include "TrkTrack/TrackCollection.h"

using namespace Muon;

MuonCombiTrackBuilder::MuonCombiTrackBuilder(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_trackMaker("Muon::MuonCombiTrackMaker/MuonCombiTrackMaker")
{
  // MoMu location segments (per chamner)
  declareProperty("TrackSteering",m_trackMaker);
  declareProperty("MuonSegmentCombinationInputLocation",m_segmentCombiLocation = "MooreSegmentCombinations");
  declareProperty("TrackOutputLocation",m_trackLocation = "MooreTracks");
}

MuonCombiTrackBuilder::~MuonCombiTrackBuilder()
{
}

StatusCode MuonCombiTrackBuilder::initialize()
{
  if (m_trackMaker.retrieve().isFailure()){
    msg(MSG::FATAL) <<"Could not get " << m_trackMaker <<endmsg; 
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << "Retrieved " << m_trackMaker << endmsg;

  return StatusCode::SUCCESS; 
}

StatusCode MuonCombiTrackBuilder::execute()
{
  
  const DataHandle<MuonSegmentCombinationCollection> combiCol;
  if (evtStore()->retrieve(combiCol,m_segmentCombiLocation).isFailure() ) {
    msg(MSG::WARNING) << " Could not find MuonSegmentCombinationCollection at " << m_segmentCombiLocation <<endmsg;
    return StatusCode::RECOVERABLE;
  }
    
  if( !combiCol ) {
    msg(MSG::WARNING) << " Obtained zero pointer for MuonSegmentCombinationCollection at " << m_segmentCombiLocation <<endmsg;
    return StatusCode::RECOVERABLE;
  }
      
  if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << " Retrieved MuonSegmentCombinationCollection "  << combiCol->size() << endmsg;
 
  TrackCollection* newtracks = m_trackMaker->find(*combiCol);
 
  // create dummy track collection
  if( !newtracks ) newtracks = new TrackCollection();

  if (evtStore()->record(newtracks,m_trackLocation,false).isFailure()){
    msg(MSG::WARNING) << "New Track Container could not be recorded in StoreGate !" << endmsg;
    delete newtracks;
    return StatusCode::RECOVERABLE;
  }
  if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Track Container '" << m_trackLocation << "' recorded in storegate, ntracks: " << newtracks->size() << endmsg;

  return StatusCode::SUCCESS;
} // execute

StatusCode MuonCombiTrackBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

