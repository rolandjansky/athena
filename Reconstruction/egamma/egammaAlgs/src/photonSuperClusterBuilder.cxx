/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "photonSuperClusterBuilder.h"
//
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
//
#include "FourMomUtils/P4Helpers.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//
#include <vector>

//////////////////////////////////////////////////////////////////////////////
// Athena interfaces.
//////////////////////////////////////////////////////////////////////////////

// Constructor.
photonSuperClusterBuilder::photonSuperClusterBuilder(const std::string& name,
                                                     ISvcLocator* pSvcLocator)
  : egammaSuperClusterBuilder(name, pSvcLocator)
{}

StatusCode
photonSuperClusterBuilder::initialize()
{
  ATH_MSG_DEBUG(" Initializing photonSuperClusterBuilder");

  // the data handle keys
  ATH_CHECK(m_inputEgammaRecContainerKey.initialize());
  ATH_CHECK(m_photonSuperRecCollectionKey.initialize());
  ATH_CHECK(m_outputPhotonSuperClustersKey.initialize());

  // retrieve conversion builder
  if (m_doConversions) {
    ATH_CHECK(m_conversionBuilder.retrieve());
  }

  return egammaSuperClusterBuilder::initialize();
}

StatusCode
photonSuperClusterBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode
photonSuperClusterBuilder::execute_r(const EventContext& ctx) const
{

  // Retrieve input egammaRec container.
  SG::ReadHandle<EgammaRecContainer> egammaRecs(m_inputEgammaRecContainerKey, ctx);

  // check is only used for serial running; remove when MT scheduler used
  if (!egammaRecs.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_inputEgammaRecContainerKey.key());
    return StatusCode::FAILURE;
  }

  // Have to register cluster container in order to properly get cluster links.
  SG::WriteHandle<xAOD::CaloClusterContainer> outputClusterContainer(
    m_outputPhotonSuperClustersKey,ctx);
  ATH_CHECK(outputClusterContainer.record(std::make_unique<xAOD::CaloClusterContainer>(),
                                          std::make_unique<xAOD::CaloClusterAuxContainer>()));

  // Create the new Photon Super Cluster based EgammaRecContainer
  SG::WriteHandle<EgammaRecContainer> newEgammaRecs(m_photonSuperRecCollectionKey,ctx);
  ATH_CHECK(newEgammaRecs.record(std::make_unique<EgammaRecContainer>()));

  // The calo Det Descr manager
  const CaloDetDescrManager* calodetdescrmgr = nullptr;
  ATH_CHECK(detStore()->retrieve(calodetdescrmgr, "CaloMgr"));

  // Loop over input egammaRec objects, build superclusters.
  std::vector<bool> isUsed(egammaRecs->size(), 0);

  for (std::size_t i = 0; i < egammaRecs->size(); ++i) {
    // Used to revert status of topos
    // in case we fail to make a supercluser.
    std::vector<bool> isUsedRevert(isUsed);
    const auto egRec = egammaRecs->at(i);
    const auto clus = egRec->caloCluster();
    // First some basic seed cuts
    if (isUsed.at(i)) {
      continue;
    }
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
    // Passed preliminary custs
    ATH_MSG_DEBUG("Creating supercluster egammaRec photon object "
                  << 'n' << "Using cluster Et = " << clus->et() << " EM Accordeon Et " << EMAccEt);
    // So it is used
    isUsed.at(i) = 1;
    // Start accumulating
    std::vector<const xAOD::CaloCluster*> accumulatedClusters;
    accumulatedClusters.push_back(clus);

    // Core Logic goes here
    ATH_MSG_DEBUG("Find secondary clusters");

    // for stats
    int nWindowClusters = 0;
    int nExtraClusters = 0;

    const std::vector<std::size_t> secondaryClusters =
      searchForSecondaryClusters(i, egammaRecs.cptr(), isUsed, nWindowClusters, nExtraClusters);

    for (auto secClus : secondaryClusters) {
      const auto secRec = egammaRecs->at(secClus);
      accumulatedClusters.push_back(secRec->caloCluster());
      // no need to add vertices
    }
    // End of core Logic

    // Create the new cluster

    ATH_MSG_DEBUG("Set up new Cluster");
    auto egType = (egRec->getNumberOfVertices() > 0) ? xAOD::EgammaParameters::convertedPhoton
                                                     : xAOD::EgammaParameters::unconvertedPhoton;

    std::unique_ptr<xAOD::CaloCluster> newCluster =
      createNewCluster(ctx, accumulatedClusters, *calodetdescrmgr, egType);

    if (!newCluster) {
      ATH_MSG_DEBUG("Creating a new cluster failed");
      // Revert status of constituent clusters.
      isUsed = isUsedRevert;
      continue;
    }

    ATH_MSG_DEBUG("window clusters: " << nWindowClusters);
    ATH_MSG_DEBUG("extra clusters: " << nExtraClusters);

    // push back the new photon super cluster
    outputClusterContainer->push_back(std::move(newCluster));

    // Add the cluster link to the super cluster
    ElementLink<xAOD::CaloClusterContainer> clusterLink(*outputClusterContainer,
                                                        outputClusterContainer->size() - 1);
    std::vector<ElementLink<xAOD::CaloClusterContainer>> phCluster{ clusterLink };

    ///////////////////////////////////////////////////////
    // Now create the new eg Rec
    auto newEgRec = std::make_unique<egammaRec>(*egRec);
    if (newEgRec) {
      newEgRec->setCaloClusters(phCluster);
      // push it back
      newEgammaRecs->push_back(std::move(newEgRec));
      ATH_MSG_DEBUG("Finished making photon egammaRec object");
    } else {
      ATH_MSG_FATAL("Couldn't make an egammaRec object");
      return StatusCode::FAILURE;
    }
  } // End loop on egammaRecs

  // Redo conversion matching given the super cluster
  if (m_doConversions) {
    for (auto egRec : *newEgammaRecs) {
      if (m_conversionBuilder->executeRec(ctx, egRec).isFailure()) {
        ATH_MSG_ERROR("Problem executing conversioBuilder on photonSuperRecs");
        return StatusCode::FAILURE;
      }
    }
  }

  return StatusCode::SUCCESS;
}

