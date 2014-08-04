/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/Had1ProngPlots.h"

namespace Tau{

	Had1ProngPlots::Had1ProngPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):PlotBase(pParent, sDir),
	m_oGeneralTauPlots(this, "", sTauJetContainerName),
	m_sTauJetContainerName(sTauJetContainerName){	
	}
	

void Had1ProngPlots::initializePlots(){

  //  m_oGeneralTauPlots.initialize();
	m_tauCoreFrac =    Book1D("CoreFrac",m_sTauJetContainerName + " Tau CoreFrac; CoreFrac; # Taus",40,0.,2.);
	m_tauEoverPTrk =   Book1D("EoverPTrk",m_sTauJetContainerName + " Tau E over pLeadTrk; EoverP; # Taus",50,0.,4.);
	m_tauTrkAvgDist =  Book1D("TrkAvgDist",m_sTauJetContainerName + " Tau track avg dist; TrkAvgDist; # Taus",10,-0.2,.8);
	m_tauIpSig =       Book1D("IpSig",m_sTauJetContainerName + " Tau lead track IP signif. ; IpSig; # Taus",30,-15.,15.);

}

void Had1ProngPlots::fill(const xAOD::TauJet& tau) {
  m_oGeneralTauPlots.fill(tau);

  float avariable=0;

  bool test = false;

  test=tau.detail(xAOD::TauJetParameters::centFrac, avariable);
  if(test)     m_tauCoreFrac->Fill(avariable,1.);
 
  test=tau.detail(xAOD::TauJetParameters::etOverPtLeadTrk, avariable);
  if(test) m_tauEoverPTrk->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::trkAvgDist, avariable);
  if (test) m_tauTrkAvgDist->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::ipSigLeadTrk, avariable);
  if (test) m_tauIpSig->Fill(avariable,1.);

}

}
