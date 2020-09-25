/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_ELECTRONSUPERCLUSTERBUILDER_H
#define EGAMMAALGS_ELECTRONSUPERCLUSTERBUILDER_H

// INCLUDE HEADER FILES:
#include "egammaSuperClusterBuilder.h"

#include "GaudiKernel/EventContext.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Fwd declarations
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

class electronSuperClusterBuilder : public egammaSuperClusterBuilder
{

public:
  // Constructor/destructor.
  electronSuperClusterBuilder(const std::string& name,
                              ISvcLocator* pSvcLocator);

  // Tool standard routines.
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  StatusCode execute(const EventContext& ctx) const override final;

private:
  bool matchSameTrack(const xAOD::TrackParticle& seedTrack,
                      const egammaRec& sec) const;

  std::vector<std::size_t> searchForSecondaryClusters(
    const size_t i,
    const EgammaRecContainer*,
    std::vector<bool>& isUsed) const;

  /** @brief Size of maximum search window in eta */
  Gaudi::Property<int> m_maxDelEtaCells{
    this,
    "MaxWindowDelEtaCells",
    5,
    "Size of maximum search window in eta"
  };
  /** @brief Size of maximum search window in phi */
  Gaudi::Property<int> m_maxDelPhiCells{
    this,
    "MaxWindowDelPhiCells",
    12,
    "Size of maximum search window in phi"
  };

  float m_maxDelEta;
  float m_maxDelPhi;
  Gaudi::Property<std::size_t> m_numberOfPixelHits{
    this,
    "NumberOfReqPixelHits",
    1,
    "Number of required pixel hits for electrons"
  };

  Gaudi::Property<std::size_t> m_numberOfSiHits{
    this,
    "NumberOfReqSiHits",
    7,
    "Number of required silicon hits for electrons"
  };

  /** @brief Key for input egammaRec container */
  SG::ReadHandleKey<EgammaRecContainer> m_inputEgammaRecContainerKey{
    this,
    "InputEgammaRecContainerName",
    "egammaRecCollection",
    "input egammaRec container"
  };

  /** @brief Key for output egammaRec container */
  SG::WriteHandleKey<EgammaRecContainer> m_electronSuperRecCollectionKey{
    this,
    "SuperElectronRecCollectionName",
    "ElectronSuperRecCollection",
    "output egammaRec container"
  };

  /** @brief Key for output clusters */
  SG::WriteHandleKey<xAOD::CaloClusterContainer>
    m_outputElectronSuperClustersKey{ this,
                                      "SuperClusterCollestionName",
                                      "ElectronSuperClusters",
                                      "output calo cluster container" };

  /** @brief Tool to perform track matching*/
  ToolHandle<IEMTrackMatchBuilder> m_trackMatchBuilder{
    this,
    "TrackMatchBuilderTool",
    "EMTrackMatchBuilder",
    "Tool that matches tracks to egammaRecs"
  };

  /** @brief private member flag to do the track matching */
  Gaudi::Property<bool> m_doTrackMatching{ this,
                                           "doTrackMatching",
                                           true,
                                           "Boolean to do track matching" };
};

#endif
