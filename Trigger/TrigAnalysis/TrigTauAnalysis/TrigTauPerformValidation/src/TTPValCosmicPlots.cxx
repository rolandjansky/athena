/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Make standard set of selection plots
#include <iostream>
#include <vector>
#include <math.h>

#include "TROOT.h" // needed for gROOT
#include "TFile.h"

#include "TrigTauPerformValidation/TTPValCosmicPlots.h"
#include "TrigTauPerformNtuple/TrigTauEvent.h"
#include "TrigTauPerformAnalysis/TrigTauTrigChainList.h"

TTPValCosmicPlots::TTPValCosmicPlots(TTPValCosmicPlots::PLOTTYPE type) : hh(0), _plotType(type),
			 _fix(TrigTauBugFix::TTP14, TrigTauBugFix::RDO),
			 _minL2EmEt(-1000000.), _maxL2EmEt(1000000.),
					 _requireAnyL2Negative(false),
				       	 _L2NegThreshold(0.),
				       	 _requireRecoMask(0),
			       		 _requireSeedMask(0),
					 _thbins(100), _thstart(0),
					 _nL1E(50), _loL1E(0.), _hiL1E(50.),
					 _nL2E(65), _loL2E(-15.), _hiL2E(50.),
					 _nEFE(60), _loEFE(-10.), _hiEFE(50.),
					 _nEta(61), _loEta(-3.05), _hiEta(3.05)
{

  // Different numbers of histograms produced depending on plot type
  switch (_plotType) {
    case TTPValCosmicPlots::DEFAULT:
      hh = new TrigTauHistHelper(6);
      break;

    case TTPValCosmicPlots::RECO_BASED:
      hh = new TrigTauHistHelper(4);
      break;

  default:
    break;
  }

  _currentWeight = 1.;
  _defVersion = TrigTauSelectionManager::MENU1400;
  _selManager = 0;
  _hltAuthorType = TrigTauFlags::ANYAUTHOR;
  _nEvent.clear();
  setPhiBins(65, -M_PI-M_PI/64., M_PI+M_PI/64.);
}

TTPValCosmicPlots::~TTPValCosmicPlots() {
  if (_selManager != 0) delete _selManager;
  if (hh != 0) delete hh;
}

void
TTPValCosmicPlots::requireMinL2EmEt(float Et) {
  _minL2EmEt = Et;
}

void
TTPValCosmicPlots::requireMaxL2EmEt(float Et) {
  _maxL2EmEt = Et;
}

void 
TTPValCosmicPlots::setL1EtBins(unsigned int nBins, float xLo, float xHi) {
  _nL1E = nBins;
  _loL1E = xLo;
  _hiL1E = xHi;
}

void 
TTPValCosmicPlots::setL2EtBins(unsigned int nBins, float xLo, float xHi) {
  _nL2E = nBins;
  _loL2E = xLo;
  _hiL2E = xHi;
}

void 
TTPValCosmicPlots::setEFEtBins(unsigned int nBins, float xLo, float xHi) {
  _nEFE = nBins;
  _loEFE = xLo;
  _hiEFE = xHi;
}

void 
TTPValCosmicPlots::setEtaBins(unsigned int nBins, float xLo, float xHi) {
  _nEta = nBins;
  _loEta = xLo;
  _hiEta = xHi;
}

void 
TTPValCosmicPlots::setPhiBins(unsigned int nBins, float xLo, float xHi) {
  _nPhi = nBins;
  _loPhi = xLo;
  _hiPhi = xHi;
}

void 
TTPValCosmicPlots::setDefaultVersion(
    TrigTauSelectionManager::MENUVERSION version)
{
  _defVersion = version;
}

void 
TTPValCosmicPlots::setSelector(TrigTauFlags::TRIGGERMENU menu) {
  if (_selManager == 0) _selManager = new TrigTauSelectionManager();
  _selManager->setMenuVersion(_defVersion);
  _selManager->setTauMenu(menu); 
}

