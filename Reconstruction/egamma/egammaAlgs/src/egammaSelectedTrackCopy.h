/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EGAMMAALGS_EGAMMASELECTEDTRACKCOPY_H
#define EGAMMAALGS_EGAMMASELECTEDTRACKCOPY_H

#include "egammaInterfaces/IEMExtrapolationTools.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "CaloDetDescr/CaloDetDescrManager.h"

#include "AthContainers/ConstDataVector.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

#include "egammaInterfaces/IegammaCaloClusterSelector.h"
#include <Gaudi/Accumulators.h>

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

/**
  @class egammaSelectedTrackCopy
  Algorithm which selects tracks to be GSF refitted
  later on.

  The algorithm select tracks that are matched to cluster using eta and phi.
  Si-track/cluster pairs with Et (computed with eta from the track) less than
  10 MeV are not considered. Non-Si-track/cluster pair with Et (from the cluster)
  less than 10 MeV are not considered.

  The eta/phi matching is done in two steps. In the first step a broad matching
  is done. Then a narrower match is done using extrapolated tracks to the second layer.
  For TRT tracks only broad phi-matching is tried.

  - Input container xAOD::CaloClusterContainer: ClusterContainerName
  - Input container xAOD::TrackParticleContainer: TrackParticleContainerName
  - Output container xAOD::TrackParticleContainer: OutputTrkPartContainerName

  The heavy work is done directly inside the algorithm, without the usage of
  any particular external tool (see selectTrack private method).

  */
class egammaSelectedTrackCopy : public AthReentrantAlgorithm
{
public:
  /** @brief Default constructor*/
  egammaSelectedTrackCopy(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  /** @brief broad track selection */
  bool selectTrack(const EventContext& ctx,
                   const xAOD::CaloCluster* cluster,
                   const xAOD::TrackParticle* track,
                   bool trkTRT,
                   const CaloDetDescrManager& caloDD) const;

  /** @brief Tool for extrapolation */
  ToolHandle<IEMExtrapolationTools> m_extrapolationTool{ this,
                                                         "ExtrapolationTool",
                                                         "EMExtrapolationTools",
                                                         "Extrapolation tool" };
  /** @brief Tool to filter the calo clusters */
  ToolHandle<IegammaCaloClusterSelector> m_egammaCaloClusterSelector{
    this,
    "egammaCaloClusterSelector",
    "egammaCaloClusterSelector",
    "Tool that makes the cluster selection"
  };

  /** @brief Names of input output collections */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterContainerKey{
    this,
    "ClusterContainerName",
    "egammaTopoCluster",
    "Input calo cluster for seeding"
  };

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleContainerKey{
    this,
    "TrackParticleContainerName",
    "InDetTrackParticles",
    "Input TrackParticles to select from"
  };

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloDetDescrMgrKey {
    this,
    "CaloDetDescrManager",
    "CaloDetDescrManager",
    "SG Key for CaloDetDescrManager in the Condition Store"
  };

  SG::WriteHandleKey<ConstDataVector<xAOD::TrackParticleContainer>>
    m_OutputTrkPartContainerKey{ this,
                                 "OutputTrkPartContainerName",
                                 "egammaSelectedTrackParticles",
                                 "Output selected TrackParticles" };

  /** @brief broad cut on deltaEta*/
  Gaudi::Property<double> m_broadDeltaEta{ this,
                                           "broadDeltaEta",
                                           0.2,
                                           "Value of broad cut for delta eta" };

  /** @brief broad cut on deltaPhi*/
  Gaudi::Property<double> m_broadDeltaPhi{ this,
                                           "broadDeltaPhi",
                                           0.3,
                                           "Value of broad cut for delta phi" };

  /** @brief narrow windows*/
  Gaudi::Property<double> m_narrowDeltaEta{
    this,
    "narrowDeltaEta",
    0.05,
    "Value of narrow cut for delta eta"
  };

  Gaudi::Property<double> m_narrowDeltaPhi{
    this,
    "narrowDeltaPhi",
    0.05,
    "Value of narrow cut for delta phi"
  };

  Gaudi::Property<double> m_narrowDeltaPhiBrem{
    this,
    "narrowDeltaPhiBrem",
    0.2,
    "Value of the narrow cut for delta phi in the brem direction"
  };

  Gaudi::Property<double> m_narrowRescale{
    this,
    "narrowDeltaPhiRescale",
    0.05,
    "Value of the narrow cut for delta phi Rescale"
  };

  Gaudi::Property<double> m_narrowRescaleBrem{
    this,
    "narrowDeltaPhiRescaleBrem",
    0.1,
    "Value of the narrow cut for delta phi Rescale Brem"
  };

  mutable Gaudi::Accumulators::Counter<> m_AllClusters;
  mutable Gaudi::Accumulators::Counter<> m_SelectedClusters;
  mutable Gaudi::Accumulators::Counter<> m_AllTracks;
  mutable Gaudi::Accumulators::Counter<> m_SelectedTracks;
  mutable Gaudi::Accumulators::Counter<> m_AllSiTracks;
  mutable Gaudi::Accumulators::Counter<> m_SelectedSiTracks;
  mutable Gaudi::Accumulators::Counter<> m_AllTRTTracks;
  mutable Gaudi::Accumulators::Counter<> m_SelectedTRTTracks;
};
#endif
