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
#include "egammaInterfaces/IegammaShowerShape.h"

#include "CaloGeoHelpers/CaloPhiRange.h"
#include "GaudiKernel/ToolHandle.h" 
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "egammaRecEvent/egammaRecContainer.h"

class StoreGateSvc;
class IBeamCondSvc;
class IEMExtrapolationTools;
class IegammaTopoClusterMap;
class egammaTopoClusterMap;
class IChronoStatSvc;
class CaloCellContainer;
class CaloCellDetPos;
class CaloClusterCellLinkContainer;

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
  
 private:

  //Superclustering routines.
  xAOD::CaloCluster* AddTopoClusters (const std::vector<const xAOD::CaloCluster*>&);

  float CalculateShowerShape(std::string name, const xAOD::CaloCluster *cluster);
  const CaloCellContainer *m_cellcoll;

  void fillPositionsInCalo(xAOD::CaloCluster *cluster);
  CaloCellDetPos *m_caloCellDetPos;

  //CaloClusterCellLinkContainer *m_cellLinks;


  CaloCellContainer *m_caloCellContainer;
  int                m_cellStart;

  //For registering supercluster container.
  StoreGateSvc*   m_storeGate;

  std::vector<const xAOD::CaloCluster*> SearchForSecondaryClusters(const xAOD::CaloCluster*,
								   const xAOD::CaloClusterContainer*,
								   EgammaRecContainer*);

  std::list<const xAOD::CaloCluster*>     usedClusters;
  std::vector<const xAOD::CaloCluster*>   secondaryClusters;
  std::vector<const xAOD::TrackParticle*> secondaryTracks;

  IChronoStatSvc*                m_timingProfile;
  ToolHandle<IegammaShowerShape> m_ShowerShape;

  inline bool ClusterIsUsed(const xAOD::CaloCluster *clus) {
    if (std::find(usedClusters.begin(), usedClusters.end(), clus) != usedClusters.end()) return true;
    return false;
  }

  ElementLink< xAOD::TrackParticleContainer > GetTrackParticleLink(const xAOD::TrackParticleContainer*,
								   const xAOD::TrackParticle*);

};

#endif
