/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "ParamPlots.h"

namespace Tau{

ParamPlots::ParamPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType)
   : PlotBase(pParent, sDir),
     eta(nullptr),
     phi(nullptr),
     pt(nullptr),
     eta_phi(nullptr),
     eta_pt(nullptr),
     m_sParticleType(std::move(sParticleType))
{
}

ParamPlots::~ParamPlots()
{
}

void ParamPlots::initializePlots()
{
   pt  = Book1D("pt" , m_sParticleType+" pt;" +m_sParticleType+" Transverse Momentum [GeV];Entries / 1 GeV",25,0.,200);
   eta = Book1D("eta", m_sParticleType+" eta;"+m_sParticleType+" Pseudo-Rapidity;Entries / 0.05", 32, -3.2, 3.2);
   phi = Book1D("phi", m_sParticleType+" phi;"+m_sParticleType+" Azimuthal Angle;Entries / 0.05", 32, -3.2, 3.2);
   eta_pt  = Book2D("eta_pt",m_sParticleType+" eta vs pt;"+m_sParticleType+" eta;"+m_sParticleType+" pt;Entries.0.05/1 GeV",32,-3.2,3.2,25,0.,200);
   eta_phi = Book2D("eta_phi",m_sParticleType+" eta vs phi;"+m_sParticleType+" eta;"+m_sParticleType+" phi;Entries.0.05/0.5",32,-3.2,3.2,32,-3.2,3.2);
}

   
void ParamPlots::fill(const xAOD::IParticle& tau, float weight)
{
  pt->Fill(tau.pt()/1000., weight);
  eta->Fill(tau.eta(), weight);
  phi->Fill(tau.phi(), weight);
  eta_pt->Fill(tau.eta(), tau.pt()/1000., weight);
  eta_phi->Fill(tau.eta(), tau.phi(), weight);
}

}

