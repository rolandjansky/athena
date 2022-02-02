/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "KinematicsPlots.h"

using CLHEP::GeV;

namespace Egamma{

  KinematicsPlots::KinematicsPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType):PlotBase(pParent, sDir), 
												   m_sParticleType(std::move(sParticleType)),
												   et(nullptr),
												   eta(nullptr),
												   phi(nullptr),
												   etvseta(nullptr)
  {}

  void KinematicsPlots::initializePlots(){
    et  = Book1D("et", "E_{T} of "+ m_sParticleType +"; E_{T} (GeV); Entries", 200, 0., 200);
    eta = Book1D("eta", "#eta of " + m_sParticleType +";#eta; Entries", 200,-5,5.);
    phi = Book1D("phi", "#varphi of " + m_sParticleType +";#varphi; Entries",128 ,-3.2,3.2);
    etvseta = Book2D("etvseta", "E_{T} vs #eta of "+ m_sParticleType +"; #eta ; E_{T} (GeV) ", 1000,-5.,5., 200, 0., 200);
  }
  
  void KinematicsPlots::fill(const xAOD::IParticle& part, const xAOD::EventInfo& eventInfo) const{

    float weight = 1.;
    weight = eventInfo.beamSpotWeight();
    
    et->Fill(part.pt()/GeV,weight);
    eta->Fill(part.eta(),weight);
    phi->Fill(part.phi(),weight);
    etvseta->Fill(part.eta(),part.pt()/GeV,weight);
  }

}
