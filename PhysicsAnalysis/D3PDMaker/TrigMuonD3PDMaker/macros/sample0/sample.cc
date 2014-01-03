/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"

#include "TrigMuonD3PD.h"

using namespace std;

void sample(){
  //initialize input
  TChain* ch1 = new TChain("trigger");
  ch1->Add("../../work/20101028/trig_muon_d3pd_level0.root");
  TrigMuonD3PD* d3pd = new TrigMuonD3PD(ch1);

  //initialize output
  TFile* fout = new TFile("output.root","recreate");
  TH1F* h_mufast_pt = new TH1F("h_mufast_pt","mufast_pt;p_{T} (GeV);entry",100,-100,100);
  TH1F* h_mucomb_pt = new TH1F("h_mucomb_pt","mucomb_pt;p_{T} (GeV);entry",100,-100,100);
  TH1F* h_muonef_sa_pt = new TH1F("h_muonef_sa_pt","muonef_sa_pt;p_{T} (GeV);entry",100,-100,100);
  TH1F* h_muonef_cb_pt = new TH1F("h_muonef_cb_pt","muonef_cb_pt;p_{T} (GeV);entry",100,-100,100);

  const int nevent = ch1->GetEntries();
  for(int i=0; i<nevent; i++){//Event Loop
    ch1->GetEntry(i);

    //muFast
    int n_mufast = d3pd->trig_L2_muonfeature_n;
    for(int i_mu=0; i_mu<n_mufast ; i_mu++){
      h_mufast_pt->Fill( d3pd->trig_L2_muonfeature_pt->at(i_mu) );
    }//muFast

    //muComb
    int n_mucomb = d3pd->trig_L2_combmuonfeature_n;
    for(int i_mu=0; i_mu<n_mucomb ; i_mu++){
      h_mucomb_pt->Fill( d3pd->trig_L2_combmuonfeature_pt->at(i_mu)*0.001 );
    }//muComb

    //TrigMuonEF
    int n_muonef = d3pd->trig_EF_trigmuonef_n;
    for(int i_mu=0; i_mu<n_muonef ; i_mu++){
      int n_mu_in_a_roi = d3pd->trig_EF_trigmuonef_track_n->at(i_mu);
      for(int j_efmu=0; j_efmu<n_mu_in_a_roi ; j_efmu++){
        //EF Stand-alone
        if(d3pd->trig_EF_trigmuonef_track_SA_hasSA->at(i_mu)[j_efmu]){
          h_muonef_sa_pt->Fill( d3pd->trig_EF_trigmuonef_track_SA_pt->at(i_mu)[j_efmu]*0.001 );
        }
        //EF Combined
        if(d3pd->trig_EF_trigmuonef_track_CB_hasCB->at(i_mu)[j_efmu]){
          h_muonef_cb_pt->Fill( d3pd->trig_EF_trigmuonef_track_CB_pt->at(i_mu)[j_efmu]*0.001 );
        }
      }
    }//TrigMuonEF
    /* for MuGirl, simply replace
     * d3pd->trig_EF_trigmuonef_* ==> d3pd->trig_EF_trigmugirl_*
     */

  }//Event Loop

  fout->Write();
}
