/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <memory>
#include "TROOT.h"
#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"
#include "AsgTools/AnaToolHandle.h"

int main( int argc, char* argv[] ) {
  
  using namespace asg::msgUserCode;
  using namespace Upgrade;
  ANA_CHECK_SET_TYPE (int);
  
  //StatusCode::enableFailure();
  CP::SystematicCode::enableFailure();
  CP::CorrectionCode::enableFailure();

  // The application's name:
  const char* APP_NAME = argv[ 0 ];

  // READ CONFIG
  int testElectrons = -1;
  int testPhotons = -1;
  int testMuons = -1;
  int testJets = -1;
  //int testTaus = -1; //To be implemented
  int testMET = -1;
  int testFlavourTagging = -1;

  bool enableHGTD = false;

  for (int i = 1 ; i < argc ; i++) {
    const char* key = strtok(argv[i], "=") ;
    const char* val = strtok(0, " ") ;

    Info( APP_NAME, "processing key %s  with value %s", key, val );

    if (strcmp(key, "testElectrons") == 0) testElectrons = atoi(val);
    if (strcmp(key, "testPhotons") == 0) testPhotons = atoi(val);
    if (strcmp(key, "testMuons") == 0) testMuons = atoi(val);
    if (strcmp(key, "testJets") == 0) testJets = atoi(val);
    //if (strcmp(key, "testTaus") == 0) testTaus = atoi(val);
    if (strcmp(key, "testMET") == 0) testMET = atoi(val);
    if (strcmp(key, "testFlavourTagging") == 0) testFlavourTagging = atoi(val);
    if (strcmp(key, "enableHGTD") == 0) enableHGTD = (atoi(val) == 0) ? false : true;
    
  }
  
  // Setup upgrade smearing functions
  auto m_upgrade = std::unique_ptr<UpgradePerformanceFunctions>( new UpgradePerformanceFunctions("UpgradePerformanceFunctions") );
  Info( APP_NAME, "Layout is %d, and mu value is %f", m_upgrade->getLayout(), m_upgrade->getAvgMu());
  ANA_CHECK( m_upgrade->setProperty("UseHGTD0", enableHGTD) );  
  if (enableHGTD) {
    std::cout << "Enabled HGTD." << std::endl;
  }

  //HGTD local tests
  ANA_CHECK( m_upgrade->setProperty("FlavourTaggingCalibrationFile", "/global/homes/s/spgriso/code/AtlasUPF/data/flavor_tags_hgtd_v2.0.root") );  
  //ANA_CHECK( m_upgrade->setProperty("FlavourTaggingCalibrationFile", "/global/homes/s/spgriso/code/AtlasUPF/data/upgradeBtaggingROOTFile/output_ITK.root") );  
  //ANA_CHECK( m_upgrade->setProperty("FlavourTaggingCalibrationFile", "/global/homes/s/spgriso/code/AtlasUPF/data/upgradeBtaggingROOTFile/output_HGTD_Initial.root") );
  //end HGTD local tests configuration
  
  ANA_CHECK( m_upgrade->initialize() );

  // This is just a test of some methods without realistic input.
  if (testPhotons > 0) {
    Info( APP_NAME, "ElectronToPhotonFakeRate at pT=50 GeV is %f", m_upgrade->getElectronToPhotonFakeRate(50.*1000., 0.0) );
    Info( APP_NAME, "Photon efficiency at pT=50 GeV is %f", m_upgrade->getPhotonEfficiency(50.*1000.) );
    Info( APP_NAME, "Photon efficiency at pT=10 GeV is %f", m_upgrade->getPhotonEfficiency(10.*1000.) );
    Info( APP_NAME, "Jet-to-photon fake rate at pT=50 GeV is %f", m_upgrade->getPhotonFakeRate(50.*1000.) );
    Info( APP_NAME, "Jet-to-photon fake rate at pT=10 GeV is %f", m_upgrade->getPhotonFakeRate(10.*1000.) );
    Info( APP_NAME, "Photon pileup fake rate at pT=50 GeV is %f", m_upgrade->getPhotonPileupFakeRate(50.*1000.) );

    // Test of the photon fake energy rescaling
    Info( APP_NAME, "Photon fake rescaled ET at ET=50 GeV is %f MeV", m_upgrade->getPhotonFakeRescaledET(50.*1000.) );
    Info( APP_NAME, "Photon pileup fake rescaled ET=50 GeV is %f MeV", m_upgrade->getPhotonPileupFakeRescaledET(50.*1000.) );
  }

  if (testMET > 0) {
    float eventMETreso = m_upgrade->getMETResolution(200000.0);
    std::cout << "MET resolution test result is " << eventMETreso << std::endl;
    std::cout << "MET smearing test result (40, 40) --> ";
    UpgradePerformanceFunctions::MET result = m_upgrade->getMETSmeared(200000., 40000., 40000., UpgradePerformanceFunctions::nominal);
    std::cout << "(" << result.first / 1000.
              << ", "  << result.second / 1000. << ")" << std::endl;
  }

  if (testMuons > 0) {
    std::cout << "Muon trigger efficiency at pT=200 GeV, eta=1.0 is "
              << m_upgrade->getSingleMuonTriggerEfficiency(200000., 1.0)
              << std::endl;

    std::cout << "Muon trigger efficiency at pT=200 GeV, eta=1.1 is "
              << m_upgrade->getSingleMuonTriggerEfficiency(200000., 1.1)
              << std::endl;

    float muonEfficiency = m_upgrade->getMuonEfficiency(2000000., 1.5);
    std::cout << "Muon efficiency at pT=2000 GeV, eta=1.5 is "
              << muonEfficiency << std::endl;
    muonEfficiency = m_upgrade->getMuonEfficiency(20000000., 1.5);
    std::cout << "Muon efficiency at pT=20000 GeV, eta=1.5 is "
              << muonEfficiency << std::endl;

    muonEfficiency = m_upgrade->getMuonEfficiency(30000., -0.5);
    std::cout << "Muon efficiency at pT=30 GeV, eta=-0.5 is "
              << muonEfficiency << std::endl;
    muonEfficiency = m_upgrade->getMuonEfficiency(3000., 1.5);
    std::cout << "Muon efficiency at pT=3 GeV, eta=1.5 is "
              << muonEfficiency << std::endl;
    muonEfficiency = m_upgrade->getMuonEfficiency(3000., 3.5);
    std::cout << "Muon efficiency at pT=3 GeV, eta=3.5 is "
              << muonEfficiency << std::endl;
    muonEfficiency = m_upgrade->getMuonEfficiency(1000., -1.5);
    std::cout << "Muon efficiency at pT=1 GeV, eta=-1.5 is "
              << muonEfficiency << std::endl;
    float muonEfficiency2 = m_upgrade->getMuonEfficiencyFallback(2000000., 1.5);
    std::cout << "Muon efficiency (old fallback code) at pT=2000 GeV, eta=1.5 is "
              << muonEfficiency2 << std::endl;

    float muonPtResolution = m_upgrade->getMuonPtResolution(500000., 2.0);
    float muonMSPtResolution = m_upgrade->getMuonMSPtResolution(500000., 2.0);
    std::cout << "Muon pT resolution at pT=500 GeV, eta=2.0 is "
              << muonPtResolution << " MeV (" << muonPtResolution / 500.0e3 * 100. << "%) " << std::endl;
    std::cout << "Muon MS pT resolution at pT=500 GeV, eta=2.0 is "
              << muonMSPtResolution << " MeV (" << muonMSPtResolution / 500.0e3 * 100. << "%) " << std::endl;
    muonPtResolution = m_upgrade->getMuonPtResolution(2000000., 1.5);
    std::cout << "Muon pT resolution at pT=2000 GeV, eta=1.5 is "
              << muonPtResolution << " MeV (" << muonPtResolution / 2000.0e3 * 100. << "%) " << std::endl;
    muonPtResolution = m_upgrade->getMuonPtResolution(12000., 0.15);
    std::cout << "Muon pT resolution at pT=12 GeV, eta=0.15 is "
              << muonPtResolution << " MeV (" << muonPtResolution / 12.0e3 * 100. << "%) " << std::endl;

    float muonQOverPtResolution = m_upgrade->getMuonQOverPtResolution(12000., 0.15);
    std::cout << "Muon qOverPT resolution at pT=12 GeV, eta=0.15 is "
              << muonQOverPtResolution << "/MeV" << std::endl;
  }

  if (testElectrons > 0) {
    std::cout << "electron efficiency at 40 GeV = " << m_upgrade->getElectronEfficiency(40000., 1.0) << std::endl;
    std::cout << "electron efficiency at 100 GeV = " << m_upgrade->getElectronEfficiency(100000., 1.0) << std::endl;
  }
  if (testMuons > 0)  std::cout << "muon efficiency = " << m_upgrade->getMuonEfficiency(40000., 2.8) << std::endl;

  if (testJets>0) {

    printf("\n====================\n= Jet related tests\n====================\n");
    std::vector<TLorentzVector> pileupJets = m_upgrade->getPileupJets();
    printf("  First event has %lu PU jets over threshold\n", pileupJets.size());
    printf("  JVT eff for pT=40 GeV, eta=1 is %.1f%% for a HS jet, %.1f%% for a PU jet\n",
           m_upgrade->getJVTeff_HSjet(40e3, 1.0) * 100, m_upgrade->getJVTeff_PUjet(40e3, 1.0) * 100);
    printf("  JVT eff for pT=40 GeV, eta=3 is %.1f%% for a HS jet, %.1f%% for a PU jet\n",
           m_upgrade->getJVTeff_HSjet(40e3, 3.0) * 100, m_upgrade->getJVTeff_PUjet(40e3, 3.0) * 100);
    printf("  JVT eff for pT=40 GeV, eta=4 is %.1f%% for a HS jet, %.1f%% for a PU jet\n",
           m_upgrade->getJVTeff_HSjet(40e3, 4.0) * 100, m_upgrade->getJVTeff_PUjet(40e3, 4.0) * 100);
    printf("  Note: outside tracking acceptance (|eta|>3.8) and jet pT 20-100 GeV, no JVT cut is applied.\n");
    printf("        Here code should return eff = 100%% (all jets pass).\n\n");

    printf("  Testing small-R jet smearing of jet with trut pT = 40 GeV 5 times\n");
    Upgrade::Jet hsJet;
    hsJet.SetPtEtaPhiM(40e3, 1, 0, 5e3);
    for (int i = 0; i < 5; ++i)
      printf("    Smeared jet pT: %.1f GeV\n", m_upgrade->getSmearedJet(hsJet).Pt() / 1000);

    Upgrade::Jets hsJets;
    hsJets.push_back(hsJet);
    printf("  Testing master smearing function 10 times for one 40 GeV HS TopoEM jet.\n");
    for (int i = 0; i < 10; ++i)
      printf("    Number of jets passing selection: %lu\n",
             m_upgrade->getSmearedJets(hsJets).size());
    printf("  Testing master smearing function 10 times for one 40 GeV HS PFlow jet.\n");
    m_upgrade->setJetAlgorithm(UpgradePerformanceFunctions::PFlow);
    m_upgrade->printJetSmearingSettings();
    for (int i = 0; i < 10; ++i)
      printf("    Number of jets passing selection: %lu\n",
             m_upgrade->getSmearedJets(hsJets).size());

    // Test jet mass smearing
    printf("\nLarge-R jet smearing. Truth pT=400, m=100 GeV\n");
    TLorentzVector lRjet_truth;
    lRjet_truth.SetPtEtaPhiM(400e3, 0, 0, 100e3);
    for (auto topo : {UpgradePerformanceFunctions::Multijet, UpgradePerformanceFunctions::Wprime, UpgradePerformanceFunctions::Zprime}) {
      TString topoStr = topo == 0 ? "multijet" : topo == UpgradePerformanceFunctions::Wprime ? "Wprime" : "Zprime";
      TLorentzVector lRjet = m_upgrade->getSmearedLargeRjet(lRjet_truth, topo);
      printf("  pT=%6.1f GeV, m=%6.1f GeV for %s topology\n", lRjet.Pt() / 1000, lRjet.M() / 1000, topoStr.Data());
    }

    printf("\n  Done jet tests.\n===============\n\n");

  }

  if (testFlavourTagging > 0) {
    printf("\n====================\n= Flavour Tagging tests\n====================\n");
    TString tagger="ip3dsv1"; //"mv2c10";
    bool testTC=false;

    std::cout << "b-tagging efficiency for pT=40 GeV, eta=1.0 (no TC) is "
	      << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'B', tagger, 70, false) << std::endl;
    std::cout << "c-tagging efficiency for pT=40 GeV, eta=1.0 (no TC) is "
	      << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'C', tagger, 70, false) << std::endl;
    if (testTC) {
      std::cout << "b-tagging efficiency for pT=40 GeV, eta=1.0 (w/ TC) is "
		<< m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'B', tagger, 70, true) << std::endl;
      std::cout << "c-tagging efficiency for pT=40 GeV, eta=1.0 (w/ TC) is "
		<< m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'C', tagger, 70, true) << std::endl;
    }
    
    std::cout << "b-tagging 85 efficiency for pT=40 GeV, eta=1.0 (no TC) is "
	      << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'B', tagger, 85, false) << std::endl;
    std::cout << "c-tagging 85 efficiency for pT=40 GeV, eta=1.0 (no TC) is "
	      << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'C', tagger, 85, false) << std::endl;
    std::cout << "l-tagging 85 efficiency for pT=40 GeV, eta=1.0 (no TC) is "
	      << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'L', tagger, 85, false) << std::endl;
    std::cout << "p-tagging 85 efficiency for pT=40 GeV, eta=1.0 (no TC) is "
	      << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'P', tagger, 85, false) << std::endl;

    if (testTC) {
      std::cout << "b-tagging 85 efficiency for pT=40 GeV, eta=1.0 (w/ TC) is "
		<< m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'B', tagger, 85, true) << std::endl;
      std::cout << "c-tagging 85 efficiency for pT=40 GeV, eta=1.0 (w/ TC) is "
		<< m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'C', tagger, 85, true) << std::endl;    
      std::cout << "l-tagging 85 efficiency for pT=40 GeV, eta=1.0 (w/ TC) is "
		<< m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'L', tagger, 85, true) << std::endl;
      std::cout << "p-tagging 85 efficiency for pT=40 GeV, eta=1.0 (w/ TC) is "
		<< m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'P', tagger, 85, true) << std::endl;
    }
    
    std::cout << "b-tagging 85 efficiency for pT=270 GeV, eta=1.0 is "
	      << m_upgrade->getFlavourTagEfficiency(270000., 1.0, 'B', tagger, 85, testTC) << std::endl;
    std::cout << "c-tagging 85 efficiency for pT=270 GeV, eta=1.0 is "
	      << m_upgrade->getFlavourTagEfficiency(270000., 1.0, 'C', tagger, 85, testTC) << std::endl;
    std::cout << "l-tagging 85 efficiency for pT=270 GeV, eta=1.0 is "
	      << m_upgrade->getFlavourTagEfficiency(270000., 1.0, 'L', tagger, 85, testTC) << std::endl;
    std::cout << "p-tagging 85 efficiency for pT=270 GeV, eta=1.0 is "
	      << m_upgrade->getFlavourTagEfficiency(270000., 1.0, 'P', tagger, 85, testTC) << std::endl;

    std::cout << "b-tagging 85 efficiency for pT=40 GeV, eta=3.5 is "
	      << m_upgrade->getFlavourTagEfficiency(40000., 3.5, 'B', tagger, 85, testTC) << std::endl;
    std::cout << "c-tagging 85 efficiency for pT=40 GeV, eta=3.5 is "
	      << m_upgrade->getFlavourTagEfficiency(40000., 3.5, 'C', tagger, 85, testTC) << std::endl;
    std::cout << "l-tagging 85 efficiency for pT=40 GeV, eta=3.5 is "
	      << m_upgrade->getFlavourTagEfficiency(40000., 3.5, 'L', tagger, 85, testTC) << std::endl;
    std::cout << "p-tagging 85 efficiency for pT=40 GeV, eta=3.5 is "
	      << m_upgrade->getFlavourTagEfficiency(40000., 3.5, 'P', tagger, 85, testTC) << std::endl;
    
    std::cout << "Four-jet trigger efficiency for pT=100 GeV, eta=1.0 is "
	      << m_upgrade->getFourJetTriggerEfficiency(100000., 1.0,
							100000., 1.0,
							100000., 1.0,
							100000., 1.0)
	      << std::endl;

  }

  if (testElectrons > 0) {

    std::cout << "Single electron trigger efficiency for pT=20 GeV, eta=1.0 is "
              << m_upgrade->getSingleElectronTriggerEfficiency(20000., 1.0) << std::endl;
    std::cout << "Dielectron trigger efficiency for pT=20 GeV, eta=1.0 is "
              << m_upgrade->getDiElectronTriggerEfficiency(20000., 20000., 1.0, 1.0) << std::endl;
    // Test electron charge-flip
    std::vector<double> PtBins  = {30000, 70000, 200000, 1e7};
    std::vector<double> EtaBins = { -0.5, 0.5, 1.0, 1.4, 2.2};

    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "Testing electron charge-flip rates for Phase II" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    for (auto pt : PtBins) {
      std::cout << "pt " << pt / 1000. << " GeV: ";
      for (auto eta : EtaBins) {
        std::cout << " | eta: " << eta << " "
                  << m_upgrade->getElectronChargeFlipProb(pt, eta)
                  << " | ";
      }
      std::cout << std::endl;
    }
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "Testing electron charge-flip rates for Run 2" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << m_upgrade->getElectronChargeFlipProb(35000, 1.0) << std::endl;
    if (m_upgrade->getElectronChargeFlipProb(35000, 1.0) == -1) {
      std::cout << "Good, it failed.. Now set the path to default." << std::endl;
    }

    for (auto pt : PtBins) {
      std::cout << "pt " << pt / 1000. << " GeV: ";
      for (auto eta : EtaBins) {
        std::cout << " | eta: " << eta << " "
                  << m_upgrade->getElectronChargeFlipProb(pt, eta)
                  << " | ";
      }
      std::cout << std::endl;
    }

    std::cout << "---------------------------------------" << std::endl;
    std::cout << "getElectronSmearedEnergy test in Run 2." << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    for (auto pt : PtBins) {
      std::cout << "pt " << pt / 1000. << " GeV: ";
      for (auto eta : EtaBins) {
        std::cout << " | eta: " << eta << " "
                  << m_upgrade->getElectronSmearedEnergy(pt, eta)
                  << " | ";
      }
      std::cout << std::endl;
    }
  }

  return 0;
}
