/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Make standard set of selection plots
bool debug = false;

#include <iostream>
#include <vector>
#include <math.h>

#include "TROOT.h" // needed for gROOT
#include "TFile.h"

#include "TrigTauPerformValidation/TTPValTrigPlots.h"
#include "TrigTauPerformNtuple/TrigTauEvent.h"
#include "TrigTauPerformAnalysis/TrigTauTrigChainList.h"

// Histograms managed by HistHelper
// Index means the following:
// 0 - L1 input
// 1 - L1 output
// 2 - L2Cal output
// 3 - L2Trk output
// 4 - EF output
// 5 - Offline reco matched

TTPValTrigPlots::TTPValTrigPlots() : hh(6), _thbins(100), _thstart(0)
{
  _currentWeight = 1.;
  _defVersion = TrigTauSelectionManager::MENU1400;
  _selManager = 0;
  _hltAuthorType = TrigTauFlags::ANYAUTHOR;
  _nEvent.clear();
}

TTPValTrigPlots::~TTPValTrigPlots() {
  if (_selManager != 0) delete _selManager;
}

void 
TTPValTrigPlots::setDefaultVersion(TrigTauSelectionManager::MENUVERSION version) {
  _defVersion = version;
}

void 
TTPValTrigPlots::setSelector(TrigTauFlags::TRIGGERMENU menu) {
  if (_selManager == 0) _selManager = new TrigTauSelectionManager();
  _selManager->setMenuVersion(_defVersion);
  _selManager->setTauMenu(menu); 
}

