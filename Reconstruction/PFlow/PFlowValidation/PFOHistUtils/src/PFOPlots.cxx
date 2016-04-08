/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOPlots.h"

namespace PFO {

  PFOPlots::PFOPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName) : PlotBase(pParent, sDir), m_sPFOContainerName(sPFOContainerName){
    m_PFO_pt = nullptr;
    m_PFO_eta = nullptr;
    m_PFO_phi = nullptr;
    m_PFO_m = nullptr;
}

  void PFOPlots::initializePlots(){

    m_PFO_pt = Book1D("PFO_Pt",m_sPFOContainerName + "_Pt (Entries/1 GeV)",300,-100.0,200.0);
    m_PFO_eta = Book1D("PFO_Eta",m_sPFOContainerName + "_Eta (Entries/0.1)",100,-5.0,5.0);
    m_PFO_phi = Book1D("PFO_Phi",m_sPFOContainerName + "_Phi (Entries/0.1)",64,-3.2,3.2);
    m_PFO_m = Book1D("PFO_m",m_sPFOContainerName + "_m (Entries/100 MeV)",100,0.0,10.0);

  }

  void PFOPlots::fill(const xAOD::PFO& PFO){
    m_PFO_pt->Fill(PFO.pt()/1000.0);
    m_PFO_eta->Fill(PFO.eta());
    m_PFO_phi->Fill(PFO.phi());
    m_PFO_m->Fill(PFO.m()/1000.0);
  }

}
