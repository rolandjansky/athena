/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CLUSTERPLOTS_H
#define CLUSTERPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODCaloEvent/CaloCluster.h"

namespace PFO {

  class ClusterPlots : public PlotBase {

  public:

     ClusterPlots(PlotBase *pParent, std::string sDir, std::string sClusterContainerName);

     void fill(const xAOD::CaloCluster& Cluster);

  private:
     TH1* m_Cluster_pt;
     TH1* m_Cluster_eta;
     TH1* m_Cluster_phi;
     TH1* m_Cluster_m;

     TH1* m_Cluster_ptEM;
     TH1* m_Cluster_etaEM;
     TH1* m_Cluster_phiEM;
     TH1* m_Cluster_mEM;

     TH1* m_Cluster_time;
     TH1* m_Cluster_clusterSize;

     TH1* m_Cluster_eta_lowpt;
     TH1* m_Cluster_pt_interval;
     TH1* m_Cluster_pt_zoom;

     TH1* m_Cluster_eSample_EM;
     TH1* m_Cluster_eSample_HAD;

     void initializePlots();
     std::string m_sClusterContainerName;

  };

}
#endif
