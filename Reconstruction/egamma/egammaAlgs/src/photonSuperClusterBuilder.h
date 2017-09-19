/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_PHOTONSUPERCLUSTERBUILDER_H
#define EGAMMAALGS_PHOTONSUPERCLUSTERBUILDER_H

// INCLUDE HEADER FILES:
#include "egammaSuperClusterBuilder.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

//Fwd declarations
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class IEMConversionBuilder;

class photonSuperClusterBuilder : public egammaSuperClusterBuilder {

 public:

  //Constructor/destructor.
  photonSuperClusterBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  //Tool standard routines.
  StatusCode initialize();
  StatusCode finalize();
  //New StoreGate-based execute routine. Will use previously-established
  //track-cluster vertex matches and make a supercluster egammaRec
  //container in StoreGate.
  StatusCode execute();
 private:

  /** Return extra clusters that can be added to make supercluster */
  std::vector<std::size_t> 
    SearchForSecondaryClusters(std::size_t photonInd,
			       const EgammaRecContainer *egammaRecs,
			       std::vector<bool>& isUsed,
			       int& nWindowClusters,
			       int& nExtraClusters) const;

  /** Does the cluster share conversion vertex? */
  bool MatchesVtx(const std::vector<const xAOD::Vertex*>& seedVertices,
		  const std::vector<xAOD::EgammaParameters::ConversionType>& seedVertexType,
		  const egammaRec *egRec) const;

  /** Does the cluster match a conversion vertex track with the seed? */
  bool MatchesVtxTrack(const std::vector<const xAOD::TrackParticle*>& seedVertexTracks,
		  const egammaRec *egRec) const;

  /////////////////////////////////////////////////////////////////////
  //internal variables
  /** @brief Key for input egammaRec container */
  SG::ReadHandleKey<EgammaRecContainer> m_inputEgammaRecContainerKey;
  /** @brief Key for output egammaRec container */
  SG::WriteHandleKey<EgammaRecContainer> m_photonSuperRecCollectionKey;
  /** @brief Key for output clusters */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputPhotonSuperClustersKey;

  /** @brief Tool to retrieve the conversions*/
  ToolHandle<IEMConversionBuilder>             m_conversionBuilder;

  // options for how to build superclusters
  bool m_addClustersInWindow; //!< add the topoclusters in window
  bool m_addClustersMatchingVtx; //!< add the topoclusters matching conversion vertex
  bool m_useOnlyLeadingVertex; //!< use only the leading vertex for matching
  bool m_useOnlySi; //!< use only vertices/tracks with silicon tracks
  bool m_addClustersMatchingVtxTracks; //!< add the topoclusters matching conversion vertex tracks
  bool m_useOnlyLeadingTrack; //!< use only the leading track for matching

  /** @brief private member flag to do the conversion building and matching */
  bool         m_doConversions;

};

#endif
