/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/GeneralTauPlots.h"

namespace Tau{

	GeneralTauPlots::GeneralTauPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):PlotBase(pParent, sDir),
	m_oParamPlots(this, "", sTauJetContainerName),
	m_sTauJetContainerName(sTauJetContainerName){	
	}
	

void GeneralTauPlots::initializePlots(){

  //  m_oParamPlots.initialize();
	m_tauCharge      = Book1D("Charge",m_sTauJetContainerName + " Tau charge; charge; # Taus",7,-3.,4.);
	m_tauNCoreTracks = Book1D("NCoreTracks",m_sTauJetContainerName + " Tau n core tracks; nCore; # Taus",10,0.,10.);
	m_tauNWideTracks = Book1D("NWideTracks",m_sTauJetContainerName + " Tau n wide tracks; nWide; # Taus",10,0.,10.);

}

void GeneralTauPlots::fill(const xAOD::TauJet& tau) {
  m_oParamPlots.fill(tau);
  m_tauCharge->Fill(tau.charge(), 1.); 
  m_tauNCoreTracks->Fill(tau.nTracks(), 1.);
  m_tauNWideTracks->Fill(tau.nWideTracks(), 1.);
}

}
