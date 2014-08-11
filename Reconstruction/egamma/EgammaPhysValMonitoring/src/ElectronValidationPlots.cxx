/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronValidationPlots.h"
#include "xAODEgamma/EgammaDefs.h"

ElectronValidationPlots::ElectronValidationPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
m_oCentralElecPlots(this,"Central/", "Central"),
m_oFrwdElecPlots(this, "Frwd/", "Forward"),
m_oTruthPlots(this, "Truth/", "Truth Electron")
{}	

void ElectronValidationPlots::initializePlots(){
  author = Book1D("author", "Electron  Author ; author;Events", 20, -0.5, 19.5);
}
 
void ElectronValidationPlots::fill(const xAOD::Electron& electron, bool isPrompt) {

  author->Fill(electron.author());
  
  if(electron.author()&xAOD::EgammaParameters::AuthorElectron||
     electron.author(xAOD::EgammaParameters::AuthorAmbiguous))  {
    m_oCentralElecPlots.fill(electron,isPrompt);
  } 
  else if(electron.author()&xAOD::EgammaParameters::AuthorFwdElectron) {
    m_oFrwdElecPlots.fill(electron, isPrompt);
  } 
}
