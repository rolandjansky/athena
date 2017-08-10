/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentTrackMaker/MuonCombiTrackBuilder.h"

#include "MuonRecToolInterfaces/IMuonCombiTrackMaker.h"
#include "StoreGate/DataHandle.h"
#include <memory>

using namespace Muon;

MuonCombiTrackBuilder::MuonCombiTrackBuilder(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_segmentCombiLocation("MooreSegmentCombinations"),
  m_trackLocation("MooreTracks"),
  m_trackMaker("Muon::MuonCombiTrackMaker/MuonCombiTrackMaker")
{
  // MoMu location segments (per chamner)
  declareProperty("MuonSegmentCombinationInputLocation",m_segmentCombiLocation );
  declareProperty("TrackOutputLocation",m_trackLocation );
  declareProperty("TrackSteering",m_trackMaker);
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

  ATH_CHECK( m_segmentCombiLocation.initialize() );
  ATH_CHECK( m_trackLocation.initialize() );

  return StatusCode::SUCCESS; 
}

StatusCode MuonCombiTrackBuilder::execute()
{
  
  if (!m_segmentCombiLocation.isValid() ) {
    msg(MSG::WARNING) << " Could not find MuonSegmentCombinationCollection at " << m_segmentCombiLocation.name() <<endmsg;
    return StatusCode::RECOVERABLE;
  }
    
  if( !m_segmentCombiLocation.cptr() ) {
    msg(MSG::WARNING) << " Obtained zero pointer for MuonSegmentCombinationCollection at " << m_segmentCombiLocation <<endmsg;
    return StatusCode::RECOVERABLE;
  }
      
  if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << " Retrieved MuonSegmentCombinationCollection "  << m_segmentCombiLocation->size() << endmsg;
 
  TrackCollection* newtracks = m_trackMaker->find(*m_segmentCombiLocation);

  // create dummy track collection
  if( !newtracks ) newtracks = new TrackCollection();

  if (m_trackLocation.record(std::unique_ptr<TrackCollection>(newtracks)).isFailure()){    
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



