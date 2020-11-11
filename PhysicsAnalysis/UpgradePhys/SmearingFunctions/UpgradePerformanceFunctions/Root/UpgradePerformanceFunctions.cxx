/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UPGRADEPERFORMANCEFUNCTIONS_CXX
#define UPGRADEPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"
#include "PathResolver/PathResolver.h"

namespace Upgrade {

UpgradePerformanceFunctions::UpgradePerformanceFunctions( const std::string& name )
  : asg::AsgMetadataTool( name ),
    m_muonRes(nullptr),
    m_muonEff(nullptr),
    // General properties
    m_layout(Step1p6),
    m_avgMu(200.),
    // Electron properties
    m_eleCutLevel(UpgradePerformanceFunctions::looseElectron),
    // Muon properties
    m_muonCutLevel(UpgradePerformanceFunctions::highPtMuon),
    m_useMuonHighEta(true),
    // Photon properties
    m_photonCutLevel(UpgradePerformanceFunctions::tightPhoton),
    m_photonNoiseScaling(-1),
    // Jet properties
    // Tau properties
    // Pileup properties
    m_bUseTrackConf(true),
    m_fPileupJetThresholdMeV(30000.),
    m_fEff(0.02),
    m_pueff(UpgradePerformanceFunctions::PU),
    // HGTD properties
    m_bUseHGTD0(true),
    m_bUseHGTD1(false),
    // Warm toroid
    m_useWarmToroid(false),
    // Random seeds
    m_electronRandomSeed(123),
    m_photonRandomSeed(1234),
    m_tauRandomSeed(12345),
    m_jetRandomSeed(123456),
    m_metRandomSeed(1234567),
    m_pileupRandomSeed(12345678),
    // Smearing root files
    m_flavourTaggingCalibrationFilename("UpgradePerformanceFunctions/CalibArea-00-01/flavor_tags_v2.0.root"),
    m_electronEnergyResolutionFilename("ElectronPhotonFourMomentumCorrection/v5/resolutionFit_electron_run2_pre.root"),
    m_muonEnergyResolutionFilename("MuonMomentumResolutions_Step1p9_171002.dat"),
    m_photonFakeFilename("UpgradePerformanceFunctions/CalibArea-00-01/PhotonFakes.root"),
    m_chargeFlipRateFilename("UpgradePerformanceFunctions/CalibArea-00-01/ChargeFlipRates.root"),
    m_METFilename("UpgradePerformanceFunctions/CalibArea-00-01/sumetPU_mu200_ttbar_gold.root"),
    m_puPath("UpgradePerformanceFunctions")
{
  // This is the default constructor, so we use default settings
  declareProperty( "Layout", m_layout = Step1p6 );
  declareProperty( "AvgMu", m_avgMu = 200. );
  // Electrons
  declareProperty( "ElectronWorkingPoint", m_eleCutLevel = UpgradePerformanceFunctions::looseElectron);
  //Muons
  declareProperty( "MuonWorkingPoint", m_muonCutLevel = UpgradePerformanceFunctions::highPtMuon);
  declareProperty( "UseMuonHighEta", m_useMuonHighEta = true ); // use high-eta muon tagger by default
  //Photons
  declareProperty( "PhotonWorkingPoint", m_photonCutLevel = UpgradePerformanceFunctions::tightPhoton);
  declareProperty( "PhotonNoiseScaling", m_photonNoiseScaling = -1 ); //use electron resolution by default
  //Jet
  //Tau
  //Pileup
  declareProperty( "UseTrackConfirmation", m_bUseTrackConf = true ); // Don't understand why this even is an option...
  declareProperty( "PileupJetThresholdMeV", m_fPileupJetThresholdMeV = 30e3 ); // 30 GeV, default
  declareProperty( "JVT_PU_Efficiency", m_fEff = 0.02 ); // Set JVT PU fake rate to 2% within tracking acceptance (default)
  declareProperty( "PileupEfficiency", m_pueff = UpgradePerformanceFunctions::PU ); // PU-scheme is only scheme supported (so maybe drop this option)
  //HGTD
  declareProperty( "UseHGTD0", m_bUseHGTD0 = true);
  declareProperty( "UseHGTD1", m_bUseHGTD1 = false);
  //Warm Toroid
  declareProperty( "UseWarmToroid", m_useWarmToroid = false);
  // Declare properties for random seeds
  declareProperty( "ElectronRandomSeed", m_electronRandomSeed = 123 );
  declareProperty( "PhotonRandomSeed", m_photonRandomSeed = 1234 );
  declareProperty( "TauRandomSeed", m_tauRandomSeed = 12345 );
  declareProperty( "JetRandomSeed", m_jetRandomSeed = 123456 );
  declareProperty( "METRandomSeed", m_metRandomSeed = 1234567 );
  declareProperty( "PileupRandomSeed", m_pileupRandomSeed = 12345678 );
  // Inital values for track confirmation and PU jet overlay
  declareProperty( "FlavourTaggingCalibrationFile", m_flavourTaggingCalibrationFilename = "UpgradePerformanceFunctions/CalibArea-00-01/flavor_tags_v2.0.root" );
  declareProperty( "ElectronEnergyResolutionFile", m_electronEnergyResolutionFilename = "ElectronPhotonFourMomentumCorrection/v5/resolutionFit_electron_run2_pre.root" );
  declareProperty( "MuonEnergyResolutionFile", m_muonEnergyResolutionFilename = "MuonMomentumResolutions_Step1p9_171002.dat" );
  declareProperty( "PhotonFakeFile", m_photonFakeFilename = "UpgradePerformanceFunctions/CalibArea-00-01/PhotonFakes.root" );
  declareProperty( "ChargeFlipRateFile", m_chargeFlipRateFilename = "UpgradePerformanceFunctions/CalibArea-00-01/ChargeFlipRates.root" );
  declareProperty( "METFile", m_METFilename = "UpgradePerformanceFunctions/CalibArea-00-01/sumetPU_mu200_ttbar_gold.root" );  
  declareProperty( "PileupPath", m_puPath = "UpgradePerformanceFunctions" );

  m_tool_init = false;
}

UpgradePerformanceFunctions::UpgradePerformanceFunctions( const std::string& name, UpgradeLayout layout, double avgMu)
  : asg::AsgMetadataTool( name ),
    m_muonRes(nullptr),
    m_muonEff(nullptr),
    // Electron properties
    m_eleCutLevel(UpgradePerformanceFunctions::looseElectron),
    // Muon properties
    m_muonCutLevel(UpgradePerformanceFunctions::highPtMuon),
    m_useMuonHighEta(true),
    // Photon properties
    m_photonCutLevel(UpgradePerformanceFunctions::tightPhoton),
    m_photonNoiseScaling(-1),
    // Jet properties
    // Tau properties
    // Pileup properties
    m_bUseTrackConf(true),
    m_fPileupJetThresholdMeV(30000.),
    m_fEff(0.02),
    m_pueff(UpgradePerformanceFunctions::PU),
    // HGTD properties
    m_bUseHGTD0(true),
    m_bUseHGTD1(false),
    // Warm toroid
    m_useWarmToroid(false),
    // Random seeds
    m_electronRandomSeed(123),
    m_photonRandomSeed(1234),
    m_tauRandomSeed(12345),
    m_jetRandomSeed(123456),
    m_metRandomSeed(1234567),
    m_pileupRandomSeed(12345678),
    // Smearing root files
    m_flavourTaggingCalibrationFilename("UpgradePerformanceFunctions/CalibArea-00-01/flavor_tags_v2.0.root"),
    m_electronEnergyResolutionFilename("ElectronPhotonFourMomentumCorrection/v5/resolutionFit_electron_run2_pre.root"),
    m_muonEnergyResolutionFilename("MuonMomentumResolutions_Step1p9_171002.dat"),
    m_photonFakeFilename("UpgradePerformanceFunctions/CalibArea-00-01/PhotonFakes.root"),
    m_chargeFlipRateFilename("UpgradePerformanceFunctions/CalibArea-00-01/ChargeFlipRates.root"),
    m_METFilename("UpgradePerformanceFunctions/CalibArea-00-01/sumetPU_mu200_ttbar_gold.root"),
    m_puPath("UpgradePerformanceFunctions")
{

  m_layout = layout;

  if (layout != UpgradePerformanceFunctions::UpgradeLayout::run2 && avgMu != 80. && avgMu != 140. && avgMu != 200.) {
    ATH_MSG_WARNING("mu value " << avgMu << " not allowed. Allowed values are 80, 140, 200 except for Run-2 layout.  Falling back to mu=" << m_avgMu );
  } else {
    m_avgMu = avgMu;
  }
  // Electrons
  declareProperty( "ElectronWorkingPoint", m_eleCutLevel = UpgradePerformanceFunctions::looseElectron);
  //Muons
  declareProperty( "MuonWorkingPoint", m_muonCutLevel = UpgradePerformanceFunctions::highPtMuon);
  declareProperty( "UseMuonHighEta", m_useMuonHighEta = true ); // use high-eta muon tagger by default
  //Photons
  declareProperty( "PhotonWorkingPoint", m_photonCutLevel = UpgradePerformanceFunctions::tightPhoton);
  declareProperty( "PhotonNoiseScaling", m_photonNoiseScaling = -1 ); //use electron resolution by default
  //Jet
  //Tau
  //Pileup
  declareProperty( "UseTrackConfirmation", m_bUseTrackConf = true ); // Don't understand why this even is an option...
  declareProperty( "PileupJetThresholdMeV", m_fPileupJetThresholdMeV = 30e3 ); // 30 GeV, default
  declareProperty( "JVT_PU_Efficiency", m_fEff = 0.02 ); // Set JVT PU fake rate to 2% within tracking acceptance (default)
  declareProperty( "PileupEfficiency", m_pueff = UpgradePerformanceFunctions::PU ); // PU-scheme is only scheme supported (so maybe drop this option)
  //HGTD
  declareProperty( "UseHGTD0", m_bUseHGTD0 = true);
  declareProperty( "UseHGTD1", m_bUseHGTD1 = false);
  //Warm Toroid
  declareProperty( "UseWarmToroid", m_useWarmToroid = false);
  // Declare properties for random seeds
  declareProperty( "ElectronRadomSeed", m_electronRandomSeed = 123 );
  declareProperty( "PhotonRandomSeed", m_photonRandomSeed = 1234 );
  declareProperty( "TauRandomSeed", m_tauRandomSeed = 12345 );
  declareProperty( "JetRandomSeed", m_jetRandomSeed = 123456 );
  declareProperty( "METRandomSeed", m_metRandomSeed = 1234567 );
  declareProperty( "PileupRandomSeed", m_pileupRandomSeed = 12345678 );
  // Inital values for track confirmation and PU jet overlay
  declareProperty( "FlavourTaggingCalibrationFile", m_flavourTaggingCalibrationFilename = "UpgradePerformanceFunctions/CalibArea-00-01/flavor_tags_v2.0.root" );
  declareProperty( "ElectronEnergyResolutionFile", m_electronEnergyResolutionFilename = "ElectronPhotonFourMomentumCorrection/v5/resolutionFit_electron_run2_pre.root" );
  declareProperty( "MuonEnergyResolutionFile", m_muonEnergyResolutionFilename = "MuonMomentumResolutions_Step1p9_171002.dat" );
  declareProperty( "PhotonFakeFile", m_photonFakeFilename = "UpgradePerformanceFunctions/CalibArea-00-01/PhotonFakes.root" );
  declareProperty( "ChargeFlipRateFile", m_chargeFlipRateFilename = "UpgradePerformanceFunctions/CalibArea-00-01/ChargeFlipRates.root" );
  declareProperty( "METFile", m_METFilename = "UpgradePerformanceFunctions/CalibArea-00-01/sumetPU_mu200_ttbar_gold.root" );  
  declareProperty( "PileupPath", m_puPath = "UpgradePerformanceFunctions" );
  
  m_tool_init = false;

}

StatusCode UpgradePerformanceFunctions::initialize() {

  if (m_tool_init) {
    ATH_MSG_ERROR("Initialising for a second time -- something is wrong!");
    return StatusCode::FAILURE;
  }

  // Greet the user:
  ATH_MSG_INFO( "Initialising..." );

  if (m_avgMu != 60. && m_avgMu != 80. && m_avgMu != 140. && m_avgMu != 200.) {
    m_avgMu = 200.;
    ATH_MSG_WARNING("mu value " << m_avgMu << " not allowed. Allowed values are 80, 140, 200 except for Run-2 layout.  Falling back to mu=" << m_avgMu );
  } 

  m_flavourTaggingCalibrationFilename = PathResolverFindCalibFile( m_flavourTaggingCalibrationFilename );
  m_electronEnergyResolutionFilename = PathResolverFindCalibFile( m_electronEnergyResolutionFilename );
  m_photonFakeFilename = PathResolverFindCalibFile( m_photonFakeFilename );
  m_chargeFlipRateFilename = PathResolverFindCalibFile( m_chargeFlipRateFilename );
  m_METFilename = PathResolverFindCalibFile( m_METFilename );

  ATH_CHECK( this->loadMETHistograms() );
  ATH_CHECK( this->initPhotonFakeHistograms() );
  ATH_CHECK( this->initializePileupTemplates() );

  if (m_bUseHGTD0) {
    if (m_ExtHGTDTrig) {
      m_JetTrigMaxEta = m_HGTDMaxTrigEta;
      m_JetTrig4jMin  = m_HGTD0MinExtTrigPt;
    } else { // max trigger eta stays default 3.2
      m_JetTrig4jMin  = m_HGTD0MinTrigPt;
    }
    ATH_MSG_INFO("UpgradePerformanceFunctions::setUseHGTD0 is set to " << m_bUseHGTD0);
  }

  if (m_bUseHGTD1) {
    if (m_ExtHGTDTrig) {
      m_JetTrigMaxEta = m_HGTDMaxTrigEta;
      m_JetTrig4jMin  = m_HGTD1MinExtTrigPt;
    } else { // max trigger eta stays default 3.2
      m_JetTrig4jMin  = m_HGTD1MinTrigPt;
    }

    ATH_MSG_INFO("UpgradePerformanceFunctions::setUseHGTD1 is set to " << m_bUseHGTD1);
  }

  m_electronRandom.SetSeed(m_electronRandomSeed);
  m_photonRandom.SetSeed(m_photonRandomSeed);
  m_tauRandom.SetSeed(m_tauRandomSeed);
  m_jetRandom.SetSeed(m_jetRandomSeed);
  m_METRandom.SetSeed(m_metRandomSeed);
  m_pileupRandom.SetSeed(m_pileupRandomSeed);

  ATH_MSG_VERBOSE("Done with initialisation");

  m_tool_init = true;

  // Return gracefully:
  return StatusCode::SUCCESS;
}

#ifdef XAOD_STANDALONE
UpgradePerformanceFunctions::UpgradeLayout UpgradePerformanceFunctions::getLayout() {
  return m_layout;
}
#else
int UpgradePerformanceFunctions::getLayout() {
  return m_layout;
}
#endif

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

}

#endif
