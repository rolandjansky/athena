/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "electronSuperClusterBuilder.h"
//
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "FourMomUtils/P4Helpers.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include <cmath>
#include <memory>

electronSuperClusterBuilder::electronSuperClusterBuilder(
  const std::string& name,
  ISvcLocator* pSvcLocator)
  : egammaSuperClusterBuilderBase(name, pSvcLocator)
  , m_maxDelEta(m_maxDelEtaCells * s_cellEtaSize * 0.5)
  , m_maxDelPhi(m_maxDelPhiCells * s_cellPhiSize * 0.5)
{}

StatusCode
electronSuperClusterBuilder::initialize()
{
  ATH_MSG_DEBUG(" Initializing electronSuperClusterBuilder");
  // Call initialize of base
  ATH_CHECK(egammaSuperClusterBuilderBase::initialize());
  // the data handle keys
  ATH_CHECK(m_inputEgammaRecContainerKey.initialize());
  ATH_CHECK(m_electronSuperRecCollectionKey.initialize());
  ATH_CHECK(m_outputElectronSuperClustersKey.initialize());
  ATH_CHECK(m_precorrClustersKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_caloDetDescrMgrKey.initialize());

  // Additional Window we search in
  m_maxDelPhi = m_maxDelPhiCells * s_cellPhiSize * 0.5;
  m_maxDelEta = m_maxDelEtaCells * s_cellEtaSize * 0.5;

  // retrieve track match builder
  if (m_doTrackMatching) {
    ATH_CHECK(m_trackMatchBuilder.retrieve());
  }
  return StatusCode::SUCCESS;
}

StatusCode
electronSuperClusterBuilder::execute(const EventContext& ctx) const
{

  SG::ReadHandle<EgammaRecContainer> egammaRecs(m_inputEgammaRecContainerKey,
                                                ctx);
  // check is only used for serial running; remove when MT scheduler used
  if (!egammaRecs.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_inputEgammaRecContainerKey.key());
    return StatusCode::FAILURE;
  }

  // Have to register cluster container in order to properly get cluster
  // links.
  SG::WriteHandle<xAOD::CaloClusterContainer> outputClusterContainer(
    m_outputElectronSuperClustersKey, ctx);

  ATH_CHECK(outputClusterContainer.record(
    std::make_unique<xAOD::CaloClusterContainer>(),
    std::make_unique<xAOD::CaloClusterAuxContainer>()));

  // Create the new Electron Super Cluster based EgammaRecContainer
  SG::WriteHandle<EgammaRecContainer> newEgammaRecs(
    m_electronSuperRecCollectionKey, ctx);
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
    const auto* egRec = (*egammaRecs)[i];
    // check for good seed cluster
    const xAOD::CaloCluster* clus = egRec->caloCluster();
    if (!seedClusterSelection(clus)) {
      continue;
    }
    // We need tracks
    if (egRec->getNumberOfTrackParticles() == 0) {
      continue;
    }
    // with possible pixel
    uint8_t nPixelHits(0);
    uint8_t uint8_value(0);
    if (egRec->trackParticle(0)->summaryValue(uint8_value,
                                              xAOD::numberOfPixelDeadSensors)) {
      nPixelHits += uint8_value;
    }
    if (egRec->trackParticle(0)->summaryValue(uint8_value,
                                              xAOD::numberOfPixelHits)) {
      nPixelHits += uint8_value;
    }
    if (nPixelHits < m_numberOfPixelHits) {
      continue;
    }
    // and with silicon (add SCT to pixel)
    uint8_t nSiHits = nPixelHits;
    if (egRec->trackParticle(0)->summaryValue(uint8_value,
                                              xAOD::numberOfSCTHits)) {
      nSiHits += uint8_value;
    }
    if (nSiHits < m_numberOfSiHits) {
      continue;
    }
    // Mark seed as used
    isUsedRevert = isUsed; // save status in case we fail to create supercluster
    isUsed[i] = true;

    // Start accumulating the clusters from the seed
    std::vector<const xAOD::CaloCluster*> accumulatedClusters;
    accumulatedClusters.push_back(clus);

    // Now we find all the secondary cluster for this seed
    const std::vector<std::size_t> secondaryIndices =
      searchForSecondaryClusters(i, egammaRecs.cptr(), isUsed);

    for (const auto& secClusIndex : secondaryIndices) {
      const auto* const secRec = (*egammaRecs)[secClusIndex];
      accumulatedClusters.push_back(secRec->caloCluster());
    }

    ATH_MSG_DEBUG("Total clusters " << accumulatedClusters.size());

    // Create the new cluster: take the full list of cluster and add their
    // cells together
    bool clusterAdded =
      createNewCluster(ctx,
                       accumulatedClusters,
                       *calodetdescrmgr,
                       xAOD::EgammaParameters::electron,
                       outputClusterContainer.ptr(),
                       precorrClustersH ? precorrClustersH->ptr() : nullptr);
    if (!clusterAdded) {
      // Revert status of constituent clusters.
      isUsed.swap(isUsedRevert);
      continue;
    }

    // Add the cluster link to the super cluster
    ElementLink<xAOD::CaloClusterContainer> clusterLink(
      *outputClusterContainer, outputClusterContainer->size() - 1, ctx);
    std::vector<ElementLink<xAOD::CaloClusterContainer>> elClusters{
      clusterLink
    };

    // Make egammaRec object, and push it back into output container.
    auto newEgRec = std::make_unique<egammaRec>(*egRec);
    if (newEgRec) {
      newEgRec->setCaloClusters(elClusters);
      newEgammaRecs->push_back(std::move(newEgRec));
    } else {
      ATH_MSG_FATAL("Couldn't make an egammaRec object");
      return StatusCode::FAILURE;
    }
  } // End loop on egammaRecs

  // Redo track matching given the super cluster
  if (m_doTrackMatching) {
    ATH_CHECK(m_trackMatchBuilder->executeRec(ctx, newEgammaRecs.ptr()));
  }
  return StatusCode::SUCCESS;
}

