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
   : AthAlgorithm(name,pSvcLocator)
   , m_trackMaker("Muon::MuonTrackFinder/MuonTrackSteering")
//   , m_convTool("")
//   , p_IMuonBackTracker("")
   , m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool")
{
  // MoMu location segments (per chamner)
  declareProperty("TrackSteering",m_trackMaker);
  declareProperty("MuonSegmentCollection", m_segmentLocation                = "MooreSegments");
  declareProperty("SpectrometerTrackOutputLocation", m_spectroTrackLocation = "MuonSpectrometerTracks");
  declareProperty("SpectrometerParticleOutputLocation", m_spectroPartiLocation = "MuonSpectrometerParticles");
  declareProperty("ExtrapolatedTrackOutputLocation",    m_extrapTrackLocation   = "" ); 
  // set to "ExtrapolatedMuonSpectrometerTracks" for muonboy-like convention);
  declareProperty("ExtrapolatedParticleOutputLocation", m_extrapPartiLocation   = "ExtrapolatedMuonSpectrometerParticles");
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

  return StatusCode::SUCCESS; 
}

StatusCode MuPatTrackBuilder::execute()
{
  typedef std::vector<const Muon::MuonSegment*> MuonSegmentCollection;

  const DataHandle<Trk::SegmentCollection> segCol;
  if (evtStore()->retrieve(segCol,m_segmentLocation).isFailure() ) {
    msg(MSG::WARNING) << "Could not find MuonSegmentCollection at " << m_segmentLocation <<endmsg;
    return StatusCode::RECOVERABLE;
  }
    
  if( !segCol ) {
    msg(MSG::WARNING) << "Obtained zero pointer for MuonSegmentCollection at " << m_segmentLocation <<endmsg;
    return StatusCode::RECOVERABLE;
  }
      
  if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Retrieved MuonSegmentCollection "  << segCol->size() << endmsg;

  MuonSegmentCollection msc;
  msc.reserve(segCol->size());
  for (unsigned int i=0;i<segCol->size();++i){
    if (!segCol->at(i)) continue;
    const Muon::MuonSegment * ms = dynamic_cast<const Muon::MuonSegment*>(segCol->at(i));
    if (ms) msc.push_back( ms );
  }

  if (msc.size() != segCol->size()){
    msg(MSG::WARNING) << "Input segment collection (size " << segCol->size() << ") and translated MuonSegment collection (size "
                      << msc.size() << ") are not the same size." << endmsg;
  }

  TrackCollection * newtracks = m_trackMaker->find(msc);
  if (!newtracks) newtracks = new TrackCollection();

  if (m_extrapTrackLocation == ""){

    // Record the track collection for a track builder reporting params only in MS
    //
    if (evtStore()->record(newtracks,m_spectroTrackLocation,false).isFailure()){
      msg(MSG::WARNING) << "New Track Container " << m_spectroTrackLocation << " could not be recorded in StoreGate !" << endmsg;
      delete newtracks;
      return StatusCode::RECOVERABLE;
    }
    if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "TrackCollection '" << m_spectroTrackLocation << "' recorded in storegate, ntracks: " << newtracks->size() << endmsg;

    // how about particle making here???

  } else {

    // Record two track collections, with and without extrapolated parameters
    if (evtStore()->record(newtracks,m_spectroTrackLocation,false).isFailure()){
      msg(MSG::WARNING) << "New Track Container " << m_spectroTrackLocation << " could not be recorded in StoreGate !" << endmsg;
      delete newtracks;
      return StatusCode::RECOVERABLE;
    }
    ATH_MSG_DEBUG ("TrackCollection '" << m_spectroTrackLocation << "' recorded in storegate, ntracks: " << newtracks->size());

    // Make track particles out of it and re-record
/*    if (!m_convTool.empty()) {
      Rec::TrackParticleContainer * recpart = new Rec::TrackParticleContainer();
      if (newtracks->empty() || m_convTool->convertCollection( newtracks , recpart ).isSuccess() ){
        ATH_MSG_DEBUG ("Sucessfully converted first track collection to track particles, "<<
                       "now have container of size "<<recpart->size());
        evtStore()->record(recpart,m_spectroPartiLocation,false).ignore();
      } else {
        ATH_MSG_DEBUG ("Could not convert first track collection to track particles!");
        delete recpart;
      }
    }
 
    // BackTrack
    TrackCollection * BkTk_TrackCollection = new TrackCollection();
    for (unsigned int tk=0;tk<newtracks->size();++tk){
      if ( !newtracks->at(tk) ) continue;
      Trk::Track* pTrack      = (*newtracks)[tk] ;
      Trk::Track* BkTk_pTrack = p_IMuonBackTracker->MuonBackTrack(pTrack) ;
      if (BkTk_pTrack){
        BkTk_TrackCollection->push_back( BkTk_pTrack );
      }else{
	ATH_MSG_WARNING("Failed to create MS only track, copying track at IP ");
        BkTk_TrackCollection->push_back(  new Trk::Track(*(*newtracks)[tk]) );
      }
    }
    if (evtStore()->record(BkTk_TrackCollection,m_extrapTrackLocation,false).isFailure()){
      msg(MSG::WARNING) << "New Track Container " << m_extrapTrackLocation << " could not be recorded in StoreGate !" << endmsg;
      delete BkTk_TrackCollection;
      return StatusCode::RECOVERABLE;
    }
    if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "TrackCollection '" << m_extrapTrackLocation << "' recorded in storegate, ntracks: " << BkTk_TrackCollection->size() << endmsg;

    // Make track particles out of that and re-record
    if (!m_convTool.empty()) {
      Rec::TrackParticleContainer * BkTk_TrackParticleContainer = new Rec::TrackParticleContainer();
      if (BkTk_TrackCollection->empty() || m_convTool->convertCollection( BkTk_TrackCollection , BkTk_TrackParticleContainer ).isSuccess() ){
        ATH_MSG_DEBUG ("Sucessfully converted modified track collection to track particles, "<<
                       "now have container of size "<<BkTk_TrackParticleContainer->size());
        evtStore()->record(BkTk_TrackParticleContainer,m_extrapPartiLocation,false).ignore();
      } else {
        ATH_MSG_DEBUG ("Could not convert first track collection to track particles!");
        delete BkTk_TrackParticleContainer;
      }
    }*/
  }

  return StatusCode::SUCCESS;
} // execute

StatusCode MuPatTrackBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

