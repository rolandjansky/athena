/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ResoTriggerMuonPlots.h"

namespace Muon{

ResoTriggerMuonPlots::ResoTriggerMuonPlots(PlotBase* pParent, std::string sDir, std::string sType):
  PlotBase(pParent, sDir),
  Res_pT(NULL),
  Res_eta(NULL),
  Res_phi(NULL),
  Res_pT_vs_pT(NULL),
  m_sType(sType)
{
  pt_slices = {"0","30","70","100"};
  etaBins= {-3., -2.5, -2.4, -1.918, -1.623, -1.348, -1.2329, -1.1479, -1.05, -0.908, -0.791, -0.652, -0.476, -0.324, -0.132, 0, 0.132, 0.324, 0.476, 0.652, 0.791, 0.908, 1.05, 1.1479, 1.2329, 1.348, 1.623, 1.918, 2.4, 2.5, 3.};

  Res_pT     = Book1D("Res"+m_sType+"_pT","Res"+m_sType+"_pT;(pT-RECOpT)/RECOpT;Entries",200,-0.5,0.5);
  Res_eta    = Book1D("Res"+m_sType+"_eta","Res"+m_sType+"_eta;eta-RECOeta;Entries",200,-0.02,0.02);
  Res_phi    = Book1D("Res"+m_sType+"_phi","Res"+m_sType+"_phi;phi-RECOphi;Entries",200,-0.005,0.005);

  Res_pT_vs_pT  = Book2D("Res"+m_sType+"_pT_vs_pT","Res"+m_sType+"_pT vs pT;pT [GeV];(pT-RECOpT)/RECOpT",
			   100,0., 500.,  100,-0.5,0.5);
  Res_pT_vs_eta.clear();
  Res_pT_vs_phi.clear();
  for (unsigned int i=0; i<pt_slices.size()-1;i++){
     
     Res_pT_vs_eta.push_back(Book2D("Res"+m_sType+"_pT_vs_eta_from"+pt_slices[i]+"_to"+pt_slices[i+1]+"GeV","Res"+m_sType+"_pT vs eta from "+pt_slices[i]+" GeV to "+pt_slices[i+1]+" GeV;eta;eta-RECOeta",etaBins.size()-1 ,&etaBins[0], 100,-0.5,0.5));
     Res_pT_vs_phi.push_back(Book2D("Res"+m_sType+"_pT_vs_phi_from"+pt_slices[i]+"_to"+pt_slices[i+1]+"GeV","Res"+m_sType+"_pT vs phi from "+pt_slices[i]+" GeV to "+pt_slices[i+1]+" GeV;phi;phi-RECOphi",96,-3.14159,3.14159, 100,-0.5,0.5));			
  }
				

}

void ResoTriggerMuonPlots::fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu) {
  //fill plots Res_pt,eta,phi
  float pt = 0.001*Recomu.pt();
  float eta = Recomu.eta();
  float phi = Recomu.phi();
  float respt = (Trigmu.pt() - Recomu.pt())/Recomu.pt();


  Res_pT->Fill(respt);
  Res_eta->Fill(Trigmu.eta() - eta);
  Res_phi->Fill(Trigmu.phi() - phi);
     
  Res_pT_vs_pT->Fill(pt,respt);  

  for (unsigned int i=0; i<pt_slices.size()-1;i++){
    
    float pt_min = atof(pt_slices[i].c_str());
    float pt_max = atof(pt_slices[i+1].c_str());
    if (pt>=pt_min && pt<pt_max) {
       Res_pT_vs_eta[i]->Fill(eta,respt);
       Res_pT_vs_phi[i]->Fill(phi,respt);
    }
  }
}

} // closing namespace Muon