// Clear plots and define histograms
void
TTPValCosmicPlots::clear() {

  // Delete all histograms
  hh->clear();

  // Define labels before creating histogram families
  _nEvent.clear();
  std::vector<TString> ylabel;

  switch (_plotType) {
  case TTPValCosmicPlots::DEFAULT:
    ylabel.resize(6);
    ylabel[0] = "L1 In";
    ylabel[1] = "L1 Out";
    ylabel[2] = "L2Cal Out";
    ylabel[3] = "L2Trk Out";
    ylabel[4] = "EF Out";
    ylabel[5] = "Offline";
    hh->setYAxisLabels(ylabel);
    
    for (unsigned int i=0; i<6; i++) _nEvent.name(i, ylabel[i].Data());
    
    break;
    
  case TTPValCosmicPlots::RECO_BASED:
    ylabel.resize(4);
    ylabel[0] = "Reco";
    ylabel[1] = "L1 Out";
    ylabel[2] = "L2 Out";
    ylabel[3] = "EF Out";
    hh->setYAxisLabels(ylabel);
    
    for (unsigned int i=0; i<4; i++) _nEvent.name(i, ylabel[i].Data());
    
    break;

  } 

  // Make sure we are in the main ROOT directory
  gROOT->cd();

  hh->create("hNRoi",   10, 0.0,  10., "ROI/ev");

  // L1 Plots
  hh->create("hL1Et",   _nL1E, _loL1E, _hiL1E, "L1 Et (GeV)");
  hh->create("hL1EmEt", _nL1E, _loL1E, _hiL1E, "L1 EM Et (1x2) (GeV)");
  hh->setLogY("hL1EmEt");
  hh->create("hL1HdEt", _nL1E, _loL1E, _hiL1E, "L1 Hd Et (2x2) (GeV)");
  hh->create("hL1EmHd",  _nL1E, _loL1E, _hiL1E, _nL1E, _loL1E, _hiL1E, "L1 EM Et (GeV)", "L1 Hd Et");

  hh->create("hL1EmI",  _nL1E, _loL1E, _hiL1E, "L1 EmIso (GeV)");
  hh->create("hL1HdI",  _nL1E, _loL1E, _hiL1E, "L1 HdIso (GeV)");
  hh->setLogY("hL1HdI");
  hh->create("hL1EmHdI", _nL1E, _loL1E, _hiL1E, _nL1E, _loL1E, _hiL1E, "L1 EmIso Et (GeV)", "L1 HdIso Et");

  hh->create("hL1Et44", _nL1E, _loL1E, _hiL1E, "L1 Et (4x4) (GeV)");

  hh->create("hL1Eta",  _nEta, _loEta, _hiEta, "L1Eta");
  hh->create("hL1Phi",  _nPhi, _loPhi, _hiPhi, "L1Phi");

  hh->create("hL1EtaPhi",  _nEta, _loEta, _hiEta, _nPhi, _loPhi, _hiPhi, "L1Eta", "L1Phi");
  hh->create("hL1EtaEt",   _nEta, _loEta, _hiEta, _nL1E, _loL1E, _hiL1E, "L1Eta", "L1 Et (GeV)");
  hh->create("hL1EtaEmEt", _nEta, _loEta, _hiEta, _nL1E, _loL1E, _hiL1E, "L1Eta", "L1 EM Et (1x2) (GeV)");
  hh->create("hL1EtaHdEt", _nEta, _loEta, _hiEta, _nL1E, _loL1E, _hiL1E, "L1Eta", "L1 Hd Et (2x2) (GeV)");

  // L2Calo Plots
  hh->create("hL2EtCalib", _nL2E, _loL2E, _hiL2E, "L2 EtCalib (GeV)");
  hh->create("hL2EtNarrow",   _nL2E, _loL2E, _hiL2E, "L2 Et (Narrow) (GeV)");
  hh->setLogY("hL2EtNarrow");
  hh->create("hL2EtMedium",   _nL2E, _loL2E, _hiL2E, "L2 Et (Wide) (GeV)");
  hh->setLogY("hL2EtMedium");

  hh->create("hL2Em",      _nL2E, _loL2E, _hiL2E, "L2 EM Et (Narrow) (GeV)");
  hh->setLogY("hL2Em");
  hh->create("hL2Em0",     _nL2E, _loL2E, _hiL2E, "L2 EM0 Et (GeV)");
  hh->setLogY("hL2Em0");
  hh->create("hL2Em1",     _nL2E, _loL2E, _hiL2E, "L2 EM1 Et (GeV)");
  hh->setLogY("hL2Em1");
  hh->create("hL2Em2",     _nL2E, _loL2E, _hiL2E, "L2 EM2 Et (GeV)");
  hh->setLogY("hL2Em2");
  hh->create("hL2Em3",     _nL2E, _loL2E, _hiL2E, "L2 EM3 Et (GeV)");
  hh->setLogY("hL2Em3");
  hh->create("hL2EmR",      51, 0.0,  1.02, "L2 EM/Sum (Narrow)");
  hh->create("hL2Em0R",     51, 0.0,  1.02, "L2 EM0/EM");
  hh->create("hL2Em1R",     51, 0.0,  1.02, "L2 EM1/EM");
  hh->create("hL2Em2R",     51, 0.0,  1.02, "L2 EM2/EM");
  hh->create("hL2Em3R",     51, 0.0,  1.02, "L2 EM3/EM");

  hh->create("hL2Hd",      _nL2E, _loL2E, _hiL2E, "L2 HD Et (Narrow) (GeV)");
  hh->setLogY("hL2Hd");
  hh->create("hL2Hd0",     _nL2E, _loL2E, _hiL2E, "L2 HD0 Et (GeV)");
  hh->setLogY("hL2Hd0");
  hh->create("hL2Hd1",     _nL2E, _loL2E, _hiL2E, "L2 HD1 Et (GeV)");
  hh->setLogY("hL2Hd1");
  hh->create("hL2Hd2",     _nL2E, _loL2E, _hiL2E, "L2 HD2 Et (GeV)");
  hh->setLogY("hL2Hd2");
  hh->create("hL2HdR",      51, 0.0,  1.02, "L2 HD/Sum (Narrow)");
  hh->create("hL2Hd0R",     51, 0.0,  1.02, "L2 HD0/HD");
  hh->create("hL2Hd1R",     51, 0.0,  1.02, "L2 HD1/HD");
  hh->create("hL2Hd2R",     51, 0.0,  1.02, "L2 HD2/HD");

  hh->create("hL2NStrips", 50,  0.0,  50., "L2 NStrips");
  hh->setLogY("hL2NStrips");
  hh->create("hL2StrWid2", 50,  0.0,  0.5, "L2 StrWid2");
  hh->create("hL2EMRad",   50, -0.5,  0.5, "L2 EM Radius");
  hh->create("hL2IsoFrac", 50,  0.0,  2.0, "L2 IsoFrac");

  hh->create("hL2L1dEt",  50, -25, 25, "L2 Et - L1 Et");
  hh->create("hL2L1Et",   _nL1E, _loL1E, _hiL1E, _nL2E, _loL2E, _hiL2E, "L1 Et", "L2 Et");
  hh->create("hL2L1EmEt", _nL1E, _loL1E, _hiL1E, _nL2E, _loL2E, _hiL2E, "L1 EmEt", "L2 EmEt");
  hh->create("hL2L1HdEt", _nL1E, _loL1E, _hiL1E, _nL2E, _loL2E, _hiL2E, "L1 HdEt", "L2 HdEt");
  hh->create("hL2WL1Et",  _nL1E, _loL1E, _hiL1E, _nL2E, _loL2E, _hiL2E, "L1 Et", "L2 Et (Wide)");
  hh->create("hL2WL2Et",  _nL2E, _loL2E, _hiL2E, _nL2E, _loL2E, _hiL2E, "L2 Et (Narrow)", "L2 Et (Wide)");
  hh->create("hL2EmHdEt", _nL2E, _loL2E, _hiL2E, _nL2E, _loL2E, _hiL2E, "L2 EmEt (Narrow)", "L2 HdEt (Narrow)");

  hh->create("hL2EtaEt",   _nEta, _loEta, _hiEta, _nL2E, _loL2E, _hiL2E, "L2Eta", "L2 Et (Narrow)");
  hh->create("hL2EtaEmEt", _nEta, _loEta, _hiEta, _nL2E, _loL2E, _hiL2E, "L2Eta", "L2 EM Et (Narrow)");
  hh->create("hL2EtaHdEt", _nEta, _loEta, _hiEta, _nL2E, _loL2E, _hiL2E, "L2Eta", "L2 HD Et (Narrow)");

  // L2Trk Plots
  hh->create("hL2NTrk",  10,   0, 10, "L2 Tracks");
  hh->setLogY("hL2NTrk");
  hh->create("hL2NIso",   5,   0,  5, "L2 Iso Tr");
  hh->setLogY("hL2NIso");
  hh->create("hL2NSlow", 10, 0, 10, "L2 Slow Tracks");
  hh->setLogY("hL2NSlow");
  hh->create("hL2PtLead", 50, 0.0, 100., "L2 PtLead (GeV)");
  hh->setLogY("hL2PtLead");
  hh->create("hL2Charge", 7, -3.5, 3.5, "L2 Charge");
  hh->create("hL2IsoR", 20, 0., 1., "L2 Pt Iso/Core");
  hh->setLogY("hL2IsoR");
  hh->create("hL2EtTrkEM", 50, 0.0, 100., "L2 Et Trk+EM (GeV)");
  hh->create("hL2Eta",  _nEta, _loEta, _hiEta, "L2Eta");
  hh->create("hL2Phi",  _nPhi, _loPhi, _hiPhi, "L2Phi");

  hh->create("hL2EtaPhi", _nEta, _loEta, _hiEta, _nPhi, _loPhi, _hiPhi, "L2Eta", "L2Phi");

  hh->create("hL2L1dEta", 50, -0.25, 0.25, "L2-L1 dEta");
  hh->create("hL2L1dPhi", 50, -0.25, 0.25, "L2-L1 dPhi");
  hh->create("hL2L1dEtadPhi", 50, -0.25, 0.25, 50, -0.25, 0.25, "L2-L1 dEta", "dPhi");
  hh->create("hL2L1EtadEta",  _nEta, _loEta, _hiEta, 50, -0.25, 0.25, "L2 Eta", "L2-L1 dEta");
  hh->create("hL2L1PhidPhi",  _nPhi, _loPhi, _hiPhi, 50, -0.25, 0.25, "L2 Phi", "L2-L1 dPhi");

  hh->create("hL2L1Eta", _nEta, _loEta, _hiEta, _nEta, _loEta, _hiEta, "L1Eta", "L2Eta");
  hh->create("hL2L1Phi", _nPhi, _loPhi, _hiPhi, _nPhi, _loPhi, _hiPhi, "L1Phi", "L2Phi");
  hh->create("hL2L1CalEta", _nEta, _loEta, _hiEta, _nEta, _loEta, _hiEta, "L1Eta", "L2CaloEta");
  hh->create("hL2L1CalPhi", _nPhi, _loPhi, _hiPhi, _nPhi, _loPhi, _hiPhi, "L1Phi", "L2CaloPhi");

  // EF Plots
  hh->create("hEFNTrk",    10, 0, 10,  "EF Tracks");
  hh->setLogY("hEFNTrk");
  hh->create("hEFCharge",   7,-3.5, 3.5, "EF Charge");
  hh->create("hEFPtLead",  50, 0, 100., "EF PtLead (GeV)");
  hh->create("hEFEMRad",   50, -0.5, 0.5, "EF EMRad");
  hh->create("hEFIsoFrac", 100, 0, 2.0, "EF IsoFrac");
  hh->setLogY("hEFIsoFrac");
  hh->create("hEFEt",     _nEFE, _loEFE, _hiEFE, "EF Et (GeV)");
  hh->create("hEFMass",    50, 0, 10., "EF Mass (GeV)");
  hh->create("hEFEta",  _nEta, _loEta, _hiEta, "EFEta");
  hh->create("hEFPhi",  _nPhi, _loPhi, _hiPhi, "EFPhi");
  hh->create("hEFEtaPhi", _nEta, _loEta, _hiEta, _nPhi, _loPhi, _hiPhi, "EFEta", "EFPhi");

  hh->create("hEFL2dEta", 50, -0.25, 0.25, "EF-L2 dEta");
  hh->create("hEFL2dPhi", 50, -0.25, 0.25, "EF-L2 dPhi");
  hh->create("hEFL1dEtadPhi", 50, -0.25, 0.25, 50, -0.25, 0.25, "EF-L1 dEta", "dPhi");
  hh->create("hEFL2dEtadPhi", 50, -0.25, 0.25, 50, -0.25, 0.25, "EF-L2 dEta", "dPhi");
  hh->create("hEFL1Eta", _nEta, _loEta, _hiEta, _nEta, _loEta, _hiEta, "L1Eta", "EFEta");
  hh->create("hEFL1Phi", _nPhi, _loPhi, _hiPhi, _nPhi, _loPhi, _hiPhi, "L1Phi", "EFPhi");
  hh->create("hEFL2EtadEta",  _nEta, _loEta, _hiEta, 50, -0.25, 0.25, "EF Eta", "EF-L2 dEta");
  hh->create("hEFL2PhidPhi",  _nPhi, _loPhi, _hiPhi, 50, -0.25, 0.25, "EF Phi", "EF-L2 dPhi");

  hh->create("hEFL2dEt",  50, -25, 25, "EF Et - L2 Et");
  hh->create("hEFL2Et", _nL2E, _loL2E, _hiL2E, _nEFE, _loEFE, _hiEFE, "L2 Et", "EF Et");

  hh->create("hEFEtaEt",  _nEta, _loEta, _hiEta, _nEFE, _loEFE, _hiEFE, "EF Eta", "EF Et");


  // Reco plots
  hh->create("hReSeed",  4, 0, 4, "Seed Type");
  hh->setLogY("hReSeed");
  hh->create("hReLike", 50, 0., 20., "Reco Likelihood"); 
  hh->setLogY("hReLike");
  hh->create("hReDNN", 50, 0, 1., "Reco NN");
  hh->setLogY("hReDNN");
  hh->create("hReBits", 16, 0, 16, "Reco selectors");
  hh->setLogY("hReBits");

  hh->create("hReEt", _nEFE, _loEFE, _hiEFE, "Reco Et (GeV)");

  hh->create("hReEMC", _nEFE, _loEFE, _hiEFE, "Reco Et EM Calib");
  hh->create("hReHdC", _nEFE, _loEFE, _hiEFE, "Reco Et Hd Calib");
  hh->create("hReEMHdC", _nEFE, _loEFE, _hiEFE, _nEFE, _loEFE, _hiEFE, "Reco Et EM", "Reco Et Hd");
  hh->create("hReERatio", 50, -0.5, 2.0, "Reco EM Ratio");
  hh->create("hReEMCell", _nEFE, _loEFE, _hiEFE, "Reco Et EM Cell");
  hh->create("hReHdCell", _nEFE, _loEFE, _hiEFE, "Reco Et Hd Cell");

  hh->create("hReEMR", 20, -0.5, 1.5, "Reco EM Radius");
  hh->create("hReHdR", 20, -0.5, 1.5, "Reco Had Radius");
  hh->create("hReIso", 50, -0.5, 2.0, "Reco IsoFrac");
  hh->create("hReCent", 25, -0.5, 2.0, "Reco CentFrac");
  hh->create("hReWidth", 20, -0.5, 1.5, "Reco StripWidth");


  hh->create("hReNTrk",  10, 0, 10, "Reco Tracks");
  hh->create("hReCharge", 7, -3.5, 3.5, "Reco Charge");
  hh->create("hRePtLead",  100, 0, 100., "Reco PtLead (GeV)");
  hh->create("hReLPtLead",  100, 0, 5., "Reco log(PtLead) (GeV)");
  hh->create("hReMass",    50, 0, 10., "Reco Mass (GeV)");
  hh->create("hReRWid",    50, 0, 0.5, "Reco RWidth2");

  // Tracking plots
  hh->create("hReNPix", 16, 0, 16, "Reco Pix Hits");
  hh->create("hReNSCT", 25, 0, 25, "Reco SCT Hits");
  hh->create("hReNSi",  40, 0, 40, "Reco Pix+SCT Hits");
  hh->create("hReBlay", 10, 0, 10, "Reco Blay Hits");
  hh->create("hReNTRT", 50, 0, 50, "Reco TRT Hits");
  hh->create("hReTRTR", 20, 0., 1., "Reco TRT/Straw Hits");
  hh->create("hReChi2", 50, 0., 5., "Reco Chi2");
  hh->create("hReA0", 30, 0., 600., "Reco A0");
  hh->create("hReA0sig", 50, 0., 1000., "Reco A0/dA0");
  hh->create("hReZ0", 25, -1000., 1000., "Reco Z0");
  hh->create("hReZ0A0", 25, -1000., 1000., 30, 0., 600., "Reco Z0", "Reco A0"); 

  hh->create("hReEta",  _nEta, _loEta, _hiEta, "ReEta");
  hh->create("hRePhi",  _nPhi, _loPhi, _hiPhi, "RePhi");
  hh->create("hRePhi2", 12, -M_PI, M_PI, "RePhi");
  hh->create("hReEtaPhi", _nEta, _loEta, _hiEta, _nPhi, _loPhi, _hiPhi, "ReEta", "RePhi");
  hh->create("hReEtaEt",  _nEta, _loEta, _hiEta, _nEFE, _loEFE, _hiEFE, "ReEta", "ReEt");

  hh->create("hReEFdEta", 50, -0.25, 0.25, "Reco-EF dEta");
  hh->create("hReEFdPhi", 50, -0.25, 0.25, "Reco-EF dPhi");
  hh->create("hReL1dEtadPhi", 50, -0.25, 0.25, 50, -0.25, 0.25, "Reco-L1 dEta", "dPhi");
  hh->create("hReL2dEtadPhi", 50, -0.25, 0.25, 50, -0.25, 0.25, "Reco-L2 dEta", "dPhi");
  hh->create("hReEFdEtadPhi", 50, -0.25, 0.25, 50, -0.25, 0.25, "Reco-EF dEta", "dPhi");
  hh->create("hReEFdEt",  50, -25, 25, "Reco Et - EF Et");
  hh->create("hReL1Et", _nL1E, _loL1E, _hiL1E, _nEFE, _loEFE, _hiEFE, "L1 Et", "Reco Et");
  hh->create("hReEFEt", _nEFE, _loEFE, _hiEFE, _nEFE, _loEFE, _hiEFE, "EF Et", "Reco Et");

  hh->create("hReEFEtadEta",  _nEta, _loEta, _hiEta, 50, -0.25, 0.25, "Re Eta", "Re-EF dEta");
  hh->create("hReEFPhidPhi",  _nPhi, _loPhi, _hiPhi, 50, -0.25, 0.25, "Re Phi", "Re-EF dPhi");
  hh->create("hReL1EtadEta",  _nEta, _loEta, _hiEta, 50, -0.25, 0.25, "Re Eta", "Re-L1 dEta");
  hh->create("hReL1PhidPhi",  _nPhi, _loPhi, _hiPhi, 50, -0.25, 0.25, "Re Phi", "Re-L1 dPhi");

}

