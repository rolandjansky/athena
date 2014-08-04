/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/EVetoPlots.h"

namespace Tau{

  EVetoPlots::EVetoPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):PlotBase(pParent, sDir),
  m_sTauJetContainerName(sTauJetContainerName){	
}
	

void EVetoPlots::initializePlots(){

  m_HadRadius = Book1D("HadRadius",m_sTauJetContainerName + " Had Radius; HadRadius; # Part",20,0,2.);
  m_EMRadius  = Book1D("EMRadius",m_sTauJetContainerName + " EM Radius; EMRadius; # Part",20,0,2.);
  m_IsoFrac   = Book1D("IsoFrac",m_sTauJetContainerName + " Iso Frac; Iso Frac; # Part",20,0,1.);
  m_CentFrac  = Book1D("CentFrac",m_sTauJetContainerName + " Cent Frac; Cent Frac; # Part",20,0,1.);

}

void EVetoPlots::fill(const xAOD::TauJet& tau) {

  float avariable=0;

  bool test = false;

  test=tau.detail(xAOD::TauJetParameters::hadRadius, avariable);
  if(test)     m_HadRadius->Fill(avariable,1.);
 
  test=tau.detail(xAOD::TauJetParameters::EMRadius, avariable);
  if(test) m_EMRadius->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::isolFrac, avariable);
  if (test) m_IsoFrac->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::centFrac, avariable);
  if (test) m_CentFrac->Fill(avariable,1.);

}

}
