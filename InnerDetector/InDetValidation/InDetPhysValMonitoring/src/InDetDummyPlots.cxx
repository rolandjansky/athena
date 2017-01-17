/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetDummyPlots.h"
#include "xAODTruth/TruthVertex.h"
#include "InDetPhysValMonitoringUtilities.h"
#include <cmath>
using namespace IDPVM;

InDetDummyPlots::InDetDummyPlots(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_lepton_disappearance_radius{},
  m_low_Pt_lepton_frac{},
  m_nOut_of_lepdeath{},
  m_brem_spectrum{},
  m_primary_photon_eta_vs_conversion_radius{},
  m_primary_photon_pt_vs_decay_radius{},
  m_primary_photon_pt{},
  m_brem_photon_eta_vs_radius_of_conversion{},
  m_brem_photon_pt_vs_radius_of_origin_of_conversion{} {
  // nop
}

void
InDetDummyPlots::initializePlots() {
  book(m_lepton_disappearance_radius, "lepton_disappearance_radius");
  book(m_low_Pt_lepton_frac, "low_Pt_lepton_frac");
  book(m_nOut_of_lepdeath, "nOut_of_lepdeath");
  book(m_brem_spectrum, "brem_spectrum");
  book(m_primary_photon_eta_vs_conversion_radius, "primary_photon_eta_vs_conversion_radius");
  book(m_primary_photon_pt_vs_decay_radius, "primary_photon_pt_vs_decay_radius");
  book(m_primary_photon_pt, "primary_photon_pt");
  book(m_brem_photon_eta_vs_radius_of_conversion, "brem_photon_eta_vs_radius_of_conversion");
  book(m_brem_photon_pt_vs_radius_of_origin_of_conversion, "brem_photon_pt_vs_radius_of_origin_of_conversion");
}

void
InDetDummyPlots::lepton_fill(const xAOD::TruthParticle& truth, float weight) {
  double R = truth.auxdata<float>("prodR");

  fillHisto(m_low_Pt_lepton_frac, R, weight);
  if (truth.hasProdVtx()) {
    const xAOD::TruthVertex* vtx = truth.prodVtx();
    double prod_rad = vtx->perp();
    // double prod_z = vtx->z(); unused
    if (truth.hasDecayVtx()) {
      const xAOD::TruthVertex* decay_vtx = truth.decayVtx();
      double decay_rad = decay_vtx->perp();
      int nOut = decay_vtx->nOutgoingParticles();
      if (prod_rad < 150) {
        fillHisto(m_lepton_disappearance_radius, decay_rad);
      }
      fillHisto(m_nOut_of_lepdeath, nOut);
      for (int i = 0; i < nOut; i++) {
        const xAOD::TruthParticle* out = decay_vtx->outgoingParticle(i);
        double brem_e = out->e() * 0.001; // convert MeV to GeV
        double log_e = std::log10(brem_e);
        if (out->pdgId() == 22) {
          fillHisto(m_brem_spectrum, decay_rad, log_e);
        }
      }
    }
  }
}

void
InDetDummyPlots::prim_photon_fill(const xAOD::TruthParticle& truth) {
  double eta = truth.eta();
  double pt = truth.pt() * 0.001; // convert MeV to GeV
  double log_pt = std::log10(pt);

  fillHisto(m_primary_photon_pt, pt);
  if (truth.hasDecayVtx()) {
    const xAOD::TruthVertex* vtx = truth.decayVtx();
    double decay_rad = vtx->perp();
    fillHisto(m_primary_photon_eta_vs_conversion_radius, decay_rad, eta);
    fillHisto(m_primary_photon_pt_vs_decay_radius, decay_rad, log_pt);
  }
}

void
InDetDummyPlots::brem_photon_fill(const xAOD::TruthParticle& truth) {
  double eta = truth.eta();
  double pt = truth.pt() * 0.001; // convert MeV to GeV
  double log_pt = std::log10(pt);
  const xAOD::TruthVertex* vtx = truth.prodVtx();

  // double prod_rad = vtx->perp(); unused
  if (truth.hasDecayVtx()) {
    double decay_rad = vtx->perp();
    fillHisto(m_brem_photon_eta_vs_radius_of_conversion, decay_rad, eta);
    fillHisto(m_brem_photon_pt_vs_radius_of_origin_of_conversion, decay_rad, log_pt);
  }
}

void
InDetDummyPlots::finalizePlots() {
}
