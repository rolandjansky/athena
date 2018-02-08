/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_ELECTRONSUPERCLUSTERBUILDER_H
#define EGAMMATOOLS_ELECTRONSUPERCLUSTERBUILDER_H

// INCLUDE HEADER FILES:
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "egammaInterfaces/IelectronSuperClusterBuilder.h"
#include "egammaSuperClusterBuilder.h"

//Fwd declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "egammaRecEvent/egammaRecContainer.h"

class electronSuperClusterBuilder : public egammaSuperClusterBuilder, 
  virtual public IelectronSuperClusterBuilder {

 public:

  //Constructor/destructor.
  electronSuperClusterBuilder(const std::string& type,
			      const std::string& name,
			      const IInterface* parent);

  electronSuperClusterBuilder();
  //Tool standard routines.
  StatusCode initialize();
  StatusCode finalize();
  //New StoreGate-based execute routine. Will use previously-established
  //track-cluster matches and make a supercluster egammaRec
  //container in StoreGate.
  StatusCode execute();

  // Execute for HLT. Works the same as offline execute but with prepared containers and no StoreGate interaction.
  StatusCode hltExecute(const EgammaRecContainer *egammaRecs,
                        EgammaRecContainer *newEgammaRecs,
                        xAOD::CaloClusterContainer *outputClusterContainer);

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
  
  StatusCode executeImpl(const EgammaRecContainer *egammaRecs,
                         EgammaRecContainer *newEgammaRecs,
                         xAOD::CaloClusterContainer *outputClusterContainer);

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
  std::string m_inputEgammaRecContainerName;
  std::string m_electronSuperRecCollectionName;
  std::string m_outputElectronSuperClusters;


};

#endif
