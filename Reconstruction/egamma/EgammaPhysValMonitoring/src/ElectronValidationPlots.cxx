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
										      res_eta(0),
										      pt_ratio(0),
										      matrix(0)

{}	

void ElectronValidationPlots::initializePlots(){

  author  = Book1D("author", "Electron  Author ; author;Events", 20, -0.5, 19.5);
  res_et  = BookTProfile("res_et"," IsoElectron;E_{T}^{Truth}, [GeV];(E_{T} - E_{T}^{Truth})/E_{T}^{Truth}",100, 0., 200.);
  res_eta = BookTProfile("res_eta"," IsoElectron;#eta;(E_{T} - E_{T}^{Truth})/E_{T}^{Truth}",50, -2.5, 2.5);
  pt_ratio= Book1D("p_{T}_ratio","IsoElectron;p_{T}^{Truth}, [GeV];p_{T}^{reco}/p_{T}^{Truth}",200,0.5,200.5);
  matrix = Book2D("matrix","reco vs truth pt",200,0.,200.,200,0.,200.);

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