// Clear plots and define histograms
void
TTPValTrigPlots::clear() {

  // Delete all histograms
  hh.clear();

  // Define labels before creating histogram families
  std::vector<TString> ylabel(7);
  ylabel[0] = "L1 In";
  ylabel[1] = "L1 Out";
  ylabel[2] = "L2Cal Out";
  ylabel[3] = "L2Trk Out";
  ylabel[4] = "EF Out";
  ylabel[5] = "Offline";
  hh.setYAxisLabels(ylabel);

  _nEvent.clear();
  for (unsigned int i=0; i<6; i++) _nEvent.name(i, ylabel[i].Data());

  // Make sure we are in the main ROOT directory
  gROOT->cd();

  hh.create("hNRoi",   10, 0.0,  10., "ROI/ev");

  // L1 Plots
  hh.create("hL1Et",   50, 0.0, 100., "L1 Et (GeV)");
  hh.create("hL1EmEt", 50, 0.0, 100., "L1 EM Et (1x2) (GeV)");
  hh.create("hL1HdEt", 50, 0.0,  50., "L1 Hd Et (2x2) (GeV)");
  hh.create("hL1EmI",  25, 0.0,  25., "L1 EmIso (GeV)");
  hh.setLogY("hL1EmI");
  hh.create("hL1HdI",  25, 0.0,  25., "L1 HdIso (GeV)");
  hh.setLogY("hL1HdI");

  hh.create("hL1Eta",  61, -3.05, 3.05, "L1Eta");
  hh.create("hL1Phi",  65, -M_PI-M_PI/64., M_PI+M_PI/64., "L1Phi");
  hh.create("hL1EtaPhi", 61, -3.05, 3.05, 65, -M_PI-M_PI/64., M_PI+M_PI/64., 
	    "L1Eta", "L1Phi");

  // L2Calo Plots
  hh.create("hL2EtCalib", 50, 0.0, 100., "L2 EtCalib (GeV)");
  hh.create("hL2NStrips", 50, 0.0,  50., "L2 NStrips");
  hh.create("hL2StrWid2", 50, 0.0,  0.2, "L2 StrWid2");
  hh.create("hL2EMRad",   50, 0.0,  0.1, "L2 EM Radius");
  hh.create("hL2IsoFrac", 50, 0.0,  1.0, "L2 IsoFrac");

  // L2Trk Plots
  hh.create("hL2NTrk",  10,   0, 10, "L2 Tracks");
  hh.create("hL2NIso",   5,   0,  5, "L2 Iso Tr");
  hh.create("hL2NSlow", 10, 0, 10, "L2 Slow Tracks");
  hh.create("hL2PtLead", 50, 0.0, 50., "L2 PtLead (GeV)");
  hh.create("hL2Charge", 7, -3.5, 3.5, "L2 Charge");
  hh.create("hL2IsoR", 20, 0., 1., "L2 Pt Iso/Core");
  hh.setLogY("hL2IsoR");
  hh.create("hL2EtTrkEM", 50, 0.0, 100., "L2 Et Trk+EM (GeV)");
  hh.create("hL2Eta",  31, -3.1, 3.1, "L2Eta");
  hh.create("hL2Phi",  31, -3.1, 3.1, "L2Phi");
  hh.create("hL2EtaPhi", 31, -3.1, 3.1, 31, -3.1, 3.1, "L2Eta", "L2Phi");

  hh.create("hL2L1dEta", 50, -0.25, 0.25, "L2-L1 dEta");
  hh.create("hL2L1dPhi", 50, -0.25, 0.25, "L2-L1 dPhi");
  hh.create("hL2L1dEtadPhi", 50, -0.25, 0.25, 50, -0.25, 0.25, 
	    "L2-L1 dEta", "dPhi");
  hh.create("hL2L1dEt",  50, -25, 25, "L2 Et - L1 Et");
  hh.create("hL2L1Et", 50, 0.0, 100.0, 50, 0.0, 100.0, "L1 Et", "L2 Et");

  // EF Plots
  hh.create("hEFNTrk",    10, 0, 10,  "EF Tracks");
  hh.create("hEFCharge",   7,-3.5, 3.5, "EF Charge");
  hh.create("hEFPtLead",  50, 0, 50., "EF PtLead (GeV)");
  hh.create("hEFEMRad",   40, 0, 0.4, "EF EMRad");
  hh.create("hEFIsoFrac", 50, 0, 0.5, "EF IsoFrac");
  hh.setLogY("hEFIsoFrac");
  hh.create("hEFEt",      50, 0, 100., "EF Et (GeV)");
  hh.create("hEFMass",    50, 0, 10., "EF Mass (GeV)");
  hh.create("hEFEta",  31, -3.1, 3.1, "EFEta");
  hh.create("hEFPhi",  31, -3.1, 3.1, "EFPhi");
  hh.create("hEFEtaPhi", 31, -3.1, 3.1, 31, -3.1, 3.1, "EFEta", "EFPhi");

  hh.create("hEFL2dEta", 50, -0.25, 0.25, "EF-L2 dEta");
  hh.create("hEFL2dPhi", 50, -0.25, 0.25, "EF-L2 dPhi");
  hh.create("hEFL2dEtadPhi", 50, -0.25, 0.25, 50, -0.25, 0.25, 
	    "EF-L2 dEta", "dPhi");
  hh.create("hEFL2dEt",  50, -25, 25, "EF Et - L2 Et");
  hh.create("hEFL2Et", 50, 0.0, 100.0, 50, 0.0, 100.0, "L2 Et", "EF Et");

  // Reco plots
  hh.create("hReNTrk",  10, 0, 10, "Reco Tracks");
  hh.create("hReCharge", 7, -3.5, 3.5, "Reco Charge");
  hh.create("hRePtLead",  50, 0, 50., "Reco PtLead (GeV)");
  hh.create("hReMass",    50, 0, 10., "Reco Mass (GeV)");
  hh.create("hReRWid",    50, 0, 0.5, "Reco RWidth2");
  hh.create("hReDNN", 50, 0, 1., "Reco NN");

  hh.create("hReEta",  31, -3.1, 3.1, "ReEta");
  hh.create("hRePhi",  31, -3.1, 3.1, "RePhi");
  hh.create("hReEtaPhi", 31, -3.1, 3.1, 31, -3.1, 3.1, "ReEta", "RePhi");

  hh.create("hReEFdEta", 50, -0.25, 0.25, "Reco-EF dEta");
  hh.create("hReEFdPhi", 50, -0.25, 0.25, "Reco-EF dPhi");
  hh.create("hReEFdEtadPhi", 50, -0.25, 0.25, 50, -0.25, 0.25, 
	    "Reco-EF dEta", "dPhi");
  hh.create("hReEFdEt",  50, -25, 25, "Reco Et - EF Et");
  hh.create("hReEFEt", 50, 0.0, 100.0, 50, 0.0, 100.0, "EF Et", "Reco Et");


}

