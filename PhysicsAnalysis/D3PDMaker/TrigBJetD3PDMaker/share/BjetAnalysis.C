//----- INCLUDE STD LIBRARIES -----//
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
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
#include "BjetTree.h"

using namespace std;

void BjetAnalysis(const std::string& fname = "bjet.root") {
  //----- read TTree -----//
  TFile *f = new TFile(fname.c_str(), "READ");
  TTree* t = dynamic_cast<TTree*>(f->Get("BjetTree"));
  TTree* t_config = dynamic_cast<TTree*>(f->Get("BjetTreeMeta/TrigConfTree"));

  //----- MakeClass objects from bjet Tree  -----//
  BjetTree bjet(t);

  //----- Tool to access handle navigation information easily -----//
  TMNUtil util;
  util.initForD3PD(t, t_config);
 


  TFile* file = new TFile("test.root", "recreate");
  //----- declare histograms -----//
  TH1F* h_L2_zp = new TH1F( "h_L2_zp", "L2 Z of Primary Vertex ; Z of Primary Vertex [L2]; Entries", 100, -200, 200);
  TH1F* h_L2_zp_pass = new TH1F( "h_L2_zp_pass", "L2 Z of Primary Vertex for accepted events ; Z of Primary Vertex [L2] ; Entries", 100, -200, 200);
  TH1F* h_EF_zp = new TH1F( "h_EF_zp", "EF Z of Primary Vertex; Z of Primary Vertex [EF] ; Entries", 100, -200, 200);
  TH1F* h_EF_zp_pass = new TH1F( "h_EF_zp_pass", "EF Z of Primary Vertex for accepted events ; Z of Primary Vertex [EF] ; Entries", 100, -200, 200);
  TH1F* h_L2_XIP3D_all = new TH1F( "h_L2_XIP3D_all", "All L2 XIP3D; XIP3D ; Entries ", 50, 0, 1);
  TH1F* h_L2_XIP3D_pass = new TH1F( "h_L2_XIP3D_pass", "Passed L2 XIP3D ; XIP3D ; Entries ", 50, 0, 1);
  TH1F* h_EF_XIP3D_all = new TH1F( "h_EF_XIP3D_all", "All EF XIP3D; XIP3D ; Entries ", 50, 0, 1);
  TH1F* h_EF_XIP3D_pass = new TH1F( "h_EF_XIP3D_pass", "Passed EF XIP3D ; XIP3D ; Entries ", 50, 0, 1);

  TH1F* h_L1_eta = new TH1F("h_L1_eta", "L1 eta ; L1 eta ; Entries", 32, -3.2, 3.2);
  TH1F* h_L1_phi = new TH1F("h_L1_phi", "L1 phi ; L1 phi ; Entries", 32, -3.2, 3.2);

  TH1F* h_L2_trkN = new TH1F("L2_trkN", "L2 Tracks number ; N ; Entries", 100, 0, 100);
  TH1F* h_L2_trkPt = new TH1F("L2_trkPt", "L2 Tracks p_{T} ; p_{T} ; Entries", 7e3, 0, 7e3);
  TH1F* h_L2_trkEta = new TH1F("L2_trkEta", "L2 Tracks #eta ; #eta ; Entries", 32, -3.2, 3.2);
  TH1F* h_L2_trkPhi = new TH1F("L2_trkPhi", "L2 Tracks #phi ; #phi ; Entries", 32, -3.2, 3.2);
  TH1F* h_L2_trkZ0 = new TH1F("L2_trkZ0", "L2 Tracks z_{0}; z_{0} ; Entries", 100, -200, 200);

  TH1F* h_EF_trkN = new TH1F("EF_trkN", "EF Tracks number ; N ; Entries", 100, 0, 100);
  TH1F* h_EF_trkPt = new TH1F("EF_trkPt", "EF Tracks p_{T} ; p_{T} ; Entries", 7e3, 0, 7e3);
  TH1F* h_EF_trkEta = new TH1F("EF_trkEta", "EF Tracks #eta ; #eta ; Entries", 32, -3.2, 3.2);
  TH1F* h_EF_trkPhi = new TH1F("EF_trkPhi", "EF Tracks #phi ; #phi ; Entries", 32, -3.2, 3.2);
  TH1F* h_EF_trkZ0 = new TH1F("EF_trkZ0", "EF Tracks z_{0} ; z_{0} ; Entries", 100, -200, 200);


  TH2F* h_L2_EF_zp = new TH2F( "h_L2_EF_zp", "L2 Z of Primary Vertex vs EF Z of Primary Vertex; Z of Primary Vertex [L2] ; Z of Primary Vertex [EF]", 50, -100, 100, 50, -100, 100);

  std::string L2chain = "L2_b20";
  std::string EFchain = "EF_b20";
  int nentries = t->GetEntries();
  //******************** Top of Event Loop ********************//
  for (int i=0; i<nentries; ++i) {
    bjet.GetEntry(i);
    util.readNavigation();

    //util.dumpChainRoIFeatures(L2chain);

    //------ Example 1 just plot variable of a feature -----//
    vector< vector<int> > l2bjet_indices_all = util.getIndexVec( "TrigL2BjetContainer", L2chain, false );
    vector< vector<int> > l2bjet_indices_passed = util.getIndexVec( "TrigL2BjetContainer", L2chain, true );
    for(vector< vector<int> >::const_iterator p = l2bjet_indices_all.begin(); p != l2bjet_indices_all.end(); ++p) {
      for (vector<int>::const_iterator l = (*p).begin(); l != (*p).end(); ++l) {
        if (bjet.trig_l2bj_valid->at(*l) == 0) continue;

        float L2_XIP3D = bjet.trig_l2bj_xIP3D->at( *l );
        float L2_zp = bjet.trig_l2bj_prmVtx->at( *l );
        h_L2_XIP3D_all->Fill(L2_XIP3D);
        h_L2_zp->Fill(L2_zp);

      }
    }
    for(vector< vector<int> >::const_iterator p = l2bjet_indices_passed.begin(); p != l2bjet_indices_passed.end(); ++p) {
      for (vector<int>::const_iterator l = (*p).begin(); l != (*p).end(); ++l) {
        float L2_XIP3D = bjet.trig_l2bj_xIP3D->at( *l );
        float L2_zp = bjet.trig_l2bj_prmVtx->at( *l );
        h_L2_XIP3D_pass->Fill(L2_XIP3D);
        h_L2_zp_pass->Fill(L2_zp);
      }
    }
    vector<vector<int> > efbjet_indices_all = util.getIndexVec( "TrigEFBjetContainer", EFchain, false );
    for(vector< vector<int> >::const_iterator p = efbjet_indices_all.begin(); p != efbjet_indices_all.end(); ++p) {
      for (vector<int>::const_iterator l = (*p).begin(); l != (*p).end(); ++l) {
        float EF_zp = bjet.trig_efbj_prmVtx->at(*l);
        float EF_XIP3D = bjet.trig_efbj_xIP3D->at(*l);
        h_EF_zp->Fill(EF_zp);
        h_EF_XIP3D_all->Fill(EF_XIP3D);
      }
    }
    vector<vector<int> > efbjet_indices_pass = util.getIndexVec( "TrigEFBjetContainer", EFchain, true );
    for(vector< vector<int> >::const_iterator p = efbjet_indices_pass.begin(); p != efbjet_indices_pass.end(); ++p) {
      for (vector<int>::const_iterator l = (*p).begin(); l != (*p).end(); ++l) {
        float EF_zp = bjet.trig_efbj_prmVtx->at(*l);
        float EF_XIP3D = bjet.trig_efbj_xIP3D->at(*l);
        h_EF_zp_pass->Fill(EF_zp);
        h_EF_XIP3D_pass->Fill(EF_XIP3D);
      }
    }

    vector<int> L2RoIs = util.getRoINumbers( ChainEntry::kRoIType_L2_b, L2chain );
    vector<int> EFRoIs = util.getRoINumbers( ChainEntry::kRoIType_EF_b, EFchain );

    // get L1 information
    for (vector<int>::const_iterator pRoI = L2RoIs.begin(); pRoI != L2RoIs.end(); ++pRoI) {
      h_L1_eta->Fill(bjet.trig_L1_jet_eta->at(*pRoI));
      h_L1_phi->Fill(bjet.trig_L1_jet_phi->at(*pRoI));
    }

    //------ Example 2  Loop over RoI -----//
    for (vector<int>::const_iterator pRoI = L2RoIs.begin(); pRoI != L2RoIs.end(); ++pRoI ) {

      int L2RoIidx = *pRoI;
      vector<int>::const_iterator pEF = std::find(EFRoIs.begin(), EFRoIs.end(), L2RoIidx);
      if (pEF == EFRoIs.end()) {
        continue;
      }
      int EFRoIidx = *pEF;

      vector<int> l2bjIndex = util.getIndexVecInRoI( "TrigL2BjetContainer", L2chain, L2RoIidx, false);
      vector<int> efbjIndex = util.getIndexVecInRoI( "TrigEFBjetContainer", EFchain, EFRoIidx, false);

      if ( (l2bjIndex.size() != 1) || (efbjIndex.size() != 1) ) {
        std::cout << "ERROR! Different sizes: (" << l2bjIndex.size() << ", " << efbjIndex.size() << ")" << std::endl;
        continue;
      }

      if ( (l2bjIndex[0] < 0) || (efbjIndex[0] < 0) ) {
        continue;
      }

      float L2_zp = bjet.trig_l2bj_prmVtx->at( l2bjIndex[0] );
      float EF_zp = bjet.trig_efbj_prmVtx->at( efbjIndex[0] );
      h_L2_EF_zp->Fill(L2_zp, EF_zp);

    }// Bottom of loop over RoI

    // get tracks using L1 RoI to match to TrigL2Bjet
    for (vector<int>::const_iterator pRoI = L2RoIs.begin(); pRoI != L2RoIs.end(); ++pRoI ) {

      int L2RoIidx = *pRoI;
      vector<int> l2trkidx = util.getIndexVecInRoI( "TrigInDetTrackCollection_TrigSiTrack_Jet", L2chain, L2RoIidx, false );

      int N = 0;
      for (vector<int>::const_iterator itrk = l2trkidx.begin(); itrk != l2trkidx.end(); ++itrk) {
	if (*itrk < 0) continue;
	++N;

	h_L2_trkPt->Fill(bjet.trig_l2sitrk_jet_pt->at(*itrk));
	h_L2_trkEta->Fill(bjet.trig_l2sitrk_jet_eta->at(*itrk));
	h_L2_trkPhi->Fill(bjet.trig_l2sitrk_jet_phi0->at(*itrk));
	h_L2_trkZ0->Fill(bjet.trig_l2sitrk_jet_z0->at(*itrk));
      }
      h_L2_trkN->Fill(N);
    }
    

    for (vector<int>::const_iterator pRoI = EFRoIs.begin(); pRoI != EFRoIs.end(); ++pRoI ) {

      int EFRoIidx = *pRoI;
      vector<int> eftrkidx = util.getIndexVecInRoI( "Rec::TrackParticleContainer", EFchain, EFRoIidx, false );

      int N = 0;
      for (vector<int>::const_iterator itrk = eftrkidx.begin(); itrk != eftrkidx.end(); ++itrk) {
	if (*itrk < 0) continue;
	++N;

	h_EF_trkPt->Fill(bjet.trig_bjefid_pt->at(*itrk));
	h_EF_trkEta->Fill(bjet.trig_bjefid_eta->at(*itrk));
	h_EF_trkPhi->Fill(bjet.trig_bjefid_phi->at(*itrk));
	h_EF_trkZ0->Fill(bjet.trig_bjefid_z0->at(*itrk));
      }
      h_EF_trkN->Fill(N);
    }

  }
  //******************** Bottom of Event Loop ********************//
  file->Write();

  gStyle->SetPalette(1,0);
  TCanvas* c = new TCanvas( "c", "", 800, 400);
  c->Divide(2,2);
  c->cd(1);
  h_L2_zp_pass->Draw();
  c->cd(2);
  h_EF_zp_pass->Draw();
  c->cd(3);
  h_L2_XIP3D_all->Draw();
  c->cd(4);
  h_L2_XIP3D_pass->Draw();


  TCanvas* c1 = new TCanvas( "c1", "", 800, 400);
  c1->Divide(2,1);
  c1->cd(1);
  h_L2_zp->Draw();
  c1->cd(2);
  h_EF_zp->Draw();

  TCanvas* c2 = new TCanvas( "c2", "", 800, 400);
  h_L2_EF_zp->Draw();

}

