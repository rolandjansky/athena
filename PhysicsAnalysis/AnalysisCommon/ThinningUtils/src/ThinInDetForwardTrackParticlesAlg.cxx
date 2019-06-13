///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ThinInDetForwardTrackParticlesAlg.cxx
// Author: James Catmore <James.Catmore@cern.ch>
// Uses thinning service to remove unwanted all InDetForwardTrackParticles
// that are not associated with Muons.
// Unlike some other algs in this package, no tool is used to select the
// objects for thinning - everything is done in this one class.
// Expression evaluation is also not used.
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "ThinInDetForwardTrackParticlesAlg.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"

// STL includes
#include <algorithm> 

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IJobOptionsSvc.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ThinInDetForwardTrackParticlesAlg::ThinInDetForwardTrackParticlesAlg( const std::string& name,
                                             ISvcLocator* pSvcLocator ) :
::AthAlgorithm( name, pSvcLocator ),
m_thinningSvc( "ThinningSvc/ThinningSvc", name ),
m_doThinning(true),
m_nEventsProcessed(0),
m_nTracksProcessed(0),
m_nTracksThinned(0)
{
   
    declareProperty("ThinningSvc",          m_thinningSvc,
                    "The ThinningSvc instance for a particular output stream" );
    
    declareProperty("ThinInDetForwardTrackParticles", m_doThinning,
                    "Should the InDetForwardTrackParticles thinning be run?");
   
}

// Destructor
///////////////
ThinInDetForwardTrackParticlesAlg::~ThinInDetForwardTrackParticlesAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ThinInDetForwardTrackParticlesAlg::initialize()
{
    ATH_MSG_DEBUG ("Initializing " << name() << "...");
    
    // Print out the used configuration
    ATH_MSG_DEBUG ( " using = " << m_thinningSvc );

    // Is truth thinning required?
    if (!m_doThinning) {
        ATH_MSG_INFO("InDetForwardTrackParticles thinning not required");
    } else {
        ATH_MSG_INFO("InDetForwardTrackParticles will be thinned");
    }

    // Initialize the counters to zero
    m_nEventsProcessed = 0;
    m_nMuons = 0;
    m_nSiFwdMuons = 0;
    m_nSiFwdAssoc = 0;
    m_nTracksProcessed = 0;
    m_nTracksThinned   = 0;
    
    ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );
    
    return StatusCode::SUCCESS;
}



StatusCode ThinInDetForwardTrackParticlesAlg::finalize()
{
    ATH_MSG_DEBUG ("Finalizing " << name() << "...");
    ATH_MSG_INFO("Processed " << m_nEventsProcessed << " events containing: ");
    ATH_MSG_INFO("    " << m_nTracksProcessed << " InDetForwardTrackParticles");     
    ATH_MSG_INFO("    " << m_nMuons << " muons of which " << m_nSiFwdMuons << " were SiliconAssociatedForward muons");
    ATH_MSG_INFO("    " << m_nSiFwdAssoc << " of the SiliconAssociatedForward muons were associated with InDetForwardTrackParticles");
    ATH_MSG_INFO("Removed " << m_nTracksThinned << " InDetForwardTrackParticles");
    return StatusCode::SUCCESS;
}



StatusCode ThinInDetForwardTrackParticlesAlg::execute()
{
    // Increase the event counter
    ++m_nEventsProcessed;
    
    // Is truth thinning required?
    if (!m_doThinning) {
        return StatusCode::SUCCESS;
    } 
   
    // Retrieve InDetForwardTrackParticles and Muons containers
    const xAOD::TrackParticleContainer* tracks(0);
    const xAOD::MuonContainer* muons(0);
    if (evtStore()->contains<xAOD::TrackParticleContainer>("InDetForwardTrackParticles")) {
        CHECK( evtStore()->retrieve( tracks , "InDetForwardTrackParticles" ) );
    } else {
        ATH_MSG_FATAL("No InDetForwardTrackParticles found.");
        return StatusCode::FAILURE;
    }
    if (evtStore()->contains<xAOD::MuonContainer>("Muons")) {
        CHECK( evtStore()->retrieve( muons , "Muons" ) );
    } else {
        ATH_MSG_FATAL("No Muons found");
        return StatusCode::FAILURE;
    }

    // Set up mask and set the main counters
    std::vector<bool> trackMask;
    unsigned int nTracks = tracks->size();
    m_nTracksProcessed += nTracks;
    trackMask.assign(nTracks,false);
    m_nMuons += muons->size();

    // Loop over the muons. Identify which are SiliconAssociatedForwardMuon. 
    // Get their associated inner detector track. Find that track in the InDetForwardTrackParticles.
    // Set the mask element.
    if (muons!=nullptr) {
        for (auto muon : *muons) {
            if (muon->muonType()==xAOD::Muon::SiliconAssociatedForwardMuon) {
                ++m_nSiFwdMuons;
                const xAOD::TrackParticle* muTrk(nullptr);
                if (muon->inDetTrackParticleLink().isValid()) muTrk = *(muon->inDetTrackParticleLink());
                if (muTrk!=nullptr) {
                    auto search = std::find(tracks->begin(), tracks->end(), muTrk);
                    if (search != tracks->end()) {++m_nSiFwdAssoc; trackMask[(*search)->index()] = true;}   
                }
            }     
        }
    }

    // Increment counters
    for (unsigned int i=0; i<nTracks; ++i) {
        if (trackMask[i]==false) ++m_nTracksThinned;  
    }

    // Apply masks to thinning service
    if (m_thinningSvc->filter(*tracks, trackMask, IThinningSvc::Operator::Or).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service failed for InDetForwardTrackParticles! ");
        return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
}



