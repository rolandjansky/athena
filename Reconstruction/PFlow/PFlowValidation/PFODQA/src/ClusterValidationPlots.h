/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CLUSTERVALIDATIONPLOTS_H
#define CLUSTERVALIDATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "PFOHistUtils/ClusterPlots.h"
#include "PFOHistUtils/ClusterMomentPlots.h"
#include "xAODCaloEvent/CaloCluster.h"

class ClusterValidationPlots : public PlotBase {

 public:

  /** Standard Constructor */
  ClusterValidationPlots(PlotBase* pParent, std::string sDir, std::string sClusterContainerName);

  /** fill the histograms up */
  void fill(const xAOD::CaloCluster& theCluster);

 private:
  /** 4-vector histograms */
  PFO::ClusterPlots m_ClusterPlots;
  /** Cluster moment histograms */
  PFO::ClusterMomentPlots m_ClusterMomentPlots;

};
#endif