// define time history plots
void 
TTPValCosmicPlots::defineTH(float tlo, float thi) {

  _thstart = tlo;

  // Make sure we are in the main ROOT directory
  gROOT->cd();

  hh->create("thL1Et",      _thbins, 0., thi-tlo, _nL1E, _loL1E, _hiL1E, "Timestamp (h)", "L1 Et (GeV)");
  hh->create("thL2EtCalib", _thbins, 0., thi-tlo, _nL2E, _loL2E, _hiL2E, "Timestamp (h)", "L2 EtCalib (GeV)");
  hh->create("thL2EtNarrow",   _thbins, 0., thi-tlo, _nL2E, _loL2E, _hiL2E, "Timestamp (h)", "L2 Et (Narrow) (GeV)");
  hh->create("thL2NTrk",    _thbins, 0., thi-tlo, 10,   0,   10, "Timestamp (h)", "L2 Tracks");
  hh->create("thEFEt",      _thbins, 0., thi-tlo, _nEFE, _loEFE, _hiEFE, "Timestamp (h)", "EF Et (GeV)");
  hh->create("thEFNTrk",    _thbins, 0., thi-tlo, 10,   0,   10, "Timestamp (h)", "EF Tracks");

  hh->create("thL1Eta", _thbins, 0.0, thi-tlo, _nEta, _loEta, _hiEta, "Timestamp (h)", "L1 Eta");
  hh->create("thL2Eta", _thbins, 0.0, thi-tlo, _nEta, _loEta, _hiEta, "Timestamp (h)", "L2 Eta");
  hh->create("thEFEta", _thbins, 0.0, thi-tlo, _nEta, _loEta, _hiEta, "Timestamp (h)", "EF Eta");
}