// define time history plots
void 
TTPValTrigPlots::defineTH(unsigned long tlo, unsigned long thi) {

  _thstart = tlo;

  // Make sure we are in the main ROOT directory
  gROOT->cd();

  hh.create("thL1Et",      _thbins, 0., thi-tlo, 50, 0.0, 100., "Timestamp", "L1 Et (GeV)");
  hh.create("thL2EtCalib", _thbins, 0., thi-tlo, 50, 0.0, 100., "Timestamp", "L2 EtCalib (GeV)");
  hh.create("thL2NTrk",    _thbins, 0., thi-tlo, 10,   0,   10, "Timestamp", "L2 Tracks");
  hh.create("thEFEt",      _thbins, 0., thi-tlo, 50,   0, 100., "Timestamp", "EF Et (GeV)");
  hh.create("thEFNTrk",    _thbins, 0., thi-tlo, 10,   0,   10, "Timestamp", "EF Tracks");

}

// Zero all histograms
void
TTPValTrigPlots::reset() {
  hh.reset();
  _nEvent.reset();
}

// More user friendly way to process file
void
TTPValTrigPlots::fill(const TString& filename, double wt) {

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

  _fix.setTTPVersion(filename);
  setCurrentWeight(wt);
  fill(tree);

  delete tree;
  delete file;
}

// Process file
void
TTPValTrigPlots::fill(TTree* tree) {

  if (tree == 0) {
    std::cerr << "TTPValTrigPlots::fill - TTree is NULL!" << std::endl;
    return;
  }

  TrigTauEvent* event = new TrigTauEvent();
  tree->SetBranchAddress("EventBranch", &event);
  tree->SetBranchStatus("*", 1);
  tree->SetBranchStatus("mcEvent*", 0);

  int nev = int(tree->GetEntries());

  // Must do time history plots here (to get time range correct)
  unsigned long tlo=0;
  unsigned long thi=0;

  event->reset();
  tree->GetEvent(0);
  tlo = event->time_stamp;

  if (nev > 0) {
    event->reset();
    tree->GetEvent(nev-1);
    thi = event->time_stamp;

    defineTH(tlo, thi);
  }

  //  cout << "Time history lo: " << tlo << " hi: " << thi << endl;

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
    hh.fill("hNRoi", itype, numCh, _currentWeight);
    if (numCh == 0) continue;
    for (unsigned int i=0; i<numCh; i++) {
      TrigTauTrigChain ch = chainList.trigChainEntry(i);
      fillRoiHist(event, ch, itype, _currentWeight);
    }

    // L1 output
    itype = 1;
    _nEvent.fill(itype, _currentWeight);
    hh.fill("hNRoi", itype, numL1, _currentWeight);
    if (numL1 == 0) continue;
    for (unsigned int i=0; i<numL1; i++) {
      TrigTauTrigChain ch = chainList.passL1ChainEntry(i);
      fillRoiHist(event, ch, itype, _currentWeight);
    }

    // L2Cal output
    itype = 2;
    _nEvent.fill(itype, _currentWeight);
    hh.fill("hNRoi", itype, numL2Cal, _currentWeight);
    if (numL2Cal == 0) continue;
    for (unsigned int i=0; i<numL2Cal; i++) {
      TrigTauTrigChain ch = chainList.passL2CalChainEntry(i);
      fillRoiHist(event, ch, itype, _currentWeight);
    }

    // L2Trk output
    itype = 3;
    _nEvent.fill(itype, _currentWeight);
    hh.fill("hNRoi", itype, numL2Trk, _currentWeight);
    if (numL2Trk == 0) continue;
    for (unsigned int i=0; i<numL2Trk; i++) {
      TrigTauTrigChain ch = chainList.passL2ChainEntry(i);
      fillRoiHist(event, ch, itype, _currentWeight);
    }

    // EF output
    itype = 4;
    _nEvent.fill(itype, _currentWeight);
    hh.fill("hNRoi", itype, numEF, _currentWeight);

    
    unsigned int numReco = 0;
    if (numEF == 0) continue;
    for (unsigned int i=0; i<numEF; i++) {
      TrigTauTrigChain ch = chainList.passEFChainEntry(i);
      fillRoiHist(event, ch, 4, _currentWeight);

      // Also add on tauRec selected
      const TrigTauRecOfflineObject* tauRec = ch.tauRec;
      if (tauRec == 0) continue;
      if (tauRec->m_discriCut < 0.5) continue;
      numReco++;
      fillRoiHist(event, ch, 5, _currentWeight);
    }
    hh.fill("hNRoi", 5, numReco, _currentWeight);
    
  }

  delete event;
}

