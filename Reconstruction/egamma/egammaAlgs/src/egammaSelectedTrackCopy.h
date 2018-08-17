/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EGAMMAALGS_EGAMMASELECTEDTRACKCOPY_H
#define EGAMMAALGS_EGAMMASELECTEDTRACKCOPY_H

/**
  @class egammaSelectedTrackCopy
  Algorithm which selectrs tracks to be GSF refitted
  later on.
  */

#include "egammaInterfaces/IEMExtrapolationTools.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "AthContainers/ConstDataVector.h"

#include <atomic>

class CaloCluster;

class egammaSelectedTrackCopy : public AthAlgorithm 
{
public:
  /** @brief Default constructor*/
  egammaSelectedTrackCopy(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute() override final;

private:
  /** @brief broad track selection */
  bool Select(const xAOD::CaloCluster* cluster,
              bool trkTRT,
              const xAOD::TrackParticle* track) const;
  /** @brief Tool for extrapolation */
  ToolHandle<IEMExtrapolationTools> m_extrapolationTool {this,
    "ExtrapolationTool", "EMExtrapolationTools", "Extrapolation tool"};

  /** @brief Names of input output collections */
  SG::ReadHandleKey<xAOD::CaloClusterContainer>  m_clusterContainerKey {this,
    "ClusterContainerName", "LArClusterEM", "Input calo cluster for seeding"};

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleContainerKey {this,
    "TrackParticleContainerName", "InDetTrackParticles", 
    "Input TrackParticles to select from"};

  SG::WriteHandleKey< ConstDataVector<xAOD::TrackParticleContainer> > m_OutputTrkPartContainerKey {this,
    "OutputTrkPartContainerName", "egammaSelectedTrackParticles", 
    "Output selected TrackParticles"};

  /** @Cut on minimum silicon hits*/
  Gaudi::Property<int> m_MinNoSiHits {this, "minNoSiHits", 4, 
    "Minimum number of silicon hits on track before it is allowed to be refitted"};

  /** @brief broad cut on deltaEta*/
  Gaudi::Property<double> m_broadDeltaEta {this, "broadDeltaEta", 0.1,
    "Value of broad cut for delta eta, it is mult by 2"};

  /** @brief broad cut on deltaPhi*/
  Gaudi::Property<double> m_broadDeltaPhi {this, "broadDeltaPhi", 0.15,
    "Value of broad cut for delta phi, it is mult by 2"};

  /** @narrow windows*/
  Gaudi::Property<double> m_narrowDeltaEta {this, "narrowDeltaEta", 0.05,
    "Value of narrow cut for delta eta"};

  Gaudi::Property<double> m_narrowDeltaPhi {this, "narrowDeltaPhi", 0.05,
    "Value of narrow cut for delta phi"};

  Gaudi::Property<double> m_narrowDeltaPhiBrem {this, "narrowDeltaPhiBrem", 0.15,
    "Value of the narrow cut for delta phi in the brem direction"};

  Gaudi::Property<double> m_narrowRescale {this, "narrowDeltaPhiRescale", 0.05,
    "Value of the narrow cut for delta phi Rescale"};

  Gaudi::Property<double> m_narrowRescaleBrem {this, "narrowDeltaPhiRescaleBrem", 0.1,
    "Value of the narrow cut for delta phi Rescale Brem"};

  /* counters. For now use mutable atomic
   * the methods will increment a local variable
   * inside the loops.
   * At the end they will add_fetch to these ones
   */
  mutable std::atomic_uint m_AllTracks{0};
  mutable std::atomic_uint m_AllTRTTracks{0};
  mutable std::atomic_uint m_AllSiTracks{0};
  mutable std::atomic_uint m_SelectedTracks{0};
  mutable std::atomic_uint m_SelectedTRTTracks{0};
  mutable std::atomic_uint m_SelectedSiTracks{0};
};
#endif 
