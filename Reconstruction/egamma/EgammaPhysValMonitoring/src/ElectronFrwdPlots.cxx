/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronFrwdPlots.h"
#include "ElectronValidationPlots.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

namespace Egamma{

ElectronFrwdPlots::ElectronFrwdPlots(PlotBase* pParent, const std::string& sDir, const std::string& sParticleType):PlotBase(pParent, sDir),
												     m_oKinFrwdAllRecoPlots(this, "All/KinPlots/", "Reco Electron"),
												     m_oKinFrwdIsoRecoPlots(this, "Iso/KinPlots/", "Reco Electron"),
												     m_oKinFrwdTightPlots(this, "FrwdTight/KinPlots/", "FrwdTight Electron"),
												     m_oClustMomAllRecoPlots(this, "All/ClusMomentPlots/", sParticleType ),
												     m_oClustMomIsoRecoPlots(this, "Iso/ClusMomentPlots/", sParticleType ),
												     nParticles(nullptr),
												     nParticles_weighted(nullptr),
												     m_sParticleType(sParticleType)

{}	

void ElectronFrwdPlots::initializePlots(){
  nParticles = Book1D("n", "Number of"+ m_sParticleType + "s;#" + m_sParticleType + " electrons;Events", 15, 0., 15.);
  nParticles_weighted = Book1D("n_weighted", "Number of"+ m_sParticleType + "s;#" + m_sParticleType + " electrons;Events", 15, 0., 15.);
}

  void ElectronFrwdPlots::fill(const xAOD::Electron& electron, const xAOD::EventInfo& eventInfo, bool isPrompt) const {
 
 
  m_oKinFrwdAllRecoPlots.fill(electron,eventInfo);
  m_oClustMomAllRecoPlots.fill(electron,eventInfo);
   
  if(!isPrompt) return;

  m_oKinFrwdIsoRecoPlots.fill(electron,eventInfo);
  m_oClustMomIsoRecoPlots.fill(electron,eventInfo);
 
  bool val_tight=false;    
  if(electron.passSelection(val_tight, "Tight")) {
    m_oKinFrwdTightPlots.fill(electron,eventInfo);
  }


}

}
