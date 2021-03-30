/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CLUSTERVALIDATIONPLOTS_H
#define CLUSTERVALIDATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "PFOHistUtils/ClusterPlots.h"
#include "PFOHistUtils/ClusterMomentPlots.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODCaloEvent/CaloClusterContainer.h"



class ClusterValidationPlots : public PlotBase {

 public:

  /** Standard Constructor */
  ClusterValidationPlots(PlotBase* pParent, std::string sDir, SG::ReadHandleKey<xAOD::CaloClusterContainer>& sClusterContainerName);

  /** fill the histograms up */
  void fill(const xAOD::CaloCluster& theCluster, const xAOD::EventInfo& eventInfo);

 private:
  /** 4-vector histograms */
  PFO::ClusterPlots m_ClusterPlots;
  /** Cluster moment histograms */
  PFO::ClusterMomentPlots m_ClusterMomentPlots;

};
#endif
