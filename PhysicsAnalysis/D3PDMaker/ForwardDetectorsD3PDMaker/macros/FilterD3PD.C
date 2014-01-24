/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include <iostream>
#include <vector>
#include <TTree.h>
#include <TFile.h>
#include <cmath>

bool FilterEvent(unsigned int LBNStart, unsigned int LBNEnd, bool MBTSCut, bool vertexCut, double MBTSTimeCut);
bool JetFilterEvent(double jetEtMinGeV);
bool MuonFilterEvent(double jetEtMinGeV);

UInt_t           EventNumber;
Int_t           vx_n;
UInt_t          lbn;
std::vector<unsigned int> *trig_L1_TAV;

Float_t         mbtime_timeDiff;
Float_t         mbtime_timeA;
Float_t         mbtime_timeC;
Int_t           mbtime_countA;
Int_t           mbtime_countC;

Int_t           antikt4HI_TowerJets_n;
vector<float>   *antikt4HI_TowerJets_E;
vector<float>   *antikt4HI_TowerJets_pt;
vector<float>   *antikt4HI_TowerJets_m;
vector<float>   *antikt4HI_TowerJets_eta;
vector<float>   *antikt4HI_TowerJets_phi;

Int_t           mu_staco_n;
vector<float>   *mu_staco_E;
vector<float>   *mu_staco_pt;
vector<float>   *mu_staco_m;
vector<float>   *mu_staco_eta;
vector<float>   *mu_staco_phi;

//  Branches
//
TBranch        *b_EventNumber;   
TBranch        *b_lbn;   
TBranch        *b_vx_n;
TBranch        *b_trig_L1_TAV; 

TBranch        *b_mbtime_timeDiff;
TBranch        *b_mbtime_timeA;   
TBranch        *b_mbtime_timeC;   
TBranch        *b_mbtime_countA;  
TBranch        *b_mbtime_countC;  

TBranch        *b_antikt4HI_TowerJets_n; 
TBranch        *b_antikt4HI_TowerJets_E; 
TBranch        *b_antikt4HI_TowerJets_pt;
TBranch        *b_antikt4HI_TowerJets_m;
TBranch        *b_antikt4HI_TowerJets_eta;
TBranch        *b_antikt4HI_TowerJets_phi;

TBranch        *b_mu_staco_n;
TBranch        *b_mu_staco_E;
TBranch        *b_mu_staco_pt;
TBranch        *b_mu_staco_m;
TBranch        *b_mu_staco_eta;
TBranch        *b_mu_staco_phi;

