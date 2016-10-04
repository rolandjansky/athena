/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/Property.h"
#include "AthenaKernel/IThinningSvc.h"
// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"


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
  m_trackpass(0),
  m_EventCounter(0),
  m_thinningSvc(nullptr)
{
  declareProperty("TrackCollectionKey", m_trackCollKey="MuonSpectrometerTracks");
}
  
// Destructor
muonTrkTrackThinTool::~muonTrkTrackThinTool() {
}  

// Athena initialize and finalize
StatusCode muonTrkTrackThinTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  StatusCode  sc;
  sc = service("ThinningSvc", m_thinningSvc);
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("ThinningSvc not found");
    return sc;
  }
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
  
  const TrackCollection* alltracks = 0;
  CHECK( evtStore()->retrieve( alltracks, m_trackCollKey ) );
  if (!alltracks){
    ATH_MSG_DEBUG( "------------- No Track Collection to filter of type: " <<  m_trackCollKey);
    return StatusCode::SUCCESS;
  }

  if (alltracks->size() == 0){
    ATH_MSG_DEBUG( "------------- Track Collection is empty, collection type: " <<  m_trackCollKey);
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
    if (m_trackCollKey == "MuonSpectrometerTracks")
      muon_tp = muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);

    if (m_trackCollKey == "CombinedMuonTracks")
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

      
  if (m_thinningSvc->filter(*alltracks, mask_t).isFailure()) {
    ATH_MSG_ERROR("------------- Application of thinning service failed! ");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}
