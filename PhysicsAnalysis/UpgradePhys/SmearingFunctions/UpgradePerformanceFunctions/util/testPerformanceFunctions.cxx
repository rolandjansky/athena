/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TROOT.h"
#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

int main() {
  std::cout << "This is a test." << std::endl;
  // This hack is needed to force dictionary loading in ROOT 5
  gROOT->ProcessLine("#include <vector>");

  UpgradePerformanceFunctions *m_upgrade = new UpgradePerformanceFunctions();
  m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
  m_upgrade->setAvgMu(200.);
  std::cout << "Layout is " << m_upgrade->getLayout() 
	    << ", and mu value is " << m_upgrade->getAvgMu() << std::endl;
  m_upgrade->setElectronWorkingPoint(UpgradePerformanceFunctions::looseElectron);
  m_upgrade->setElectronRandomSeed(1);
  m_upgrade->setMuonWorkingPoint(UpgradePerformanceFunctions::tightMuon);
  m_upgrade->setPhotonWorkingPoint(UpgradePerformanceFunctions::tightPhoton);
  m_upgrade->loadMETHistograms("UpgradePerformanceFunctions/sumetPU_mu200_ttbar_gold.root");
  m_upgrade->setJetRandomSeed(1);
  m_upgrade->setPileupRandomSeed(1);
  m_upgrade->setPileupUseTrackConf(true);
  m_upgrade->setUseHGTD1(true);
  //m_upgrade->setUseHGTD0(false);
  m_upgrade->setPileupJetPtThresholdMeV(30000.);
  m_upgrade->setPileupEfficiencyScheme(UpgradePerformanceFunctions::PU);
  m_upgrade->setPileupEff(0.02);
  // m_upgrade->setHGTDPUEff(0.1);
  // m_upgrade->setHGTDHSEff(0.9);
  //  m_upgrade->setPileupTemplatesPath("/afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData/UpgradePerformanceFunctions/");
  m_upgrade->setPileupTemplatesPath("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/UpgradePerformanceFunctions/");
  m_upgrade->initPhotonFakeHistograms("UpgradePerformanceFunctions/PhotonFakes.root");

  // This is just a test of some methods without realistic input.
  std::cout << "ElectronToPhotonFakeRate at pT=50 GeV is " 
	    << m_upgrade->getElectronToPhotonFakeRate(50.*1000., 0.0) 
  	    << std::endl;

  std::cout << "Photon efficiency at pT=50 GeV is "
	    << m_upgrade->getPhotonEfficiency(50.*1000.) << std::endl;
  std::cout << "Photon efficiency at pT=10 GeV is "
	    << m_upgrade->getPhotonEfficiency(10.*1000.) << std::endl;
  std::cout << "Jet-to-photon fake rate at pT=50 GeV is "
	    << m_upgrade->getPhotonFakeRate(50.*1000.) << std::endl;
  std::cout << "Jet-to-photon fake rate at pT=10 GeV is "
	    << m_upgrade->getPhotonFakeRate(10.*1000.) << std::endl;
  std::cout << "Photon pileup fake rate at pT=50 GeV is "
	    << m_upgrade->getPhotonPileupFakeRate(50.*1000.) << std::endl;

  // Test of the photon fake energy rescaling
  std::cout << "Photon fake rescaled ET at ET=50 GeV is " 
	    << m_upgrade->getPhotonFakeRescaledET(50.*1000.) << " MeV" << std::endl;
  std::cout << "Photon pileup fake rescaled ET=50 GeV is " 
	    << m_upgrade->getPhotonPileupFakeRescaledET(50.*1000.) << " MeV" << std::endl;

  float eventMETreso = m_upgrade->getMETResolution(100000.0);
  std::cout << "MET resolution test result is " << eventMETreso << std::endl;

  std::cout << "Muon trigger efficiency at pT=200 GeV, eta=1.0 is "
	    << m_upgrade->getSingleMuonTriggerEfficiency(200000.,1.0)
	    << std::endl;
  
  std::cout << "Muon trigger efficiency at pT=200 GeV, eta=1.1 is "
	    << m_upgrade->getSingleMuonTriggerEfficiency(200000.,1.1)
	    << std::endl;

  float muonEfficiency = m_upgrade->getMuonEfficiency(2000000., 1.5);
  std::cout << "Muon efficiency at pT=2000 GeV, eta=1.5 is "
	    << muonEfficiency << std::endl;

  float muonPtResolution = m_upgrade->getMuonPtResolution(500000., 2.0);
  std::cout << "Muon pT resolution at pT=500 GeV, eta=2.0 is " 
	    << muonPtResolution << " MeV" << std::endl;
  muonPtResolution = m_upgrade->getMuonPtResolution(2000000., 1.5);
  std::cout << "Muon pT resolution at pT=2000 GeV, eta=1.5 is "
	    << muonPtResolution << " MeV" << std::endl;
  muonPtResolution = m_upgrade->getMuonPtResolution(12000., 0.15);
  std::cout << "Muon pT resolution at pT=12 GeV, eta=0.15 is "
	    << muonPtResolution << " MeV" << std::endl;

  float muonQOverPtResolution = m_upgrade->getMuonQOverPtResolution(12000., 0.15);
  std::cout << "Muon qOverPT resolution at pT=12 GeV, eta=0.15 is "
	    << muonQOverPtResolution << "/MeV" << std::endl;

  m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
  std::cout << "electron efficiency (gold) = " << m_upgrade->getElectronEfficiency(40000., 1.0) << std::endl;
  std::cout << "muon efficiency (gold) = " << m_upgrade->getMuonEfficiency(40000., 2.8) << std::endl;
  m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
  std::cout << "electron efficiency (silver) = " << m_upgrade->getElectronEfficiency(40000., 1.0) << std::endl;
  std::cout << "muon efficiency (silver) = " << m_upgrade->getMuonEfficiency(40000., 2.8) << std::endl;
  m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
  std::cout << "electron efficiency (bronze) = " << m_upgrade->getElectronEfficiency(40000., 1.0) << std::endl;
  std::cout << "muon efficiency (bronze) = " << m_upgrade->getMuonEfficiency(40000., 2.8) << std::endl;
  m_upgrade->setLayout(UpgradePerformanceFunctions::gold);

  std::vector<TLorentzVector> pileupJets = m_upgrade->getPileupJets();
  std::cout << "Found " <<  pileupJets.size() << " pileup jets above threshold" << std::endl;
  std::cout << "Track Confirmation Efficiency for PU pT=40 GeV, eta=1.0 is "
	    << m_upgrade->getTrackJetConfirmEff(40000., 1.0, "PU") << std::endl;
  std::cout << "Track Confirmation Efficiency for HS pT=40 GeV, eta=1.0 is "
	    << m_upgrade->getTrackJetConfirmEff(40000., 1.0, "HS") << std::endl;
  std::cout << "Track Confirmation Efficiency for PU pT=40 GeV, eta=4.5 is "
	    << m_upgrade->getTrackJetConfirmEff(40000., 4.5, "PU") << std::endl;
  std::cout << "Track Confirmation Efficiency for HS pT=40 GeV, eta=4.5 is "
	    << m_upgrade->getTrackJetConfirmEff(40000., 4.5, "HS") << std::endl;

  // Temporarily change layout for flavour tagging
  m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
  m_upgrade->setFlavourTaggingCalibrationFilename("UpgradePerformanceFunctions/flavor_tags_v1.1.root");

  std::cout << "b-tagging efficiency for pT=40 GeV, eta=1.0 (no TC) is " 
	    << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'B', "mv1", 70) << std::endl;
  std::cout << "c-tagging efficiency for pT=40 GeV, eta=1.0 (no TC) is " 
	    << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'C', "mv1", 70) << std::endl;
  std::cout << "b-tagging efficiency for pT=40 GeV, eta=1.0 (w/ TC) is " 
	    << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'B', "mv1", 70, true) << std::endl;
  std::cout << "c-tagging efficiency for pT=40 GeV, eta=1.0 (w/ TC) is " 
	    << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'C', "mv1", 70, true) << std::endl;

  std::cout << "b-tagging 85 efficiency for pT=40 GeV, eta=1.0 (no TC) is " 
	    << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'B', "mv1", 85) << std::endl;
  std::cout << "c-tagging 85 efficiency for pT=40 GeV, eta=1.0 (no TC) is " 
	    << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'C', "mv1", 85) << std::endl;
  std::cout << "b-tagging 85 efficiency for pT=40 GeV, eta=1.0 (w/ TC) is " 
	    << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'B', "mv1", 85, true) << std::endl;
  std::cout << "c-tagging 85 efficiency for pT=40 GeV, eta=1.0 (w/ TC) is " 
	    << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'C', "mv1", 85, true) << std::endl;

  std::cout << "Single electron trigger efficiency for pT=20 GeV, eta=1.0 is "
	    << m_upgrade->getSingleElectronTriggerEfficiency(20000., 1.0) << std::endl;
  std::cout << "Dielectron trigger efficiency for pT=20 GeV, eta=1.0 is "
	    << m_upgrade->getDiElectronTriggerEfficiency(20000., 20000., 1.0, 1.0) << std::endl;

  std::cout << "Four-jet trigger efficiency for pT=100 GeV, eta=1.0 is "
	    << m_upgrade->getFourJetTriggerEfficiency(100000., 1.0,
						      100000., 1.0,
						      100000., 1.0,
						      100000., 1.0) 
	    << std::endl;

  return 0;
}