void
TTPValTrigPlots::fillRoiHist(const TrigTauEvent* ev, 
			     const TrigTauTrigChain& ch, 
			     unsigned int type,
			     double wt) {

  const TrigTauL1Object* L1 = ch.L1;
  if (L1 == 0) return;

  hh.fill("hL1Et",   type, L1->m_TauEnergy/1000., wt);
  hh.fill("hL1EmEt", type, L1->m_EmEnergy/1000., wt);
  hh.fill("hL1HdEt", type, L1->m_HdCore/1000., wt);
  hh.fill("hL1EmI",  type, L1->m_EmIsol/1000., wt);
  hh.fill("hL1HdI",  type, L1->m_HdIsol/1000., wt);
  hh.fill("hL1Eta",  type, L1->Eta(), wt); 
  hh.fill("hL1Phi",  type, L1->Phi(), wt);
  hh.fill("hL1EtaPhi", type, L1->Eta(), L1->Phi(), wt);

  // Time histories
  hh.fill("thL1Et",  type, ev->time_stamp-_thstart, L1->m_TauEnergy/1000., wt);
 
  const TrigTauL2Object* L2 = ch.L2;
  if (L2 == 0) return;

  hh.fill("hL2EtCalib", type, L2->etClusCalib()/1000., wt);
  hh.fill("hL2NStrips", type, L2->m_nStrips, wt);
  hh.fill("hL2StrWid2", type, L2->m_stripWidth2, wt);
  hh.fill("hL2EMRad",   type, L2->m_EMRadius, wt);
  hh.fill("hL2IsoFrac", type, L2->m_isoFrac, wt);


  // L2 track information now in L2 object correctly
  hh.fill("hL2NTrk",   type, L2->m_nCoreTracks, wt);
  hh.fill("hL2NIso",   type, L2->m_nIsoTracks,  wt);
  hh.fill("hL2NSlow",  type, L2->m_nSlowTracks, wt);
  hh.fill("hL2PtLead", type, L2->m_leadingTrackPt/1000., wt);
  if (L2->m_scalarPtSumCore > 0) {
    hh.fill("hL2IsoR",   type, L2->m_scalarPtSumIso/L2->m_scalarPtSumCore, wt);
  } else {
    hh.fill("hL2IsoR",   type, 99., wt);
  }

  hh.fill("hL2Charge", type, L2->m_charge, wt);

  float emEt = L2->m_EMEnergyWide[0]+L2->m_EMEnergyWide[1]+L2->m_EMEnergyWide[2];
  emEt /= TMath::CosH(L2->m_caloEta);
  
  TLorentzVector trkEMSum;
  trkEMSum.SetPtEtaPhiM(emEt, L2->m_caloEta, L2->m_caloPhi, 0.);
  trkEMSum += L2->m_3fastest;
  hh.fill("hL2EtTrkEM", type, trkEMSum.Et()/1000., wt);

  hh.fill("hL2Eta",    type, L2->Eta(), wt);
  hh.fill("hL2Phi",    type, L2->Phi(), wt);
  hh.fill("hL2EtaPhi", type, L2->Eta(), L2->Phi(), wt);

  hh.fill("hL2L1dEta", type, L2->Eta()-L1->Eta(), wt);
  hh.fill("hL2L1dPhi", type, L2->DeltaPhi(*L1),   wt);
  hh.fill("hL2L1dEtadPhi", type, L2->Eta()-L1->Eta(), L2->DeltaPhi(*L1), wt);

  hh.fill("hL2L1dEt", type, L2->etClusCalib()/1000. - L1->m_TauEnergy/1000., wt);
  hh.fill("hL2L1Et", type,  L1->m_TauEnergy/1000., L2->etClusCalib()/1000., wt);

  // Time histories
  hh.fill("thL2EtCalib", type, ev->time_stamp-_thstart, L2->etClusCalib()/1000., wt);
  hh.fill("thL2NTrk",    type, ev->time_stamp-_thstart, L2->m_nCoreTracks, wt);

  const TrigTauRecObject* EF = ch.EF;
  if (EF == 0) return;

  hh.fill("hEFNTrk", type, EF->nTracks(), wt);
  hh.fill("hEFCharge", type, EF->charge(), wt);
  if (EF->leadingTrack() != 0)
    hh.fill("hEFPtLead", type, EF->leadingTrack()->Pt()/1000., wt);
  hh.fill("hEFEMRad", type, EF->m_EMRadius, wt);
  hh.fill("hEFIsoFrac", type, EF->m_isoFrac, wt);
  hh.fill("hEFEt", type, EF->Et()/1000., wt);
  //  hh.fill("hEFMass", type, EF->Mass()/1000., wt);
  hh.fill("hEFEta", type, EF->Eta(), wt);
  hh.fill("hEFPhi", type, EF->Phi(), wt);
  hh.fill("hEFEtaPhi", type, EF->Eta(), EF->Phi(), wt);

  hh.fill("hEFL2dEta", type, EF->Eta()-L2->Eta(), wt);
  hh.fill("hEFL2dPhi", type, EF->DeltaPhi(*L2),   wt);
  hh.fill("hEFL2dEtadPhi", type, EF->Eta()-L2->Eta(), EF->DeltaPhi(*L2), wt);

  hh.fill("hEFL2dEt", type, EF->Et()/1000. - L2->etClusCalib()/1000., wt);
  hh.fill("hEFL2Et", type,  L2->etClusCalib()/1000., EF->Et()/1000., wt);

  // Time histories
  hh.fill("thEFNTrk", type, ev->time_stamp-_thstart, EF->nTracks(), wt);
  hh.fill("thEFEt",   type, ev->time_stamp-_thstart, EF->Et()/1000., wt);

  const TrigTauRecOfflineObject* tauRec = ch.tauRec;
  if (tauRec == 0) return;

  hh.fill("hReNTrk", type, tauRec->nTracks(), wt);
  hh.fill("hReCharge", type, tauRec->charge(), wt);
  if (tauRec->leadingTrack() != 0)
    hh.fill("hRePtLead", type, tauRec->leadingTrack()->Pt()/1000., wt);
  hh.fill("hReMass", type, tauRec->m_MTrk3P/1000., wt);
  hh.fill("hReRWid", type, tauRec->m_RWidth2Trk3P, wt);
  hh.fill("hReDNN", type, tauRec->m_discriNN, wt);

  hh.fill("hReEta", type, tauRec->Eta(), wt);
  hh.fill("hRePhi", type, tauRec->Phi(), wt);
  hh.fill("hReEtaPhi", type, tauRec->Eta(), tauRec->Phi(), wt);

  hh.fill("hReEFdEta", type, tauRec->Eta()-EF->Eta(), wt);
  hh.fill("hReEFdPhi", type, tauRec->DeltaPhi(*EF),   wt);
  hh.fill("hReEFdEtadPhi", type, tauRec->Eta()-EF->Eta(), 
	  tauRec->DeltaPhi(*EF),   wt);

  hh.fill("hReEFdEt", type, tauRec->Et()/1000. - EF->Et()/1000., wt);
  hh.fill("hReEFEt", type, EF->Et()/1000., tauRec->Et()/1000., wt);
  return;
}

void
TTPValTrigPlots::draw(const TString& outfile) {

  TCanvas* c1 = new TCanvas("c1", "", 10, 10, 800, 620);
  c1->Divide(3, 2);
  hh.drawAll(outfile, 6, c1);
  delete c1;

  _nEvent.print();
}


