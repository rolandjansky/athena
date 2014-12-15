/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_Eff.h"

InDetPerfPlot_Eff::InDetPerfPlot_Eff(PlotBase* pParent, const std::string & sDir, std::string particleName):PlotBase(pParent, sDir),
  m_pDenomPlots(this, sDir+"Denom", particleName),
  m_pNumPlots(this, sDir+"Num", particleName)
{
  // sDir should be unique for each instance to avoid ROOT memory nonsense
  m_sParticleType = particleName;
}


void 
InDetPerfPlot_Eff::initializePlots() {

  const bool prependDirectory(false);

   eff_eta  = Book1D("Eff_eta", m_pNumPlots.eta, "Eta Efficiency; truth eta; efficiency", prependDirectory);
   eff_phi  = Book1D("Eff_phi", m_pNumPlots.phi, "Phi Efficiency;truth Azimuthal Angle; efficiency", prependDirectory);
   eff_pt   = Book1D("Eff_pt" , m_pNumPlots.pt, "p_{T} Efficiency; truth Transverse Momentum [GeV]; efficiency", prependDirectory);
   eff_pti  = Book1D("Eff_pti" , m_pNumPlots.pt, "p_{T} integrated Efficiency; Integrated truth Transverse Momentum [GeV]; efficiency", prependDirectory);

   eff_eta_pt  = Book2D("Eff_eta_pt", (TH2*)m_pNumPlots.eta_pt, m_sParticleType+" truth eta vs pt;"+m_sParticleType+" eta;"+m_sParticleType+" pt; efficiency", prependDirectory);
   eff_eta_phi = Book2D("Eff_eta_phi", (TH2*)m_pNumPlots.eta_phi, m_sParticleType+" truth eta vs phi;"+m_sParticleType+" truth eta;"+m_sParticleType+" truth phi; efficiency", prependDirectory);

}

void InDetPerfPlot_Eff::fillNumerator(const xAOD::IParticle& particle){
  m_pNumPlots.fill(particle);
}

void InDetPerfPlot_Eff::fillDenominator(const xAOD::IParticle& particle){
  m_pDenomPlots.fill(particle);
}

void InDetPerfPlot_Eff::finalizePlots() {
  EfficiencyPurityCalculator calc;
  calc.calculateEfficiency(m_pDenomPlots.eta, m_pNumPlots.eta, eff_eta);
  calc.calculateEfficiency(m_pDenomPlots.phi, m_pNumPlots.phi, eff_phi);
  calc.calculateEfficiency(m_pDenomPlots.pt, m_pNumPlots.pt, eff_pt);
  calc.calculateIntegrated(m_pDenomPlots.pt, m_pNumPlots.pt, eff_pti, 1, EfficiencyPurityCalculator::kX);

  eff_eta_pt->Sumw2();
  eff_eta_pt->Divide( m_pNumPlots.eta_pt, m_pDenomPlots.eta_pt, 1, 1, "B" );
  eff_eta_phi->Sumw2();
  eff_eta_phi->Divide( m_pNumPlots.eta_phi, m_pDenomPlots.eta_phi, 1, 1, "B" );

}

