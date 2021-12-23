///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef THINNINGUTILS_ThinTrkTrackAlg_H
#define THINNINGUTILS_ThinTrkTrackAlg_H
/**
 @class ThinTrkTrackAlg
*/
// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ThinningHandleKey.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"

class ThinTrkTrackAlg : public AthReentrantAlgorithm
{
public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;
  /// Athena algorithm's initalize hook
  virtual StatusCode initialize() override final;

  /// Athena algorithm's execute hook
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  /// Inline method
  StatusCode doEGamma(const EventContext& ctx) const;
  StatusCode doMuons(const EventContext& ctx) const;

  StringProperty m_streamName{ this,
                               "StreamName",
                               "",
                               "Name of the stream being thinned" };

  /// Should the thinning run?
  /// The containers of Objects of interest Electron/Muon
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonsKey{
    this,
    "MuonsKey",
    "Muons",
    "StoreGate key for muons container"
  };

  SG::ReadHandleKey<xAOD::ElectronContainer> m_electronsKey{
    this,
    "ElectronsKey",
    "Electrons",
    "StoreGate key for electrons container"
  };

  SG::ReadHandleKey<xAOD::PhotonContainer> m_photonsKey{
    this,
    "PhotonsKey",
    "Photons",
    "StoreGate key for photon container"
  };

  /// Containers to thin
  SG::ThinningHandleKey<TrackCollection> m_CombinedMuonsTracksKey{
    this,
    "CombinedMuonsTrackKey",
    "CombinedMuonTracks",
    "StoreGate key for combined muons Trk::Track container"
  };

  SG::ThinningHandleKey<TrackCollection> m_GSFTracksKey{
    this,
    "GSFTrackKey",
    "GSFTracks",
    "StoreGate key for GSF Trk::Track container"
  };

  Gaudi::Property<bool> m_doElectrons{ this, "doElectrons", true };
  Gaudi::Property<bool> m_doPhotons{ this, "doPhotons", true };
  Gaudi::Property<bool> m_doMuons{ this, "doMuons", true };
  Gaudi::Property<bool> m_bestonlyElectrons{ this, "OnlyBestElectrons", true };
  Gaudi::Property<bool> m_bestonlyPhotons{ this, "OnlyBestPhotons", true };
  Gaudi::Property<double> m_minptElectrons{ this, "minptElectrons", 4000. };
  Gaudi::Property<double> m_minptPhotons{ this, "minptPhotons", 4000. };
  Gaudi::Property<double> m_minptMuons{ this, "minptMuons", 4000. };
};

#endif
