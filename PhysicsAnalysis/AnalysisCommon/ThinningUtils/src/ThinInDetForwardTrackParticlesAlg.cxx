///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// STL includes
#include <algorithm>

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ThinningHandle.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
ThinInDetForwardTrackParticlesAlg::ThinInDetForwardTrackParticlesAlg(
  const std::string& name,
  ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode
ThinInDetForwardTrackParticlesAlg::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG(" using = " << m_streamName);

  // Is truth thinning required?
  if (!m_doThinning) {
    ATH_MSG_INFO("InDetForwardTrackParticles thinning not required");
  } else {
    ATH_MSG_INFO("InDetForwardTrackParticles will be thinned");
  }

  if (m_doThinning && m_streamName.empty()) {
    ATH_MSG_ERROR("StreamName property was not initialized.");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_tracksKey.initialize(m_streamName, m_doThinning));
  ATH_CHECK(m_muonsKey.initialize(m_doThinning));
  ATH_MSG_DEBUG("==> done with initialize " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode
ThinInDetForwardTrackParticlesAlg::finalize()
{
  ATH_MSG_DEBUG("Finalizing " << name() << "...");
  ATH_MSG_INFO("Processed " << m_nEventsProcessed << " events containing: ");
  ATH_MSG_INFO("    " << m_nTracksProcessed << " InDetForwardTrackParticles");
  ATH_MSG_INFO("    " << m_nMuons << " muons of which " << m_nSiFwdMuons
                      << " were SiliconAssociatedForward muons");
  ATH_MSG_INFO("    " << m_nSiFwdAssoc
                      << " of the SiliconAssociatedForward muons were "
                         "associated with InDetForwardTrackParticles");
  ATH_MSG_INFO("Removed " << m_nTracksThinned << " InDetForwardTrackParticles");
  return StatusCode::SUCCESS;
}

StatusCode
ThinInDetForwardTrackParticlesAlg::execute(const EventContext& ctx) const
{
  // Increase the event counter
  m_nEventsProcessed.fetch_add(1, std::memory_order_relaxed);

  // Is truth thinning required?
  if (!m_doThinning) {
    return StatusCode::SUCCESS;
  }

  // Retrieve InDetForwardTrackParticles and Muons containers
  SG::ThinningHandle<xAOD::TrackParticleContainer> tracks(m_tracksKey, ctx);
  SG::ReadHandle<xAOD::MuonContainer> muons(m_muonsKey, ctx);

  // Set up mask and set the main counters
  std::vector<bool> trackMask;
  unsigned int nTracks = tracks->size();
  m_nTracksProcessed.fetch_add(nTracks, std::memory_order_relaxed);
  trackMask.assign(nTracks, false);
  m_nMuons.fetch_add(muons->size(), std::memory_order_relaxed);

  // Loop over the muons. Identify which are SiliconAssociatedForwardMuon.
  // Get their associated inner detector track. Find that track in the
  // InDetForwardTrackParticles. Set the mask element.
  unsigned int nSiFwdAssoc = 0;
  unsigned int nSiFwdMuons = 0;
  for (const auto* muon : *muons) {
    if (muon->muonType() == xAOD::Muon::SiliconAssociatedForwardMuon) {
      ++nSiFwdMuons;
      const xAOD::TrackParticle* muTrk(nullptr);
      if (muon->inDetTrackParticleLink().isValid())
        muTrk = *(muon->inDetTrackParticleLink());
      if (muTrk != nullptr) {
        auto search = std::find(tracks->begin(), tracks->end(), muTrk);
        if (search != tracks->end()) {
          ++nSiFwdAssoc;
          trackMask[(*search)->index()] = true;
        }
      }
    }
  }
  m_nSiFwdAssoc.fetch_add(nSiFwdAssoc, std::memory_order_relaxed);
  m_nSiFwdMuons.fetch_add(nSiFwdMuons, std::memory_order_relaxed);

  // Increment counters
  unsigned int nTracksThinned = 0;
  for (unsigned int i = 0; i < nTracks; ++i) {
    if (!trackMask[i])
      ++nTracksThinned;
  }
  m_nTracksThinned.fetch_add(nTracksThinned, std::memory_order_relaxed);
  // Apply masks to thinning service
  tracks.keep(trackMask);

  return StatusCode::SUCCESS;
}

