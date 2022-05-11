///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef THINNINGUTILS_ThinInDetForwardTrackParticlesAlg_H
#define THINNINGUTILS_ThinInDetForwardTrackParticlesAlg_H 1

/**
 @class ThinInDetForwardTrackParticlesAlg
*/

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ThinningHandleKey.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// STL includes
#include <atomic>
#include <string>

class ThinInDetForwardTrackParticlesAlg final : public AthReentrantAlgorithm
{
public:
  /// Constructor with parameters:
  ThinInDetForwardTrackParticlesAlg(const std::string& name,
                                    ISvcLocator* pSvcLocator);

  /// Destructor:
  virtual ~ThinInDetForwardTrackParticlesAlg() = default;

  /// Athena algorithm's initalize hook
  virtual StatusCode initialize() override;

  /// Athena algorithm's execute hook
  virtual StatusCode execute(const EventContext& ctx) const override;

  /// Athena algorithm's finalize hook
  virtual StatusCode finalize() override;

private:
  StringProperty m_streamName{ this,
                               "StreamName",
                               "",
                               "Stream for which to thin" };

  SG::ThinningHandleKey<xAOD::TrackParticleContainer> m_tracksKey{
    this,
    "TracksKey",
    "InDetForwardTrackParticles",
    "Tracks to thin"
  };

  SG::ReadHandleKey<xAOD::MuonContainer>
    m_muonsKey{ this, "MuonsKey", "Muons", "Muons to use for thinning" };

  /// Should the thinning run?
  BooleanProperty m_doThinning{
    this,
    "ThinInDetForwardTrackParticles",
    true,
    "Should the InDetForwardTrackParticles thinning be run?"
  };

  /// Counters
  mutable std::atomic<unsigned long> m_nEventsProcessed{};
  mutable std::atomic<unsigned long> m_nTracksProcessed{};
  mutable std::atomic<unsigned long> m_nTracksThinned{};
  mutable std::atomic<unsigned long> m_nMuons{};
  mutable std::atomic<unsigned long> m_nSiFwdMuons{};
  mutable std::atomic<unsigned long> m_nSiFwdAssoc{};
};

#endif //> !THINNINGUTILS_ThinInDetForwardTrackParticlesAlg_H
