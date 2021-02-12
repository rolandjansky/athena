/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_PHOTONSUPERCLUSTERBUILDER_H
#define EGAMMAALGS_PHOTONSUPERCLUSTERBUILDER_H

#include "egammaSuperClusterBuilder.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "GaudiKernel/EventContext.h"

// Fwd declarations
#include "egammaInterfaces/IEMConversionBuilder.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/EgammaEnums.h"

#include <string>
#include <vector>

/**
 * @brief Create supercluster under photon hypothesis
 *
 * The algorithm creates superclusters for photons merging topoclusters.
 * Input containers:
 * - \ref photonSuperClusterBuilder.m_inputEgammaRecContainerKey "InputEgammaRecContainerName"
 *  (default=egammaRecCollection): collection of
 * EgammaRec objects to be used
 *
 * Output containers:
 * - \ref photonSuperClusterBuilder.m_photonSuperRecCollectionKey "SuperPhotonRecCollectionName"
 *  (default=PhotonSuperRecCollection): collection of
 * EgammaRec objects with the cluster set to be the supercluster
 * - \ref photonSuperClusterBuilder.m_outputPhotonSuperClustersKey "SuperClusterCollectionName"
 *  (default=PhotonSuperClusters): collection of clusters (the supercluster)
 *
 * The loop is on the clusters of the EgammaRec objects from the input container. Fist, the first
 * cluster is considered as a seed.. The cluster seed must pass some selection:
 * - having a second sampling with |eta| not larger than 10
 * - pT (from the sum of the three accordion layer) not below
 *  \ref egammaSuperClusterBuilder.m_EtThresholdCut "EtThresholdCut"
 *
 * Clusters to be merged in a supercluster are selected using the
 * photonSuperClusterBuilder::searchForSecondaryClusters function. Then the procedure is redone,
 * testing new seeds, for all the other clusters that have not been used to make superclusters.
 * The building of the supercluster is done with egammaSuperClusterBuilder::createNewCluster
 * which selects the cells to be used.
 *
 * Add the end, if the property doConversions is true, the conversion matching is redone on top of new
 * superclusters, using the tool configured by the property
 *  \ref photonSuperClusterBuilder::m_conversionBuilder "ConversionBuilderTool", by default EMConversionBuilder.
 * 
 * \see electronSuperClusterBuilder
 */
class photonSuperClusterBuilder : public egammaSuperClusterBuilder
{

public:
  photonSuperClusterBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  /** Return extra clusters that can be added to make supercluster
   * @param photonInd: index of the EgammaRec object in the input container corresponding to the seed
   * @param egammaRecs: input container of EgammaRec
   * @param isUsed: boolean mask to avoid to reuse clusters (1=already used, 0=not used). When calling this
   * function the element corresponding to the seed is marked as used
   * @param nWindowClusters: how many clusters are added by the matchesInWindow
   * @param nExtraClusters how many clusters are added by the other methods
   *
   * The function returns a vector of index corresponding to secondary clusters to be merged with the seed.
   *
   * If there is a conversion associated to the seed cluster the conversion tracks are considered
   * only if the conversion is single or double Si or if the \ref photonSuperClusterBuilder.m_useOnlySi "UseOnlySi"
   * property is false.
   *
   * The secondary cluster is added if it pass one of the functions:
   * - egammaSuperClusterBuilder::matchesInWindow
   * - photonSuperClusterBuilder::matchesVtx: if satellite cluster and seed cluster share a common conversion vertex
   * - photonSuperClusterBuilder::matchesVtxTrack: if satellite cluster and seed cluster share a common track, considering the track
   *   from the conversion vertex of the seed and the ones associated to the satellite cluster
   * each one can be disabled using the property AddClustersInWindow, AddClustersMatchingVtx and
   *  AddClustrsMatchingVtxTracks.
   * 
   **/
  std::vector<std::size_t> searchForSecondaryClusters(
    std::size_t photonInd,
    const EgammaRecContainer* egammaRecs,
    std::vector<bool>& isUsed) const;

  /** Does the cluster share conversion vertex? */
  bool matchesVtx(
    const std::vector<const xAOD::Vertex*>& seedVertices,
    const std::vector<xAOD::EgammaParameters::ConversionType>& seedVertexType,
    const egammaRec* egRec) const;

  /** Does the cluster match a conversion vertex track with the seed? */
  bool matchesVtxTrack(
    const std::vector<const xAOD::TrackParticle*>& seedVertexTracks,
    const egammaRec* egRec) const;

  // internal variables
  /** @brief Key for input egammaRec container */
  SG::ReadHandleKey<EgammaRecContainer> m_inputEgammaRecContainerKey{
    this,
    "InputEgammaRecContainerName",
    "egammaRecCollection",
    "input egammaRec container"
  };

  /** @brief Key for output egammaRec container */
  SG::WriteHandleKey<EgammaRecContainer> m_photonSuperRecCollectionKey{
    this,
    "SuperPhotonRecCollectionName",
    "PhotonSuperRecCollection",
    "output egammaRec container"
  };

  /** @brief Key for output clusters */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputPhotonSuperClustersKey{
    this,
    "SuperClusterCollectionName",
    "PhotonSuperClusters",
    "output calo cluster container"
  };

  /** @brief Tool to retrieve the conversions*/
  ToolHandle<IEMConversionBuilder> m_conversionBuilder{
    this,
    "ConversionBuilderTool",
    "EMConversionBuilder",
    "Tool that matches conversion vertices to egammaRecs"
  };

  // options for how to build superclusters
  /** @brief add the topoclusters in window */
  Gaudi::Property<bool> m_addClustersInWindow{
    this,
    "AddClustersInWindow",
    true,
    "add the topoclusters in window"
  };

  /** @brief add the topoclusters matching conversion vertex */
  Gaudi::Property<bool> m_addClustersMatchingVtx{
    this,
    "AddClustersMatchingVtx",
    true,
    "add the topoclusters matching conversion vertex"
  };

  /** @brief use only the leading vertex for matching */
  Gaudi::Property<bool> m_useOnlyLeadingVertex{
    this,
    "UseOnlyLeadingVertex",
    true,
    "use only the leading vertex for matching"
  };

  /** @brief use only vertices/tracks with silicon tracks */
  Gaudi::Property<bool> m_useOnlySi{
    this,
    "UseOnlySi",
    true,
    "use only vertices/tracks with silicon tracks for adding sec. clusters "
    "(Mix not considered Si)"
  };

  /** @brief add the topoclusters matching conversion vertex tracks */
  Gaudi::Property<bool> m_addClustersMatchingVtxTracks{
    this,
    "AddClustrsMatchingVtxTracks",
    true,
    "add the topoclusters matching conversion vertex tracks"
  };

  /** @brief use only the leading track for matching */
  Gaudi::Property<bool> m_useOnlyLeadingTrack{
    this,
    "UseOnlyLeadingTrack",
    true,
    "use only the leading track for matching"
  };

  /** @brief private member flag to do the conversion building and matching */
  Gaudi::Property<bool> m_doConversions{ this,
                                         "doConversions",
                                         true,
                                         "Boolean to do conversion matching" };
};

#endif
