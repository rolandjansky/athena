/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronValidationPlots.h"
#include "xAODEgamma/EgammaDefs.h"

ElectronValidationPlots::ElectronValidationPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
										      m_oCentralElecPlots(this,"Central/", "Central"),
										      m_oFrwdElecPlots(this, "Frwd/", "Forward"),
										      m_oTruthAllPlots(this, "Truth/All/", "Truth Electron All"),
										      m_oTruthAllIsoPlots(this, "Truth/All/Iso", "Truth Electron Prompt"),
										      m_oTruthIsoPlots(this, "Truth/Iso/", "Truth Electron Prompt"),
										      author(0),
										      res_et(0),
										      res_eta(0)
{}	

void ElectronValidationPlots::initializePlots(){

  author  = Book1D("author", "Electron  Author ; author;Events", 20, -0.5, 19.5);
  res_et  = BookTProfile("res_et"," IsoElectron;E_{T}^{truth}, [GeV];(E_{T} - E_{T}^{truth})/E_{T}^{truth}",100, 0., 200.);
  res_eta = BookTProfile("res_eta"," IsoElectron;#eta;(E_{T} - E_{T}^{truth})/E_{T}^{truth}",50, -2.5, 2.5);

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
