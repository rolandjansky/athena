/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// muonTrkTrackThinTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Saminder Dhaliwal (s.dhaliwal@cern.ch)
// Follows closely from dimuonTaggingTool and DerviationFrameworkExamples

// STL includes
#include <algorithm>
#include <iostream>

// FrameWork includes
#include "Gaudi/Property.h"
// StoreGate
#include "StoreGate/DataHandle.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"


#include "PrimaryDPDMaker/muonTrkTrackThinTool.h"
//#include "AthenaKernel/errorcheck.h"
#include <vector>
#include <string>

// xAOD containers
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// Constructor
muonTrkTrackThinTool::muonTrkTrackThinTool(const std::string& name,ISvcLocator* pSvcLocator):
  AthAlgorithm (name, pSvcLocator),
  m_All(0),
  m_pass(0),
  m_trackAll(0),
  m_trackpass(0)
{
}
  
// Destructor
muonTrkTrackThinTool::~muonTrkTrackThinTool() {
}  

// Athena initialize and finalize
StatusCode muonTrkTrackThinTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  ATH_CHECK( m_trackCollKey.initialize (m_streamName) );
  return StatusCode::SUCCESS;
}

StatusCode muonTrkTrackThinTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("muonTrkTrackThinTool Filter number of muons PASSED "<<m_pass<<" FROM "<< m_All);
  ATH_MSG_INFO("muonTrkTrackThinTool Filter number of Trk::Track PASSED "<<m_trackpass<<" FROM "<< m_trackAll);
  //  ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
  return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode muonTrkTrackThinTool::execute()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  
  SG::ThinningHandle<TrackCollection> alltracks (m_trackCollKey, ctx);

  if (alltracks->size() == 0){
    ATH_MSG_DEBUG( "------------- Track Collection is empty, collection type: " <<  m_trackCollKey.key());
    return StatusCode::SUCCESS;
  }

  std::vector<bool> mask_t;
  mask_t.resize(alltracks->size(), false);
  m_trackAll+=  alltracks->size();

  // Retrieve the muons:
  const xAOD::MuonContainer* muons = 0;
  CHECK( evtStore()->retrieve( muons, "Muons" ) );
  if (!muons){
    ATH_MSG_ERROR( "------------- No Muon Collection to filter tracks of type Muons");
    return StatusCode::FAILURE;
  }
  m_All+=  muons->size();

  // Loop over muon:
  for( const auto* muon : *muons ) {    
    m_pass++;
    const xAOD::TrackParticle* muon_tp = 0;
    //Needed the below 'if' statements to separate trackparticle cases
    if (m_trackCollKey.key() == "MuonSpectrometerTracks")
      muon_tp = muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);

    if (m_trackCollKey.key() == "CombinedMuonTracks")
      muon_tp = muon->trackParticle(xAOD::Muon::CombinedTrackParticle);

    if (!muon_tp) {
      ATH_MSG_DEBUG( "------------- No muon track particle " );
      continue; // segtagged muons
    }

    //using element link rather than "const Trk::Track* muontr = muontp->track();"
    //avoids an additional loop over track collection
    const ElementLink< TrackCollection >& muontr_link = muon_tp->trackLink();
    if (!muontr_link.isValid()){
      ATH_MSG_DEBUG("------------- Elementlink from trackparticle to trk::track not valid");
      continue; //This shouldn't happen
    }

    int trIndex = muontr_link.index();
    if (trIndex < int(mask_t.size())){
      mask_t[trIndex] = true;
      m_trackpass++;
    }
  }//close muon loop


  alltracks.keep (mask_t);
  
  return StatusCode::SUCCESS;
}
