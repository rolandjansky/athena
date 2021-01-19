/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
 *
   author: Max Baugh
   15/9/15
 *
 */

#include "InDet_BadMatchRate.h"
#include "InDetPhysValMonitoringUtilities.h"

using namespace IDPVM;


InDet_BadMatchRate::InDet_BadMatchRate(InDetPlotBase* pParent, std::string sDir) :
  InDetPlotBase(pParent, sDir),
  m_BadMatchRate{nullptr},
  m_BMR_vs_logpt{nullptr},
  m_ReallyFakeRate{nullptr}{
  //
}

void
InDet_BadMatchRate::initializePlots() {
  // Bad Match Rate plots, Truth Matching Probability < 50.1%
  book(m_BadMatchRate, "BadMatchRate");
  book(m_BMR_vs_logpt, "BadMatchRate_vs_logpt");

  // Really Fake Rate plots, Truth Matching Probability < 50.0%
  book(m_ReallyFakeRate, "ReallyFakeRate");

}

void
InDet_BadMatchRate::fillBMR(const xAOD::TrackParticle& particle, float weight) {
  float trketa = particle.eta();
  float trkpt = particle.pt();
  float logpt = std::log10(trkpt) - 3.0; // -3 converts from MeV to GeV

  fillHisto(m_BadMatchRate, trketa, weight);
  fillHisto(m_BMR_vs_logpt, logpt, weight);
}

void
InDet_BadMatchRate::fillRF(const xAOD::TrackParticle& particle, float weight) {
  float trketa = particle.eta();

  fillHisto(m_ReallyFakeRate, trketa, weight);
}

