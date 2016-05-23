/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_PHOTONSUPERCLUSTERBUILDER_H
#define EGAMMATOOLS_PHOTONSUPERCLUSTERBUILDER_H

// INCLUDE HEADER FILES:
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "egammaInterfaces/IphotonSuperClusterBuilder.h"

//Fwd declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODEgamma/EgammaEnums.h"

class photonSuperClusterBuilder : public AthAlgTool,  virtual public IphotonSuperClusterBuilder {

 public:

  //Constructor/destructor.
  photonSuperClusterBuilder(const std::string& type,
			    const std::string& name,
			    const IInterface* parent);

  photonSuperClusterBuilder();
  ~photonSuperClusterBuilder();
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
			       std::vector<bool>& isUsed) const;

  /** Is clus in window center around ref? */
  bool MatchesInWindow(const xAOD::CaloCluster *ref,
		       const xAOD::CaloCluster *clus) const;

  /** Does the cluster share conversion vertex? */
  bool MatchesVtx(const std::vector<const xAOD::Vertex*>& seedVertices,
		  const std::vector<xAOD::EgammaParameters::ConversionType>& seedVertexType,
		  const egammaRec *egRec) const;

  /** Does the cluster match a conversion vertex track with the seed? */
  bool MatchesVtxTrack(const std::vector<const xAOD::TrackParticle*>& seedVertexTracks,
		  const egammaRec *egRec) const;


  StatusCode AddEMCellsToCluster(xAOD::CaloCluster* self,
				 const xAOD::CaloCluster* ref);

  /////////////////////////////////////////////////////////////////////
  //internal variables
  float m_EtThresholdCut;
  float m_emFracCut;
  std::string m_inputEgammaRecContainerName;
  // std::string m_inputClusterContainerName; // let's try just going via egammaRecs
  std::string m_photonSuperRecCollectionName;
  std::string m_outputPhotonSuperClusters;

  // options for how to build superclusters
  bool m_addClustersInWindow; //!< add the topoclusters in window
  bool m_addClustersMatchingVtx; //!< add the topoclusters matching conversion vertex
  bool m_useOnlyLeadingVertex; //!< use only the leading vertex for matching
  bool m_useOnlySi; //!< use only vertices/tracks with silicon tracks
  bool m_addClustersMatchingVtxTracks; //!< add the topoclusters matching conversion vertex tracks
  bool m_useOnlyLeadingTrack; //!< use only the leading track for matching
  // bool m_addAllCellsInWindow; //!< add all the cells in window (not implemented yet)

  /** @brief Size of window in eta */
  int   m_delEtaCells;
  /** @brief Size of window in phi */
  int   m_delPhiCells;

  // these are calculated values, based on above
  float m_delEta; //!< half of window size, converted to units of eta
  float m_delPhi; //!< half of window size, converted to units of phi

  //clusters added to seed clusters.
  //int m_n3x5Clusters, m_nExtrapClusters;

};

#endif
