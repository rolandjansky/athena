/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_Eff.h"
// #include "TrkValHistUtils/EfficiencyPurityCalculator.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "InDetPhysValMonitoringUtilities.h"
#include <cmath>
using namespace IDPVM;

InDetPerfPlot_Eff::InDetPerfPlot_Eff(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_trackeff_vs_eta{},
  m_trackeff_vs_pt{},
  m_trackeff_vs_phi{},
  m_trackeff_vs_d0{},
  m_trackeff_vs_z0{},
  m_trackeff_vs_R{},
  m_trackeff_vs_Z{},
  m_trackeff_vs_prodR{},
  m_trackeff_vs_prodZ{},
  m_eff_vs_eta_of_daughters{},
  m_eff_vs_theta_of_daughters{},
  m_eff_vs_theta_tan_of_daughters{},
  m_eff_vs_theta_cotan_of_daughters{},
  m_eff_vs_phi_of_daughters{},
  m_eff_vs_phi_sin_of_daughters{},
  m_eff_vs_phi_cos_of_daughters{},
  m_eff_vs_mu{},
  m_eff_vs_mu2{},
  m_eff_vs_mu3{},
  m_eff_vs_muTotal{} 
{
  // nop
}

void
InDetPerfPlot_Eff::initializePlots() {
  book(m_trackeff_vs_eta, "trackeff_vs_eta");
  book(m_trackeff_vs_pt, "trackeff_vs_pt");
  book(m_trackeff_vs_phi, "trackeff_vs_phi");
  book(m_trackeff_vs_d0, "trackeff_vs_d0");
  book(m_trackeff_vs_z0, "trackeff_vs_z0");
  book(m_trackeff_vs_R, "trackeff_vs_R");
  book(m_trackeff_vs_Z, "trackeff_vs_Z");

  book(m_trackeff_vs_prodR, "trackeff_vs_prodR");
  book(m_trackeff_vs_prodZ, "trackeff_vs_prodZ");

  book(m_eff_vs_eta_of_daughters, "eff_vs_eta_of_daughters");
  book(m_eff_vs_theta_of_daughters, "eff_vs_theta_of_daughters");
  book(m_eff_vs_theta_tan_of_daughters, "eff_vs_theta_tan_of_daughters");
  book(m_eff_vs_theta_cotan_of_daughters, "eff_vs_theta_cotan_of_daughters");
  book(m_eff_vs_phi_of_daughters, "eff_vs_phi_of_daughters");
  book(m_eff_vs_phi_sin_of_daughters, "eff_vs_phi_sin_of_daughters");
  book(m_eff_vs_phi_cos_of_daughters, "eff_vs_phi_cos_of_daughters");
  
  book(m_eff_vs_mu,"eff_vs_mu");
  book(m_eff_vs_mu2,"eff_vs_mu2");
  book(m_eff_vs_mu3,"eff_vs_mu3");
  book(m_eff_vs_muTotal,"eff_vs_muTotal");
}

void
InDetPerfPlot_Eff::fill(const xAOD::TruthParticle& truth, const bool isGood) {
  double eta = truth.eta();
  double pt = truth.pt() * 1_GeV; // convert MeV to GeV
  double phi = truth.phi();

  fillHisto(m_trackeff_vs_eta, eta, isGood);
  fillHisto(m_trackeff_vs_pt, pt, isGood);
  fillHisto(m_trackeff_vs_phi, phi, isGood);

  double d0 = truth.auxdata<float>("d0");
  double z0 = truth.auxdata<float>("z0");
  double R = truth.auxdata<float>("prodR");
  double Z = truth.auxdata<float>("prodZ");
  fillHisto(m_trackeff_vs_d0, d0, isGood);
  fillHisto(m_trackeff_vs_z0, z0, isGood);
  fillHisto(m_trackeff_vs_R, R, isGood);
  fillHisto(m_trackeff_vs_Z, Z, isGood);

  if (truth.hasProdVtx()) {
    const xAOD::TruthVertex* vtx = truth.prodVtx();
    double prod_rad = vtx->perp();
    double prod_z = vtx->z();
    fillHisto(m_trackeff_vs_prodR, prod_rad, isGood);
    fillHisto(m_trackeff_vs_prodZ, prod_z, isGood);
  }
}

void
InDetPerfPlot_Eff::BT_fill(const xAOD::TruthParticle& truth, float weight) {
  double eta = safelyGetEta(truth);
  double theta = truth.auxdata< float >("theta");
  double phi = truth.phi();

  double tan_theta = std::tan(theta);
  double cot_theta = (1.0 / tan_theta);
  double sin_phi = std::sin(phi);
  double cos_phi = std::cos(phi);

  fillHisto(m_eff_vs_eta_of_daughters, eta, weight);
  fillHisto(m_eff_vs_theta_of_daughters, theta, weight);
  fillHisto(m_eff_vs_theta_tan_of_daughters, tan_theta, weight);
  fillHisto(m_eff_vs_theta_cotan_of_daughters, cot_theta, weight);
  fillHisto(m_eff_vs_phi_of_daughters, phi, weight);
  fillHisto(m_eff_vs_phi_sin_of_daughters, sin_phi, weight);
  fillHisto(m_eff_vs_phi_cos_of_daughters, cos_phi, weight);
}

void
InDetPerfPlot_Eff::mu_fill(const xAOD::TruthParticle& truth, const bool isGood, const unsigned int nMuEvents) {
  float absTruthEta  = std::abs(safelyGetEta(truth));
  fillHisto(m_eff_vs_muTotal, nMuEvents, isGood);
  if(absTruthEta < 2.7) 
    fillHisto(m_eff_vs_mu, nMuEvents, isGood);
  else if (2.7 <= absTruthEta && 3.5 > absTruthEta )
    fillHisto(m_eff_vs_mu2, nMuEvents, isGood);
  else
    fillHisto(m_eff_vs_mu3, nMuEvents, isGood);  
}

void
InDetPerfPlot_Eff::finalizePlots() {
}
