/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "electronSuperClusterBuilder.h"
//
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "FourMomUtils/P4Helpers.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include <memory>

electronSuperClusterBuilder::electronSuperClusterBuilder(const std::string& name,
                                                         ISvcLocator* pSvcLocator)
  : egammaSuperClusterBuilder(name, pSvcLocator)
{
  // Additional Window we search in
  m_maxDelPhi = m_maxDelPhiCells * s_cellPhiSize * 0.5;
  m_maxDelEta = m_maxDelEtaCells * s_cellEtaSize * 0.5;
}

StatusCode
electronSuperClusterBuilder::initialize()
{
  ATH_MSG_DEBUG(" Initializing electronSuperClusterBuilder");
  // Call initialize of base
  ATH_CHECK(egammaSuperClusterBuilder::initialize());
  // the data handle keys
  ATH_CHECK(m_inputEgammaRecContainerKey.initialize());
  ATH_CHECK(m_electronSuperRecCollectionKey.initialize());
  ATH_CHECK(m_outputElectronSuperClustersKey.initialize());

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
electronSuperClusterBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode
electronSuperClusterBuilder::execute_r(const EventContext& ctx) const
{

  SG::ReadHandle<EgammaRecContainer> egammaRecs(m_inputEgammaRecContainerKey, ctx);
  // check is only used for serial running; remove when MT scheduler used
  if (!egammaRecs.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_inputEgammaRecContainerKey.key());
    return StatusCode::FAILURE;
  }

  // Have to register cluster container in order to properly get cluster links.
  SG::WriteHandle<xAOD::CaloClusterContainer> outputClusterContainer(
    m_outputElectronSuperClustersKey, ctx);

  ATH_CHECK(outputClusterContainer.record(std::make_unique<xAOD::CaloClusterContainer>(),
                                          std::make_unique<xAOD::CaloClusterAuxContainer>()));

  // Create the new Electron Super Cluster based EgammaRecContainer
  SG::WriteHandle<EgammaRecContainer> newEgammaRecs(m_electronSuperRecCollectionKey, ctx);
  ATH_CHECK(newEgammaRecs.record(std::make_unique<EgammaRecContainer>()));

  // The calo Det Descr manager
  const CaloDetDescrManager* calodetdescrmgr = nullptr;
  ATH_CHECK(detStore()->retrieve(calodetdescrmgr, "CaloMgr"));

  // Reserve a vector to keep track of what is used
  std::vector<bool> isUsed(egammaRecs->size(), 0);
  std::vector<bool> isUsedRevert(egammaRecs->size(), 0);
  // Loop over input egammaRec objects, build superclusters.
  for (std::size_t i = 0; i < egammaRecs->size(); ++i) {

    // Used to reset back status of selected
    // in case we fail to form a supercluster
    isUsedRevert = isUsed;

    // Check if used
    auto egRec = egammaRecs->at(i);
    if (isUsed.at(i)) {
      continue;
    }

    // Seed selections
    const xAOD::CaloCluster* clus = egRec->caloCluster();
    // The seed should have 2nd sampling
    if (!clus->hasSampling(CaloSampling::EMB2) && !clus->hasSampling(CaloSampling::EME2)) {
      continue;
    }
    const double eta2 = fabs(clus->etaBE(2));
    if (eta2 > 10) {
      continue;
    }
    // Accordeon Energy samplings 1 to 3
    const double EMAccEnergy = clus->energyBE(1) + clus->energyBE(2) + clus->energyBE(3);
    const double EMAccEt = EMAccEnergy / cosh(eta2);
    // Require minimum energy for supercluster seeding.
    if (EMAccEt < m_EtThresholdCut) {
      continue;
    }
    // We need tracks
    if (egRec->getNumberOfTrackParticles() == 0) {
      continue;
    }

    // with possible pixel
    uint8_t nPixelHits(0);
    uint8_t uint8_value(0);
    if (egRec->trackParticle(0)->summaryValue(uint8_value, xAOD::numberOfPixelDeadSensors)) {
      nPixelHits += uint8_value;
    }
    if (egRec->trackParticle(0)->summaryValue(uint8_value, xAOD::numberOfPixelHits)) {
      nPixelHits += uint8_value;
    }
    if (nPixelHits < m_numberOfPixelHits) {
      continue;
    }

    // and with silicon (add SCT to pixel)
    uint8_t nSiHits = nPixelHits;
    if (egRec->trackParticle(0)->summaryValue(uint8_value, xAOD::numberOfSCTHits)) {
      nSiHits += uint8_value;
    }
    if (nSiHits < m_numberOfSiHits) {
      continue;
    }
    ATH_MSG_DEBUG("Creating supercluster egammaRec electron using cluster Et = "
                  << egRec->caloCluster()->et() << " eta " << egRec->caloCluster()->eta() << " phi "
                  << egRec->caloCluster()->phi() << " EM Accordeon Et " << EMAccEt << " pixel hits "
                  << static_cast<unsigned int>(nPixelHits) << " silicon hits "
                  << static_cast<unsigned int>(nSiHits));
    // Mark seed as used
    isUsed.at(i) = true;

    // Add the seed as the 1st entry in the secondaries list
    std::vector<const xAOD::CaloCluster*> accumulatedClusters;
    accumulatedClusters.push_back(egRec->caloCluster());

    // Now we find all the secondary cluster for this seed
    const std::vector<std::size_t> secondaryIndices =
      searchForSecondaryClusters(i, egammaRecs.cptr(), isUsed);
    for (const auto& secIndex : secondaryIndices) {
      const auto secRec = egammaRecs->at(secIndex);
      accumulatedClusters.push_back(secRec->caloCluster());
    }

    ATH_MSG_DEBUG("Total clusters " << accumulatedClusters.size());

    // Take the full list of cluster and add their cells together
    std::unique_ptr<xAOD::CaloCluster> newClus = createNewCluster(
      ctx, accumulatedClusters, *calodetdescrmgr, xAOD::EgammaParameters::electron);

    if (!newClus) {
      isUsed = isUsedRevert;
      continue;
    }

    // Push back the new cluster into the output container.
    outputClusterContainer->push_back(std::move(newClus));
    ElementLink<xAOD::CaloClusterContainer> clusterLink(*outputClusterContainer,
                                                        outputClusterContainer->size() - 1);
    std::vector<ElementLink<xAOD::CaloClusterContainer>> elClusters{ clusterLink };

    // Make egammaRec object, and push it back into output container.
    auto newEgRec = std::make_unique<egammaRec>(*egRec);
    newEgRec->setCaloClusters(elClusters);
    newEgammaRecs->push_back(std::move(newEgRec));
  } // End loop on egammaRecs

  // Redo track matching given the super cluster
  if (m_doTrackMatching) {
    ATH_CHECK(m_trackMatchBuilder->executeRec(ctx, newEgammaRecs.ptr()));
  }
  return StatusCode::SUCCESS;
}

