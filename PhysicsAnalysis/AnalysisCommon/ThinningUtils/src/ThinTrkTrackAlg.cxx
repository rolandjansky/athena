///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ThinTrkTrackAlg.cxx
// Author: Christos
// Lets see if we can write Trk::Tracks in xAOD
///////////////////////////////////////////////////////////////////

#include "ThinTrkTrackAlg.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "StoreGate/ThinningHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"

// STL includes
#include <algorithm>

StatusCode
ThinTrkTrackAlg::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG(" using = " << m_streamName);

  // Is truth thinning required?
  if (m_doElectrons) {
    ATH_MSG_INFO("Will thin " << m_electronsKey
                              << " Trk::Tracks with key (electrons) "
                              << m_GSFTracksKey);
  }
  if (m_doPhotons) {
    ATH_MSG_INFO("Will thin " << m_photonsKey
                              << " Trk::Tracks with key (photons) "
                              << m_GSFTracksKey);
  }
  if (m_doMuons) {
    ATH_MSG_INFO("Will thin " << m_muonsKey << " Trk::Tracks with key "
                              << m_CombinedMuonsTracksKey);
  }

  if (m_streamName.empty()) {
    ATH_MSG_ERROR("StreamName property was not initialized");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_GSFTracksKey.initialize(m_streamName));
  ATH_CHECK(m_CombinedMuonsTracksKey.initialize(m_streamName, m_doMuons));
  ATH_CHECK(m_electronsKey.initialize(m_doElectrons));
  ATH_CHECK(m_photonsKey.initialize(m_doPhotons));
  ATH_CHECK(m_muonsKey.initialize(m_doMuons));

  ATH_MSG_DEBUG("==> done with initialize " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode
ThinTrkTrackAlg::execute(const EventContext& ctx) const
{

  if (m_doElectrons || m_doPhotons) {
    CHECK(doEGamma(ctx));
  }
  if (m_doMuons) {
    CHECK(doMuons(ctx));
  }
  return StatusCode::SUCCESS;
}

StatusCode
ThinTrkTrackAlg::doEGamma(const EventContext& ctx) const
{

  // Prepare for the Thinning of Trk::Tracks
  std::vector<bool> keptTracks;
  SG::ThinningHandle<TrackCollection> trackPC(m_GSFTracksKey, ctx);
  ATH_MSG_DEBUG("Number of " << m_GSFTracksKey.key() << " " << trackPC->size());
  if (keptTracks.size() < trackPC->size()) {
    keptTracks.resize(trackPC->size(), false);
  }

  size_t kept(0);

  if (m_doElectrons) {
    // Get the electrons
    SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronsKey, ctx);

    // Loop over electrons
    for (const auto *el : *electrons) {
      if (el->pt() < m_minptElectrons) {
        continue;
      }
      size_t nel = el->nTrackParticles();
      if (m_bestonlyElectrons && nel > 1) {
        nel = 1;
      }
      for (size_t i = 0; i < nel; i++) {
        const xAOD::TrackParticle* tp = el->trackParticle(i);
        if (!tp || !tp->trackLink().isValid()) {
          continue;
        }
        size_t index = tp->trackLink().index();
        keptTracks[index] = true;
        ++kept;
      }
    }
  }

  if (m_doPhotons) {
    // Get the photons
    SG::ReadHandle<xAOD::PhotonContainer> photons(m_photonsKey, ctx);

    // Loop over photons
    for (const auto *ph : *photons) {
      if (ph->pt() < m_minptPhotons) {
        continue;
      }
      size_t nvx = ph->nVertices();
      if (m_bestonlyPhotons && nvx > 1) {
        nvx = 1;
      }
      for (size_t i = 0; i < nvx; i++) {
        const xAOD::Vertex* vx = ph->vertex(i);
        if (vx) {
          size_t ntp = vx->nTrackParticles();
          for (size_t j = 0; j < ntp; j++) {
            const xAOD::TrackParticle* tp = vx->trackParticle(j);
            if (!tp || !tp->trackLink().isValid()) {
              continue;
            }
            size_t index = tp->trackLink().index();
            keptTracks[index] = true;
            ++kept;
          }
        }
      }
    }
  }

  ATH_MSG_DEBUG("keep " << kept << " out of " << keptTracks.size());
  ATH_MSG_DEBUG("Do the Thinning");
  trackPC.keep(keptTracks);
  return StatusCode::SUCCESS;
}

StatusCode
ThinTrkTrackAlg::doMuons(const EventContext& ctx) const
{

  // Prepare for the Thinning of Trk::Tracks
  std::vector<bool> keptTracks;
  SG::ThinningHandle<TrackCollection> trackPC(m_CombinedMuonsTracksKey, ctx);
  ATH_MSG_DEBUG("Number of " << m_CombinedMuonsTracksKey.key() << " "
                             << trackPC->size());
  if (keptTracks.size() < trackPC->size()) {
    keptTracks.resize(trackPC->size(), false);
  }

  // Get the muons
  SG::ReadHandle<xAOD::MuonContainer> muons(m_muonsKey, ctx);

  // Loop over muons
  size_t kept(0);
  for (const auto *mu : *muons) {
    if (mu->pt() < m_minptMuons) {
      continue;
    }
    const static SG::AuxElement::Accessor<
      ElementLink<xAOD::TrackParticleContainer>>
      acc("combinedTrackParticleLink");
    if (!acc.isAvailable(*mu)) {
      continue;
    }
    const ElementLink<xAOD::TrackParticleContainer> link = acc(*mu);
    if (!link.isValid()) {
      continue;
    }
    const xAOD::TrackParticle* tp = (*link);
    if (!tp || !tp->trackLink().isValid()) {
      continue;
    }
    size_t index = tp->trackLink().index();
    keptTracks[index] = true;
    ++kept;
  }

  ATH_MSG_DEBUG("keep " << kept << " out of " << keptTracks.size());
  ATH_MSG_DEBUG("Do the Thinning");
  trackPC.keep(keptTracks);
  return StatusCode::SUCCESS;
}
