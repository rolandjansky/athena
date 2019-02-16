/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "egammaInterfaces/IEMTrackMatchBuilder.h"

class electronSuperClusterBuilder : public egammaSuperClusterBuilder {

 public:

  //Constructor/destructor.
  electronSuperClusterBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  //Tool standard routines.
  StatusCode initialize() override final;
  StatusCode finalize() override final;
  StatusCode execute() override final;

 private:

  bool MatchSameTrack(const egammaRec *seed,
		      const egammaRec *sec) const;

  const std::vector<std::size_t> SearchForSecondaryClusters(const size_t i,
							    const EgammaRecContainer*,
							    std::vector<bool>& isUsed);
  
  bool PassesSimpleBremSearch(const xAOD::CaloCluster *seed,
			      const xAOD::CaloCluster *sec,
			      float perigeeExtrapEta,
			      float perigeeExtrapPhi) const;
  
  
  /////////////////////////////////////////////////////////////////////
  
  /** @brief Size of maximum search window in eta */
  Gaudi::Property<int> m_maxDelEtaCells {this, 
      "MaxWindowDelEtaCells", 5,
      "Size of maximum search window in eta"};
  float m_maxDelEta;

  Gaudi::Property<float> m_bremExtrapMatchDelEta {this,
      "BremExtrapDelEtaCut",  0.05,
      "maximum DelEta for brem search"};

  Gaudi::Property<float> m_secEOverPCut {this,
      "BremSearchEOverPCut",  1.5,
      "E/p requirement when doing brem search"};
 
  /** @brief Size of maximum search window in phi */
  Gaudi::Property<int> m_maxDelPhiCells {this,
      "MaxWindowDelPhiCells", 12,
      "Size of maximum search window in phi"};
  float m_maxDelPhi;

  Gaudi::Property<float> m_bremExtrapMatchDelPhi {this,
      "BremExtrapDelPhiCut",  0.1,
      "maximum DelPhi for brem search"};

  //Keep track of # of 3x5 and brem point
  //clusters added to seed clusters.
  int m_nWindowClusters;
  int m_nExtraClusters;
  int m_nSameTrackClusters;
  int m_nSimpleBremSearchClusters;

  Gaudi::Property<std::size_t> m_numberOfSiHits {this, 
      "NumberOfReqSiHits", 4, "Number of required silicon hits for electrons"};

  /** @brief Key for input egammaRec container */
  SG::ReadHandleKey<EgammaRecContainer> m_inputEgammaRecContainerKey {this,
      "InputEgammaRecContainerName", "egammaRecCollection",
      "input egammaRec container"};
      
  /** @brief Key for output egammaRec container */
  SG::WriteHandleKey<EgammaRecContainer> m_electronSuperRecCollectionKey {this,
      "SuperElectronRecCollectionName", "ElectronSuperRecCollection",
      "output egammaRec container"};
  
  /** @brief Key for output clusters */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputElectronSuperClustersKey {this,
      "SuperClusterCollestionName", "ElectronSuperClusters",
      "output calo cluster container"};

  /** @brief Tool to perform track matching*/
  ToolHandle<IEMTrackMatchBuilder> m_trackMatchBuilder {this,
      "TrackMatchBuilderTool", "EMTrackMatchBuilder",
      "Tool that matches tracks to egammaRecs"};

  /** @brief private member flag to do the track matching */
  Gaudi::Property<bool> m_doTrackMatching {this, "doTrackMatching", true,
      "Boolean to do track matching"};

};

#endif
