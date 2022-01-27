/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
*/

#include "ThinTRTStandaloneTrackAlg.h"
#include "StoreGate/ThinningHandle.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

/**
 * @brief Gaudi initialize method.
 */
StatusCode
ThinTRTStandaloneTrackAlg::initialize()
{
  ATH_CHECK(m_InDetTrackParticlesKey.initialize(m_streamName));

  ATH_CHECK(m_InputElectronContainerKey.initialize(m_doElectron));
  ATH_CHECK(m_InputPhotonContainerKey.initialize(m_doPhoton));
  ATH_CHECK(m_InputTauJetContainerKey.initialize(m_doTau));
  ATH_CHECK(m_inputMuonContainerKey.initialize(m_doMuon));
  return StatusCode::SUCCESS;
}

/**
 * @brief Execute the algorithm.
 * @param ctx Current event context.
 */
StatusCode
ThinTRTStandaloneTrackAlg::execute(const EventContext& ctx) const
{
  // Retrieve InDet TrackParticles
  SG::ThinningHandle<xAOD::TrackParticleContainer> indetTrackPC(
    m_InDetTrackParticlesKey, ctx);

  if (!indetTrackPC.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve " << m_InDetTrackParticlesKey.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Number of InDet TrackParticles " << indetTrackPC->size());

  // We set to false ONLY the TRT standalone track particles
  // We then reset to true whatever is to be kept
  // from the domains
  std::vector<bool> keptInDetTrackParticles;
  keptInDetTrackParticles.resize(indetTrackPC->size(), true);
  for (const auto* track : *indetTrackPC) {
    if (track->patternRecoInfo().test(xAOD::TRTStandalone)) {
      keptInDetTrackParticles[track->index()] = false;
    }
  }

  if (m_doElectron) {
    // Retrieve electrons
    SG::ReadHandle<xAOD::ElectronContainer> electrons(
      m_InputElectronContainerKey, ctx);
    if (!electrons.isValid()) {
      ATH_MSG_FATAL("Failed to retrieve " << m_InputElectronContainerKey.key());
      return StatusCode::FAILURE;
    }

    // Loop over electrons
    for (const xAOD::Electron* electron : *electrons) {
      auto trackParticleLinks = electron->trackParticleLinks();
      for (const auto& link : trackParticleLinks) {
        if (!link.isValid()) {
          continue;
        }
        ATH_MSG_DEBUG(
          "Electons : Keeping InDet Track Particle with index : "
          << (xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(*link))
               ->index());
        keptInDetTrackParticles
          [(xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(*link))
             ->index()] = true;
      }
    }
  }

  if (m_doPhoton) {
    // Retrieve photons
    SG::ReadHandle<xAOD::PhotonContainer> photons(m_InputPhotonContainerKey,
                                                  ctx);
    if (!photons.isValid()) {
      ATH_MSG_FATAL("Failed to retrieve " << m_InputPhotonContainerKey.key());
      return StatusCode::FAILURE;
    }

    // Loop over photons
    for (const xAOD::Photon* photon : *photons) {
      auto vertexLinks = photon->vertexLinks();
      for (const auto& vxlink : vertexLinks) {
        if (!vxlink.isValid()) {
          continue;
        }
        const xAOD::Vertex* vx = *(vxlink);
        if (!vx) {
          continue;
        }
        auto trackParticleLinks = vx->trackParticleLinks();
        for (const auto& link : trackParticleLinks) {
          if (!link.isValid()) {
            continue;
          }
          ATH_MSG_DEBUG(
            "Photons : Keeping InDet Track Particle with index : "
            << (xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(*link))
                 ->index());
          keptInDetTrackParticles
            [(xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(*link))
               ->index()] = true;
        }
      }
    }
  }

  if (m_doTau) {
    // Retrieve taus
    SG::ReadHandle<xAOD::TauJetContainer> taus(m_InputTauJetContainerKey, ctx);
    if (!taus.isValid()) {
      ATH_MSG_FATAL("Failed to retrieve " << m_InputTauJetContainerKey.key());
      return StatusCode::FAILURE;
    }
    static const SG::AuxElement::ConstAccessor<char> acc_passThinning(
      "passThinning");

    // Loop over taus
    for (const xAOD::TauJet* tau : *taus) {
      if (!acc_passThinning(*tau)) {
        continue;
      }
      for (const xAOD::TauTrack* track : tau->allTracks()) {
        // LRTs are not in the InDetTrackParticles container, so skip them
        if (!track->flag(
              xAOD::TauJetParameters::TauTrackFlag::LargeRadiusTrack)) {
          keptInDetTrackParticles[track->track()->index()] = true;
        }
      }
    }
  }
  if (m_doMuon) {
    SG::ReadHandle<xAOD::MuonContainer> muons(m_inputMuonContainerKey, ctx);
    if (!muons.isValid()) {
      ATH_MSG_FATAL("Failed to retrieve " << m_inputMuonContainerKey.key());
      return StatusCode::FAILURE;
    }
    for (const xAOD::Muon* muon : *muons) {
      const xAOD::TrackParticle* trk =
        muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
      /// SaF muons have the ID link to the InDetForwardTrackParticles
      if (muon->muonType() != xAOD::Muon::SiliconAssociatedForwardMuon && trk)
        keptInDetTrackParticles[trk->index()] = true;
    }
  }

  // Do the thinning
  indetTrackPC.keep(keptInDetTrackParticles);

  return StatusCode::SUCCESS;
}