// Zero all histograms
void
TTPValCosmicPlots::reset() {
  hh->reset();
  _nEvent.reset();
}

// More user friendly way to process file
void
TTPValCosmicPlots::fill(const TString& filename, double wt) {

  // Open file
  TFile* file = new TFile(filename);
  if (!file->IsOpen()) {
    std::cerr << "Can't open file " << filename << std::endl;
    return;
  }

  // Initialize tree
  TTree* tree = (TTree*)file->Get("eventTree");
  if (tree == 0) {
    std::cerr << "Can't find eventTree in " << filename << std::endl;
    return;
  }

  //  _fix.setTTPVersion(filename);

  setCurrentWeight(wt);
  fill(tree);

  delete tree;
  delete file;
}

// Process file
void
TTPValCosmicPlots::fill(TTree* tree) {

  if (tree == 0) {
    std::cerr << "TTPValCosmicPlots::fill - TTree is NULL!" << std::endl;
    return;
  }

  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);
  tree->SetBranchStatus("mcEvent*", 0);

  int nev = int(tree->GetEntries());

  // Must do time history plots here (to get time range correct)
  float tlo=0.;
  float thi=0.;

  event->reset();
  tree->GetEvent(0);
  tlo = floor(event->time_stamp/3600.);

  if (nev > 0) {
    event->reset();
    tree->GetEvent(nev-1);
    thi = ceil(event->time_stamp/3600.);

    defineTH(tlo, thi);
  }

  //  cout << "Time history lo: " << tlo << " hi: " << thi << endl;

  switch (_plotType) {
    case TTPValCosmicPlots::DEFAULT:
      fillDefault(tree, event);
      break;

    case TTPValCosmicPlots::RECO_BASED:
      fillReco(tree, event);
      break;
  }

  delete event;
}

