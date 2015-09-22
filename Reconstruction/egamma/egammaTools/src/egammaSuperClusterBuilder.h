/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EGAMMASUPERCLUSTERBUILDER_H
#define EGAMMATOOLS_EGAMMASUPERCLUSTERBUILDER_H

// INCLUDE HEADER FILES:
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"

#include "egammaInterfaces/IegammaSuperClusterBuilder.h"

#include "CaloGeoHelpers/CaloPhiRange.h"
#include "GaudiKernel/ToolHandle.h" 
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "egammaRecEvent/egammaRecContainer.h"

#include "TrkCaloExtension/CaloExtensionHelpers.h"

class StoreGateSvc;
class IegammaTopoClusterMap;
class egammaTopoClusterMap;
class CaloCellContainer;
class CaloCellDetPos;
class CaloClusterCellLinkContainer;

namespace Trk
{
  class INeutralParticleParameterCalculator;
  class IParticleCaloExtensionTool;
}

class egammaSuperClusterBuilder : public AthAlgTool,  virtual public IegammaSuperClusterBuilder {

 public:

  //Constructor/destructor.
  egammaSuperClusterBuilder(const std::string& type,
			    const std::string& name,
			    const IInterface* parent);

  egammaSuperClusterBuilder();
  ~egammaSuperClusterBuilder();
  
  //Tool standard routines.
  StatusCode initialize();
  StatusCode finalize();

  //New StoreGate-based execute routine. Will use previously-established
  //track-cluster matches and make a supercluster egammaRec
  //container in StoreGate.
  StatusCode execute();

  //Method to set container of brem point TSOS.
  inline void setTPAndTSOS(TPAndTSOS *tpAndTSOS) {
    m_tpAndTSOS = tpAndTSOS;
  }
  
 private:

  CaloPhiRange m_phiHelper;
  
  std::string m_inputClusterContainerName;

  bool m_recordFullClusters;

  //Superclustering routines.
  xAOD::CaloCluster* AddTopoClusters (const std::vector<const xAOD::CaloCluster*>&);

  const CaloCellContainer *m_cellcoll;

  void fillPositionsInCalo(xAOD::CaloCluster *cluster);
  CaloCellDetPos *m_caloCellDetPos;

  //CaloClusterCellLinkContainer *m_cellLinks;

  CaloCellContainer *m_caloCellContainer;
  int                m_cellStart;

  //For registering supercluster container.
  StoreGateSvc*   m_storeGate;

  //For saving brem points as decorators on superclusters.
  std::vector<double> bpRetainedEnFrac, bpExtrapEta, bpExtrapPhi;
  TPAndTSOS *m_tpAndTSOS;
  bool       m_useBremFinder;

  void GetBremExtrapolations(const xAOD::CaloCluster*,
			     const xAOD::CaloClusterContainer*,
			     EgammaRecContainer*);
  bool OverlapsABremPoint   (const xAOD::CaloCluster*);

  bool ClustersMatchSameTrack(const xAOD::CaloCluster*,
			      const xAOD::CaloCluster*,
			      EgammaRecContainer*);
  
  std::vector<const xAOD::CaloCluster*> SearchForSecondaryClusters(const xAOD::CaloCluster*,
								   const xAOD::CaloClusterContainer*,
								   EgammaRecContainer*);

  float m_delEta;
  float m_delPhi;

  /** @brief Size of window in eta */
  int   m_delEtaCells;

  /** @brief Size of window in phi */
  int   m_delPhiCells;

  bool  MatchesInWindow(const xAOD::CaloCluster *ref,
			const xAOD::CaloCluster *clus);

  std::list<const xAOD::CaloCluster*>     usedClusters;
  std::vector<const xAOD::CaloCluster*>   secondaryClusters;
  std::vector<const xAOD::TrackParticle*> secondaryTracks;

  IChronoStatSvc*                m_timingProfile;

  inline bool ClusterIsUsed(const xAOD::CaloCluster *clus) {
    if (std::find(usedClusters.begin(), usedClusters.end(), clus) != usedClusters.end()) return true;
    return false;
  }

  ElementLink< xAOD::TrackParticleContainer > GetTrackParticleLink(const xAOD::TrackParticleContainer*,
								   const xAOD::TrackParticle*);

  bool getHackEtaPhiAtCalo (const  Trk::TrackParameters*, 
                            float *etaAtCalo,
                            float *phiAtCalo
			    ) const;
  ToolHandle<Trk::IParticleCaloExtensionTool>     m_defaultParticleCaloExtensionTool;

};

#endif
