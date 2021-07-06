/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ClusterValidationPlots.h"

ClusterValidationPlots::ClusterValidationPlots(PlotBase* pParent, std::string sDir, SG::ReadHandleKey<xAOD::CaloClusterContainer>& sClusterContainerName) : PlotBase(pParent, sDir),m_ClusterPlots(this,"PFO_JetETMiss/",sClusterContainerName),m_ClusterMomentPlots(this,"PFO_JetETMiss/",sClusterContainerName)
{}

void ClusterValidationPlots::fill(const xAOD::CaloCluster& theCluster, const xAOD::EventInfo& eventInfo){
  m_ClusterPlots.fill(theCluster, eventInfo);
  m_ClusterMomentPlots.fill(theCluster, eventInfo);
}


