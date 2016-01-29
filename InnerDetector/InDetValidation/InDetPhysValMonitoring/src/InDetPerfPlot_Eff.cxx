/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_Eff.h"
#include "TrkValHistUtils/EfficiencyPlots.h"
#include "TrkValHistUtils/EfficiencyPurityCalculator.h"
#include "xAODTruth/TruthParticle.h"

InDetPerfPlot_Eff::InDetPerfPlot_Eff(InDetPlotBase* pParent, const std::string & sDir, std::string particleName):InDetPlotBase(pParent, sDir),
  m_pDenomPlots(this, sDir+"Denom", particleName),
  m_pNumPlots(this, sDir+"Num", particleName)
{
  // sDir should be unique for each instance to avoid ROOT memory nonsense
  m_sParticleType = particleName;
}


void 
InDetPerfPlot_Eff::initializePlots() {

  book(m_eff_pt_Numerator, "eff_pt_Numerator");
  book(m_eff_pt_Denominator, "eff_pt_Denominator");
  book(m_eff_eta_Numerator, "eff_eta_Numerator");
  book(m_eff_eta_Denominator, "eff_eta_Denominator");
  book(m_eff_phi_Numerator, "eff_phi_Numerator");
  book(m_eff_phi_Denominator, "eff_phi_Denominator");
  book(m_eff_d0_Numerator, "eff_d0_Numerator");
  book(m_eff_d0_Denominator, "eff_d0_Denominator");
  book(m_eff_z0_Numerator, "eff_z0_Numerator");
  book(m_eff_z0_Denominator, "eff_z0_Denominator");
  book(m_eff_R_Numerator, "eff_R_Numerator");
  book(m_eff_R_Denominator, "eff_R_Denominator");
  book(m_eff_Z_Numerator, "eff_Z_Numerator");
  book(m_eff_Z_Denominator, "eff_Z_Denominator");

  book(m_eff_vs_pt, "eff_vs_pt");
  book(m_eff_vs_eta, "eff_vs_eta");
  book(m_eff_vs_phi, "eff_vs_phi");
  book(m_eff_vs_d0, "eff_vs_d0");
  book(m_eff_vs_z0, "eff_vs_z0");
  book(m_eff_vs_R, "eff_vs_R");
  book(m_eff_vs_Z, "eff_vs_Z");

  book(m_eff_pt_Numerator_st, "eff_pt_Numerator_st");
  book(m_eff_eta_Numerator_st, "eff_eta_Numerator_st");
  book(m_eff_phi_Numerator_st, "eff_phi_Numerator_st");
  book(m_eff_d0_Numerator_st, "eff_d0_Numerator_st");
  book(m_eff_z0_Numerator_st, "eff_z0_Numerator_st");
  book(m_eff_R_Numerator_st, "eff_R_Numerator_st");
  book(m_eff_Z_Numerator_st, "eff_Z_Numerator_st");

  book(m_eff_vs_pt_st, "eff_vs_pt_st");
  book(m_eff_vs_eta_st, "eff_vs_eta_st");
  book(m_eff_vs_phi_st, "eff_vs_phi_st");
  book(m_eff_vs_d0_st, "eff_vs_d0_st");
  book(m_eff_vs_z0_st, "eff_vs_z0_st");
  book(m_eff_vs_R_st, "eff_vs_R_st");
  book(m_eff_vs_Z_st, "eff_vs_Z_st");


  const bool prependDirectory(false);

  // Replaced these Book1D calls since the resultant efficiency histograms ended up with an incorrect x-axis scale. Maybe there's a bug in the Book1D call.

  //  eff_eta  = Book1D("Eff_eta", m_pNumPlots.eta, "Eta Efficiency; truth eta; efficiency", prependDirectory);
  //  eff_phi  = Book1D("Eff_phi", m_pNumPlots.phi, "Phi Efficiency;truth Azimuthal Angle; efficiency", prependDirectory);
  //  eff_pt   = Book1D("Eff_pt" , m_pNumPlots.pt, "p_{T} Efficiency; truth Transverse Momentum [GeV]; efficiency", prependDirectory);
  //  eff_pti  = Book1D("Eff_pti" , m_pNumPlots.pt, "p_{T} integrated Efficiency; Integrated truth Transverse Momentum [GeV]; efficiency", prependDirectory);
  
  eff_eta  = Book1D("Eff_eta","Eta Efficiency; truth eta; efficiency", m_pNumPlots.eta->GetNbinsX(), m_pNumPlots.eta->GetXaxis()->GetXmin(), m_pNumPlots.eta->GetXaxis()->GetXmax(), prependDirectory);
  eff_phi  = Book1D("Eff_phi","Phi Efficiency;truth Azimuthal Angle; efficiency", m_pNumPlots.phi->GetNbinsX(), m_pNumPlots.phi->GetXaxis()->GetXmin(), m_pNumPlots.phi->GetXaxis()->GetXmax(), prependDirectory);
  eff_pt   = Book1D("Eff_pt" ,"p_{T} Efficiency; truth Transverse Momentum [GeV]; efficiency", m_pNumPlots.pt->GetNbinsX(), m_pNumPlots.pt->GetXaxis()->GetXmin(), m_pNumPlots.pt->GetXaxis()->GetXmax(), prependDirectory);
  eff_pti  = Book1D("Eff_pti","p_{T} integrated Efficiency; Integrated truth Transverse Momentum [GeV]; efficiency",m_pNumPlots.pt->GetNbinsX(), m_pNumPlots.pt->GetXaxis()->GetXmin(), m_pNumPlots.pt->GetXaxis()->GetXmax(), prependDirectory);
  
  eff_eta_pt  = Book2D("Eff_eta_pt", (TH2*)m_pNumPlots.eta_pt, m_sParticleType+" truth eta vs pt;"+m_sParticleType+" eta;"+m_sParticleType+" pt; efficiency", prependDirectory);
  eff_eta_phi = Book2D("Eff_eta_phi", (TH2*)m_pNumPlots.eta_phi, m_sParticleType+" truth eta vs phi;"+m_sParticleType+" truth eta;"+m_sParticleType+" truth phi; efficiency", prependDirectory);

}

