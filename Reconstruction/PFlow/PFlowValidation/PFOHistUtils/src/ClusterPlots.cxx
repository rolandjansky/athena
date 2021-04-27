/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/ClusterPlots.h"


namespace PFO {

  ClusterPlots::ClusterPlots(PlotBase* pParent, std::string sDir, SG::ReadHandleKey<xAOD::CaloClusterContainer>& sClusterContainerName) : PlotBase(pParent, sDir), m_sClusterContainerName(sClusterContainerName){}

  void ClusterPlots::initializePlots(){

    m_Cluster_pt = Book1D("Cluster_Pt",m_sClusterContainerName.key() + "_Pt (Entries/1 GeV)",300,-100.0,200.0);
    m_Cluster_eta = Book1D("Cluster_Eta",m_sClusterContainerName.key() + "_Eta (Entries/0.1)",100,-5.0,5.0);
    m_Cluster_phi = Book1D("Cluster_Phi",m_sClusterContainerName.key() + "_Phi (Entries/0.1)",64,-3.2,3.2);
    m_Cluster_m = Book1D("Cluster_m",m_sClusterContainerName.key() + "_m (Entries/100 MeV)",100,0.0,10.0);

    m_Cluster_ptEM = Book1D("Cluster_PtEM",m_sClusterContainerName.key() + "_PtEM (Entries/1 GeV)",300,-100.0,200.0);
    m_Cluster_etaEM = Book1D("Cluster_EtaEM",m_sClusterContainerName.key() + "_EtaEM (Entries/0.1)",100,-5.0,5.0);
    m_Cluster_phiEM = Book1D("Cluster_PhiEM",m_sClusterContainerName.key() + "_PhiEM (Entries/0.1)",64,-3.2,3.2);
    m_Cluster_mEM = Book1D("Cluster_mEM",m_sClusterContainerName.key() + "_mEM (Entries/100 MeV)",100,0.0,10.0);

    m_Cluster_time = Book1D("Cluster_time",m_sClusterContainerName.key() + "_time (Entries)",30,-200.0,100.0); 
    m_Cluster_clusterSize = Book1D("Cluster_clusterSize",m_sClusterContainerName.key() + "_clusterSize (Entries)",20,0.0,20.0);

    m_Cluster_eta_lowpt = Book1D("Cluster_Eta_lowPt",m_sClusterContainerName.key() + "_Eta_lowPt (Entries/0.1)",100,-5.0,5.0);
    m_Cluster_pt_interval = Book1D("Cluster_Pt_interval",m_sClusterContainerName.key() + "_Pt_interval (Entries/1 GeV)", 980, 20.0, 1000.0);
    m_Cluster_pt_zoom = Book1D("Cluster_Pt_zoom",m_sClusterContainerName.key() + "_Pt_zomm (Entries/0.1 GeV)",3000,-100.0,200.0);

    m_Cluster_eSample_EM = Book1D("Cluster_eSample_EM",m_sClusterContainerName.key() +"_eSample_EM (Entries/1 GeV) ",300,-100.0,200.0);
    m_Cluster_eSample_HAD = Book1D("Cluster_eSample_HAD",m_sClusterContainerName.key() +"_eSample_HAD (Entries/1 GeV) ",300,-100.0,200.0);

  }

  void ClusterPlots::fill(const xAOD::CaloCluster& Cluster, const xAOD::EventInfo& eventInfo){
    m_Cluster_pt->Fill(Cluster.pt()/1000.0,eventInfo.beamSpotWeight());
    m_Cluster_eta->Fill(Cluster.eta(),eventInfo.beamSpotWeight());
    m_Cluster_phi->Fill(Cluster.phi(),eventInfo.beamSpotWeight());
    m_Cluster_m->Fill(Cluster.m()/1000.0,eventInfo.beamSpotWeight());

    m_Cluster_ptEM->Fill((Cluster.rawE()/cosh(Cluster.rawEta()))/1000.0,eventInfo.beamSpotWeight());
    m_Cluster_etaEM->Fill(Cluster.rawEta(),eventInfo.beamSpotWeight());
    m_Cluster_phiEM->Fill(Cluster.rawPhi(),eventInfo.beamSpotWeight());
    m_Cluster_mEM->Fill(Cluster.rawM()/1000.0,eventInfo.beamSpotWeight());

    m_Cluster_time->Fill(Cluster.time(),eventInfo.beamSpotWeight());
    m_Cluster_clusterSize->Fill(Cluster.clusterSize(),eventInfo.beamSpotWeight());

    if (Cluster.pt()/1000.0 > -20 && Cluster.pt()/1000.0 < 20){
        m_Cluster_eta_lowpt->Fill(Cluster.eta(),eventInfo.beamSpotWeight());
    }
    m_Cluster_pt_interval->Fill(Cluster.pt()/1000.0,eventInfo.beamSpotWeight());
    m_Cluster_pt_zoom->Fill(Cluster.pt()/1000.0,eventInfo.beamSpotWeight());

    float eSample_EM;
   eSample_EM = Cluster.eSample(CaloSampling::PreSamplerB) + Cluster.eSample(CaloSampling::EMB1) + Cluster.eSample(CaloSampling::EMB2) + Cluster.eSample(CaloSampling::EMB3) + Cluster.eSample(CaloSampling::PreSamplerE) + Cluster.eSample(CaloSampling::EME1) + Cluster.eSample(CaloSampling::EME2) + Cluster.eSample(CaloSampling::EME3) + Cluster.eSample(CaloSampling::FCAL0);
   m_Cluster_eSample_EM->Fill((double)eSample_EM/1000.0,eventInfo.beamSpotWeight());
   
   
   
   float eSample_HAD;
   eSample_HAD = Cluster.eSample(CaloSampling::TileBar0) + Cluster.eSample(CaloSampling::TileBar1) + Cluster.eSample(CaloSampling::TileBar2) + Cluster.eSample(CaloSampling::HEC0) + Cluster.eSample(CaloSampling::HEC1) + Cluster.eSample(CaloSampling::HEC2) + Cluster.eSample(CaloSampling::HEC3) + Cluster.eSample(CaloSampling::FCAL1) + Cluster.eSample(CaloSampling::FCAL2);
   m_Cluster_eSample_HAD->Fill((double)eSample_HAD/1000.0,eventInfo.beamSpotWeight());

  }

}
