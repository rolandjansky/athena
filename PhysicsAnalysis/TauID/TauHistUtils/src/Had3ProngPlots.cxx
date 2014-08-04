/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/Had3ProngPlots.h"

namespace Tau{

	Had3ProngPlots::Had3ProngPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):PlotBase(pParent, sDir),
  m_oGeneralTauPlots(this, "", sTauJetContainerName),
	m_sTauJetContainerName(sTauJetContainerName){	
	}
	

void Had3ProngPlots::initializePlots(){

  //  m_oGeneralTauPlots.initialize();
  m_tauCoreFrac =    Book1D("CentFrac",m_sTauJetContainerName + " Tau CentFrac; CentFrac; # Taus",40,0.,2.);
  m_tauEoverPTrk =   Book1D("EoverPTrk",m_sTauJetContainerName + " Tau E over pLeadTrk; EoverP; # Taus",50,0.,4.);
  m_tauTrkAvgDist =  Book1D("TrkAvgDist",m_sTauJetContainerName + " Tau track avg dist; TrkAvgDist; # Taus",10,-0.2,.8);
  m_tauDRMax =       Book1D("DRMax",m_sTauJetContainerName + " Tau DR Max track-seed; DRMax; # Taus",20,0.,0.5);		  
  m_tauSflight =     Book1D("Sflight",m_sTauJetContainerName + " Tau flight sign. ; Sflight; # Taus",100,-10.,20.);
  m_tauMtrks =       Book1D("Mtracks",m_sTauJetContainerName + " Tau tracks mass ; Mtrks; # Taus",100,-10.,20.);
}

void Had3ProngPlots::fill(const xAOD::TauJet& tau) {
  m_oGeneralTauPlots.fill(tau);

  float avariable=0;

  bool test = false;

  test=tau.detail(xAOD::TauJetParameters::centFrac, avariable);
  if(test)     m_tauCoreFrac->Fill(avariable,1.);
 
  test=tau.detail(xAOD::TauJetParameters::etOverPtLeadTrk, avariable);
  if(test) m_tauEoverPTrk->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::trkAvgDist, avariable);
  if (test) m_tauTrkAvgDist->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::dRmax, avariable);
  if (test) m_tauDRMax->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::trFlightPathSig, avariable);
  if (test) m_tauSflight->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::massTrkSys, avariable);
  if (test) m_tauMtrks->Fill(avariable/1000.,1.);

}

}
