/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /*
   EGamma.C
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


void EGamma(const std::string& fname="physics.root") {
  //----- read TTree -----//
  TFile *f = new TFile(fname.c_str(), "READ");
  TTree* t = dynamic_cast<TTree*>(f->Get("physics"));
  TTree* t_config = dynamic_cast<TTree*>(f->Get("physicsMeta/TrigConfTree"));

  //----- MakeClass objects from egamma Tree  -----//
  physics phys(t);

  //----- Tool to access handle navigation information easily -----//
  TMNUtil util;
  util.initForD3PD(t, t_config);
 


  TFile* file = new TFile("hoge.root", "recreate");
  //----- declare histograms -----//
  TH1F* h_emcl_et_all = new TH1F( "h_emcl_et_all", ";TrigEMCluster Et[GeV];Entries", 100, 0, 100);
  TH1F* h_emcl_et_passed = new TH1F( "h_emcl_et_passed", ";TrigEMCluster Et[GeV];Entries", 100, 0, 100);
  TH2F* h_col_eta_l1_emcl = new TH2F( "h_col_eta_l1_emcl", ";EmTau_RoI eta;TrigEMCluster eta ",
				      64, -3.2, 3.2 , 64, -3.2, 3.2);

  std::string L2chain = "L2_e10_loose";
  int nentries = t->GetEntries();
  //******************** Top of Event Loop ********************//
  for (int i=0; i<nentries; ++i) {
    phys.GetEntry(i);
    util.readNavigation();

    //------ Example 1 just plot variable of a feature -----//
    vector<int> emcl_indices_all = util.getIndex( "TrigEMCluster", L2chain, false );
    vector<int> emcl_indices_passed = util.getIndex( "TrigEMCluster", L2chain, true );
    vector<int>::const_iterator p;
    for(p=emcl_indices_all.begin(); p!=emcl_indices_all.end(); ++p) {
      float L2_emcl_et = phys.trig_L2_emcl_Et->at( *p );
      h_emcl_et_all->Fill( L2_emcl_et * 0.001);
    }
    for(p=emcl_indices_passed.begin(); p!=emcl_indices_passed.end(); ++p) {
      float L2_emcl_et = phys.trig_L2_emcl_Et->at( *p );
      h_emcl_et_passed->Fill( L2_emcl_et * 0.001);
    }



    //------ Example 2  Loop over RoI -----//
    vector<int>::const_iterator pRoI;
    vector<int> L2RoIs = util.getRoINumbers( ChainEntry::kRoIType_L2_e, L2chain );
    for( pRoI=L2RoIs.begin(); pRoI!=L2RoIs.end(); ++pRoI ) {
      int emclIndex = util.getIndexInRoI( "TrigEMCluster", L2chain, *pRoI, true);
      if( emclIndex < 0 ) continue;
      float L1_eta = phys.trig_L1_emtau_eta->at( *pRoI );
      float L2_emcl_eta = phys.trig_L2_emcl_eta->at( emclIndex );
      h_col_eta_l1_emcl->Fill( L1_eta, L2_emcl_eta );
    }// Bottom of loop over RoI


  }
  //******************** Bottom of Event Loop ********************//
  file->Write();

  gStyle->SetPalette(1,0);
  TCanvas* c = new TCanvas( "c", "", 800, 400);
  c->Divide(2,1);
  c->cd(1);
  h_emcl_et_all->Draw();
  h_emcl_et_passed->SetLineColor(kYellow);
  h_emcl_et_passed->SetFillColor(kYellow);
  h_emcl_et_passed->Draw("same");
  c->cd(2);
  h_col_eta_l1_emcl->Draw("colz");
}
