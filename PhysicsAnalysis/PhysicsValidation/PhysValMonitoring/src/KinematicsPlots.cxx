/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "KinematicsPlots.h"

using CLHEP::GeV;

namespace PhysVal{

KinematicsPlots::KinematicsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir), 
m_sParticleType(sParticleType)
{}	
  
void KinematicsPlots::initializePlots(){
  m_lead = nullptr;
  m_npart = 0;

   n  = Book1D("n", "Number of "+ m_sParticleType +"; n ;Events", 50, 0., 50);

   et  = Book1D("et", "E_{T} of "+ m_sParticleType +"; E_{T} (GeV) ;Events", 200, 0., 200);
   eta = Book1D("eta", "#eta of " + m_sParticleType +";#eta; Events ", 200,-5,5.);
   phi = Book1D("phi", "#varphi of " + m_sParticleType +";#varphi;Events ",128 ,-3.2,3.2);

   lead_et  = Book1D("lead_et", "E_{T} of lead "+ m_sParticleType +"; E_{T} (GeV) ;Events", 200, 0., 200);
   lead_eta = Book1D("lead_eta", "#eta of lead " + m_sParticleType +";#eta; Events ", 200,-5,5.);
   lead_phi = Book1D("lead_phi", "#varphi of lead " + m_sParticleType +";#varphi;Events ",128 ,-3.2,3.2);
}

void KinematicsPlots::fill(const xAOD::IParticle* part){

  et->Fill(part->pt()/GeV);
  eta->Fill(part->eta());
  phi->Fill(part->phi());

  if (m_lead == nullptr) m_lead = part;

  if (part->pt() > m_lead->pt()) {
     m_lead = part;
   }

  ++m_npart;  
}

void KinematicsPlots::fill(){
  n->Fill(m_npart);

  if (m_lead != nullptr) {
    lead_et->Fill(m_lead->pt()/GeV);
    lead_eta->Fill(m_lead->eta());
    lead_phi->Fill(m_lead->phi());
  }

  m_lead = nullptr;

}

void KinematicsPlots::initializeEvent() {
  m_lead = nullptr;
  m_npart = 0;

}

}

