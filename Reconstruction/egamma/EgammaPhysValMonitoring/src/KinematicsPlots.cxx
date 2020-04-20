/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "KinematicsPlots.h"

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
  
  void KinematicsPlots::fill(const xAOD::IParticle& part){
    et->Fill(part.pt()*0.001); //MeV to GeV conversion, change that to something more official
    eta->Fill(part.eta());
    phi->Fill(part.phi());
    etvseta->Fill(part.eta(),part.pt()*0.001);
  }

}
