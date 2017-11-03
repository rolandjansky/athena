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
#include "egammaInterfaces/IEMConversionBuilder.h"

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
  SG::ReadHandleKey<EgammaRecContainer> m_inputEgammaRecContainerKey {this,
      "InputEgammaRecContainerName", "egammaRecCollection",
      "input egammaRec container"};

  /** @brief Key for output egammaRec container */
  SG::WriteHandleKey<EgammaRecContainer> m_photonSuperRecCollectionKey {this,
      "SuperPhotonRecCollectionName", "PhotonSuperRecCollection",
      "output egammaRec container"};

  /** @brief Key for output clusters */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputPhotonSuperClustersKey {this,
      "SuperClusterCollestionName", "PhotonSuperClusters",
      "output calo cluster container"};

  /** @brief Tool to retrieve the conversions*/
  ToolHandle<IEMConversionBuilder> m_conversionBuilder {this,
      "ConversionBuilderTool", "EMConversionBuilder",
      "Tool that matches conversion vertices to egammaRecs"};

  // options for how to build superclusters
  /** @brief add the topoclusters in window */
  Gaudi::Property<bool> m_addClustersInWindow {this,
      "AddClustersInWindow", true, "add the topoclusters in window"};

  /** @brief add the topoclusters matching conversion vertex */
  Gaudi::Property<bool>  m_addClustersMatchingVtx {this,
      "AddClustersMatchingVtx", true, 
      "add the topoclusters matching conversion vertex"};

  /** @brief use only the leading vertex for matching */
  Gaudi::Property<bool>  m_useOnlyLeadingVertex {this,
      "UseOnlyLeadingVertex", true, 
      "use only the leading vertex for matching"};

  /** @brief use only vertices/tracks with silicon tracks */
  Gaudi::Property<bool>  m_useOnlySi {this, "UseOnlySi", true, 
      "use only vertices/tracks with silicon tracks for adding sec. clusters (Mix not considered Si)"};
 
  /** @brief add the topoclusters matching conversion vertex tracks */
  Gaudi::Property<bool>  m_addClustersMatchingVtxTracks {this, 
      "AddClustrsMatchingVtxTracks", true, 
      "add the topoclusters matching conversion vertex tracks"};

  /** @brief use only the leading track for matching */
  Gaudi::Property<bool>  m_useOnlyLeadingTrack {this, 
      "UseOnlyLeadingTrack", true, 
      "use only the leading track for matching"}; 

  /** @brief private member flag to do the conversion building and matching */
  Gaudi::Property<bool> m_doConversions {this, "doConversions", true,
      "Boolean to do conversion matching"};

};

#endif