std::vector<std::size_t>
electronSuperClusterBuilder::searchForSecondaryClusters(const std::size_t electronIndex,
                                                        const EgammaRecContainer* egammaRecs,
                                                        std::vector<bool>& isUsed) const
{
  std::vector<std::size_t> secondaryClusters;
  if (!egammaRecs) {
    return secondaryClusters;
  }
  const auto seedEgammaRec = egammaRecs->at(electronIndex);
  const xAOD::CaloCluster* seedCluster = seedEgammaRec->caloCluster();

  const xAOD::TrackParticle* seedTrackParticle = seedEgammaRec->trackParticle();

  for (std::size_t i = 0; i < egammaRecs->size(); ++i) {

    // if already used continue
    if (isUsed.at(i)) {
      continue;
    }
    // if not retrieve the relevant info
    const auto egRec = egammaRecs->at(i);
    const xAOD::CaloCluster* clus = egRec->caloCluster();
    float seedSecdEta(fabs(seedCluster->eta() - clus->eta()));
    float seedSecdPhi(fabs(P4Helpers::deltaPhi(seedCluster->phi(), clus->phi())));
    /* add the cluster  if
     * 1.matches the seed in a narrow window
     * OR
     * 2.Is inside the range for additonal criteria
     * AND matches the same track
     */
    bool addCluster = (matchesInWindow(seedCluster, clus) ||
                       ((seedSecdEta < m_maxDelEta && seedSecdPhi < m_maxDelPhi) &&
                        (matchSameTrack(*seedTrackParticle, *egRec))));
    // Add it to the list of secondary clusters if it matches.
    if (addCluster) {
      secondaryClusters.push_back(i);
      isUsed.at(i) = 1;
    }
  }
  ATH_MSG_DEBUG("Found: " << secondaryClusters.size() << " secondaries");
  return secondaryClusters;
}

bool
electronSuperClusterBuilder::matchSameTrack(const xAOD::TrackParticle& seedTrack,
                                            const egammaRec& sec) const
{
  bool matchesSameTrack(false);
  const xAOD::TrackParticle* secTrack = sec.trackParticle();
  if (secTrack) {
    // Check that the tracks are the same.
    matchesSameTrack = (seedTrack.index() == secTrack->index());
  }
  return matchesSameTrack;
}

