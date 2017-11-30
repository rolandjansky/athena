/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/ClusterPlots.h"

namespace PFO {

  ClusterPlots::ClusterPlots(PlotBase* pParent, std::string sDir, std::string sClusterContainerName) : PlotBase(pParent, sDir), m_sClusterContainerName(sClusterContainerName){}

  void ClusterPlots::initializePlots(){

    m_Cluster_pt = Book1D("Cluster_Pt",m_sClusterContainerName + "_Pt (Entries/1 GeV)",300,-100.0,200.0);
    m_Cluster_eta = Book1D("Cluster_Eta",m_sClusterContainerName + "_Eta (Entries/0.1)",100,-5.0,5.0);
    m_Cluster_phi = Book1D("Cluster_Phi",m_sClusterContainerName + "_Phi (Entries/0.1)",64,-3.2,3.2);
    m_Cluster_m = Book1D("Cluster_m",m_sClusterContainerName + "_m (Entries/100 MeV)",100,0.0,10.0);

    m_Cluster_ptEM = Book1D("Cluster_PtEM",m_sClusterContainerName + "_PtEM (Entries/1 GeV)",300,-100.0,200.0);
    m_Cluster_etaEM = Book1D("Cluster_EtaEM",m_sClusterContainerName + "_EtaEM (Entries/0.1)",100,-5.0,5.0);
    m_Cluster_phiEM = Book1D("Cluster_PhiEM",m_sClusterContainerName + "_PhiEM (Entries/0.1)",64,-3.2,3.2);
    m_Cluster_mEM = Book1D("Cluster_mEM",m_sClusterContainerName + "_mEM (Entries/100 MeV)",100,0.0,10.0);

    m_Cluster_time = Book1D("Cluster_time",m_sClusterContainerName + "_time (Entries/1)",300,-200.0,100.0); 
    m_Cluster_clusterSize = Book1D("Cluster_clusterSize",m_sClusterContainerName + "_clusterSize (Entries/1)",20,0.0,20.0);

  }

  void ClusterPlots::fill(const xAOD::CaloCluster& Cluster){
    m_Cluster_pt->Fill(Cluster.pt()/1000.0);
    m_Cluster_eta->Fill(Cluster.eta());
    m_Cluster_phi->Fill(Cluster.phi());
    m_Cluster_m->Fill(Cluster.m()/1000.0);

    m_Cluster_ptEM->Fill((Cluster.rawE()/cosh(Cluster.rawEta()))/1000.0);
    m_Cluster_etaEM->Fill(Cluster.rawEta());
    m_Cluster_phiEM->Fill(Cluster.rawPhi());
    m_Cluster_mEM->Fill(Cluster.rawM()/1000.0);

    m_Cluster_time->Fill(Cluster.time());
    m_Cluster_clusterSize->Fill(Cluster.clusterSize());

  }

}
