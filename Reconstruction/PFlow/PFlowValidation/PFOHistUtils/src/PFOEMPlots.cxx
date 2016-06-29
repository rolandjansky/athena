/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOEMPlots.h"

namespace PFO {

  PFOEMPlots::PFOEMPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName) : PlotBase(pParent, sDir), m_sPFOContainerName(sPFOContainerName){
    m_PFO_ptEM = nullptr;
    m_PFO_etaEM = nullptr;
    m_PFO_phiEM = nullptr;
    m_PFO_mEM = nullptr;
}

  void PFOEMPlots::initializePlots(){

    m_PFO_ptEM = Book1D("PFO_PtEM",m_sPFOContainerName + "_PtEM (Entries/1 GeV)",30,-10.0,20.0);
    m_PFO_etaEM = Book1D("PFO_EtaEM",m_sPFOContainerName + "_EtaEM (Entries/0.1)",100,-5.0,5.0);
    m_PFO_phiEM = Book1D("PFO_PhiEM",m_sPFOContainerName + "_PhiEM (Entries/0.1)",64,-3.2,3.2);
    m_PFO_mEM = Book1D("PFO_mEM",m_sPFOContainerName + "_mEM (Entries/100 MeV)",10,0.0,0.5);

  }

  void PFOEMPlots::fill(const xAOD::PFO& PFO){
    m_PFO_ptEM->Fill(PFO.ptEM()/1000.0);
    m_PFO_etaEM->Fill(PFO.etaEM());
    m_PFO_phiEM->Fill(PFO.phiEM());
    m_PFO_mEM->Fill(PFO.mEM()/1000.0);
  }

}
