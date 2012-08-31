/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /*
   Tau.C
*/
//----- INCLUDE STD LIBRARIES -----//
#include <vector>
#include <iostream>
//----- INCLUDE ROOT LIBRARIES -----//
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TStyle.h"
//----- INCLUDE ATHENA LIBRARIES -----//
#include "TriggerMenuNtuple/TMNUtil.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"
//----- INCLUDE MAKECLASS OBJECTS LIBRARIES -----//
#include "physics.h"


using namespace std;


void Tau(const std::string& fname="physics.root") {
  //----- read TTree -----//
  TFile *f = new TFile(fname.c_str(), "READ");
  TTree* t = dynamic_cast<TTree*>(f->Get("physics"));
  TTree* t_config = dynamic_cast<TTree*>(f->Get("physicsMeta/TrigConfTree"));

  //----- MakeClass objects from egamma Tree  -----//
  physics phys(t);

  //----- preparation to use TMNUtil-----//
  TMNUtil util;
  util.initForD3PD(t, t_config);
 

  TFile *file = new TFile( "hist.root", "RECREATE");
  //----- declare histograms -----//
  TH1F* h_l2tau_pt_all = new TH1F( "h_l2tau_pt_all_all", ";TrigTau Pt[GeV];Entries", 100, 0, 100);
  TH1F* h_l2tau_pt_passed = new TH1F( "h_l2tau_pt_passed", ";TrigTau Pt[GeV];Entries", 100, 0, 100);
  TH2F* h_col_eta_l2 = new TH2F( "h_col_eta_l2", ";TrigTauCluster eta;TrigTau eta ",
				 64, -3.2, 3.2 , 64, -3.2, 3.2);


  std::string L2chain = "L2_tau29i_loose";
  int nentries = t->GetEntries();
  //******************** Top of Event Loop ********************//
  for (int i=0; i<nentries; ++i) {
//     cout << "---------- Entry::" << i << " ----------" << endl;
    phys.GetEntry(i);
    util.readNavigation();


    //------ Example 1 just plot variable of a feature -----//
    vector<int> l2tau_indices_all = util.getIndex( "TrigTau", L2chain, false );
    vector<int> l2tau_indices_passed = util.getIndex( "TrigTau", L2chain, true );
    vector<int>::const_iterator p;
    for(p=l2tau_indices_all.begin(); p!=l2tau_indices_all.end(); ++p) {
      float L2_Tau_pt = phys.trig_L2_tau_pt->at( *p );
      h_l2tau_pt_all->Fill( L2_Tau_pt * 0.001);
    }
    for(p=l2tau_indices_passed.begin(); p!=l2tau_indices_passed.end(); ++p) {
      float L2_Tau_pt = phys.trig_L2_tau_pt->at( *p );
      h_l2tau_pt_passed->Fill( L2_Tau_pt * 0.001);
    }



    //------ Example 2  Loop over RoI -----//
    vector<int>::const_iterator pRoI;
    vector<int> L2RoIs = util.getRoINumbers( ChainEntry::kRoIType_L2_tau, L2chain );
    for( pRoI=L2RoIs.begin(); pRoI!=L2RoIs.end(); ++pRoI ) {
      int l2clIndex = util.getIndexInRoI( "TrigTauCluster", L2chain, *pRoI, true);
      int l2tauIndex = util.getIndexInRoI( "TrigTauCluster", L2chain, *pRoI, true);
      if( l2clIndex < 0  ||  l2tauIndex < 0 ) continue;

      float TrigTauCluster_eta = phys.trig_L2_tau_cluster_eta->at(l2clIndex);
      float TrigTau_eta = phys.trig_L2_tau_eta->at(l2tauIndex);
      h_col_eta_l2->Fill(TrigTauCluster_eta, TrigTau_eta);
    }// Bottom of loop over RoI


  }
  //******************** Bottom of Event Loop ********************//
  file->Write();


  gStyle->SetPalette(1,0);
  TCanvas* c = new TCanvas( "c", "", 800, 400);
  c->Divide(2,1);
  c->cd(1);
  h_l2tau_pt_all->Draw();
  h_l2tau_pt_passed->SetLineColor(kYellow);
  h_l2tau_pt_passed->SetFillColor(kYellow);
  h_l2tau_pt_passed->Draw("same");
  c->cd(2);
  h_col_eta_l2->Draw("colz");
}
