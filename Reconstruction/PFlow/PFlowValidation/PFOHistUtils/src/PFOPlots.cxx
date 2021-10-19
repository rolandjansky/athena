/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOPlots.h"

namespace PFO {

  PFOPlots::PFOPlots(PlotBase* pParent, std::string sDir, std::string sFEContainerName) : PlotBase(pParent, sDir), m_sFEContainerName(sFEContainerName){
    m_FE_pt = nullptr;
    m_FE_eta = nullptr;
    m_FE_phi = nullptr;
    m_FE_m = nullptr;
    m_FE_charge = nullptr;

    m_FE_pt_low = nullptr;
    m_FE_pt_middle = nullptr;
    m_FE_pt_high = nullptr;

    m_FE_eta_posE = nullptr;

    m_FE_pt_etaBinA = nullptr;
    m_FE_pt_etaBinB = nullptr;
    m_FE_pt_etaBinC = nullptr;
  }

  void PFOPlots::initializePlots(){    
    if(!m_sFEContainerName.empty()){
      //book Flow Element histograms
      m_FE_pt = Book1D("_Pt",m_sFEContainerName + "_Pt (Entries/1 GeV)",30,-10.0,20.0);
      m_FE_eta = Book1D("_Eta",m_sFEContainerName + "_Eta (Entries/0.1)",100,-5.0,5.0);
      m_FE_phi = Book1D("_Phi",m_sFEContainerName + "_Phi (Entries/0.1)",64,-3.2,3.2);
      m_FE_m = Book1D("_m",m_sFEContainerName + "_m (Entries/100 MeV)",10,0.0,0.5);
      m_FE_charge = Book1D("_charge",m_sFEContainerName + "_charge (Entries/unit charge)",5,-2,2);

      m_FE_pt_low = Book1D("_Pt_low",m_sFEContainerName + "_Pt_low (Entries/1 GeV, pT <= 5 GeV)",15,-10.0,5.0);
      m_FE_pt_middle = Book1D("_Pt_middle",m_sFEContainerName + "_Pt_middle (Entries/1 GeV), 20 > pT > 5 GeV",45,5.0,50.0);
      m_FE_pt_high = Book1D("_Pt_high",m_sFEContainerName + "_Pt_high (Entries/1 GeV), Pt > 20 GeV",45,5.0,50.0);
      
      m_FE_pt_etaBinA = Book1D("_Pt_A",m_sFEContainerName + "_Pt (Entries/1 GeV, |eta| < 1 )",30,-10.0,20.0);
      m_FE_pt_etaBinB = Book1D("_Pt_B",m_sFEContainerName + "_Pt (Entries/1 GeV,  1 <= |eta| < 2 )",30,-10.0,20.0);
      m_FE_pt_etaBinC = Book1D("_Pt_C",m_sFEContainerName + "_Pt (Entries/1 GeV, |eta| >= 2 )",30,-10.0,20.0);
      m_FE_eta_posE=Book1D("_EtaPosE",m_sFEContainerName+"_Eta (E>0) (Entries/0.1)",100,-5.0,5.0);
    }
  }

  void PFOPlots::fill(const xAOD::FlowElement& FE, const xAOD::EventInfo& eventInfo){
    m_FE_pt->Fill(FE.pt()/1000.0,eventInfo.beamSpotWeight());
    m_FE_eta->Fill(FE.eta(),eventInfo.beamSpotWeight());
    m_FE_phi->Fill(FE.phi(),eventInfo.beamSpotWeight());
    m_FE_m->Fill(FE.m()/1000.0,eventInfo.beamSpotWeight());
    m_FE_charge->Fill(FE.charge(),eventInfo.beamSpotWeight());

    if (FE.pt()/1000.0 <= 5) m_FE_pt_low->Fill(FE.pt()/1000.0,eventInfo.beamSpotWeight());
    else if (20 >= FE.pt()/1000.0 && FE.pt()/1000.0 > 4) m_FE_pt_middle->Fill(FE.pt()/1000.0,eventInfo.beamSpotWeight());
    else m_FE_pt_high->Fill(FE.pt()/1000.0,eventInfo.beamSpotWeight());

    if (fabs(FE.eta()) < 1)  m_FE_pt_etaBinA->Fill(FE.pt()/1000.0,eventInfo.beamSpotWeight());
    else if (fabs(FE.eta()) < 2) m_FE_pt_etaBinB->Fill(FE.pt()/1000.0,eventInfo.beamSpotWeight());
    else m_FE_pt_etaBinC->Fill(FE.pt()/1000.0,eventInfo.beamSpotWeight());
    // additional debug plot: Eta given FlowElem energy>0
    if (FE.e()>0){
      m_FE_eta_posE->Fill(FE.eta(),eventInfo.beamSpotWeight());
    }
  }

}