// The main filter routine
// 
void FilterD3PD(const char* inputFile, int LBNStart, int LBNEnd, bool MBTSCut, bool vertexCut, double MBTSTimeCut,
		bool writeMinBias, const char* outputFile, bool doJetFiltering = false, 
		const char* outJetFile = 0, double jetEtMinGeV = 100,
		bool doMuonFiltering = false, const char* outMuonFile = 0, double muonPtMinGeV = 20)
{

  TFile inputTFile(inputFile);
  if (!inputTFile.IsOpen()) {
    std::cout << "Error opening input file, quitting" << std::endl;
    return;
  }

  TTree* inputTree_p = (TTree*) inputTFile.GetObjectChecked("HeavyIonD3PD", "TTree");
  if (inputTree_p == 0) {
    std::cout << "Error reading tree from opening input file, quitting" << std::endl;
    return;
  }

  std::cout << std::endl;
  std::cout << "Running filter on %s" << inputFile << " LB range " 
	    << LBNStart << " " << LBNEnd << std::endl;
  std::cout << "Output to %s" << outputFile << std::endl;
  std::cout << "MBTSCut " << MBTSCut << " time cut " << MBTSTimeCut 
	    << " vertexCut " << vertexCut << std::endl;
  std::cout << std::endl;

  //  Set branch addresses for data we need
  //
  trig_L1_TAV = 0;

  inputTree_p->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
  inputTree_p->SetBranchAddress("lbn", &lbn, &b_lbn);
  inputTree_p->SetBranchAddress("vx_n", &vx_n, &b_vx_n);
  inputTree_p->SetBranchAddress("trig_L1_TAV", &trig_L1_TAV, &b_trig_L1_TAV);
  inputTree_p->SetBranchAddress("mbtime_timeDiff", &mbtime_timeDiff, &b_mbtime_timeDiff);
  inputTree_p->SetBranchAddress("mbtime_timeA", &mbtime_timeA, &b_mbtime_timeA);
  inputTree_p->SetBranchAddress("mbtime_timeC", &mbtime_timeC, &b_mbtime_timeC);
  inputTree_p->SetBranchAddress("mbtime_countA", &mbtime_countA, &b_mbtime_countA);
  inputTree_p->SetBranchAddress("mbtime_countC", &mbtime_countC, &b_mbtime_countC);

  // Make sure we open the output file before cloning so output tree has oplace to write data.
  //
  TFile* outMinBiasTFile_p = 0;
  TTree* minBiasTree_p = 0;

  if (writeMinBias) {
    outMinBiasTFile_p = new TFile(outputFile, "recreate");
    if (!outMinBiasTFile_p->IsOpen()) {
      std::cout << "Error opening output file, quitting" << std::endl;
      return;
    }

    //  Now clone the input tree
    //
    minBiasTree_p = (TTree*) inputTree_p->CloneTree(0);
  }

  TFile* outJetTFile_p = 0;
  TTree* jetCloneTree_p = 0;
  if (doJetFiltering) {
    outJetTFile_p = new TFile(outJetFile, "recreate");
    if (!outJetTFile_p->IsOpen()) {
      std::cout << "Error opening output jet file, quitting" << std::endl;
      return;
    }
    
    jetCloneTree_p = (TTree*) inputTree_p->CloneTree(0);

    antikt4HI_TowerJets_E = 0;
    antikt4HI_TowerJets_pt = 0;
    antikt4HI_TowerJets_m = 0;
    antikt4HI_TowerJets_eta = 0;
    antikt4HI_TowerJets_phi = 0;

    inputTree_p->SetBranchAddress("antikt4HI_TowerJets_n", &antikt4HI_TowerJets_n, &b_antikt4HI_TowerJets_n);
    inputTree_p->SetBranchAddress("antikt4HI_TowerJets_E", &antikt4HI_TowerJets_E, &b_antikt4HI_TowerJets_E);
    inputTree_p->SetBranchAddress("antikt4HI_TowerJets_pt", &antikt4HI_TowerJets_pt, &b_antikt4HI_TowerJets_pt);
    inputTree_p->SetBranchAddress("antikt4HI_TowerJets_m", &antikt4HI_TowerJets_m, &b_antikt4HI_TowerJets_m);
    inputTree_p->SetBranchAddress("antikt4HI_TowerJets_eta", &antikt4HI_TowerJets_eta, &b_antikt4HI_TowerJets_eta);
    inputTree_p->SetBranchAddress("antikt4HI_TowerJets_phi", &antikt4HI_TowerJets_phi, &b_antikt4HI_TowerJets_phi);
  }

  TFile* outMuonTFile_p = 0;
  TTree* muonCloneTree_p = 0;
  if (doMuonFiltering) {
    outMuonTFile_p = new TFile(outMuonFile, "recreate");
    if (!outMuonTFile_p->IsOpen()) {
      std::cout << "Error opening output muon file, quitting" << std::endl;
      return;
    }

    muonCloneTree_p = (TTree*) inputTree_p->CloneTree(0);

    mu_staco_E = 0;
    mu_staco_pt = 0;
    mu_staco_m = 0;
    mu_staco_eta = 0;
    mu_staco_phi = 0;

    inputTree_p->SetBranchAddress("mu_staco_n", &mu_staco_n, &b_mu_staco_n);
    inputTree_p->SetBranchAddress("mu_staco_E", &mu_staco_E, &b_mu_staco_E);
    inputTree_p->SetBranchAddress("mu_staco_pt", &mu_staco_pt, &b_mu_staco_pt);
//    inputTree_p->SetBranchAddress("mu_staco_m", &mu_staco_m, &b_mu_staco_m);
    inputTree_p->SetBranchAddress("mu_staco_eta", &mu_staco_eta, &b_mu_staco_eta);
    inputTree_p->SetBranchAddress("mu_staco_phi", &mu_staco_phi, &b_mu_staco_phi);
  }

  //std::cout << "Processing " << inputFile << std::endl;

  // Now loop and perform the filling after cuts
  //
  int numEntries = inputTree_p->GetEntries();
  int numJetEvents = 0;
  int numMuonEvents = 0;
  int numCopied = 0;
  int numPassLBN = 0;
  int numPassMinBias = 0;

  for (int iEntry = 0; iEntry < numEntries; iEntry++) {
    int nbytes = inputTree_p->GetEntry(iEntry);
    if (nbytes <= 0) {
      std::cout << "Error reading entry " << iEntry << " From tree, skipping" << std::endl;
      continue;
    }

    if(!(iEntry%1000)) 
      std::cout << "checking event " << iEntry << std::endl;

    if (!((lbn < LBNStart) || (lbn > LBNEnd))) 
      numPassLBN++;

    // Test the filter
    //
    if (FilterEvent(LBNStart, LBNEnd, MBTSCut, vertexCut, MBTSTimeCut)) {
      numPassMinBias++;

      if (writeMinBias) {
	//std::cout << "Filling new tree for event " << iEntry << std::endl;
	minBiasTree_p->Fill();
	numCopied++;
      }

      if (doJetFiltering) {
	if (JetFilterEvent(jetEtMinGeV)) {
	  numJetEvents++;
	  jetCloneTree_p->Fill();
	}
      }

      if (doMuonFiltering) {
	if (MuonFilterEvent(muonPtMinGeV)) {
	  numMuonEvents++;
	  muonCloneTree_p->Fill();
	}
      }
    }
  }

    std::cout << "All entries " << numEntries << " Passed after LBN cuts: " << numPassLBN << std::endl;
    std::cout << " Passed min-bias cuts: " << numPassMinBias << " , fraction =  " << ((float) numPassMinBias)/((float)numEntries) << std::endl;
    std::cout << " Copied " << numCopied << " minimum bias Events, fraction =  " << ((float) numCopied)/((float)numEntries) << std::endl;
    std::cout << " Copied " << numJetEvents << " Jet Events " << std::endl;
    std::cout << " Copied " << numMuonEvents << " Muon Events " << std::endl;

  //  minBiasTree_p->Print();
  if (writeMinBias)  {
    if (numCopied > 0) minBiasTree_p->AutoSave();
    outMinBiasTFile_p->Close();
  }

  if (doJetFiltering) {
    if  (numJetEvents > 0) jetCloneTree_p->AutoSave();
    outJetTFile_p->Close();
  }

  if (doMuonFiltering) {
    if  (numMuonEvents > 0) muonCloneTree_p->AutoSave();
    outMuonTFile_p->Close();
  }
}

