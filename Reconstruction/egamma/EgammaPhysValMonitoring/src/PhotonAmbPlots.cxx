/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhotonAmbPlots.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

namespace Egamma{


PhotonAmbPlots::PhotonAmbPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir), 
											       m_oKinAllPlots(this, "All/KinPlots/", "Reco " + sParticleType +" Photon"), 
											       m_oShowerShapesAllPlots(this, "All/ShowerShapesPlots/", sParticleType  +" Photon"),
											       m_sParticleType(sParticleType),
											       m_nParticles(0)
{}	

void PhotonAmbPlots::initializePlots(){
  m_nParticles = Book1D("n", "Number of"+ m_sParticleType + "s;#" + m_sParticleType + "s;Events", 15, 0., 15.);
}



  void PhotonAmbPlots::fill(const xAOD::Photon& photon, bool /*isPrompt*/){
  // if(!isPrompt) ;//return;
  
  m_oKinAllPlots.fill(photon);
  m_oShowerShapesAllPlots.fill(photon);


}
}
