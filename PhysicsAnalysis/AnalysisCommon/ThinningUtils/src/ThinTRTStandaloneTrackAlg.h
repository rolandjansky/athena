/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
*/

#ifndef THINNINGUTILS_THINTRTSTANDALONETRACKALG_H
#define THINNINGUTILS_THINTRTSTANDALONETRACKALG_H

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMuon/MuonContainer.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/ReadHandleKey.h"


/**
 * @brief Thin TRT standalone tracks
 *
 * Algorithm that thins TRT standalone tracks that are neither used by egamma nor by taus
 */
class ThinTRTStandaloneTrackAlg : public AthReentrantAlgorithm
{
 public:

  /** @brief Default constructor*/
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

  /** @brief initialize method*/
  StatusCode initialize() override final;

  /** @brief execute method*/
  StatusCode execute(const EventContext& ctx) const override final;

 private:

  /** @brief Thin electron tracks */
  Gaudi::Property<bool> m_doElectron { this, "doElectron", true };

  /** @brief Thin photon tracks */
  Gaudi::Property<bool> m_doPhoton { this, "doPhoton", true };

  /** @brief Thin tau tracks */
  Gaudi::Property<bool> m_doTau { this, "doTau", true };

  Gaudi::Property<bool> m_doMuon{this, "doMuon", true};
  /** @brief Track Particle container to thin */
  SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_InDetTrackParticlesKey
    { this, "InDetTrackParticleContainerName", "InDetTrackParticles", "Name of the TrackParticle container to thin" };
  
  /** @brief Name of the stream being thinned */
  StringProperty m_streamName
    { this, "StreamName", "StreamAOD", "Name of the stream being thinned" };

  /** @brief electron collection input name*/
  SG::ReadHandleKey<xAOD::ElectronContainer> m_InputElectronContainerKey
    { this, "InputElectronContainerName", "Electrons", "Name of the input electron container" };

  /** @brief photon collection input name*/
  SG::ReadHandleKey<xAOD::PhotonContainer> m_InputPhotonContainerKey
    { this, "InputPhotonContainerName", "Photons", "Name of the input photon container" };

  /** @brief photon collection input name*/
  SG::ReadHandleKey<xAOD::TauJetContainer> m_InputTauJetContainerKey
    { this, "InputTauJetContainerName", "TauJets", "Name of the input tau container" };
  SG::ReadHandleKey<xAOD::MuonContainer> m_inputMuonContainerKey{
     this, "InputMuonContainerName", "Muons", "Name of the input muon container"
  };
};

#endif // THINNINGUTILS_THINTRTSTANDALONETRACKALG_H