bool FilterEvent(unsigned int LBNStart, unsigned int LBNEnd, bool MBTSCut, bool vertexCut, double MBTSTimeCut)
{

  if ((lbn < LBNStart) || (lbn > LBNEnd)) return false;

  if (MBTSCut) {

    bool passMBTS = (( (*trig_L1_TAV)[7]&0x10) == 0);
    bool passZDCAND = (((*trig_L1_TAV)[5]&0x20000000) != 0);
    bool passTE50 = (((*trig_L1_TAV)[4]&0x400) != 0);
    bool passLucid = (((*trig_L1_TAV)[6]&400000) != 0);

    if (!passMBTS && !passZDCAND && !passTE50 && !passLucid) return false;
    if (mbtime_timeA == 0) return false;
    if (mbtime_timeC == 0) return false;
    if (fabs(mbtime_timeA - mbtime_timeC) > MBTSTimeCut) return false;
  }

  if (vertexCut) {
    if (vx_n < 2) return false;
  }

  return true;
}

bool JetFilterEvent(double jetEtMinGeV)
{
  for (int ijet = 0; ijet < antikt4HI_TowerJets_n; ijet++) {
    double eta = (*antikt4HI_TowerJets_eta)[ijet];
    double et = (*antikt4HI_TowerJets_E)[ijet]/1000 / std::cosh(eta);

    if (et > jetEtMinGeV) {
      std::cout << "Found jet in event " << EventNumber << ", lbn = " << lbn << ", et = " << std::setprecision(5) << std::setw(7) << et 
		<< ", " << std::setprecision(5) << std::setw(7) << eta << ", " << std::setprecision(5) << std::setw(7) 
		<< (*antikt4HI_TowerJets_phi)[ijet]/1000 << std::endl;
      return true;
    }
  }

  return false;
}

bool MuonFilterEvent(double muonPtMinGeV)
{
  for (int imuon = 0; imuon < mu_staco_n; imuon++) {
    double eta = (*mu_staco_eta)[imuon];
    double pt = (*mu_staco_pt)[imuon]/1000;

    if (pt > muonPtMinGeV) {
      std::cout << "Found muon in event " << EventNumber << ", lbn = " << lbn << ", pt = " << std::setprecision(5) << std::setw(7) << pt
		<< ", " << std::setprecision(5) << std::setw(7) << eta << ", " << std::setprecision(5) << std::setw(7) 
		<< (*mu_staco_phi)[imuon] << std::endl;
      return true;
    }
  }

  return false;
}

