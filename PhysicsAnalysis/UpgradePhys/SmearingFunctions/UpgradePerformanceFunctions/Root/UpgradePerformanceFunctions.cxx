/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UPGRADEPERFORMANCEFUNCTIONS_CXX
#define UPGRADEPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

UpgradePerformanceFunctions::UpgradePerformanceFunctions( const std::string& name )
  : asg::AsgMetadataTool( name ),
    m_layout(gold),
    m_avgMu(200.),
    m_pueff(PU),
    m_puPath("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/UpgradePerformanceFunctions/"),
    m_fEff(0.02),
    m_bUseTrackConf(true),
    m_fPileupJetThresholdMeV(30e3),
    m_photonNoiseScaling(-1),
    m_muonRes(nullptr),
    m_muonEff(nullptr),
    m_useMuonHighEta(true) {
  // This is the default constructor, so we use default settings
  declareProperty( "Layout", m_layout = gold );
  declareProperty( "AvgMu", m_avgMu = 200. );
  declareProperty( "PhotonNoiseScaling", m_photonNoiseScaling = -1 ); //use electron resolution by default
  declareProperty( "UseMuonHighEta", m_useMuonHighEta = true );  // use high-eta muon tagger by default

  // Initialize the missing ET performance histograms
  // For now, this is done inside the MissingETPerformanceFunctions

  // Initialize the tracking performance histograms
  // For now, this is done inside the TrackingPerformanceFunctions

  // Initialize JetEtMiss settings
  // Set random seeds
  m_jetRandom.SetSeed(1234567);
  m_pileupRandom.SetSeed(12345678);

  // JER setting initial values already set in header file (possible in C++11)

  // Inital values for track confirmation and PU jet overlay
  declareProperty("PileupJetThresholdMeV", m_fPileupJetThresholdMeV = 30e3 ); // 30 GeV, default
  declareProperty("PUeff", m_pueff = PU );            // PU-scheme is only scheme supported (so maybe drop this option)
  declareProperty("UseTrackConfirmation", m_bUseTrackConf = true );  // Don't understand why this even is an option...
  declareProperty("JVT_PU_Efficiency", m_fEff = 0.02 );           // Set JVT PU fake rate to 2% within tracking acceptance (default)
  declareProperty("PUPath", m_puPath = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/UpgradePerformanceFunctions/" );
}

UpgradePerformanceFunctions::UpgradePerformanceFunctions( const std::string& name, UpgradeLayout layout, double avgMu)
  : asg::AsgMetadataTool( name ),
    m_muonRes(nullptr),
    m_muonEff(nullptr) {
  m_layout = layout;
  //m_avgMu = avgMu;
  if (layout != UpgradePerformanceFunctions::UpgradeLayout::run2 && avgMu != 80. && avgMu != 140. && avgMu != 200.) {
    ATH_MSG_WARNING("mu value " << avgMu << " not allowed. Allowed values are 80, 140, 200 except for Run-2 layout.  Falling back to mu=" << m_avgMu );
  } else {
    m_avgMu = avgMu;
  }
  m_muonRes = 0;
}

void UpgradePerformanceFunctions::setLayout(UpgradeLayout layout) {
  m_layout = layout;
}

void UpgradePerformanceFunctions::setAvgMu(double avgMu) {
  if (avgMu != 60. && avgMu != 80. && avgMu != 140. && avgMu != 200.) {
    ATH_MSG_WARNING("mu value " << avgMu << " not allowed. Allowed values are 80, 140, 200 except for Run-2 layout.  Falling back to mu=" << m_avgMu );
  } else {
    m_avgMu = avgMu;
  }

}

UpgradePerformanceFunctions::UpgradeLayout UpgradePerformanceFunctions::getLayout() {
  return m_layout;
}

double UpgradePerformanceFunctions::getAvgMu() {
  return m_avgMu;
}

// Private helper function
TGraph *UpgradePerformanceFunctions::makeGraph(std::vector<double> x, std::vector<double> y) {
  if (x.size() != y.size()) ATH_MSG_FATAL("Bad input to UpgradePerformanceFunctions::makeGraph");
  TGraph *g = new TGraph();
  for (size_t i = 0; i < x.size(); ++i)
    g->SetPoint(i, x[i], y[i]);
  return g;
}

#endif
