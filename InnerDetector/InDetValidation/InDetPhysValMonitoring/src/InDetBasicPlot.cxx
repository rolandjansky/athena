/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetBasicPlot.cxx
 * @author shaun roe
 **/


#include "InDetPhysValMonitoringUtilities.h"

#include "InDetBasicPlot.h"
#include <cmath>
using namespace IDPVM;

InDetBasicPlot::InDetBasicPlot(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),
  m_paramNames{"d0", "z0", "phi", "theta", "eta", "qOverP", "pt"},
  m_truthParamNames{"z0st", "prodR", "prodZ"},
  m_basicTruthPlots{nullptr},
  m_extraTruthPlots{nullptr},
  m_basicTrackPlots{nullptr},
  m_basic_pt_vs_eta(nullptr),
  m_basic_phi_vs_eta(nullptr),
  m_truth_pt_vs_eta(nullptr),
  m_truth_phi_vs_eta(nullptr),
  m_d0IsExactlyZeroInTrackCounter(0),
  m_d0IsExactlyZeroInTruthCounter(0),
  m_numCallsToFillTruth(0),
  m_numCallsToFillTrack(0) {
}

void
InDetBasicPlot::initializePlots() {
  unsigned int i(0);
  const std::string prefix("basic");
  const std::string truthPrefix("truth");

  for (const auto& p:m_paramNames) {
    const std::string particleHistoId = prefix + p;
    const std::string truthHistoId = truthPrefix + p;
    book(m_basicTruthPlots[i], truthHistoId);
    book(m_basicTrackPlots[i], particleHistoId);
    ++i;
  }
  book(m_basic_pt_vs_eta, prefix + "_pt_vs_eta");
  book(m_basic_phi_vs_eta, prefix + "_phi_vs_eta");
  i = 0;
  for (const auto& p:m_truthParamNames) {
    const std::string truthHistoId = truthPrefix + p;
    book(m_extraTruthPlots[i], truthHistoId);
    ++i;
  }
  book(m_truth_pt_vs_eta, truthPrefix + "_pt_vs_eta");
  book(m_truth_phi_vs_eta, truthPrefix + "_phi_vs_eta");
}

void
InDetBasicPlot::fill(const xAOD::TruthParticle& particle) {
  ++m_numCallsToFillTruth;
  float truthPartParams[NPARAMS];
  // initalize values with NaN
  std::fill_n(truthPartParams, NPARAMS, std::numeric_limits<float>::quiet_NaN());
  // quantities with xAOD::TruthParticle accessors:
  truthPartParams[PHI] = particle.phi();
  truthPartParams[ETA] = particle.eta();
  truthPartParams[PT] = particle.pt() * 1_GeV;
  // quantities that do not have accesors & histograms
  unsigned int idx(0);
  for (const auto& p:m_paramNames) {
    // fill only values that are at initial values (were not yet filled via accessors)
    if (std::isnan(truthPartParams[idx]) && particle.isAvailable<float>(p)) {
      float pVal = particle.auxdata< float >(p);
      if (0 == idx && 0. == pVal) {
        ++m_d0IsExactlyZeroInTrackCounter;
      }
      truthPartParams[idx] = pVal;
    }
    fillHisto(m_basicTruthPlots[idx], truthPartParams[idx]);
    ++idx;
  }
  // extra truth-only parameters
  idx = 0;
  for (const auto& p:m_truthParamNames) {
    if (particle.isAvailable<float>(p)) {
      const auto thisParameterValue = particle.auxdata< float >(p);
      fillHisto(m_extraTruthPlots[idx], thisParameterValue);
    }
    ++idx;
  }
  // 2D histograms
  fillHisto(m_truth_pt_vs_eta, truthPartParams[PT], truthPartParams[ETA]);
  fillHisto(m_truth_phi_vs_eta, truthPartParams[PHI], truthPartParams[ETA]);
}

void
InDetBasicPlot::fill(const xAOD::TrackParticle& particle) {
  ++m_numCallsToFillTrack;
  float trkParticleParams[NPARAMS];
  trkParticleParams[D0] = particle.d0();
  if (trkParticleParams[D0] == 0.) {
    ++m_d0IsExactlyZeroInTrackCounter;
  }
  trkParticleParams[Z0] = particle.z0();
  trkParticleParams[PHI] = particle.phi0();
  trkParticleParams[THETA] = particle.theta();
  trkParticleParams[ETA] = particle.eta();
  trkParticleParams[QOVERP] = particle.qOverP();
  trkParticleParams[PT] = particle.pt() * 1_GeV;
  for (unsigned int i(0); i < NPARAMS; ++i) {
    const auto& thisParameterValue = trkParticleParams[i];
    fillHisto(m_basicTrackPlots[i], thisParameterValue);
  }
  fillHisto(m_basic_pt_vs_eta, trkParticleParams[PT], trkParticleParams[ETA]);
  fillHisto(m_basic_phi_vs_eta, trkParticleParams[PHI], trkParticleParams[ETA]);
}

void
InDetBasicPlot::finalizePlots() {
  ATH_MSG_DEBUG(
    "Number of exact zero values for d0 in tracks: " << m_d0IsExactlyZeroInTrackCounter << " out of " <<
      m_numCallsToFillTrack);
  ATH_MSG_DEBUG(
    "Number of exact zero values for d0 in truth: " << m_d0IsExactlyZeroInTruthCounter << " out of " <<
      m_numCallsToFillTruth);
}
