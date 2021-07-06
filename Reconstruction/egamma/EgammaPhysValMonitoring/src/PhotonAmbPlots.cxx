/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PhotonAmbPlots.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

namespace Egamma{


PhotonAmbPlots::PhotonAmbPlots(PlotBase* pParent, const std::string& sDir, const std::string& sParticleType):PlotBase(pParent, sDir), 
											       m_oKinAllPlots(this, "All/KinPlots/", "Reco " + sParticleType +" Photon"), 
											       m_oShowerShapesAllPlots(this, "All/ShowerShapesPlots/", sParticleType  +" Photon"),
											       m_sParticleType(sParticleType),
											       m_nParticles(nullptr),
											       m_nParticles_weighted(nullptr)
{}	

void PhotonAmbPlots::initializePlots(){
  m_nParticles = Book1D("n", "Number of "+ m_sParticleType + "s;#" + m_sParticleType + "s;Events", 15, 0., 15.);
  m_nParticles_weighted = Book1D("n_weighted", "Number of "+ m_sParticleType + "s;#" + m_sParticleType + "s;Events", 15, 0., 15.);
}

  void PhotonAmbPlots::fill(const xAOD::Photon& photon, const xAOD::EventInfo& eventInfo, bool /*isPrompt*/) const{
  //if(!isPrompt) ;//return;
  
  m_oKinAllPlots.fill(photon,eventInfo);
  m_oShowerShapesAllPlots.fill(photon,eventInfo);

  }
}