void
TTPValCosmicPlots::fillDefault(TTree* tree, TrigTauEvent* event) {

  int nev = int(tree->GetEntries());
 
  // Loop over all events
  for(int iev = 0; iev<nev; ++iev) {

    event->reset();
    tree->GetEvent(iev);

    // Bugfix
    _fix.fix(event);

    // Analyze this event
    TrigTauTrigChainList chainList;    
    chainList.setRoiWordMatching();
    chainList.setHLTAuthor(_hltAuthorType);
    chainList.fillChainList(event);
    chainList.fillTriggerList(*_selManager);

    long itype;

    unsigned int numCh = chainList.nTrigChains();
    unsigned int numL1 = chainList.nPassL1Chains();
    unsigned int numL2Cal = chainList.nPassL2CalChains();
    unsigned int numL2Trk = chainList.nPassL2Chains();
    unsigned int numEF = chainList.nPassEFChains();

    // L1 input
    itype = 0;
    _nEvent.fill(itype, _currentWeight);
    hh->fill("hNRoi", itype, numCh, _currentWeight);
    if (numCh == 0) continue;
    for (unsigned int i=0; i<numCh; i++) {
      TrigTauTrigChain ch = chainList.trigChainEntry(i);
      fillRoiHist(event, ch, itype, _currentWeight);
    }

    // L1 output
    itype = 1;
    hh->fill("hNRoi", itype, numL1, _currentWeight);
    if (numL1 == 0) continue;
    _nEvent.fill(itype, _currentWeight);
    for (unsigned int i=0; i<numL1; i++) {
      TrigTauTrigChain ch = chainList.passL1ChainEntry(i);
      fillRoiHist(event, ch, itype, _currentWeight);
    }

    // L2Cal output
    itype = 2;
    hh->fill("hNRoi", itype, numL2Cal, _currentWeight);
    if (numL2Cal == 0) continue;

    // Extra check for L2
    bool passed = false;
    for (unsigned int i=0; i<numL2Cal; i++) {
      TrigTauTrigChain ch = chainList.passL2CalChainEntry(i);
      if (!passedExtraL2(ch)) continue;
      fillRoiHist(event, ch, itype, _currentWeight);
      passed = true;
    }

    if (!passed) continue;
    _nEvent.fill(itype, _currentWeight);

    // L2Trk output
    itype = 3;
    hh->fill("hNRoi", itype, numL2Trk, _currentWeight);
    if (numL2Trk == 0) continue;
    _nEvent.fill(itype, _currentWeight);
    for (unsigned int i=0; i<numL2Trk; i++) {
      TrigTauTrigChain ch = chainList.passL2ChainEntry(i);
      if (!passedExtraL2(ch)) continue;
      fillRoiHist(event, ch, itype, _currentWeight);
    }

    // EF output
    itype = 4;
    hh->fill("hNRoi", itype, numEF, _currentWeight);
    
    unsigned int numReco = 0;
    if (numEF == 0) continue;
    _nEvent.fill(itype, _currentWeight);
    for (unsigned int i=0; i<numEF; i++) {
      TrigTauTrigChain ch = chainList.passEFChainEntry(i);
      if (!passedExtraL2(ch)) continue;
      fillRoiHist(event, ch, 4, _currentWeight);

      // Also add on tauRec selected
      const TrigTauRecOfflineObject* tauRec = ch.tauRec;
      if (tauRec == 0) continue;
      //      if (tauRec->m_discriCut < 0.5) continue;
      numReco++;
      fillRoiHist(event, ch, 5, _currentWeight);
    }
    hh->fill("hNRoi", 5, numReco, _currentWeight);

    if (numReco > 0) _nEvent.fill(5, _currentWeight);    
  }

}

void
TTPValCosmicPlots::fillReco(TTree* tree, TrigTauEvent* event) {

  int nev = int(tree->GetEntries());
  std::cout << " Found " << nev << " events " << std::endl;

  long nRead = 0;
 
  // Loop over all events
  for(int iev = 0; iev<nev; ++iev) {

    nRead++;

    event->reset();
    tree->GetEvent(iev);

    // Bugfix
    _fix.fix(event);

    // Analyze this event
    TrigTauTrigChainList chainList;    
    chainList.setMaxL1dR(0.3);
    chainList.setMaxL2dR(0.3);
    chainList.setMaxEFdR(0.3);
    chainList.setRoiWordMatching();
    chainList.setHLTAuthor(_hltAuthorType);
    chainList.fillChainList(event);
    chainList.fillTriggerList(*_selManager);

    long itype;

    unsigned int numReco = 0;
    unsigned int numL1 = 0;
    unsigned int numL2 = 0;
    unsigned int numEF = 0;

    // Offline Reco
    for (unsigned int i=0; i<event->nTauRec(); i++) {
      const TrigTauRecOfflineObject tau = event->tauRecEntry(i);

      // Apply the reconstruction mask
      if ((_requireRecoMask != 0) && !(tau.m_accept & _requireRecoMask)) continue;

      // Apply the seed mask
      if ((_requireSeedMask != 0) && !(tau.m_seedType & _requireSeedMask)) continue;

      // Must do some gymnastics here to get a chain built properly
      // for every offline reco tau found.  

      // Try to match chain at EF, then L2, then L1.  Attach reco to
      // the end of the chain and plot this.
      TrigTauTrigChain ch;
      const TrigTauTrigChain* trigChain;

      trigChain = chainList.getEFChain(tau);
      if (trigChain == 0) trigChain = chainList.getL2Chain(tau);
      if (trigChain == 0) trigChain = chainList.getL1Chain(tau);

      // Now we have some chain (empty if no trigger matches) add the reco to the end
      if (trigChain != 0) ch = *trigChain; 
      ch.tauRec = &tau;

      // Reco
      itype = 0;
      _nEvent.fill(itype, _currentWeight);
      numReco++;
      fillRoiHist(event, ch, itype, _currentWeight);

      // L1 output
      if (ch.L1 == 0) continue;
      numL1++;
      itype = 1;
      _nEvent.fill(itype, _currentWeight);
      fillRoiHist(event, ch, itype, _currentWeight);

      // L2 output
      if (ch.L2 == 0) continue;
      numL2++;
      itype = 2;
      _nEvent.fill(itype, _currentWeight);
      fillRoiHist(event, ch, itype, _currentWeight);

      // EF output
      if (ch.EF == 0) continue;
      numEF++;
      itype = 3;
      _nEvent.fill(itype, _currentWeight);
      fillRoiHist(event, ch, itype, _currentWeight);

    }

    hh->fill("hNRoi", 0, numReco, _currentWeight);
    hh->fill("hNRoi", 1, numL1,   _currentWeight);
    hh->fill("hNRoi", 2, numL2,   _currentWeight);
    hh->fill("hNRoi", 3, numEF,   _currentWeight);
  }

  std::cout << " Read  " << nRead << " events " << std::endl;
}

bool
TTPValCosmicPlots::passedExtraL2(const TrigTauTrigChain& ch) {
  const TrigTauL2Object* L2 = ch.L2;
  if (L2 == 0) return false;

  float emEt0 = L2->m_EMEnergyNarrow[0]/TMath::CosH(L2->m_caloEta);
  float emEt1 = L2->m_EMEnergyNarrow[1]/TMath::CosH(L2->m_caloEta);
  float emEt2 = L2->m_EMEnergyNarrow[2]/TMath::CosH(L2->m_caloEta);
  float emEt3 = L2->m_EMEnergyNarrow[3]/TMath::CosH(L2->m_caloEta);
  float emEtSum = emEt0+emEt1+emEt2+emEt3;

  if (emEtSum < _minL2EmEt) return false;
  if (emEtSum > _maxL2EmEt) return false;

  if (_requireAnyL2Negative) {
    for (unsigned int i=0; i<TrigTauL2Object::NEMSAMPLES; i++)
      if (L2->m_EMEnergyMedium[i] < _L2NegThreshold) return true;
    for (unsigned int i=0; i<TrigTauL2Object::NHADSAMPLES; i++)
      if (L2->m_HadEnergyMedium[i] < _L2NegThreshold) return true;
    return false;
  }

  return true;
}