std::vector<std::size_t>
photonSuperClusterBuilder::searchForSecondaryClusters(std::size_t photonInd,
                                                      const EgammaRecContainer* egammaRecs,
                                                      std::vector<bool>& isUsed,
                                                      int& nWindowClusters,
                                                      int& nExtraClusters) const
{

  std::vector<std::size_t> secondaryClusters;
  if (!egammaRecs) {
    ATH_MSG_WARNING("photon egammaRec container is null! Returning an empty vector ...");
    return secondaryClusters;
  }

  const auto seedPhoton = egammaRecs->at(photonInd);
  const auto seedCaloClus = seedPhoton->caloCluster();
  if (!seedCaloClus) {
    ATH_MSG_WARNING("The seed egammaRec does not have a cluster");
    return secondaryClusters;
  }

  // let's determine some things about the seed
  std::vector<const xAOD::Vertex*> seedVertices;
  std::vector<xAOD::EgammaParameters::ConversionType> seedVertexType;
  std::vector<const xAOD::TrackParticle*> seedVertexTracks; // tracks from conversion vertex

  auto numVertices = seedPhoton->getNumberOfVertices();
  if (m_useOnlyLeadingVertex && numVertices > 0) {
    numVertices = 1;
  }

  for (std::size_t vx = 0; vx < numVertices; ++vx) {
    const auto vertex = seedPhoton->vertex(vx);
    const auto convType = xAOD::EgammaHelpers::conversionType(vertex);
    seedVertices.push_back(vertex);
    seedVertexType.push_back(convType);
    const bool addTracks = !m_useOnlySi || convType == xAOD::EgammaParameters::singleSi ||
                           convType == xAOD::EgammaParameters::doubleSi;
    if (addTracks) {
      for (unsigned int tp = 0; tp < vertex->nTrackParticles(); ++tp) {
        seedVertexTracks.push_back(vertex->trackParticle(tp));
      }
    }
  }

  // Now loop over the potential secondary clusters
  for (std::size_t i = 0; i < egammaRecs->size(); ++i) {

    // First some basic seed cuts
    if (isUsed.at(i)) {
      continue;
    }

    const auto egRec = egammaRecs->at(i);
    const auto caloClus = egRec->caloCluster();
    if (!caloClus) {
      ATH_MSG_WARNING("The potentially secondary egammaRec does not have a cluster");
      continue;
    }
    // Now perform a number of tests to see if the cluster should be added
    bool addCluster = false;
    if (m_addClustersInWindow && matchesInWindow(seedCaloClus, caloClus)) {
      ATH_MSG_DEBUG("Cluster  with Et : " << caloClus->et() << " matched in window");
      nWindowClusters++;
      addCluster = true;
    } else if (m_addClustersMatchingVtx && matchesVtx(seedVertices, seedVertexType, egRec)) {
      ATH_MSG_DEBUG("conversion vertices match");
      addCluster = true;
      nExtraClusters++;
    } else if (m_addClustersMatchingVtxTracks && matchesVtxTrack(seedVertexTracks, egRec)) {
      ATH_MSG_DEBUG("conversion track match");
      addCluster = true;
      nExtraClusters++;
    }
    if (addCluster) {
      secondaryClusters.push_back(i);
      isUsed[i] = true;
    }
  }
  ATH_MSG_DEBUG("Found: " << secondaryClusters.size() << " secondaries");
  return secondaryClusters;
}

bool
photonSuperClusterBuilder::matchesVtx(
  const std::vector<const xAOD::Vertex*>& seedVertices,
  const std::vector<xAOD::EgammaParameters::ConversionType>& seedVertexType,
  const egammaRec* egRec) const
{

  auto numTestVertices = egRec->getNumberOfVertices();
  if (m_useOnlyLeadingVertex && numTestVertices > 0) {
    numTestVertices = 1;
  }
  for (size_t seedVx = 0; seedVx < seedVertices.size(); ++seedVx) {
    if (!m_useOnlySi || seedVertexType[seedVx] == xAOD::EgammaParameters::singleSi ||
        seedVertexType[seedVx] == xAOD::EgammaParameters::doubleSi) {

      for (size_t testVx = 0; testVx < numTestVertices; ++testVx) {
        if (seedVertices[seedVx] == egRec->vertex(testVx)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool
photonSuperClusterBuilder::matchesVtxTrack(
  const std::vector<const xAOD::TrackParticle*>& seedVertexTracks,
  const egammaRec* egRec) const
{
  auto numTestTracks = egRec->getNumberOfTrackParticles();
  if (m_useOnlyLeadingTrack && numTestTracks > 0) {
    numTestTracks = 1;
  }
  for (auto seedVertexTrack : seedVertexTracks) {
    // selected tracks alread are just Si if we are only looking at Si tracks
    for (size_t testTk = 0; testTk < numTestTracks; ++testTk) {
      if (seedVertexTrack == egRec->trackParticle(testTk)) {
        return true;
      }
    }
  }
  return false;
}
