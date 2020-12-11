/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOPlots.h"

namespace PFO {

  PFOPlots::PFOPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName, std::string sFEContainerName) : PlotBase(pParent, sDir), m_sPFOContainerName(sPFOContainerName), m_sFEContainerName(sFEContainerName){
    m_PFO_pt = nullptr;
    m_PFO_eta = nullptr;
    m_PFO_phi = nullptr;
    m_PFO_m = nullptr;
    m_PFO_charge = nullptr;
  }

  void PFOPlots::initializePlots(){
    if(!m_sPFOContainerName.empty()){
      // book PFO histograms
    m_PFO_pt = Book1D("_Pt",m_sPFOContainerName + "_Pt (Entries/1 GeV)",30,-10.0,20.0);
    m_PFO_eta = Book1D("_Eta",m_sPFOContainerName + "_Eta (Entries/0.1)",100,-5.0,5.0);
    m_PFO_phi = Book1D("_Phi",m_sPFOContainerName + "_Phi (Entries/0.1)",64,-3.2,3.2);
    m_PFO_m = Book1D("_m",m_sPFOContainerName + "_m (Entries/100 MeV)",10,0.0,0.5);
    m_PFO_charge = Book1D("_charge",m_sPFOContainerName + "_charge (Entries/unit charge)",5,-2,2);
    
    m_PFO_pt_etaBinA = Book1D("_Pt_A",m_sPFOContainerName + "_Pt (Entries/1 GeV, |eta| < 1 )",30,-10.0,20.0);
    m_PFO_pt_etaBinB = Book1D("_Pt_B",m_sPFOContainerName + "_Pt (Entries/1 GeV,  1 <= |eta| < 2 )",30,-10.0,20.0);
    m_PFO_pt_etaBinC = Book1D("_Pt_C",m_sPFOContainerName + "_Pt (Entries/1 GeV, |eta| >= 2 )",30,-10.0,20.0);
    }
    if(!m_sFEContainerName.empty()){
      //book Flow Element histograms
      m_FE_pt = Book1D("_Pt",m_sFEContainerName + "_Pt (Entries/1 GeV)",30,-10.0,20.0);
      m_FE_eta = Book1D("_Eta",m_sFEContainerName + "_Eta (Entries/0.1)",100,-5.0,5.0);
      m_FE_phi = Book1D("_Phi",m_sFEContainerName + "_Phi (Entries/0.1)",64,-3.2,3.2);
      m_FE_m = Book1D("_m",m_sFEContainerName + "_m (Entries/100 MeV)",10,0.0,0.5);
      m_FE_charge = Book1D("_charge",m_sFEContainerName + "_charge (Entries/unit charge)",5,-2,2);
      
      m_FE_pt_etaBinA = Book1D("_Pt_A",m_sFEContainerName + "_Pt (Entries/1 GeV, |eta| < 1 )",30,-10.0,20.0);
      m_FE_pt_etaBinB = Book1D("_Pt_B",m_sFEContainerName + "_Pt (Entries/1 GeV,  1 <= |eta| < 2 )",30,-10.0,20.0);
      m_FE_pt_etaBinC = Book1D("_Pt_C",m_sFEContainerName + "_Pt (Entries/1 GeV, |eta| >= 2 )",30,-10.0,20.0);
      m_FE_eta_posE=Book1D("_EtaPosE",m_sFEContainerName+"_Eta (E>0) (Entries/0.1)",100,-5.0,5.0);
    }
  }

  void PFOPlots::fill(const xAOD::PFO& PFO){
    m_PFO_pt->Fill(PFO.pt()/1000.0);
    m_PFO_eta->Fill(PFO.eta());
    m_PFO_phi->Fill(PFO.phi());
    m_PFO_m->Fill(PFO.m()/1000.0);
    m_PFO_charge->Fill(PFO.charge());

    if (fabs(PFO.eta()) < 1)  m_PFO_pt_etaBinA->Fill(PFO.pt()/1000.0);
    else if (fabs(PFO.eta()) < 2) m_PFO_pt_etaBinB->Fill(PFO.pt()/1000.0);
    else m_PFO_pt_etaBinC->Fill(PFO.pt()/1000.0);
    
  }

  void PFOPlots::fill(const xAOD::FlowElement& FE){
    m_FE_pt->Fill(FE.pt()/1000.0);
    m_FE_eta->Fill(FE.eta());
    m_FE_phi->Fill(FE.phi());
    m_FE_m->Fill(FE.m()/1000.0);
    m_FE_charge->Fill(FE.charge());

    if (fabs(FE.eta()) < 1)  m_FE_pt_etaBinA->Fill(FE.pt()/1000.0);
    else if (fabs(FE.eta()) < 2) m_FE_pt_etaBinB->Fill(FE.pt()/1000.0);
    else m_FE_pt_etaBinC->Fill(FE.pt()/1000.0);
    // additional debug plot: Eta given FlowElem energy>0
    if (FE.e()>0){
      m_FE_eta_posE->Fill(FE.eta());
    }
  }

}
