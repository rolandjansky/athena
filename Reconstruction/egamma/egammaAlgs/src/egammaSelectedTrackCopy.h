/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "AthContainers/ConstDataVector.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

#include "egammaInterfaces/IegammaCaloClusterSelector.h"
#include <Gaudi/Accumulators.h>

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

class CaloCluster;

class egammaSelectedTrackCopy : public AthAlgorithm
{
public:
  /** @brief Default constructor*/
  egammaSelectedTrackCopy(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute() override final
  {
    return execute_r(Algorithm::getContext());
  }
  // This will become the normal execute when
  // inheriting from AthReentrantAlgorithm
  StatusCode execute_r(const EventContext& ctx) const;

private:
  /** @brief broad track selection */
  bool Select(const EventContext& ctx,
              const xAOD::CaloCluster* cluster,
              const xAOD::TrackParticle* track,
              IEMExtrapolationTools::Cache& cache,
              bool trkTRT) const;

  /** @brief Tool for extrapolation */
  ToolHandle<IEMExtrapolationTools> m_extrapolationTool{ this,
                                                         "ExtrapolationTool",
                                                         "EMExtrapolationTools",
                                                         "Extrapolation tool" };

  /** @brief Tool for extrapolation */
  ToolHandle<IEMExtrapolationTools> m_extrapolationToolCommonCache{
    this,
    "ExtrapolationToolCommonCache",
    "EMExtrapolationToolsCommonCache",
    "Extrapolation tool using the ATLAS common cache"
  };

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

  SG::WriteHandleKey<ConstDataVector<xAOD::TrackParticleContainer>>
    m_OutputTrkPartContainerKey{ this,
                                 "OutputTrkPartContainerName",
                                 "egammaSelectedTrackParticles",
                                 "Output selected TrackParticles" };

  /** @Cut on minimum silicon hits*/
  Gaudi::Property<int> m_MinNoSiHits{ this,
                                      "minNoSiHits",
                                      4,
                                      "Minimum number of silicon hits on track "
                                      "before it is allowed to be refitted" };

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

  /** @narrow windows*/
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

  // For P->T converters of ID tracks with Pixel
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection>
    m_pixelDetEleCollKey{ this,
                          "PixelDetEleCollKey",
                          "PixelDetectorElementCollection",
                          "Key of SiDetectorElementCollection for Pixel" };
  // For P->T converters of ID tracks with SCT
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection>
    m_SCTDetEleCollKey{ this,
                        "SCTDetEleCollKey",
                        "SCT_DetectorElementCollection",
                        "Key of SiDetectorElementCollection for SCT" };

  mutable Gaudi::Accumulators::Counter<unsigned long> m_AllClusters;
  mutable Gaudi::Accumulators::Counter<unsigned long> m_SelectedClusters;
  mutable Gaudi::Accumulators::Counter<unsigned long> m_AllTracks;
  mutable Gaudi::Accumulators::Counter<unsigned long> m_SelectedTracks;
  mutable Gaudi::Accumulators::Counter<unsigned long> m_AllSiTracks;
  mutable Gaudi::Accumulators::Counter<unsigned long> m_SelectedSiTracks;
  mutable Gaudi::Accumulators::Counter<unsigned long> m_AllTRTTracks;
  mutable Gaudi::Accumulators::Counter<unsigned long> m_SelectedTRTTracks;
};
#endif
