/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ClusterValidationPlots.h"

ClusterValidationPlots::ClusterValidationPlots(PlotBase* pParent, std::string sDir,  std::string sClusterContainerName) : PlotBase(pParent, sDir),m_ClusterPlots(this,"PFO_JetETMiss/",sClusterContainerName),m_ClusterMomentPlots(this,"PFO_JetETMiss/",sClusterContainerName)
{}

void ClusterValidationPlots::fill(const xAOD::CaloCluster& theCluster){
  m_ClusterPlots.fill(theCluster);
  m_ClusterMomentPlots.fill(theCluster);
}


