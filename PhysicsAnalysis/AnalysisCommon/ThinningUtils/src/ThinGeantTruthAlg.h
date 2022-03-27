///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef THINNINGUTILS_ThinGeantTruthAlg_H
#define THINNINGUTILS_ThinGeantTruthAlg_H

/**
 @class ThinGeantTruthAlg
*/

// STL includes
#include <atomic>
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ThinningHandleKey.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

class ThinGeantTruthAlg : public AthReentrantAlgorithm
{
public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode execute(const EventContext& ctx) const override final;

  /// Inline method
  void ancestors(const xAOD::TruthParticle*,
                 std::vector<bool>&,
                 std::unordered_set<int>&) const;
  void descendants(const xAOD::TruthParticle*,
                   std::vector<bool>&,
                   std::unordered_set<int>&) const;
  static bool isStatus1BSMParticle(const xAOD::TruthParticle*) ;

private:
  StringProperty m_streamName{ this,
                               "StreamName",
                               "",
                               "Stream for which thinning is to be done." };

  Gaudi::Property<int> m_geantOffset{ this,
                                      "GeantBarcodeOffset",
                                      200000,
                                      "Barcode offset for Geant particles" };

  Gaudi::Property<float> m_etaMaxEgTruth{ this,
                                          "EtaMaxEGammaTruth",
                                          2.525,
                                          "Max eta value for e-gamma truth particles" };

  Gaudi::Property<bool> m_keepMuons{ this, "keepMuons", true };
  Gaudi::Property<bool> m_keepEGamma{ this, "keepEGamma", true };

  Gaudi::Property<std::vector<int>> m_longlived{
    this,
    "LongLivedParticleList",
    { 310, 3122, 3222, 3112, 3322, 3312 },
    "List of long lifetime particles which are likely to be decayed by "
    "Geant but whose children must be kept"
  };

  SG::ThinningHandleKey<xAOD::TruthParticleContainer> m_truthParticlesKey{
    this,
    "TruthParticlesKey",
    "TruthParticles",
    "Name of the input Truth Particle container"
  };

  SG::ThinningHandleKey<xAOD::TruthVertexContainer> m_truthVerticesKey{
    this,
    "TruthVerticesKey",
    "TruthVertices",
    "Name of the input Truth Vertices container"
  };

  SG::ReadHandleKey<xAOD::ElectronContainer> m_electronsKey{
    this,
    "ElectronsKey",
    "Electrons",
    "Name of the input electron container"
  };

  SG::ReadHandleKey<xAOD::ElectronContainer> m_fwdElectronsKey{
    this,
    "FwdElectronsKey",
    "",
    "Name of the input forward electron container"
  };

  SG::ReadHandleKey<xAOD::PhotonContainer> m_photonsKey{
    this,
    "PhotonsKey",
    "Photons",
    "Name of the input photon container"
  };

  SG::ReadHandleKey<xAOD::MuonContainer>
    m_muonsKey{ this, "MuonsKey", "Muons", "Name of the input muon container" };

  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_egammaTruthKey{
    this,
    "EGammaTruthKey",
    "egammaTruthParticles",
    "Name of the input egammaTruth container"
  };

  /// Counters
  mutable std::atomic<unsigned long> m_nEventsProcessed{};
  mutable std::atomic<unsigned long> m_nParticlesProcessed{};
  mutable std::atomic<unsigned long> m_nVerticesProcessed{};
  mutable std::atomic<unsigned long> m_nParticlesThinned{};
  mutable std::atomic<unsigned long> m_nVerticesThinned{};
};

#endif //> !THINNINGUTILS_ThinGeantTruthAlg_H
