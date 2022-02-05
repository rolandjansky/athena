/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "photonSuperClusterBuilder.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include <cmath>
#include <memory>

photonSuperClusterBuilder::photonSuperClusterBuilder(const std::string& name,
                                                     ISvcLocator* pSvcLocator)
  : egammaSuperClusterBuilderBase(name, pSvcLocator)
{}

StatusCode
photonSuperClusterBuilder::initialize()
{
  ATH_MSG_DEBUG(" Initializing photonSuperClusterBuilder");

  // the data handle keys
  ATH_CHECK(m_inputEgammaRecContainerKey.initialize());
  ATH_CHECK(m_photonSuperRecCollectionKey.initialize());
  ATH_CHECK(m_outputPhotonSuperClustersKey.initialize());
  ATH_CHECK(m_precorrClustersKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_caloDetDescrMgrKey.initialize());

  // retrieve conversion builder
  if (m_doConversions) {
    ATH_CHECK(m_conversionBuilder.retrieve());
  }

  return egammaSuperClusterBuilderBase::initialize();
}

StatusCode
photonSuperClusterBuilder::execute(const EventContext& ctx) const
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
    m_outputPhotonSuperClustersKey, ctx);
  ATH_CHECK(outputClusterContainer.record(
    std::make_unique<xAOD::CaloClusterContainer>(),
    std::make_unique<xAOD::CaloClusterAuxContainer>()));

  // Create the new Photon Super Cluster based EgammaRecContainer
  SG::WriteHandle<EgammaRecContainer> newEgammaRecs(
    m_photonSuperRecCollectionKey, ctx);
  ATH_CHECK(newEgammaRecs.record(std::make_unique<EgammaRecContainer>()));

  std::optional<SG::WriteHandle<xAOD::CaloClusterContainer> > precorrClustersH;
  if (!m_precorrClustersKey.empty()) {
    precorrClustersH.emplace (m_precorrClustersKey, ctx);
    ATH_CHECK( precorrClustersH->record
               (std::make_unique<xAOD::CaloClusterContainer>(),
                std::make_unique<xAOD::CaloClusterAuxContainer>()) );
  }

  // The calo Det Descr manager
  SG::ReadCondHandle<CaloDetDescrManager> caloDetDescrMgrHandle { m_caloDetDescrMgrKey, ctx };
  ATH_CHECK(caloDetDescrMgrHandle.isValid());
  const CaloDetDescrManager* calodetdescrmgr = *caloDetDescrMgrHandle;

  // Reserve a vector to keep track of what is used
  std::vector<bool> isUsed(egammaRecs->size(), false);
  std::vector<bool> isUsedRevert(egammaRecs->size(), false);
  // Loop over input egammaRec objects, build superclusters.
  for (std::size_t i = 0; i < egammaRecs->size(); ++i) {
    if (isUsed[i]){
      continue;
    }

    const auto* const egRec = (*egammaRecs)[i];
    // Seed cluster selections
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
    }

    // Create the new cluster: take the full list of cluster and add their cells
    // together
    auto egType = (egRec->getNumberOfVertices() > 0)
                    ? xAOD::EgammaParameters::convertedPhoton
                    : xAOD::EgammaParameters::unconvertedPhoton;

    bool clusterAdded =
      createNewCluster(ctx,
                       accumulatedClusters,
                       *calodetdescrmgr,
                       egType,
                       outputClusterContainer.ptr(),
                       precorrClustersH ? precorrClustersH->ptr() : nullptr);

    if (!clusterAdded) {
      isUsed.swap(isUsedRevert);
      continue;
    }

    // Add the cluster link to the super cluster
    ElementLink<xAOD::CaloClusterContainer> clusterLink(
      *outputClusterContainer, outputClusterContainer->size() - 1, ctx);
    std::vector<ElementLink<xAOD::CaloClusterContainer>> phCluster{
      clusterLink
    };

    // Make egammaRec object, and push it back into output container.
    auto newEgRec = std::make_unique<egammaRec>(*egRec);
    if (newEgRec) {
      newEgRec->setCaloClusters(phCluster);
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

// assume egammaRecs != 0, since the ReadHadler is valid
// assume seed egammaRec has a valid cluster, since it has been already used
std::vector<std::size_t>
photonSuperClusterBuilder::searchForSecondaryClusters(
  std::size_t seedIndex,
  const EgammaRecContainer* egammaRecs,
  std::vector<bool>& isUsed) const
{

  std::vector<std::size_t> secondaryIndices;

  const auto* const seedEgammaRec = (*egammaRecs)[seedIndex];
  const xAOD::CaloCluster* const seedCaloClus = seedEgammaRec->caloCluster();

  // let's determine some things about the seed
  std::vector<const xAOD::Vertex*> seedVertices;
  std::vector<xAOD::EgammaParameters::ConversionType> seedVertexType;
  std::vector<const xAOD::TrackParticle*>
    seedVertexTracks; // tracks from conversion vertex

  auto numVertices = seedEgammaRec->getNumberOfVertices();
  if (m_useOnlyLeadingVertex && numVertices > 0) {
    numVertices = 1;
  }

  for (std::size_t vx = 0; vx < numVertices; ++vx) {
    const auto* const vertex = seedEgammaRec->vertex(vx);
    const auto convType = xAOD::EgammaHelpers::conversionType(vertex);
    seedVertices.push_back(vertex);
    seedVertexType.push_back(convType);
    const bool addTracks = !m_useOnlySi ||
                           convType == xAOD::EgammaParameters::singleSi ||
                           convType == xAOD::EgammaParameters::doubleSi;
    if (addTracks) {
      for (unsigned int tp = 0; tp < vertex->nTrackParticles(); ++tp) {
        seedVertexTracks.push_back(vertex->trackParticle(tp));
      }
    }
  }

  // for stats
  int nWindowClusters = 0;
  int nExtraClusters = 0;

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
    } else if (m_addClustersMatchingVtx &&
               matchesVtx(seedVertices, seedVertexType, secEgammaRec)) {
      ATH_MSG_DEBUG("conversion vertices match");
      addCluster = true;
      ++nExtraClusters;
    } else if (m_addClustersMatchingVtxTracks &&
               matchesVtxTrack(seedVertexTracks, secEgammaRec)) {
      ATH_MSG_DEBUG("conversion track match");
      addCluster = true;
      ++nExtraClusters;
    }
    // Add it to the list of secondary clusters if it matches.
    if (addCluster) {
      secondaryIndices.push_back(i);
      isUsed[i] = true;
    }
  }
  ATH_MSG_DEBUG("Found: " << secondaryIndices.size() << " secondaries");
  ATH_MSG_DEBUG("window clusters: " << nWindowClusters);
  ATH_MSG_DEBUG("extra clusters: " << nExtraClusters);

  return secondaryIndices;
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
    if (!m_useOnlySi ||
        seedVertexType[seedVx] == xAOD::EgammaParameters::singleSi ||
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
  for (const auto* seedVertexTrack : seedVertexTracks) {
    // selected tracks alread are just Si if we are only looking at Si tracks
    for (size_t testTk = 0; testTk < numTestTracks; ++testTk) {
      if (seedVertexTrack == egRec->trackParticle(testTk)) {
        return true;
      }
    }
  }
  return false;
}
