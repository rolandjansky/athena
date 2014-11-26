/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronFrwdPlots.h"
#include "ElectronValidationPlots.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

namespace Egamma{

ElectronFrwdPlots::ElectronFrwdPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir),
												     m_oKinFrwdAllRecoPlots(this, "All/KinPlots/", "Reco Electron"),
												     m_oKinFrwdIsoRecoPlots(this, "Iso/KinPlots/", "Reco Electron"),
												     m_oKinFrwdTightPlots(this, "FrwdTight/KinPlots/", "FrwdTight Electron"),
												     m_oClustMomAllRecoPlots(this, "All/ClusMomentPlots/", sParticleType ),
												     m_oClustMomIsoRecoPlots(this, "Iso/ClusMomentPlots/", sParticleType ),
												     nParticles(0),
												     m_sParticleType(sParticleType)

{}	

void ElectronFrwdPlots::initializePlots(){
  nParticles = Book1D("n", "Number of"+ m_sParticleType + "s;#" + m_sParticleType + " electrons;Events", 15, 0., 15.);
}

  void ElectronFrwdPlots::fill(const xAOD::Electron& electron, bool isPrompt) {
 
 
  m_oKinFrwdAllRecoPlots.fill(electron);
  m_oClustMomAllRecoPlots.fill(electron);
   
  if(!isPrompt) return;

  m_oKinFrwdIsoRecoPlots.fill(electron);
  m_oClustMomIsoRecoPlots.fill(electron);
 
  bool val_tight=false;    
  if(electron.passSelection(val_tight, "Tight")) {
    m_oKinFrwdTightPlots.fill(electron);
  }


}

}
