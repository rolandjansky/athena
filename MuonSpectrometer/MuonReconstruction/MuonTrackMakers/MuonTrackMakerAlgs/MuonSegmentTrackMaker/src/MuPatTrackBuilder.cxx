/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentTrackMaker/MuPatTrackBuilder.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"

#include "MuonRecToolInterfaces/IMuonTrackFinder.h"
#include "StoreGate/DataHandle.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "MuonSegment/MuonSegment.h"

#include "TrkTrack/TrackStateOnSurface.h"
#include "Particle/TrackParticleContainer.h"
//#include "MboyEDMToolInterfaces/IMuonboyToParticleTool.h"
//#include "MboyAthToolInterfaces/IMuonBackTracker.h"

#include <vector>

using namespace Muon;

MuPatTrackBuilder::MuPatTrackBuilder(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name,pSvcLocator),
     m_segmentKey("MooreSegments"),
     m_spectroTrackKey("MuonSpectrometerTracks"),
     m_spectroPartiKey("MuonSpectrometerParticles"),
     // m_extrapTrackKey(""),
     m_extrapPartiKey("ExtrapolatedMuonSpectrometerParticles"),
     m_trackMaker("Muon::MuonTrackFinder/MuonTrackSteering"),
//   , m_convTool("")
//   , p_IMuonBackTracker("")
     m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool")
{
  // MoMu Key segments (per chamner)
  declareProperty("TrackSteering",m_trackMaker);
  declareProperty("MuonSegmentCollection", m_segmentKey);
  declareProperty("SpectrometerTrackOutputLocation", m_spectroTrackKey);
  declareProperty("SpectrometerParticleOutputLocation", m_spectroPartiKey);
  // declareProperty("ExtrapolatedTrackOutputLocation",    m_extrapTrackKey);
  // set to "ExtrapolatedMuonSpectrometerTracks" for muonboy-like convention);
  declareProperty("ExtrapolatedParticleOutputLocation", m_extrapPartiKey);
//   declareProperty("TrackToParticleTool",   m_convTool);
//  declareProperty("MuonBackTracker",   p_IMuonBackTracker);
}

MuPatTrackBuilder::~MuPatTrackBuilder()
{;}

StatusCode MuPatTrackBuilder::initialize()
{
  if (m_trackMaker.retrieve().isFailure()){
    msg(MSG::FATAL) <<"Could not get " << m_trackMaker <<endmsg; 
    return StatusCode::FAILURE;
  }
  if (m_helper.retrieve().isFailure()){
    msg(MSG::FATAL) <<"Could not get " << m_helper <<endmsg; 
    return StatusCode::FAILURE;
  }
  if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Retrieved " << m_trackMaker << endmsg;

/*  if (!m_convTool.empty()) {
    if (m_convTool.retrieve().isFailure()){
      ATH_MSG_FATAL ("Could not get track converter " << m_convTool << ". STACO will have problems.");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ("Retrieved " << m_convTool);
    }
  }

  if (!p_IMuonBackTracker.empty()) {
    if (p_IMuonBackTracker.retrieve().isFailure()){
      ATH_MSG_FATAL ("Could not get track converter " << p_IMuonBackTracker << ". STACO will have problems.");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ("Retrieved " << p_IMuonBackTracker);
    }
  }*/
  
 ATH_CHECK( m_segmentKey.initialize() );
 ATH_CHECK( m_spectroTrackKey.initialize() );
 ATH_CHECK( m_spectroPartiKey.initialize() );
 // ATH_CHECK( m_extrapTrackKey.initialize() );
 ATH_CHECK( m_extrapPartiKey.initialize() );

  return StatusCode::SUCCESS; 
}

StatusCode MuPatTrackBuilder::execute()
{
  typedef std::vector<const Muon::MuonSegment*> MuonSegmentCollection;

  SG::ReadHandle<Trk::SegmentCollection> segmentColl (m_segmentKey);
  if (!segmentColl.isValid() ) {
    msg(MSG::WARNING) << "Could not find MuonSegmentCollection at " << segmentColl.name() <<endmsg;
    return StatusCode::RECOVERABLE;
  }
    
  if( !segmentColl.cptr() ) {
    msg(MSG::WARNING) << "Obtained zero pointer for MuonSegmentCollection at " << segmentColl.name() <<endmsg;
    return StatusCode::RECOVERABLE;
  }
      
  if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Retrieved MuonSegmentCollection "  << segmentColl->size() << endmsg;

  MuonSegmentCollection msc;
  msc.reserve(segmentColl->size());
  for (unsigned int i=0;i<segmentColl->size();++i){
    if (!segmentColl->at(i)) continue;
    const Muon::MuonSegment * ms = dynamic_cast<const Muon::MuonSegment*>(segmentColl->at(i));
    if (ms) msc.push_back( ms );
  }

  if (msc.size() != segmentColl->size()){
    msg(MSG::WARNING) << "Input segment collection (size " << segmentColl->size() << ") and translated MuonSegment collection (size "
                      << msc.size() << ") are not the same size." << endmsg;
  }

  TrackCollection * newtracks = m_trackMaker->find(msc);
  if (!newtracks) newtracks = new TrackCollection();

  SG::WriteHandle<TrackCollection> spectroTracks(m_spectroTrackKey); 	  
  if (spectroTracks.record(std::unique_ptr<TrackCollection>(newtracks)).isFailure()){    
      ATH_MSG_WARNING( "New Track Container " << spectroTracks.name() << " could not be recorded in StoreGate !");
      return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG ("TrackCollection '" << m_spectroTrackKey.key() << "' recorded in storegate, ntracks: " << newtracks->size());

  return StatusCode::SUCCESS;
} // execute

StatusCode MuPatTrackBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