std::vector<std::size_t>
electronSuperClusterBuilder::searchForSecondaryClusters(
  const std::size_t seedIndex,
  const EgammaRecContainer* egammaRecs,
  std::vector<bool>& isUsed) const
{
  // assume egammaRecs != 0, since the ReadHadler is valid
  // assume seed egammaRec has a valid cluster, since it has been already used
  std::vector<std::size_t> secondaryIndices;

  const auto* const seedEgammaRec = (*egammaRecs)[seedIndex];
  const xAOD::CaloCluster* const seedCaloClus = seedEgammaRec->caloCluster();

  const xAOD::TrackParticle* seedTrackParticle = seedEgammaRec->trackParticle();

  // Now loop over the potential secondary clusters
  for (std::size_t i = 0; i < egammaRecs->size(); ++i) {

    // if already used continue
    if (isUsed[i]) {
      continue;
    }

    const auto* const secEgammaRec = (*egammaRecs)[i];
    const xAOD::CaloCluster* const secClus = secEgammaRec->caloCluster();
    // Now perform a number of tests to see if the cluster should be added

    const auto seedSecdEta = std::abs(seedCaloClus->eta() - secClus->eta());
    const auto seedSecdPhi =
      std::abs(P4Helpers::deltaPhi(seedCaloClus->phi(), secClus->phi()));

    const bool addCluster =
      (matchesInWindow(seedCaloClus, secClus) ||
       ((seedSecdEta < m_maxDelEta && seedSecdPhi < m_maxDelPhi) &&
        (matchSameTrack(*seedTrackParticle, *secEgammaRec))));
    // Add it to the list of secondary clusters if it matches.
    if (addCluster) {
      secondaryIndices.push_back(i);
      isUsed[i] = true;
    }
  }
  ATH_MSG_DEBUG("Found: " << secondaryIndices.size() << " secondaries");
  return secondaryIndices;
}

bool
electronSuperClusterBuilder::matchSameTrack(
  const xAOD::TrackParticle& seedTrack,
  const egammaRec& sec)
{
  const xAOD::TrackParticle* secTrack = sec.trackParticle();
  if (secTrack) {
    // Check that the tracks are the same.
    return seedTrack.index() == secTrack->index();
  }
  return false;
}

