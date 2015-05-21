/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonTruthAlgs includes
#include "MuonTruthSummaryAlg.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonSegment/MuonSegment.h"
#include "xAODTracking/TrackParticleContainer.h"

MuonTruthSummaryAlg::MuonTruthSummaryAlg( const std::string& name, ISvcLocator* pSvcLocator ) 
  : 
  AthAlgorithm( name, pSvcLocator ),
  m_truthSummaryTool("MuonTruthSummaryTool"){
  declareProperty("SegmentContainerName"      , m_segmentContainerName = "MuonSegments");
  declareProperty("MSTracksContainerName"     , m_msTracksContainerName = "MuonSpectrometerTracks");
  declareProperty("MSTrackletsContainerName"     , m_msTrackletContainerName = "MSonlyTracklets");
  declareProperty("MuonTruthSummaryTool"      ,	m_truthSummaryTool );
}


MuonTruthSummaryAlg::~MuonTruthSummaryAlg() {}


StatusCode MuonTruthSummaryAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  if( m_truthSummaryTool.retrieve().isFailure()){
    ATH_MSG_ERROR("Failed to initialize " << m_truthSummaryTool );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonTruthSummaryAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode MuonTruthSummaryAlg::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const Trk::SegmentCollection* segments = 0;
  if (evtStore()->retrieve(segments, m_segmentContainerName).isSuccess()){
    for (auto seg : *segments) {
      const Trk::Segment* tseg = seg;
      const Muon::MuonSegment* muonSeg = dynamic_cast<const Muon::MuonSegment*>(tseg);
      if(muonSeg) m_truthSummaryTool->add(*muonSeg,2);
      else ATH_MSG_WARNING ("Trk::Segment cannot be casted in Muon::MuonSegment.");
    }
  }
  
  const TrackCollection* tracks = 0;
  if (evtStore()->retrieve(tracks, m_msTracksContainerName).isSuccess()){
    for (auto trk : *tracks) {
    	m_truthSummaryTool->add(*trk,3);
    }
  }

//  const xAOD::TrackParticleContainer* trackPs = 0;
//  if (evtStore()->retrieve(trackPs, m_msTrackletContainerName).isSuccess()){
//    for (auto trkP : *trackPs) {
//        m_truthSummaryTool->add(*trkP,4);
//    }
//  }

  //ATH_MSG_VERBOSE(m_truthSummaryTool->printSummary());
  return StatusCode::SUCCESS;
}


