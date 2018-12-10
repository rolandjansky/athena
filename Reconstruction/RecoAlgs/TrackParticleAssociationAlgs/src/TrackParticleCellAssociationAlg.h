/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLEASSOCIATIONALGS_TRACKPARTICLECELLASSOCIATIONALG_H
#define TRACKPARTICLEASSOCIATIONALGS_TRACKPARTICLECELLASSOCIATIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include <string>
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODAssociations/TrackParticleClusterAssociation.h"
#include "xAODAssociations/TrackParticleClusterAssociationContainer.h"
#include "xAODAssociations/TrackParticleClusterAssociationAuxContainer.h"


namespace Rec {
  class IParticleCaloCellAssociationTool;
}

class TrackParticleCellAssociationAlg : public AthAlgorithm
{
 public:
  TrackParticleCellAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~TrackParticleCellAssociationAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  ToolHandle<Rec::IParticleCaloCellAssociationTool> m_caloCellAssociationTool;

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackParticleCollectionName{this,"TrackParticleContainerName", 
      "InDetTrackParticles","SG Key of track particle container"};

  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_clusterContainerName{this,"ClusterContainerName", 
      "InDetTrackParticlesAssociatedClusters","SG Key of output cluster container"};

  SG::WriteHandleKey<CaloClusterCellLinkContainer> m_clusterCellLinkName{this,"CaloClusterCellLinkName",
      "InDetTrackParticlesAssociatedClusters_links","SG Key of out CaloCluserCellLInkContainer"};


  SG::WriteHandleKey< xAOD::TrackParticleClusterAssociationContainer> m_associationContainerName{this,"AssociationContainerName",
      "InDetTrackParticlesClusterAssociations","SG Key of association container"};

  double m_ptCut;
};


#endif
