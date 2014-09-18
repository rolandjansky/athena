/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZeeValidation/ReconElectronsPlots.h"
#include "xAODEgamma/EgammaDefs.h"
	
using CLHEP::GeV;

namespace ZeeValidation{
  
  ReconElectronsPlots::ReconElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir),
  m_sParticleType(sParticleType)
  {} 

  const std::string ReconElectronsPlots::cLevelLabel[nLevels] = {
    "Reco",
    "OQ", 
    "Loose",
    "Medium",
    "Tight"
  };    
  
  void ReconElectronsPlots::initializePlots(){
    TH1::SetDefaultSumw2(kTRUE);    

    electron_n  = Book1D("NElectrons", "Number of Electrons; n ;Events", 20, 0., 20);
    photon_n = Book1D("NPhotons", "Number of Photons; n ;Events", 20, 0., 20);
    electron_author = Book1D("Author", "author of " + m_sParticleType +";author;Events ",10 ,0.0,10.0);
    nvtx = Book1D("Nvtx", "Nvtx distribution;n_{vtx};Events ", 30, 0, 30);
    mu = Book1D("Mu", "<#mu> distribution;<mu>;Events", 40, 0, 40);
    bcid = Book1D("BCID", "BCID distribution;BCID;Events", 3500, 0, 3500);
    track_n = Book1D("NTracks", "#Tracks on primary vtx;N_{tracks};Events", 100, 0, 200);
    pv_x = Book1D("Pv_x", "Primary Vertex X Position;x_{vtx};Events", 200, -.5, .5);
    pv_y = Book1D("Pv_y", "Primary Vertex Y Position;y_{vtx};Events", 200, -1.5, 1.5);
    pv_z = Book1D("Pv_z", "Primary Vertex Z Position;z_{vtx};Events", 200, -250, 250);

    for (int i = 0; i < nLevels; i++) {
      electron_pt[i]  = Book1D("Pt" + cLevelLabel[i], "P_{T} of "+ m_sParticleType + "; E_{T} (GeV);Events", 100, 20., 140.);
      electron_eta[i] = Book1D("Eta" + cLevelLabel[i], "#eta of " + m_sParticleType + ";#eta;Events", 100, -2.5, 2.5);
      electron_phi[i] = Book1D("Phi" + cLevelLabel[i], "#varphi of " + m_sParticleType + ";#varphi;Events", 100, -TMath::Pi(), TMath::Pi());
      electron_etavsphi[i] = Book2D("EtaVSPhi" + cLevelLabel[i], "#eta vs #varphi of " + m_sParticleType + ";#eta;#varphi", 100, -2.5, 2.5, 100, -TMath::Pi(), TMath::Pi());
    }

   for (int i = 0; i < nLevels-1; i++) {
      electron_eff_pt[i]  = Book1D("PtEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs P_{T} of "+ m_sParticleType + "; E_{T} (GeV);" + cLevelLabel[i+1] + "/" + cLevelLabel[0], 100, 20., 140.);
      electron_eff_eta[i] = Book1D("EtaEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #eta of " + m_sParticleType + ";#eta;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 100, -2.5, 2.5);
      electron_eff_phi[i] = Book1D("PhiEff" + cLevelLabel[i+1] + cLevelLabel[0], cLevelLabel[i+1] + " Efficiency vs #varphi of " + m_sParticleType + ";#varphi;"  + cLevelLabel[i+1] + "/" + cLevelLabel[0], 100, -TMath::Pi(), TMath::Pi());
    }

  }
  
  void ReconElectronsPlots::fill(const xAOD::EventInfo* eventInfo, const xAOD::VertexContainer* vertices){
    
    int nGoodVertices = 0;
    for (const auto* vtx : vertices -> stdcont()){
      if (vtx -> vertexType() == xAOD::VxType::PriVtx){
	pv_x -> Fill(vtx -> x());
	pv_y -> Fill(vtx -> y());
	pv_z -> Fill(vtx -> z());
	track_n ->Fill(vtx -> nTrackParticles());
      }
      if (vtx -> vertexType() == xAOD::VxType::NoVtx) continue;
      nGoodVertices++;
    }
    nvtx -> Fill(nGoodVertices);
    mu -> Fill(eventInfo -> averageInteractionsPerCrossing());
    bcid -> Fill(eventInfo -> bcid()); 
  }

  void ReconElectronsPlots::fill(const xAOD::Electron* electron, int level){
    electron_eta[level] -> Fill(electron -> eta());
    electron_etavsphi[level] -> Fill(electron -> eta(), electron -> phi());
  }

  void ReconElectronsPlots::fillinAcc(const xAOD::Electron* electron, int level){
    electron_pt[level] -> Fill(electron -> pt()/GeV);
    electron_phi[level] -> Fill(electron -> phi());
  }

  void ReconElectronsPlots::finalize() {

    for (int i = 0; i < nLevels-1; i++) {
      electron_eff_pt[i] -> Divide(electron_pt[i+1], electron_pt[0], 1, 1, "B");
      electron_eff_eta[i] -> Divide(electron_eta[i+1], electron_eta[0], 1, 1, "B");
      electron_eff_phi[i] -> Divide(electron_phi[i+1], electron_phi[0], 1, 1, "B");
      }
  }
  
}
