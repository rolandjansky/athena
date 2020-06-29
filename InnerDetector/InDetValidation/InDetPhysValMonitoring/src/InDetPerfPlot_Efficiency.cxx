/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_Efficiency.h"
// #include "TrkValHistUtils/EfficiencyPurityCalculator.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "InDetPhysValMonitoringUtilities.h"
#include <cmath>
using namespace IDPVM;

InDetPerfPlot_Efficiency::InDetPerfPlot_Efficiency(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_efficiency_vs_eta{},
  m_efficiency_vs_pt{},
  m_efficiency_vs_pt_low{},
  m_efficiency_vs_phi{},
  m_efficiency_vs_d0{},
  m_efficiency_vs_z0{},
  m_efficiency_vs_R{},
  m_efficiency_vs_Z{},
  m_extended_efficiency_vs_d0{},
  m_extended_efficiency_vs_z0{},
  m_efficiency_vs_prodR{},
  m_efficiency_vs_prodZ{} {
  // nop
}

void
InDetPerfPlot_Efficiency::initializePlots() {

  book(m_efficiency_vs_eta, "efficiency_vs_eta");
  book(m_efficiency_vs_pt, "efficiency_vs_pt");
  book(m_efficiency_vs_pt_low, "efficiency_vs_pt_low");
  book(m_efficiency_vs_phi, "efficiency_vs_phi");
  book(m_efficiency_vs_d0, "efficiency_vs_d0");
  book(m_efficiency_vs_z0, "efficiency_vs_z0");
  book(m_efficiency_vs_R, "efficiency_vs_R");
  book(m_efficiency_vs_Z, "efficiency_vs_Z");

  book(m_extended_efficiency_vs_d0, "extended_efficiency_vs_d0");
  book(m_extended_efficiency_vs_z0, "extended_efficiency_vs_z0");
  book(m_efficiency_vs_prodR, "efficiency_vs_prodR");
  book(m_efficiency_vs_prodZ, "efficiency_vs_prodZ");

}

void
InDetPerfPlot_Efficiency::fill(const xAOD::TruthParticle& truth, const bool isGood) {
  double eta = truth.eta();
  double pt = truth.pt() / Gaudi::Units::GeV; // convert MeV to GeV
  double phi = truth.phi();

  fillHisto(m_efficiency_vs_eta, eta, isGood);
  fillHisto(m_efficiency_vs_pt, pt, isGood);
  fillHisto(m_efficiency_vs_pt_low, pt, isGood);
  fillHisto(m_efficiency_vs_phi, phi, isGood);

  double d0 = truth.auxdata<float>("d0");
  double z0 = truth.auxdata<float>("z0");
  double R = truth.auxdata<float>("prodR");
  double Z = truth.auxdata<float>("prodZ");
  fillHisto(m_efficiency_vs_d0, d0, isGood);
  fillHisto(m_efficiency_vs_z0, z0, isGood);
  fillHisto(m_efficiency_vs_R, R, isGood);
  fillHisto(m_efficiency_vs_Z, Z, isGood);

  fillHisto(m_extended_efficiency_vs_d0, d0, isGood);
  fillHisto(m_extended_efficiency_vs_z0, z0, isGood);
  if (truth.hasProdVtx()) {
    const xAOD::TruthVertex* vtx = truth.prodVtx();
    double prod_rad = vtx->perp();
    double prod_z = vtx->z();
    fillHisto(m_efficiency_vs_prodR, prod_rad, isGood);
    fillHisto(m_efficiency_vs_prodZ, prod_z, isGood);
  }
}

void
InDetPerfPlot_Efficiency::finalizePlots() {
}
