/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMASUPERCLUSTERBUILDER_H
#define EGAMMAALGS_EGAMMASUPERCLUSTERBUILDER_H

#include "egammaSuperClusterBuilderBase.h"

#include "GaudiKernel/EventContext.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "CaloDetDescr/CaloDetDescrManager.h"

// Fwd declarations
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/EgammaEnums.h"

#include <string>
#include <vector>

/**
 * @brief Create supercluster under egamma (no tracking) hypothesis
 * Useful if you want to run calo reconstuction without tracking
 *
 * The algorithm creates superclusters  merging topoclusters.
 * Input containers:
 * - \ref egammaSuperClusterBuilder.m_inputEgammaRecContainerKey
 * "InputEgammaRecContainerName" (default=egammaRecCollection): collection of
 * EgammaRec objects to be used
 *
 * Output containers:
 * - \ref egammaSuperClusterBuilder.m_egammaSuperRecCollectionKey
 * "SuperegammaRecCollectionName" (default=egammaSuperRecCollection): collection
 * of EgammaRec objects with the cluster set to be the supercluster
 * - \ref egammaSuperClusterBuilder.m_outputegammaSuperClustersKey
 * "SuperClusterCollectionName" (default=egammaSuperClusters): collection of
 * clusters (the supercluster)
 *
 * The loop is on the clusters of the EgammaRec objects from the input
 * container. Fist, the first cluster is considered as a seed. The cluster seed
 * must pass some selection:
 * - having a second sampling with |eta| not larger than 10
 * - pT (from the sum of the three accordion layer) not below
 *  \ref egammaSuperClusterBuilderBase.m_EtThresholdCut "EtThresholdCut"
 *
 * Clusters to be merged in a supercluster are selected using the
 * egammaSuperClusterBuilder::searchForSecondaryClusters function. Then the
 * procedure is redone, testing new seeds, for all the other clusters that have
 * not been used to make superclusters. The building of the supercluster is done
 * with egammaSuperClusterBuilderBase::createNewCluster which selects the cells
 * to be used.
 */
class egammaSuperClusterBuilder : public egammaSuperClusterBuilderBase
{

public:
  egammaSuperClusterBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  /** Return extra clusters that can be added to make supercluster
   * @param egammaInd: index of the EgammaRec object in the input container
   *corresponding to the seed
   * @param egammaRecs: input container of EgammaRec
   * @param isUsed: boolean mask to avoid to reuse clusters (1=already used,
   *0=not used). When calling this function the element corresponding to the
   *seed is marked as used
   * @param nWindowClusters: how many clusters are added by the matchesInWindow
   *
   * The function returns a vector of index corresponding to secondary clusters
   *to be merged with the seed.
   *
   * The secondary cluster is added if it pass one of the functions:
   * - egammaSuperClusterBuilderBase::matchesInWindow
   **/
  std::vector<std::size_t> searchForSecondaryClusters(
    std::size_t egammaInd,
    const EgammaRecContainer* egammaRecs,
    std::vector<bool>& isUsed) const;

  xAOD::EgammaParameters::EgammaType m_egTypeForCalibration;

  
  /** @brief type to be assumed for calibration */
  Gaudi::Property<std::string> m_calibrationType{
    this,
    "CalibrationType",
    "electron",
    "type to be assumed for calibration: electron , photon"
  };

  /** @brief Key for input egammaRec container */
  SG::ReadHandleKey<EgammaRecContainer> m_inputEgammaRecContainerKey{
    this,
    "InputEgammaRecContainerName",
    "egammaRecCollection",
    "input egammaRec container"
  };

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloDetDescrMgrKey{
    this,
    "CaloDetDescrManager",
    "CaloDetDescrManager",
    "SG Key for CaloDetDescrManager in the Condition Store"
  };

  /** @brief Key for output egammaRec container */
  SG::WriteHandleKey<EgammaRecContainer> m_egammaSuperRecCollectionKey{
    this,
    "SuperegammaRecCollectionName",
    "egammaSuperRecCollection",
    "output egammaRec container"
  };

  /** @brief Key for output clusters */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputegammaSuperClustersKey{
    this,
    "SuperClusterCollectionName",
    "egammaSuperClusters",
    "output calo cluster container"
  };

  /** @brief Optional key for pre-correction clusters */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_precorrClustersKey{
    this,
    "precorrClustersName",
    "",
    "optional pre-correction clusters"
  };
};

#endif
