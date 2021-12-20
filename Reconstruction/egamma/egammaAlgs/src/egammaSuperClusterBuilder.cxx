/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaSuperClusterBuilder.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include <cmath>
#include <memory>

egammaSuperClusterBuilder::egammaSuperClusterBuilder(const std::string& name,
                                                     ISvcLocator* pSvcLocator)
  : egammaSuperClusterBuilderBase(name, pSvcLocator)
  , m_egTypeForCalibration(xAOD::EgammaParameters::electron)
{}

StatusCode
egammaSuperClusterBuilder::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaSuperClusterBuilder");

  // the data handle keys
  ATH_CHECK(m_inputEgammaRecContainerKey.initialize());
  ATH_CHECK(m_egammaSuperRecCollectionKey.initialize());
  ATH_CHECK(m_outputegammaSuperClustersKey.initialize());
  ATH_CHECK(m_precorrClustersKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_caloDetDescrMgrKey.initialize());
  if (m_calibrationType == "electron") {
    m_egTypeForCalibration = xAOD::EgammaParameters::electron;
  } else if (m_calibrationType == "photon") {
    m_egTypeForCalibration = xAOD::EgammaParameters::unconvertedPhoton;
  } else {
    ATH_MSG_ERROR("Unsupported calibration for " << m_calibrationType);
    return StatusCode::FAILURE;
  }
  return egammaSuperClusterBuilderBase::initialize();
}

StatusCode
egammaSuperClusterBuilder::execute(const EventContext& ctx) const
{

  // Retrieve input egammaRec container.
  SG::ReadHandle<EgammaRecContainer> egammaRecs(m_inputEgammaRecContainerKey,
                                                ctx);

  // check is only used for serial running; remove when MT scheduler used
  if (!egammaRecs.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_inputEgammaRecContainerKey.key());
    return StatusCode::FAILURE;
  }

  // Have to register cluster container in order to properly get cluster links.
  SG::WriteHandle<xAOD::CaloClusterContainer> outputClusterContainer(
    m_outputegammaSuperClustersKey, ctx);
  ATH_CHECK(outputClusterContainer.record(
    std::make_unique<xAOD::CaloClusterContainer>(),
    std::make_unique<xAOD::CaloClusterAuxContainer>()));

  // Create the new egamma Super Cluster based EgammaRecContainer
  SG::WriteHandle<EgammaRecContainer> newEgammaRecs(
    m_egammaSuperRecCollectionKey, ctx);
  ATH_CHECK(newEgammaRecs.record(std::make_unique<EgammaRecContainer>()));

  std::optional<SG::WriteHandle<xAOD::CaloClusterContainer>> precorrClustersH;
  if (!m_precorrClustersKey.empty()) {
    precorrClustersH.emplace(m_precorrClustersKey, ctx);
    ATH_CHECK(precorrClustersH->record(
      std::make_unique<xAOD::CaloClusterContainer>(),
      std::make_unique<xAOD::CaloClusterAuxContainer>()));
  }

  // The calo Det Descr manager
  SG::ReadCondHandle<CaloDetDescrManager> caloDetDescrMgrHandle{
    m_caloDetDescrMgrKey, ctx
  };
  ATH_CHECK(caloDetDescrMgrHandle.isValid());

  const CaloDetDescrManager* calodetdescrmgr = *caloDetDescrMgrHandle;

  // Reserve a vector to keep track of what is used
  std::vector<bool> isUsed(egammaRecs->size(), false);
  std::vector<bool> isUsedRevert(egammaRecs->size(), false);
  // Loop over input egammaRec objects, build superclusters.
  for (std::size_t i = 0; i < egammaRecs->size(); ++i) {
    if (isUsed[i]) {
      continue;
    }

    const auto* const egRec = (*egammaRecs)[i];
    // check for good seed cluster
    const xAOD::CaloCluster* clus = egRec->caloCluster();
    if (!seedClusterSelection(clus)) {
      continue;
    }
    // Passed preliminary custs
    // Mark seed as used
    isUsedRevert = isUsed; // save status in case we fail to create supercluster
    isUsed[i] = true;

    // Start accumulating the clusters from the seed
    std::vector<const xAOD::CaloCluster*> accumulatedClusters;
    accumulatedClusters.push_back(clus);

    const std::vector<std::size_t> secondaryIndices =
      searchForSecondaryClusters(i, egammaRecs.cptr(), isUsed);

    for (const auto secClusIndex : secondaryIndices) {
      const auto* const secRec = (*egammaRecs)[secClusIndex];
      accumulatedClusters.push_back(secRec->caloCluster());
      // no need to add vertices
    }

    std::unique_ptr<xAOD::CaloCluster> newCluster =
      createNewCluster(ctx,
                       accumulatedClusters,
                       *calodetdescrmgr,
                       m_egTypeForCalibration,
                       precorrClustersH ? precorrClustersH->ptr() : nullptr);

    if (!newCluster) {
      ATH_MSG_DEBUG("Creating a new cluster failed");
      // Revert status of constituent clusters.
      isUsed.swap(isUsedRevert);
      continue;
    }

    // push back the new egamma super cluster to the output container
    outputClusterContainer->push_back(std::move(newCluster));

    // Add the cluster link to the super cluster
    ElementLink<xAOD::CaloClusterContainer> clusterLink(
      *outputClusterContainer, outputClusterContainer->size() - 1, ctx);
    std::vector<ElementLink<xAOD::CaloClusterContainer>> egCluster{
      clusterLink
    };

    // Make egammaRec object, and push it back into output container.
    auto newEgRec = std::make_unique<egammaRec>(*egRec);
    if (newEgRec) {
      newEgRec->setCaloClusters(egCluster);
      newEgammaRecs->push_back(std::move(newEgRec));
      ATH_MSG_DEBUG("Finished making egamma egammaRec object");
    } else {
      ATH_MSG_FATAL("Couldn't make an egammaRec object");
      return StatusCode::FAILURE;
    }
  } // End loop on egammaRecs

  return StatusCode::SUCCESS;
}

