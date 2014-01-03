/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"

#include "TrigMuonD3PD.h"
#include "TrigConfTree.h"
using namespace std;

void sample(){
  //initialize input
  TChain* ch1 = new TChain("trigger");
  TChain* ch2 = new TChain("triggerMeta/TrigConfTree");

  ch1->Add("../../../work/20101028/trig_muon_d3pd_level0.root");
  ch2->Add("../../../work/20101028/trig_muon_d3pd_level0.root");
  TrigMuonD3PD* d3pd = new TrigMuonD3PD(ch1);
  TrigConfTree* conf = new TrigConfTree(ch2);
  ch2->GetEntry(0);

  //initialize output
  TFile* fout = new TFile("output.root","recreate");
  TH1F* h_mu10passed_muidcb_pt = new TH1F("h_mu10passed_muidcb_pt","mu10passed_muidcb_pt;p_{T} (GeV);entry",100,0,100);
  TH1F* h_mu20passed_muidcb_pt = new TH1F("h_mu20passed_muidcb_pt","mu20passed_muidcb_pt;p_{T} (GeV);entry",100,0,100);

  //get ChainCounter of interest
  std::map<std::string, int>::iterator p;
  const short id_EF_mu10 = (p = conf->HLTNameMap->find("EF_mu10")) != conf->HLTNameMap->end() ?
                           (short)p->second : -1;
  const short id_EF_mu20 = (p = conf->HLTNameMap->find("EF_mu20")) != conf->HLTNameMap->end() ?
                           (short)p->second : -1;

  const int nevent = ch1->GetEntries();
  for(int i=0; i<nevent; i++){//Event Loop
    ch1->GetEntry(i);

    /*
     *  check pt distribution of muid cb muon
     *  with the events passing EF_mu10 or EF_mu20
     *
     */
    bool EF_mu10_passed=false;
    bool EF_mu20_passed=false;
    // if the id is found in trig_EF_passedPhysics, it means the event passed the trigger 
    for(int imenu=0,nmenu=d3pd->trig_EF_passedPhysics->size();imenu<nmenu;imenu++){
      if(d3pd->trig_EF_passedPhysics->at(imenu)==id_EF_mu10)EF_mu10_passed=true;
      if(d3pd->trig_EF_passedPhysics->at(imenu)==id_EF_mu20)EF_mu20_passed=true;
      if(EF_mu10_passed&&EF_mu20_passed)break;
    }
    
    int n_muon = d3pd->muon_muid_n;
    for(int i_mu=0; i_mu<n_muon ; i_mu++){
      if( (d3pd->muon_muid_allauthor->at(i_mu)>>4)&1 ){//muid combined
        if(EF_mu10_passed)h_mu10passed_muidcb_pt->Fill(d3pd->muon_muid_pt->at(i_mu)*0.001);
        if(EF_mu20_passed)h_mu20passed_muidcb_pt->Fill(d3pd->muon_muid_pt->at(i_mu)*0.001);
      }
    }

  }//Event Loop

  fout->Write();
}
