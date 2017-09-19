/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_ELECTRONSUPERCLUSTERBUILDER_H
#define EGAMMAALGS_ELECTRONSUPERCLUSTERBUILDER_H

// INCLUDE HEADER FILES:
#include "egammaSuperClusterBuilder.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

//Fwd declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class IEMTrackMatchBuilder;

class electronSuperClusterBuilder : public egammaSuperClusterBuilder {

 public:

  //Constructor/destructor.
  electronSuperClusterBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  //Tool standard routines.
  StatusCode initialize();
  StatusCode finalize();
  //New StoreGate-based execute routine. Will use previously-established
  //track-cluster matches and make a supercluster egammaRec
  //container in StoreGate.
  StatusCode execute();

 private:

  bool MatchSameTrack(const egammaRec *seed,
		      const egammaRec *sec) const;

  const std::vector<std::size_t> SearchForSecondaryClusters(const size_t i,
							    const EgammaRecContainer*,
							    std::vector<bool>& isUsed);
  
  bool PassesSimpleBremSearch(const xAOD::CaloCluster *seed,
			      const xAOD::CaloCluster *sec,
			      float perigeeExtrapEta,
			      float perigeeExtrapPhi,
			      float seedEOverP) const;
  
  
  /////////////////////////////////////////////////////////////////////
  
  /** @brief Size of maximum search window in eta */
  int   m_maxDelEtaCells;
  float m_maxDelEta;
  float m_bremExtrapMatchDelEta;
  float m_secEOverPCut;  
  /** @brief Size of maximum search window in eta */
  int   m_maxDelPhiCells;
  float m_maxDelPhi;
  float m_bremExtrapMatchDelPhi;
  //Keep track of # of 3x5 and brem point
  //clusters added to seed clusters.
  int m_nWindowClusters;
  int m_nExtraClusters;
  int m_nSameTrackClusters;
  int m_nSimpleBremSearchClusters;

  float m_numberOfSiHits;

  /** @brief Key for input egammaRec container */
  SG::ReadHandleKey<EgammaRecContainer> m_inputEgammaRecContainerKey;
  /** @brief Key for output egammaRec container */
  SG::WriteHandleKey<EgammaRecContainer> m_electronSuperRecCollectionKey;
  /** @brief Key for output clusters */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputElectronSuperClustersKey;

  /** @brief Tool to perform track matching*/
  ToolHandle<IEMTrackMatchBuilder>             m_trackMatchBuilder;

  /** @brief private member flag to do the TrackMatching (and conversion building)*/
  bool         m_doTrackMatching;

};

#endif