// assume egammaRecs != 0, since the ReadHadler is valid
// assume seed egammaRec has a valid cluster, since it has been already used
std::vector<std::size_t>
egammaSuperClusterBuilder::searchForSecondaryClusters(
  std::size_t seedIndex,
  const EgammaRecContainer* egammaRecs,
  std::vector<bool>& isUsed) const
{

  std::vector<std::size_t> secondaryIndices;

  const auto* const seedEgammaRec = (*egammaRecs)[seedIndex];
  const xAOD::CaloCluster* const seedCaloClus = seedEgammaRec->caloCluster();

  // for stats
  int nWindowClusters = 0;
  // Now loop over the potential secondary clusters
  for (std::size_t i = 0; i < egammaRecs->size(); ++i) {
    // if already used continue
    if (isUsed[i]) {
      continue;
    }
    const auto* const secEgammaRec = (*egammaRecs)[i];
    const xAOD::CaloCluster* const secClus = secEgammaRec->caloCluster();
    if (!secClus) {
      ATH_MSG_WARNING(
        "The potentially secondary egammaRec does not have a cluster");
      continue;
    }
    bool addCluster = false;
    if (matchesInWindow(seedCaloClus, secClus)) {
      ATH_MSG_DEBUG("Cluster with Et: " << secClus->et()
                                        << " matched in window");
      ++nWindowClusters;
      addCluster = true;
    }
    // Add it to the list of secondary clusters if it matches.
    if (addCluster) {
      secondaryIndices.push_back(i);
      isUsed[i] = true;
    }
  }
  ATH_MSG_DEBUG("Found: " << secondaryIndices.size() << " secondaries");
  ATH_MSG_DEBUG("window clusters: " << nWindowClusters);
  return secondaryIndices;
}