void InDetPerfPlot_Eff::fillNumerator(const xAOD::IParticle& particle){
  m_pNumPlots.fill(particle);
}

void InDetPerfPlot_Eff::fillDenominator(const xAOD::IParticle& particle){
  m_pDenomPlots.fill(particle);
}

void InDetPerfPlot_Eff::fill(const xAOD::TruthParticle & truth, const bool isReconstructed, const bool isStandardTrack){
  const unsigned int RecoNum=(unsigned int)isReconstructed;
  const unsigned int StandardRecoNum=(unsigned int)isStandardTrack;
  //  constexpr double degreesPerRadian(180./M_PI);
  double pt = truth.pt()/1000.;
  double eta = truth.eta();
  double phi = truth.phi();
  //  double phi_degrees(phi * degreesPerRadian);
  double d0 = truth.auxdata<float>("d0");
  double z0 = truth.auxdata<float>("z0");
  double R = truth.auxdata<float>("prodR");
  double Z = truth.auxdata<float>("prodZ");

  m_eff_pt_Denominator->Fill(pt);
  m_eff_eta_Denominator->Fill(eta);
  m_eff_phi_Denominator->Fill(phi);
  m_eff_d0_Denominator->Fill(d0);
  m_eff_z0_Denominator->Fill(z0);
  m_eff_R_Denominator->Fill(R);
  m_eff_Z_Denominator->Fill(Z);

  if(isReconstructed){
    m_eff_pt_Numerator->Fill(pt);
    m_eff_eta_Numerator->Fill(eta);
    m_eff_phi_Numerator->Fill(phi);
    m_eff_d0_Numerator->Fill(d0);
    m_eff_z0_Numerator->Fill(z0);
    m_eff_R_Numerator->Fill(R);
    m_eff_Z_Numerator->Fill(Z);
  }
  m_eff_vs_pt->Fill(pt,RecoNum);
  m_eff_vs_eta->Fill(eta,RecoNum);
  m_eff_vs_phi->Fill(phi,RecoNum);
  m_eff_vs_d0->Fill(d0,RecoNum);
  m_eff_vs_z0->Fill(z0,RecoNum);
  m_eff_vs_R->Fill(R,RecoNum);
  m_eff_vs_Z->Fill(Z,RecoNum);

  if(isStandardTrack){
    m_eff_pt_Numerator_st->Fill(pt);
    m_eff_eta_Numerator_st->Fill(eta);
    m_eff_phi_Numerator_st->Fill(phi);
    m_eff_d0_Numerator_st->Fill(d0);
    m_eff_z0_Numerator_st->Fill(z0);
    m_eff_R_Numerator_st->Fill(R);
    m_eff_Z_Numerator_st->Fill(Z);
  }
  m_eff_vs_pt_st->Fill(pt,StandardRecoNum);
  m_eff_vs_eta_st->Fill(eta,StandardRecoNum);
  m_eff_vs_phi_st->Fill(phi,StandardRecoNum);
  m_eff_vs_d0_st->Fill(d0,StandardRecoNum);
  m_eff_vs_z0_st->Fill(z0,StandardRecoNum);
  m_eff_vs_R_st->Fill(R,StandardRecoNum);
  m_eff_vs_Z_st->Fill(Z,StandardRecoNum);
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

