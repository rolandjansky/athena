/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/ParamPlots.h"

namespace Tau{

ParamPlots::ParamPlots(PlotBase* pParent, std::string sDir, std::string sParticleType)
   : PlotBase(pParent, sDir),
     eta(nullptr),
     phi(nullptr),
     pt(nullptr),
     eta_phi(nullptr),
     eta_pt(nullptr),
     m_sParticleType(sParticleType)
{
}

ParamPlots::~ParamPlots()
{
}

void ParamPlots::initializePlots()
{
   pt  = Book1D("pt" , m_sParticleType+" pt;" +m_sParticleType+" Transverse Momentum [GeV];Entries / 1 GeV",200,0.,200);
   eta = Book1D("eta", m_sParticleType+" eta;"+m_sParticleType+" Pseudo-Rapidity;Entries / 0.05", 32, -3.2, 3.2);
   phi = Book1D("phi", m_sParticleType+" phi;"+m_sParticleType+" Azimuthal Angle;Entries / 0.05", 32, -3.2, 3.2);
   eta_pt  = Book2D("eta_pt",m_sParticleType+" eta vs pt;"+m_sParticleType+" eta;"+m_sParticleType+" pt;Entries.0.05/1 GeV",32,-3.2,3.2,200,0.,200);
   eta_phi = Book2D("eta_phi_test",m_sParticleType+" eta vs phi;"+m_sParticleType+" eta;"+m_sParticleType+" phi;Entries.0.05/0.5",32,-3.2,3.2,32,-3.2,3.2);
}

   
void ParamPlots::fill(const xAOD::IParticle& tau)
{
   pt->Fill(tau.pt()*0.001);
   eta->Fill(tau.eta());
   phi->Fill(tau.phi());
   eta_pt->Fill(tau.eta(),tau.pt()*0.001);
   eta_phi->Fill(tau.eta(),tau.phi());
}

}

