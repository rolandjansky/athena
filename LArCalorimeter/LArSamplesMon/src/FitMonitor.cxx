/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/FitMonitor.h"

#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/Data.h"

#include "TTree.h"
#include "TFile.h"
#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


bool FitMonitor::makeSummary(const char* fileName) const
{
  TFile* f = TFile::Open(fileName, "RECREATE");
  TTree* summary = new TTree("summary", "");
  
  int tree_hash, tree_calo, tree_layer, tree_iEta, tree_iPhi, tree_feb, tree_channel, tree_index;
  float tree_chi2, tree_refitChi2;
  float tree_time, tree_energy, tree_adcMax, tree_refitTime, tree_refitADCMax;
   
  summary->Branch("hash", &tree_hash, "hash/I");
  summary->Branch("calo", &tree_calo, "calo/I");
  summary->Branch("layer", &tree_layer, "layer/I");
  summary->Branch("iEta", &tree_iEta, "iEta/I");
  summary->Branch("iPhi", &tree_iPhi, "iPhi/I");
  summary->Branch("feb", &tree_feb, "feb/I");
  summary->Branch("channel", &tree_channel, "channel/I");
  summary->Branch("index", &tree_index, "index/I");
  summary->Branch("time", &tree_time, "time/F");
  summary->Branch("energy", &tree_energy, "energy/F");
  summary->Branch("adcMax", &tree_adcMax, "adcMax/F");
  summary->Branch("refitTime", &tree_refitTime, "refitTime/F");
  summary->Branch("chi2", &tree_chi2, "chi2/F");
  summary->Branch("refitChi2", &tree_refitChi2, "refitChi2/F");
  summary->Branch("refitADCMax", &tree_refitADCMax, "refitADCMax/F");

  for (unsigned int i = 0; i < nChannels(); i++) {
    const History* history = cellHistory(i);
    if (!history || !history->isValid()) continue;
    
    if (i%1000 == 0) cout << "-> History hash = " << i << endl;
    History* refitHistory = history->refit();
    
    tree_hash = i;
    tree_calo = history->cellInfo()->calo();
    tree_layer = history->cellInfo()->layer();
    tree_iEta = history->cellInfo()->iEta();
    tree_iPhi = history->cellInfo()->iPhi();
    tree_feb  = history->cellInfo()->feb();
    tree_channel = history->cellInfo()->channel();

    for (unsigned int j = 0; j < history->nData(); j++) {
      tree_index = j;
      tree_chi2   = history->chi2(j);
      tree_time   = history->data(j)->ofcTime();
      tree_adcMax = history->data(j)->adcMax();
      tree_energy = history->data(j)->energy();
      tree_refitChi2 = refitHistory->chi2(j);
      tree_refitTime = refitHistory->data(j)->ofcTime();
      tree_refitADCMax = refitHistory->data(j)->adcMax();
      summary->Fill();
    }    
    delete refitHistory;
  }
    
  summary->Write();
  cout << "Closing file..." << endl;
  
  f->Close();
  return true;
}

