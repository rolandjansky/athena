/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPerfPlot_FakeRate.h"
// #include "TrkValHistUtils/EfficiencyPurityCalculator.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "InDetPhysValMonitoringUtilities.h"
#include <cmath>
using namespace IDPVM;

InDetPerfPlot_FakeRate::InDetPerfPlot_FakeRate(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_fakerate_vs_eta{},
  m_fakerate_vs_pt{},
  m_fakerate_vs_phi{},
  m_fakerate_vs_d0{},
  m_fakerate_vs_z0{} {
  // nop
}

void
InDetPerfPlot_FakeRate::initializePlots() {

  book(m_fakerate_vs_eta, "fakerate_vs_eta");
  book(m_fakerate_vs_pt, "fakerate_vs_pt");
  book(m_fakerate_vs_phi, "fakerate_vs_phi");
  book(m_fakerate_vs_d0, "fakerate_vs_d0");
  book(m_fakerate_vs_z0, "fakerate_vs_z0");

}

void
InDetPerfPlot_FakeRate::fill(const xAOD::TrackParticle& track, const bool isFake, float weight) {

  double eta = track.eta();
  double pt = track.pt() / Gaudi::Units::GeV; // convert MeV to GeV
  double phi = track.phi();
  double d0 = track.d0();
  double z0 = track.z0();

  fillHisto(m_fakerate_vs_eta, eta, isFake, weight);
  fillHisto(m_fakerate_vs_pt, pt, isFake, weight);
  fillHisto(m_fakerate_vs_phi, phi, isFake, weight); 
  fillHisto(m_fakerate_vs_d0, d0, isFake, weight);
  fillHisto(m_fakerate_vs_z0, z0, isFake, weight);

}

void
InDetPerfPlot_FakeRate::finalizePlots() {

}