void
TTPValCosmicPlots::fillRoiHist(const TrigTauEvent* ev, 
			     const TrigTauTrigChain& ch, 
			     unsigned int type,
			     double wt) {

  const TrigTauL1Object* L1 = ch.L1;

  if (L1 != 0) {

  hh->fill("hL1Et",   type, L1->m_TauEnergy/1000., wt);
  hh->fill("hL1EmEt", type, L1->m_EmEnergy/1000., wt);
  hh->fill("hL1HdEt", type, L1->m_HdCore/1000., wt);
  hh->fill("hL1EmI",  type, L1->m_EmIsol/1000., wt);
  hh->fill("hL1HdI",  type, L1->m_HdIsol/1000., wt);
  hh->fill("hL1Et44",  type, L1->Et()/1000., wt);

  hh->fill("hL1EmHd", type, L1->m_EmEnergy/1000., L1->m_HdCore/1000., wt);
  hh->fill("hL1EmHdI", type, L1->m_EmIsol/1000., L1->m_HdIsol/1000., wt);

  hh->fill("hL1Eta",  type, L1->Eta(), wt); 
  hh->fill("hL1Phi",  type, L1->Phi(), wt);
  hh->fill("hL1EtaPhi", type, L1->Eta(), L1->Phi(), wt);

  hh->fill("hL1EtaEt", type, L1->Eta(), L1->m_TauEnergy/1000., wt);
  hh->fill("hL1EtaEmEt", type, L1->Eta(), L1->m_EmEnergy/1000., wt);
  hh->fill("hL1EtaHdEt", type, L1->Eta(), L1->m_HdCore/1000., wt);

  // Time histories
  hh->fill("thL1Et",  type, ev->time_stamp/3600.-_thstart, L1->m_TauEnergy/1000., wt);
  hh->fill("thL1Eta", type, ev->time_stamp/3600.-_thstart, L1->Eta(), wt);
  }
 
  const TrigTauL2Object* L2 = ch.L2;
  if (L2 != 0) {

  hh->fill("hL2EtCalib", type, L2->etClusCalib()/1000., wt);
  hh->fill("hL2EtNarrow",   type, L2->etNarrow()/1000., wt);

  float emEt0 = L2->m_EMEnergyNarrow[0]/1000./TMath::CosH(L2->m_caloEta);
  float emEt1 = L2->m_EMEnergyNarrow[1]/1000./TMath::CosH(L2->m_caloEta);
  float emEt2 = L2->m_EMEnergyNarrow[2]/1000./TMath::CosH(L2->m_caloEta);
  float emEt3 = L2->m_EMEnergyNarrow[3]/1000./TMath::CosH(L2->m_caloEta);
  float emEtSum = emEt0+emEt1+emEt2+emEt3;

  hh->fill("hL2Em",      type, emEtSum, wt);
  hh->fill("hL2EmR",     type, (L2->etNarrow() > 0 ? 1000.*emEtSum/L2->etNarrow() : 0), wt);
  hh->fill("hL2Em0",     type, emEt0, wt);
  hh->fill("hL2Em1",     type, emEt1, wt);
  hh->fill("hL2Em2",     type, emEt2, wt);
  hh->fill("hL2Em3",     type, emEt3, wt);
  if (emEtSum>0) {
    hh->fill("hL2Em0R",    type, emEt0/emEtSum, wt);
    hh->fill("hL2Em1R",    type, emEt1/emEtSum, wt);
    hh->fill("hL2Em2R",    type, emEt2/emEtSum, wt);
    hh->fill("hL2Em3R",    type, emEt3/emEtSum, wt);
  }

  float hdEt0 = L2->m_HadEnergyNarrow[0]/1000./TMath::CosH(L2->m_caloEta);
  float hdEt1 = L2->m_HadEnergyNarrow[1]/1000./TMath::CosH(L2->m_caloEta);
  float hdEt2 = L2->m_HadEnergyNarrow[2]/1000./TMath::CosH(L2->m_caloEta);
  float hdEtSum = hdEt0+hdEt1+hdEt2;

  hh->fill("hL2Hd",      type, hdEtSum, wt);
  hh->fill("hL2HdR",     type, (L2->etNarrow() > 0 ? 1000*hdEtSum/L2->etNarrow() : 0), wt);
  hh->fill("hL2Hd0",     type, hdEt0, wt);
  hh->fill("hL2Hd1",     type, hdEt1, wt);
  hh->fill("hL2Hd2",     type, hdEt2, wt);
  if (hdEtSum>0) {
    hh->fill("hL2Hd0R",    type, hdEt0/hdEtSum, wt);
    hh->fill("hL2Hd1R",    type, hdEt1/hdEtSum, wt);
    hh->fill("hL2Hd2R",    type, hdEt2/hdEtSum, wt);
  }

  hh->fill("hL2NStrips", type, L2->m_nStrips, wt);
  hh->fill("hL2StrWid2", type, L2->m_stripWidth2, wt);
  hh->fill("hL2EMRad",   type, L2->m_EMRadius, wt);
  hh->fill("hL2IsoFrac", type, L2->m_isoFrac, wt);


  // L2 track information now in L2 object correctly
  hh->fill("hL2NTrk",   type, L2->m_nCoreTracks, wt);
  hh->fill("hL2NIso",   type, L2->m_nIsoTracks,  wt);
  hh->fill("hL2NSlow",  type, L2->m_nSlowTracks, wt);
  hh->fill("hL2PtLead", type, L2->m_leadingTrackPt/1000., wt);
  if (L2->m_scalarPtSumCore > 0) {
    hh->fill("hL2IsoR",   type, L2->m_scalarPtSumIso/L2->m_scalarPtSumCore, wt);
  } else {
    hh->fill("hL2IsoR",   type, 99., wt);
  }

  hh->fill("hL2Charge", type, L2->m_charge, wt);

  float emEt = L2->m_EMEnergyWide[0]+L2->m_EMEnergyWide[1]+L2->m_EMEnergyWide[2];
  emEt /= TMath::CosH(L2->m_caloEta);
  
  TLorentzVector trkEMSum;
  trkEMSum.SetPtEtaPhiM(emEt, L2->m_caloEta, L2->m_caloPhi, 0.);
  trkEMSum += L2->m_3fastest;
  hh->fill("hL2EtTrkEM", type, trkEMSum.Et()/1000., wt);

  hh->fill("hL2Eta",    type, L2->Eta(), wt);
  hh->fill("hL2Phi",    type, L2->Phi(), wt);
  hh->fill("hL2EtaPhi", type, L2->Eta(), L2->Phi(), wt);
  hh->fill("hL2L1Eta", type, L1->Eta(), L2->Eta(), wt);
  hh->fill("hL2L1Phi", type, L1->Phi(), L2->Phi(), wt);

  hh->fill("hL2L1CalEta", type, L1->Eta(), L2->m_caloEta, wt);
  hh->fill("hL2L1CalPhi", type, L1->Phi(), L2->m_caloPhi, wt);

  hh->fill("hL2L1dEta", type, L2->Eta()-L1->Eta(), wt);
  hh->fill("hL2L1dPhi", type, L2->DeltaPhi(*L1),   wt);
  hh->fill("hL2L1dEtadPhi", type, L2->Eta()-L1->Eta(), L2->DeltaPhi(*L1), wt);
  hh->fill("hL2L1EtadEta", type, L2->Eta(), L2->Eta()-L1->Eta(), wt);
  hh->fill("hL2L1PhidPhi", type, L2->Phi(), L2->DeltaPhi(*L1), wt);

  hh->fill("hL2L1dEt", type, L2->etNarrow()/1000. - L1->m_TauEnergy/1000., wt);
  hh->fill("hL2L1Et", type,  L1->m_TauEnergy/1000., L2->etNarrow()/1000., wt);
  hh->fill("hL2L1EmEt", type,  L1->m_EmEnergy/1000., emEtSum, wt);
  hh->fill("hL2L1HdEt", type,  L1->m_HdCore/1000.,   hdEtSum, wt);

  hh->fill("hL2EtaEt", type, L2->Eta(), L2->etNarrow()/1000., wt);
  hh->fill("hL2EtaEmEt", type, L2->Eta(), emEtSum, wt);
  hh->fill("hL2EtaHdEt", type, L2->Eta(), hdEtSum, wt);


  float etMedium = 0.;
  for (unsigned int i=0; i<TrigTauL2Object::NEMSAMPLES; i++)
    etMedium += L2->m_EMEnergyMedium[i];
  for (unsigned int i=0; i<TrigTauL2Object::NHADSAMPLES; i++)
    etMedium += L2->m_HadEnergyMedium[i];
  etMedium /= cosh(L2->m_caloEta);

  hh->fill("hL2WL1Et", type, L1->m_TauEnergy/1000., etMedium/1000., wt);
  hh->fill("hL2WL2Et", type, L2->etNarrow()/1000., etMedium/1000., wt);
  hh->fill("hL2EmHdEt", type, emEtSum, hdEtSum, wt);

  hh->fill("hL2EtMedium", type, etMedium/1000., wt);

  // Check for weirdness
  //  float dphi = fabs(L2->DeltaPhi(*L1));
  //  if (dphi > 0.25 && dphi < 2.85) {
  //    cout << ">>>" << ev->runNumber << " " << ev->eventNumber << " has dPhi(L2-L1) " << dphi << " and L2 phi " << L2->Phi() << endl;
  //  }
 
  // Time histories
  hh->fill("thL2EtCalib", type, ev->time_stamp/3600.-_thstart, L2->etClusCalib()/1000., wt);
  hh->fill("thL2EtNarrow",   type, ev->time_stamp/3600.-_thstart, L2->etNarrow()/1000., wt);
  hh->fill("thL2NTrk",    type, ev->time_stamp/3600.-_thstart, L2->m_nCoreTracks, wt);
  hh->fill("thL2Eta",     type, ev->time_stamp/3600.-_thstart, L2->Eta(), wt);
  }

  const TrigTauRecObject* EF = ch.EF;
  if (EF != 0) {

  hh->fill("hEFNTrk", type, EF->nTracks(), wt);
  hh->fill("hEFCharge", type, EF->charge(), wt);
  if (EF->leadingTrack() != 0)
    hh->fill("hEFPtLead", type, EF->leadingTrack()->Pt()/1000., wt);
  hh->fill("hEFEMRad", type, EF->m_EMRadius, wt);
  hh->fill("hEFIsoFrac", type, EF->m_isoFrac, wt);
  hh->fill("hEFEt", type, EF->Et()/1000., wt);
  //  hh->fill("hEFMass", type, EF->Mass()/1000., wt);
  hh->fill("hEFEta", type, EF->Eta(), wt);
  hh->fill("hEFPhi", type, EF->Phi(), wt);
  hh->fill("hEFEtaPhi", type, EF->Eta(), EF->Phi(), wt);

  hh->fill("hEFL2dEta", type, EF->Eta()-L2->Eta(), wt);
  hh->fill("hEFL2dPhi", type, EF->DeltaPhi(*L2),   wt);
  hh->fill("hEFL2dEtadPhi", type, EF->Eta()-L2->Eta(), EF->DeltaPhi(*L2), wt);
  hh->fill("hEFL1dEtadPhi", type, EF->Eta()-L1->Eta(), EF->DeltaPhi(*L1), wt);

  hh->fill("hEFL2EtadEta", type, EF->Eta(), EF->Eta()-L2->Eta(), wt);
  hh->fill("hEFL2PhidPhi", type, EF->Phi(), EF->DeltaPhi(*L2), wt);

  hh->fill("hEFL1Eta", type, L1->Eta(), EF->Eta(), wt);
  hh->fill("hEFL1Phi", type, L1->Phi(), EF->Phi(), wt);

  hh->fill("hEFL2dEt", type, EF->Et()/1000. - L2->etNarrow()/1000., wt);
  hh->fill("hEFL2Et", type,  L2->etNarrow()/1000., EF->Et()/1000., wt);

  hh->fill("hEFEtaEt", type, EF->Eta(), EF->Et()/1000., wt);

  // Check for weirdness
  //  dphi = fabs(EF->DeltaPhi(*L1));
  //  if (dphi > 0.25 && dphi < 2.85) {
  //    cout << ">>>" << ev->runNumber << " " << ev->eventNumber << " has dPhi(EF-L1) " << dphi << " and EF phi " << EF->Phi() << endl;
  //  }

  // Time histories
  hh->fill("thEFNTrk", type, ev->time_stamp/3600.-_thstart, EF->nTracks(), wt);
  hh->fill("thEFEt",   type, ev->time_stamp/3600.-_thstart, EF->Et()/1000., wt);
  hh->fill("thEFEta",  type, ev->time_stamp/3600.-_thstart, EF->Eta(), wt);
  }

  const TrigTauRecOfflineObject* tauRec = ch.tauRec;
  if (tauRec == 0) return;

  hh->fill("hReSeed", type, tauRec->m_seedType, wt);
  hh->fill("hReEt", type, tauRec->Et()/1000., wt);

  hh->fill("hReNTrk", type, tauRec->nTracks(), wt);
  hh->fill("hReCharge", type, tauRec->charge(), wt);

  const TrigTauTrack* tr = tauRec->leadingTrack();
  if (tr != 0) {
    hh->fill("hRePtLead", type, tr->Pt()/1000., wt);
    hh->fill("hReLPtLead", type, log10(tr->Pt()/1000.), wt);

    // Leading track parameters
    hh->fill("hReNPix", type, tr->m_nPixelHits, wt);
    hh->fill("hReNSCT", type, tr->m_nSCTHits, wt);
    hh->fill("hReNSi",  type, tr->nSiHits(), wt);
    hh->fill("hReBlay", type, tr->nBlayer(), wt); 
    hh->fill("hReNTRT", type, tr->m_nTRTHits, wt);
    hh->fill("hReTRTR", type, tr->nTRHItsRatio(), wt);

    hh->fill("hReChi2", type, tr->m_chi2, wt);
    hh->fill("hReA0", type, tr->m_a0, wt);
  
    hh->fill("hReA0sig", type, tr->m_a0/tr->m_a0error, wt);
    hh->fill("hReZ0",  type, tr->m_z0, wt);

    hh->fill("hReZ0A0", type, tr->m_z0, tr->m_a0, wt);
  }

  hh->fill("hReMass", type, tauRec->m_MTrk3P/1000., wt);
  hh->fill("hReRWid", type, tauRec->m_RWidth2Trk3P, wt);

  hh->fill("hReLike", type, tauRec->m_discriLL, wt);
  hh->fill("hReDNN", type, tauRec->m_discriNN, wt);

  // Fill event bits
  hh->fill("hReBits", type, 0, wt);
  if (tauRec->isTauCutLoose())  hh->fill("hReBits", type, 1, wt);
  if (tauRec->isTauCutMedium()) hh->fill("hReBits", type, 2, wt);
  if (tauRec->isTauCutTight())  hh->fill("hReBits", type, 3, wt);
  if (tauRec->isTauLlhLoose())  hh->fill("hReBits", type, 4, wt);
  if (tauRec->isTauLlhMedium()) hh->fill("hReBits", type, 5, wt);
  if (tauRec->isTauLlhTight())  hh->fill("hReBits", type, 6, wt);
  if (tauRec->isTauCutSafeLoose())      hh->fill("hReBits", type, 7, wt);
  if (tauRec->isTauCutSafeMedium())     hh->fill("hReBits", type, 8, wt);
  if (tauRec->isTauCutSafeTight())      hh->fill("hReBits", type, 9, wt);
  if (tauRec->isTauCutSafeCaloLoose())  hh->fill("hReBits", type, 10, wt);
  if (tauRec->isTauCutSafeCaloMedium()) hh->fill("hReBits", type, 11, wt);
  if (tauRec->isTauCutSafeCaloTight())  hh->fill("hReBits", type, 12, wt);
  if (tauRec->isElectronVetoLoose())    hh->fill("hReBits", type, 13, wt);
  if (tauRec->isElectronVetoMedium())   hh->fill("hReBits", type, 14, wt);
  if (tauRec->isElectronVetoTight())    hh->fill("hReBits", type, 15, wt);

  hh->fill("hReEMC", type, tauRec->m_etEMCalib/1000., wt);
  hh->fill("hReHdC", type, tauRec->m_etHadCalib/1000., wt);
  hh->fill("hReEMHdC", type, tauRec->m_etEMCalib/1000., tauRec->m_etHadCalib/1000., wt);
  hh->fill("hReEMCell", type, tauRec->m_sumEmCellEt/1000., wt);
  hh->fill("hReHdCell", type, tauRec->m_sumHadCellEt/1000., wt);

  hh->fill("hReEMR", type, tauRec->m_EMRadius, wt);
  hh->fill("hReHdR", type, tauRec->m_hadRadius, wt);
  hh->fill("hReIso", type, tauRec->m_isoFrac, wt);
  hh->fill("hReCent", type, tauRec->m_centralityFraction, wt);
  hh->fill("hReWidth", type, tauRec->m_stripWidth2, wt);
  hh->fill("hReERatio", type, tauRec->m_etEMCalib/(tauRec->m_etEMCalib+tauRec->m_etHadCalib), wt);

  hh->fill("hReEta", type, tauRec->Eta(), wt);
  hh->fill("hRePhi", type, tauRec->Phi(), wt);
  hh->fill("hRePhi2", type, tauRec->Phi(), wt);
  hh->fill("hReEtaPhi", type, tauRec->Eta(), tauRec->Phi(), wt);
  hh->fill("hReEtaEt", type, tauRec->Eta(), tauRec->Et()/1000., wt);

  if (L1 == 0) return;
  hh->fill("hReL1dEtadPhi", type, tauRec->Eta()-L1->Eta(), tauRec->DeltaPhi(*L1),   wt);
  hh->fill("hReL1Et", type, L1->m_TauEnergy/1000., tauRec->Et()/1000., wt);

  hh->fill("hReL1EtadEta", type, tauRec->Eta(), tauRec->Eta()-L1->Eta(), wt);
  hh->fill("hReL1PhidPhi", type, tauRec->Phi(), tauRec->DeltaPhi(*L1), wt);

  if (L2 == 0) return;
  hh->fill("hReL2dEtadPhi", type, tauRec->Eta()-L2->Eta(), tauRec->DeltaPhi(*L2),   wt);

  if (EF == 0) return;
  hh->fill("hReEFdEta", type, tauRec->Eta()-EF->Eta(), wt);
  hh->fill("hReEFdPhi", type, tauRec->DeltaPhi(*EF),   wt);
  hh->fill("hReEFdEtadPhi", type, tauRec->Eta()-EF->Eta(), tauRec->DeltaPhi(*EF),   wt);

  hh->fill("hReEFdEt", type, tauRec->Et()/1000. - EF->Et()/1000., wt);
  hh->fill("hReEFEt", type, EF->Et()/1000., tauRec->Et()/1000., wt);

  hh->fill("hReEFEtadEta", type, tauRec->Eta(), tauRec->Eta()-EF->Eta(), wt);
  hh->fill("hReEFPhidPhi", type, tauRec->Phi(), tauRec->DeltaPhi(*EF), wt);
 
  return;
}

void
TTPValCosmicPlots::draw(const TString& outfile) {

  TCanvas* c1 = new TCanvas("c1", "", 10, 10, 800, 620);

  switch (_plotType) {
  case TTPValCosmicPlots::DEFAULT:
    c1->Divide(3, 2);
    hh->drawAll(outfile, 6, c1);
    break;

  case TTPValCosmicPlots::RECO_BASED:
    c1->Divide(2,2);
    hh->drawAll(outfile, 4, c1);
    break;

  }
  delete c1;

  _nEvent.print();
}

void
TTPValCosmicPlots::drawEff(const TString& outfile) {

  TCanvas* c2 = new TCanvas("c2", "", 10, 10, 800, 620);

  switch (_plotType) {
  case TTPValCosmicPlots::DEFAULT:
    c2->Divide(3, 2);
    hh->drawEffAll(outfile, 6, 0, c2);
    break;

  case TTPValCosmicPlots::RECO_BASED:
    c2->Divide(2,2);
    hh->drawEffAll(outfile, 4, 0, c2);
    break;

  }

  delete c2;

  _nEvent.print();
}


