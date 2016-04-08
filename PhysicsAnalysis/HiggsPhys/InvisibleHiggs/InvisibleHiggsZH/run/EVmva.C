/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// program used to select the variables needed for the Boosted Tree analysis
// - reads in the events after using EV as a ntuple dumper
// - applies all preselection cuts to select events with 2 leptons from a Z and large missing pt

using namespace std;

#define CHAIN

#include <fstream>
using std::ofstream;

/******************* Include files for C++ **************************/
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <string.h>
#include <vector>
#include <list>  
#include <stdlib.h>

/******************* Include files for ROOT *************************/
#include "TROOT.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TLine.h"
#include "TPaveLabel.h"
#include "TNtuple.h"
#include "TH1.h"
#include "THStack.h"
#include "TPad.h"
#include "TFile.h"
#include "TLegend.h"
#include "TChain.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TFrame.h"
#include "TAttFill.h"
#include "Random.h"
#include "TRandom3.h"
#include "TLatex.h"

// - - - Initiates the GUI interface of ROOT - - -
using std::string;
using std::setprecision;
extern void InitGui();

//============================================================================

int main(int argc, char **argv)
{
// execute rootlogon.C to get ATLAS style setup.
#include "rootlogon.C"

  VoidFuncPtr_t initfuncs[] = { InitGui, 0 };

//   void ATLAS_LABEL(Double_t x,Double_t y,Color_t color=1) {

    TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
//     l.SetNDC();
//     l.SetTextFont(72);
//     l.SetTextColor(color);
//     l.DrawLatex(x,y,"ATLAS");
//   }

  TROOT root("","", initfuncs);
  
  TApplication theApp("App", &argc, argv);
//   gROOT->Reset();
//   gROOT->SetStyle("ATLAS");
//   gROOT->ForceStyle();   
  
  // bool EF_passed = true;
  
   int seed = 47;
   //   srand ( time(0) ); // for a random seed
   srand(seed);  // for a fixed start

  double zmm[15] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double zee[15] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double zmm0[15] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double zee0[15] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double zmm1[15] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double zee1[15] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  
  int counter = 0;
  int typeNumber = 0;
  int ntype = 8;
  const int ndatasets = 1;
  
  double generated[15] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  
  const string eventType[15] = {("HZ120"),("ZZ"),("ttbar"),("WWenuenu"),("WWenumunu"),("WWmunuenu"),("WWmunumunu"),("ZZtautaununu"),("ZZlltautau"),("ZW+"),("ZW-"),("Zeel"),("Zeeb"),("Zmml"),("Zmmb") }; 
  
//   //  mH = 110 GeV
//      double sigmaBR[15] = {0.0614, 0.398, 7.36, 1.350, 1.327, 1.327, 1.305, 0.1995,  0.067, 0.4275, 0.2675, 1452.6556, 99.8886, 1452.7175, 99.80114}; 
//      double filterEff[15] = {0.470, 0.292, 0.0113, 0.0898, 0.000634, 0.000623, 0.0893, 0.0516, 0.714, 1.0, 1.0, 0.0000528,0.0016551,0.000052,0.0016118};

//   //  mH = 120 GeV
      double sigmaBR[15] = {0.0464, 0.398, 7.36, 1.350, 1.327, 1.327, 1.305, 0.1995,  0.067, 0.4275, 0.2675, 1452.6556, 99.8886, 1452.7175, 99.80114}; 
      double filterEff[15] = {0.496, 0.292, 0.0113, 0.0898, 0.000634, 0.000623, 0.0893, 0.0516, 0.714, 1.0, 1.0, 0.0000528,0.0016551,0.000052,0.0016118};

// mH = 130 GeV
//      double sigmaBR[15] = { 0.0359, 0.398, 7.36, 1.350, 1.327, 1.327, 1.305, 0.1995,  0.067, 0.4275, 0.2675, 1452.6556, 99.8886, 1452.7175, 99.80114}; 
//      double filterEff[15] = {0.754, 0.292, 0.0113, 0.0898, 0.000634, 0.000623, 0.0893, 0.0516, 0.714, 1.0, 1.0, 0.0000528,0.0016551,0.000052,0.0016118};

// //   // mH = 140 GeV
//      double sigmaBR[15] = { 0.0275, 0.398, 7.36, 1.350, 1.327, 1.327, 1.305, 0.1995,  0.067, 0.4275, 0.2675, 1452.6556, 99.8886, 1452.7175, 99.80114}; 
//      double filterEff[15] = {0.759, 0.292, 0.0113, 0.0898, 0.000634, 0.000623, 0.0893, 0.0516, 0.714, 1.0, 1.0, 0.0000528,0.0016551,0.000052,0.0016118};
  
// //   // mH = 150 GeV
//      double sigmaBR[15] = { 0.0218, 0.398, 7.36, 1.350, 1.327, 1.327, 1.305, 0.1995,  0.067, 0.4275, 0.2675, 1452.6556, 99.8886, 1452.7175, 99.80114}; 
//      double filterEff[15] = {0.564, 0.292, 0.0113, 0.0898, 0.000634, 0.000623, 0.0893, 0.0516, 0.714, 1.0, 1.0, 0.0000528,0.0016551,0.000052,0.0016118};
  
// //   // mH = 200 GeV
//      double sigmaBR[15] = { 0.0068, 0.398, 7.36, 1.350, 1.327, 1.327, 1.305, 0.1995,  0.067, 0.4275, 0.2675, 1452.6556, 99.8886, 1452.7175, 99.80114}; 
//      double filterEff[15] = {0.646, 0.292, 0.0113, 0.0898, 0.000634, 0.000623, 0.0893, 0.0516, 0.714, 1.0, 1.0, 0.0000528,0.0016551,0.000052,0.0016118};
  
// //   // mH = 250 GeV
//       double sigmaBR[15] = { 0.00293, 0.398, 7.36, 1.350, 1.327, 1.327, 1.305, 0.1995,  0.067, 0.4275, 0.2675, 1452.6556, 99.8886, 1452.7175, 99.80114}; 
//       double filterEff[15] = {0.701, 0.292, 0.0113, 0.0898, 0.000634, 0.000623, 0.0893, 0.0516, 0.714, 1.0, 1.0, 0.0000528,0.0016551,0.000052,0.0016118};
  
  int found[15];
  double Total[15];
  double TotalUM[15] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double weight[15]; 
  double totalRejected[15];
  int nrec[15];
  double passed[15];
  int sanity[15];
  
  double reject_MET[15] ;
  double reject_nlep[15] ;
  double reject_trigger[15] ;
  double reject_mZ[15] ;
  double reject_leppid[15] ;
  double reject_btag[15] ;
  
  string histName1 = ("ptlep1");
  string histName2 = ("ptlep2");
  string histName3 = ("cosll2D");
  string histName4 = ("cosll3D");
  string histName5 = ("MET");
  string histName6 = ("mt");
  string histName7 = ("cosMETPtlep");
  string histName8 = ("mZll");
  string histName9 = ("cosphi");
  string histName10 = ("cosJetPtmiss");
  string histName11 = ("cone1");
  string histName12 = ("cone2");
  string histName13 = ("ejet1");
  string histName14 = ("ejet2");
  string histName15 = ("ejet3");
  string histName16 = ("njet");
  string histName17 = ("nel");
  string histName18 = ("nmu");
  string histName19 = ("nlep");
  string histName20 = ("lepid");
   
  int doublepair[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  
  double lumi = 30000.0;
  double TotalBgnd = 0.;
  
  int L1XET100[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  
  const unsigned int XC1=0, YC1=0, XC2=800, YC2=800;
  const double G=0.004;
  
  TCanvas* c1 = new TCanvas("c1","variables set 1", XC1, YC1, XC2, YC2);
  c1->cd();
  c1->SetFillColor(30);
  c1->GetFrame()->SetFillColor(10);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);
  TPad* pad11= new TPad("pad11", "Pad11",       G,     G,       0.5-G,   0.5-G);
  TPad* pad12= new TPad("pad12", "Pad12",   0.5+G,     G,       1.0-G,   0.5-G);
  TPad* pad13= new TPad("pad13", "Pad13",       G, 0.5+G,       0.5-G,   1.0-G);
  TPad* pad14= new TPad("pad14", "Pad14",   0.5+G, 0.5+G,       1.0-G,   1.0-G);
  pad11->SetFillColor(10); pad11->Draw();
  pad12->SetFillColor(10); pad12->Draw();   
  pad13->SetFillColor(10); pad13->Draw();
  pad14->SetFillColor(10); pad14->Draw();

  TCanvas* c2 = new TCanvas("c2","variables set 2", XC1, YC1, XC2, YC2);
  c2->cd();
  c2->SetFillColor(10);
  c2->GetFrame()->SetFillColor(10);
  c2->GetFrame()->SetBorderSize(6);
  c2->GetFrame()->SetBorderMode(-1);
  TPad* pad21= new TPad("pad21", "Pad21",       G,     G,       0.5-G,   0.5-G);
  TPad* pad22= new TPad("pad22", "Pad22",   0.5+G,     G,       1.0-G,   0.5-G);
  TPad* pad23= new TPad("pad23", "Pad23",       G, 0.5+G,       0.5-G,   1.0-G);
  TPad* pad24= new TPad("pad24", "Pad24",   0.5+G, 0.5+G,       1.0-G,   1.0-G);
  pad21->SetFillColor(10); pad21->Draw();
  pad22->SetFillColor(10); pad22->Draw();   
  pad23->SetFillColor(10); pad23->Draw();
  pad24->SetFillColor(10); pad24->Draw();
  
  TCanvas* c3 = new TCanvas("c3","variables set 3", XC1, YC1, XC2, YC2);
  c3->cd();
  c3->SetFillColor(10);
  c3->GetFrame()->SetFillColor(10);
  c3->GetFrame()->SetBorderSize(6);
  c3->GetFrame()->SetBorderMode(-1);
  TPad* pad31= new TPad("pad31", "Pad31",       G,     G,       0.5-G,   0.5-G);
  TPad* pad32= new TPad("pad32", "Pad32",   0.5+G,     G,       1.0-G,   0.5-G);
  TPad* pad33= new TPad("pad33", "Pad33",       G, 0.5+G,       0.5-G,   1.0-G);
  TPad* pad34= new TPad("pad34", "Pad34",   0.5+G, 0.5+G,       1.0-G,   1.0-G);
  pad31->SetFillColor(10); pad31->Draw();
  pad32->SetFillColor(10); pad32->Draw();   
  pad33->SetFillColor(10); pad33->Draw();
  pad34->SetFillColor(10); pad34->Draw();
  
  TCanvas* c4 = new TCanvas("c4","variables set 4", XC1, YC1, XC2, YC2);
  c4->cd();
  c4->SetFillColor(10);
  c4->GetFrame()->SetFillColor(10);
  c4->GetFrame()->SetBorderSize(6);
  c4->GetFrame()->SetBorderMode(-1);
  TPad* pad41= new TPad("pad41", "Pad41",       G,     G,       0.5-G,   0.5-G);
  TPad* pad42= new TPad("pad42", "Pad42",   0.5+G,     G,       1.0-G,   0.5-G);
  TPad* pad43= new TPad("pad43", "Pad43",       G, 0.5+G,       0.5-G,   1.0-G);
  TPad* pad44= new TPad("pad44", "Pad44",   0.5+G, 0.5+G,       1.0-G,   1.0-G);
  pad41->SetFillColor(10); pad41->Draw();
  pad42->SetFillColor(10); pad42->Draw();   
  pad43->SetFillColor(10); pad43->Draw();
  pad44->SetFillColor(10); pad44->Draw();
  
  TCanvas* c5 = new TCanvas("c5","variables set 5", XC1, YC1, XC2, YC2);
  c5->cd();
  c5->SetFillColor(10);
  c5->GetFrame()->SetFillColor(10);
  c5->GetFrame()->SetBorderSize(6);
  c5->GetFrame()->SetBorderMode(-1);
  TPad* pad51= new TPad("pad51", "Pad51",       G,     G,       0.5-G,   0.5-G);
  TPad* pad52= new TPad("pad52", "Pad52",   0.5+G,     G,       1.0-G,   0.5-G);
  TPad* pad53= new TPad("pad53", "Pad53",       G, 0.5+G,       0.5-G,   1.0-G);
  TPad* pad54= new TPad("pad54", "Pad54",   0.5+G, 0.5+G,       1.0-G,   1.0-G);
  pad51->SetFillColor(10); pad51->Draw();
  pad52->SetFillColor(10); pad52->Draw();   
  pad53->SetFillColor(10); pad53->Draw();
  pad54->SetFillColor(10); pad54->Draw();
  
  TCanvas* c6 = new TCanvas("c6","variables set 6", XC1, YC1, XC2, YC2);
  c6->cd();
  c6->SetFillColor(10);
  c6->GetFrame()->SetFillColor(10);
  c6->GetFrame()->SetBorderSize(6);
  c6->GetFrame()->SetBorderMode(-1);
  TPad* pad61= new TPad("pad61", "Pad21",       G,     G,       0.5-G,   0.5-G);
  TPad* pad62= new TPad("pad62", "Pad22",   0.5+G,     G,       1.0-G,   0.5-G);
  TPad* pad63= new TPad("pad63", "Pad23",       G, 0.5+G,       0.5-G,   1.0-G);
  TPad* pad64= new TPad("pad64", "Pad24",   0.5+G, 0.5+G,       1.0-G,   1.0-G);
  pad61->SetFillColor(10); pad61->Draw();
  pad62->SetFillColor(10); pad62->Draw();   
  pad63->SetFillColor(10); pad63->Draw();
  pad64->SetFillColor(10); pad64->Draw();

  // -----------------------------------------------------------
  //      - - -  Creating stacks - - -  
  // -----------------------------------------------------------
  
  THStack hs1("hs1","pt_lepton 1");
  THStack hs2("hs2","pt_lepton 2");
  THStack hs3("hs3","cosll2D");
  THStack hs4("hs4","cosll3D");
  THStack hs5("hs5","MET");
  THStack hs6("hs6","mt");
  THStack hs7("hs7","cosMETPtlep");
  THStack hs8("hs8","mZll");
  THStack hs9("hs9","cos Phi");
  THStack hs10("hs10","cosJetMET");
  THStack hs11("hs11","cone1");
  THStack hs12("hs12","cone2");
  THStack hs13("hs13","ejet1");
  THStack hs14("hs14","ejet2");
  THStack hs15("hs15","ejet3");
  THStack hs16("hs16","njet");
  THStack hs17("hs17","nel");
  THStack hs18("hs18","nmu");
  THStack hs19("hs19","nlep");
  THStack hs20("hs20","lepid");

//   THStack hs1("hs1","");
//   THStack hs2("hs2","");
//   THStack hs3("hs3","");
//   THStack hs4("hs4","");
//   THStack hs5("hs5","");
//   THStack hs6("hs6","");
//   THStack hs7("hs7","");
//   THStack hs8("hs8","");
//   THStack hs9("hs9","");
//   THStack hs10("hs10","");
//   THStack hs11("hs11","");
//   THStack hs12("hs12","");
//   THStack hs13("hs13","");
//   THStack hs14("hs14","");
//   THStack hs15("hs15","");
//   THStack hs16("hs16","");
//   THStack hs17("hs17","");
//   THStack hs18("hs18","");
//   THStack hs19("hs19","");
//   THStack hs20("hs20","");

  // -----------------------------------------------------------
  //      - - -  Creating many histograms - - -  
  // -----------------------------------------------------------
  TH1F* hgauss1= new TH1F("gaussian", "MET before ", 100, 90., 300.);
  TH1F* hgauss2= new TH1F("gaussian", "MET after", 100, 90., 300.);

  TH1F* hptlep1[ntype];
  TH1F* hptlep2[ntype];
  TH1F* hcosll2D[ntype];
  TH1F* hcosll3D[ntype];
  TH1F* hMET[ntype];
  TH1F* hmt[ntype];
  TH1F* hcosMETPtlep[ntype];
  TH1F* hmZll[ntype];
  TH1F* hcosphi[ntype];
  TH1F* hcosJetMET[ntype];
  TH1F* hcone1[ntype];
  TH1F* hcone2[ntype];
  TH1F* hejet1[ntype];
  TH1F* hejet2[ntype];
  TH1F* hejet3[ntype];
  TH1F* hnjet[ntype];
  TH1F* hnel[ntype];
  TH1F* hnmu[ntype];
  TH1F* hnlep[ntype];
  TH1F* hlepid[ntype];
  
  string ka[8] = {"0", "1", "2", "3", "4", "5", "6", "7"};
  
  for (int m=0; m<ntype; m++) {
    histName1.insert(6,ka[m]);
    histName2.insert(6,ka[m]);
    histName3.insert(7,ka[m]);
    histName4.insert(7,ka[m]);
    histName5.insert(3,ka[m]);
    histName6.insert(2,ka[m]);
    histName7.insert(11,ka[m]);
    histName8.insert(4,ka[m]);
    histName9.insert(6,ka[m]);
    histName10.insert(9,ka[m]);
    histName11.insert(5,ka[m]);
    histName12.insert(5,ka[m]);
    histName13.insert(5,ka[m]);
    histName14.insert(5,ka[m]);
    histName15.insert(5,ka[m]);
    histName16.insert(4,ka[m]);
    histName17.insert(3,ka[m]);
    histName18.insert(3,ka[m]);
    histName19.insert(4,ka[m]);
    histName20.insert(5,ka[m]);
    
    hptlep1[m] = new TH1F(histName1.c_str(), "; p_T^{lepton @#1} ;@# of entries", 50, 0, 300.0);
    hptlep2[m] = new TH1F(histName2.c_str(), ";p_T^{lepton @#2} ;@# of entries", 50, 0, 100.0);
    hcosll2D[m] = new TH1F(histName3.c_str(), ";#cos_{ll} (xy plane);@# of entries", 50, -1.1, 1.1);
    hcosll3D[m] = new TH1F(histName4.c_str(), ";#cos_{ll};@# of entries", 50, -1.1, 1.1);
    hMET[m] = new TH1F(histName5.c_str(), ";p_{T} missing per event;@# of entries", 50, 50, 500);
    hmt[m] = new TH1F(histName6.c_str(), ";m_T;@# of entries", 50, 0, 700.0);
    hcosMETPtlep[m] = new TH1F(histName7.c_str(), ";#cos MET-Ptlep;@# of entries", 50, -1.1, 1.1);
    hmZll[m] = new TH1F(histName8.c_str(), ";M_{Z #rightarrow ll};@# of entries", 50, 71.19, 111.19);
    hcosphi[m] = new TH1F(histName9.c_str(), ";#cos #phi;@# of entries", 50, -1.1, 1.1);
    hcosJetMET[m] = new TH1F(histName16.c_str(), ";#cos #phi (between p_{T} miss and jet;@# of entries", 50, -1.0, 1.2);
    hcone1[m] = new TH1F(histName11.c_str(), ";electron isolation cone 0.10 ;@# of entries", 50, 0.0, 300.0);
    hcone2[m] = new TH1F(histName12.c_str(), ";muon isolation cone 0.10 ;@# of entries", 50, 0.0, 300.0);
    hejet1[m] = new TH1F(histName13.c_str(), ";energy jet @#1 ;@# of entries", 50, 0, 300.0);
    hejet2[m] = new TH1F(histName14.c_str(), ";energy jet @#2 ;@# of entries", 50, 0, 300.0);
    hejet3[m] = new TH1F(histName15.c_str(), ";energy jet @#3 ;@# of entries", 50, 0, 300.0);
    hnjet[m] = new TH1F(histName10.c_str(), ";@# jets ;@# of entries", 11, 0, 10);
    hnel[m] = new TH1F(histName17.c_str(), ";@#electrons ;@# of entries", 6, 0, 5);
    hnmu[m] = new TH1F(histName18.c_str(), ";@# muons ;@# of entries", 6, 0, 5);
    hnlep[m] = new TH1F(histName19.c_str(), ";@# leptons ;@# of entries", 11, 0, 10);
    hlepid[m] = new TH1F(histName20.c_str(), ";lepton ID ;@# of entries", 26, 0, 25);
   
    hptlep1[m]->SetTitle("p_{T} lepton @# 1");
    hptlep2[m]->SetTitle("p_{T} lepton @# 1");
    hcosll2D[m]->SetTitle("#cos l l in 2D");
    hcosll3D[m]->SetTitle("#cos l l in 3D");
    hMET[m]->SetTitle("Missing E_{T}");
    hmt[m]->SetTitle("transverse mass");
    hcosMETPtlep[m]->SetTitle("");
    hmZll[m]->SetTitle("Z mass");
    hcosphi[m]->SetTitle("#cos #phi");
    hcosJetMET[m]->SetTitle("#cos jet-MET");
    hcone1[m]->SetTitle("E cone lepton @# 1");
    hcone2[m]->SetTitle("E cone lepton @# 2");
    hejet1[m]->SetTitle("energy jet @#1");
    hejet2[m]->SetTitle("energy jet @#2");
    hejet3[m]->SetTitle("energy jet @#3");
    hnjet[m]->SetTitle("@# jets");
    hnel[m]->SetTitle("@# electrons");
    hnmu[m]->SetTitle("@# muons");
    hnlep[m]->SetTitle("@# leptons");
    hlepid[m]->SetTitle("lepton ID");
    
    hptlep1[m]->SetTitle("@# of entries");
    hptlep2[m]->SetTitle("@# of entries");
    hcosll2D[m]->SetTitle("@# of entries");
    hcosll3D[m]->SetTitle("@# of entries");
    hMET[m]->SetTitle("@# of entries");
    hmt[m]->SetTitle("@# of entries");
    hcosMETPtlep[m]->SetTitle("@# of entries");
    hmZll[m]->SetTitle("@# of entries");
    hcosphi[m]->SetTitle("@# of entries");
    hcosJetMET[m]->SetTitle("@# of entries");
    hcone1[m]->SetTitle("@# of entries");
    hcone2[m]->SetTitle("@# of entries");
    hejet1[m]->SetTitle("@# of entries");
    hejet2[m]->SetTitle("@# of entries");
    hejet3[m]->SetTitle("@# of entries");
    hnjet[m]->SetTitle("@# of entries");
    hnel[m]->SetTitle("@# of entries");
    hnmu[m]->SetTitle("@# of entries");
    hnlep[m]->SetTitle("@# of entries");
    hlepid[m]->SetTitle("@# of entries");

//     hptlep1[m]->GetXaxis()->SetTitle("p_{T} lepton @# 1");
//     hptlep2[m]->GetXaxis()->SetTitle("p_{T} lepton @# 1");
//     hcosll2D[m]->GetXaxis()->SetTitle("#cos l l in 2D");
//     hcosll3D[m]->GetXaxis()->SetTitle("#cos l l in 3D");
//     hMET[m]->GetXaxis()->SetTitle("Missing E_{T}");
//     hmt[m]->GetXaxis()->SetTitle("transverse mass");
//     hcosMETPtlep[m]->GetXaxis()->SetTitle("");
//     hmZll[m]->GetXaxis()->SetTitle("Z mass");
//     hcosphi[m]->GetXaxis()->SetTitle("#cos #phi");
//     hcosJetMET[m]->GetXaxis()->SetTitle("#cos jet-MET");
//     hcone1[m]->GetXaxis()->SetTitle("E cone lepton @# 1");
//     hcone2[m]->GetXaxis()->SetTitle("E cone lepton @# 2");
//     hejet1[m]->GetXaxis()->SetTitle("energy jet @#1");
//     hejet2[m]->GetXaxis()->SetTitle("energy jet @#2");
//     hejet3[m]->GetXaxis()->SetTitle("energy jet @#3");
//     hnjet[m]->GetXaxis()->SetTitle("@# jets");
//     hnel[m]->GetXaxis()->SetTitle("@# electrons");
//     hnmu[m]->GetXaxis()->SetTitle("@# muons");
//     hnlep[m]->GetXaxis()->SetTitle("@# leptons");
//     hlepid[m]->GetXaxis()->SetTitle("lepton ID");
    
//     hptlep1[m]->GetYaxis()->SetTitle("@# of entries");
//     hptlep2[m]->GetYaxis()->SetTitle("@# of entries");
//     hcosll2D[m]->GetYaxis()->SetTitle("@# of entries");
//     hcosll3D[m]->GetYaxis()->SetTitle("@# of entries");
//     hMET[m]->GetYaxis()->SetTitle("@# of entries");
//     hmt[m]->GetYaxis()->SetTitle("@# of entries");
//     hcosMETPtlep[m]->GetYaxis()->SetTitle("@# of entries");
//     hmZll[m]->GetYaxis()->SetTitle("@# of entries");
//     hcosphi[m]->GetYaxis()->SetTitle("@# of entries");
//     hcosJetMET[m]->GetYaxis()->SetTitle("@# of entries");
//     hcone1[m]->GetYaxis()->SetTitle("@# of entries");
//     hcone2[m]->GetYaxis()->SetTitle("@# of entries");
//     hejet1[m]->GetYaxis()->SetTitle("@# of entries");
//     hejet2[m]->GetYaxis()->SetTitle("@# of entries");
//     hejet3[m]->GetYaxis()->SetTitle("@# of entries");
//     hnjet[m]->GetYaxis()->SetTitle("@# of entries");
//     hnel[m]->GetYaxis()->SetTitle("@# of entries");
//     hnmu[m]->GetYaxis()->SetTitle("@# of entries");
//     hnlep[m]->GetYaxis()->SetTitle("@# of entries");
//     hlepid[m]->GetYaxis()->SetTitle("@# of entries");
 
    Color_t color = kWhite;
    if (m==0) color = kRed;
    if (m==1) color = kYellow;
    if (m==2) color = kBlue;
    if (m==3) color = kGreen;
    if (m==4) color = kBlack;
    if (m==5) color = kWhite;
    if (m==6) color = kMagenta;
    if (m==7) color = kBlack;

//     int style = 0;
//     if (m==0) style = 3001;
//     if (m==1) style = 3006;
//     if (m==2) style = 3017;
//     if (m==3) style = 3004;
//     if (m==4) style = 3005;
    
    hptlep1[m]->SetFillColor(color);
    hptlep2[m]->SetFillColor(color);
    hcosll2D[m]->SetFillColor(color);
    hcosll3D[m]->SetFillColor(color);
    hMET[m]->SetFillColor(color);     
    hmt[m]->SetFillColor(color);     
    hcosMETPtlep[m]->SetFillColor(color);     
    hmZll[m]->SetFillColor(color);    
    hcosphi[m]->SetFillColor(color);     
    hcosJetMET[m]->SetFillColor(color);     
    hcone1[m]->SetFillColor(color);
    hcone2[m]->SetFillColor(color);
    hejet1[m]->SetFillColor(color);
    hejet2[m]->SetFillColor(color);
    hejet3[m]->SetFillColor(color);
    hnjet[m]->SetFillColor(color);
    hnel[m]->SetFillColor(color);
    hnmu[m]->SetFillColor(color);
    hnlep[m]->SetFillColor(color);
    hlepid[m]->SetFillColor(color);

//     hptlep1[m]->SetFillStyle(style);
//     hptlep2[m]->SetFillStyle(style);
//     hcosll2D[m]->SetFillStyle(style);
//     hcosll3D[m]->SetFillStyle(style);
//     hMET[m]->SetFillStyle(style);     
//     hmt[m]->SetFillStyle(style);     
//     hcosMETPtlep[m]->SetFillStyle(style);     
//     hmZll[m]->SetFillStyle(style);    
//     hcosphi[m]->SetFillStyle(style);     
//     hcosJetMET[m]->SetFillStyle(style);     
//     hcone1[m]->SetFillStyle(style);
//     hcone2[m]->SetFillStyle(style);
//     hejet1[m]->SetFillStyle(style);
//     hejet2[m]->SetFillStyle(style);
//     hejet3[m]->SetFillStyle(style);
//     hnjet[m]->SetFillStyle(style);
  }
  
  // variables of the ntuples are being declared below.  
  
  // cuts for AOD 
  const double eptmin  = 10.0;
  const double mptmin  = 10.0;
  const double etamin   = 2.5;
  const int    nlepcut  = 2;
  const double METcut   = 90.0;
  const double mZcut    = 20.;
  const double mZ0      = 91.190;
  
  double modulus =0.;
  double cosll2D = 0.;
  double cosll3D = 0.;
  double cosPhi = 0.;
  double cosptlepMET = 0.;
  double mt = 0.;
  double pt2l = 0.;
  double PX = 0.;
  double PY = 0.;
  double PT = 0.;
  
  double ptZ0 = 0.;
  double eta1 = 0.;
  double Cone1 = 0.;
  double pt1 = 0.;
  double px1 = 0.;
  double py1 = 0.;
  double pz1 = 0.;
  double eta2 = 0.;
  double Cone2 = 0.;
  double px2 = 0.;
  double py2 = 0.;
  double pz2 = 0.;
  double pt2 = 0.;
  double cosjetmet = 0.;
  
  const double mass_e = 0.00051099892;
  const double mass_mu = 0.105658369;
  
  int nb=0;
  double portion;
  bool last_rec=0;
  
  // open output file to save the cutflow results
  ofstream outClientFile( "cutflow.dat", ios::out );
  
  // exit program if unable to open file
  if ( !outClientFile ) {
    cerr << "File could not be opened" << endl;
    exit (1);
  } // end if
  
  // create a new ROOT file
  TFile f("preselection-HZ120-10jun.root","recreate");
  
  // create two TTree, one for signal, one for background
  TTree treeS("TreeS","preselected signal events");
  TTree treeB("TreeB","preselected background events");
  
  int njet;
  int nlep = 0;
  int nel = 0;;
  int nmu = 0;
  float ejet1 = 0.0;
  float ejet2 = 0.0;
  float ejet3 = 0.0;
  float ptlep1;
  float ptlep2;
  float cone1;
  float cone2;
  float massZ;
  float cosJetMET = 0.;
  float cosll2d;
  float cosll3d;
  float mT;
  float MET;
  float cosMETptlep;
  float cosphi;
  
  int lepID;
  
  int runnum;
  int evtnum;
  int type;
  float w;
  
  // create one branch for each variable
  
  treeS.Branch("njet",&njet,"njet/I");	         
  treeS.Branch("ejet1",&ejet1,"ejet1/F");	         
  treeS.Branch("ejet2",&ejet2,"ejet2/F");	         
  treeS.Branch("ejet3",&ejet3,"ejet3/F");	         
  treeS.Branch("ptlep1",&ptlep1,"ptlep1/F");	         
  treeS.Branch("ptlep2",&ptlep2,"ptlep2/F");	         
  treeS.Branch("cone1",&cone1,"cone1/F");	         
  treeS.Branch("cone2",&cone2,"cone2/F");	         
  treeS.Branch("nel",&nel,"nel/I");	         
  treeS.Branch("nmu",&nmu,"nmu/I");	         
  treeS.Branch("nlep",&nlep,"nlep/I");	         
  treeS.Branch("lepID",&lepID,"lepID/I");	         
  treeS.Branch("massZ",&massZ,"massZ/F");	         
  treeS.Branch("cosll2d",&cosll2d,"cosll2d/F");	         
  treeS.Branch("cosll3d",&cosll3d,"cosll3d/F");	         
  treeS.Branch("mT",&mT,"mT/F");	         
  treeS.Branch("MET",&MET,"MET/F");	         
  treeS.Branch("cosMETptlep",&cosMETptlep,"cosMETptlep/F");	         
  treeS.Branch("cosJetMET",&cosJetMET,"cosJetMET/F");	         
  treeS.Branch("cosphi",&cosphi,"cosphi/F");	         
  treeS.Branch("runnum",&runnum,"runnum/I");
  treeS.Branch("evtnum",&evtnum,"evtnum/I");
  treeS.Branch("type",&type,"type/I");
  treeS.Branch("w",&w,"w/F");
  
  treeB.Branch("njet",&njet,"njet/I");	         
  treeB.Branch("ejet1",&ejet1,"ejet1/F");	         
  treeB.Branch("ejet2",&ejet2,"ejet2/F");	         
  treeB.Branch("ejet3",&ejet3,"ejet3/F");	         
  treeB.Branch("ptlep1",&ptlep1,"ptlep1/F");	         
  treeB.Branch("ptlep2",&ptlep2,"ptlep2/F");	         
  treeB.Branch("cone1",&cone1,"cone1/F");	         
  treeB.Branch("cone2",&cone2,"cone2/F");	         
  treeB.Branch("nel",&nel,"nel/I");	         
  treeB.Branch("nmu",&nmu,"nmu/I");	         
  treeB.Branch("nlep",&nlep,"nlep/I");	         
  treeB.Branch("lepID",&lepID,"lepID/I");	         
  treeB.Branch("massZ",&massZ,"massZ/F");	         
  treeB.Branch("cosll2d",&cosll2d,"cosll2d/F");	         
  treeB.Branch("cosll3d",&cosll3d,"cosll3d/F");	         
  treeB.Branch("mT",&mT,"mT/F");	         
  treeB.Branch("MET",&MET,"MET/F");	         
  treeB.Branch("cosMETptlep",&cosMETptlep,"cosMETptlep/F");	         
  treeB.Branch("cosJetMET",&cosJetMET,"cosJetMET/F");	         
  treeB.Branch("cosphi",&cosphi,"cosphi/F");	         
  treeB.Branch("runnum",&runnum,"runnum/I");
  treeB.Branch("evtnum",&evtnum,"evtnum/I");
  treeB.Branch("type",&type,"type/I");
  treeB.Branch("w",&w,"w/F");
  
  
  // -----------------------------------------------------------
  // ----------- loop over the various datasets ----------
  // -----------------------------------------------------------
  for (int n=0; n<ndatasets; n++) {
    reject_MET[n] =0.;
    reject_nlep[n] =0.;
    reject_trigger[n] =0.;
    reject_leppid[n] =0.;
    reject_mZ[n] =0.;
    reject_btag[n] =0.;
    totalRejected[n] =0.;
    sanity[n]=0;
  }
  
  // loop over all datasets
  for (int n=0; n<ndatasets; n++) {

    found[n]=0;
    TRandom * random = new TRandom3(seed);

    
    /***************** Getting file/tree from disk *********************/
    // We define chain, which is basically a tree to add root files with the
    // same tree name.  If you have two or more files, keep adding in the 
    // nfchain variable.
    
    TChain *chain=0;
    
#ifdef CHAIN
    //     chain = new TChain("EVCands");
    chain = new TChain("EV0");
    int nfchain=0;
    
// // //     //    ZZllll sample - 50k filtered events
// // //          if (n==1) {
// // //            sigmaBR[1] = 0.0670; 
// // //            filterEff[1] = 0.219;
// // //            typeNumber = 10;
// // //            nfchain += chain->Add("/data/ntuples/ZZ4l_ntuple.root");}
    
//     // ChL SUSY sample - 35k
//          if (n==1) {
//            typeNumber = 1;
//            nfchain += chain->Add("/data/ntuples/ChL_ntuple.root");}
    
    
    // HZ110 sample - 50k filtered events
    if (n==0 && eventType[0] == "HZ110") {
      typeNumber = 0;
      nfchain += chain->Add("/data/ntuples/mh110_ntuple.root");}

    // HZ120 sample - 50k filtered events - filtered with multilepton filter
    if (n==0 && eventType[0] == "HZ120") {
      typeNumber = 0;
      nfchain += chain->Add("/data/HZ/user.PaulineGagnon.EV-HZinv-HZinv-006387_tid10758-2.AANT0._00001.root");
      nfchain += chain->Add("/data/HZ/user.PaulineGagnon.EV-HZinv-HZinv-006387_tid10758-2.AANT0._00002.root");
      nfchain += chain->Add("/data/HZ/user.PaulineGagnon.EV-HZinv-HZinv-006387_tid10758-2.AANT0._00003.root");
      nfchain += chain->Add("/data/HZ/user.PaulineGagnon.EV-HZinv-HZinv-006387_tid10758-2.AANT0._00004.root");
      nfchain += chain->Add("/data/HZ/user.PaulineGagnon.EV-HZinv-HZinv-006387_tid10758-2.AANT0._00005.root");
      nfchain += chain->Add("/data/HZ/user.PaulineGagnon.EV-HZinv-HZinv-006387_tid10758-2.AANT0._00006.root");
      nfchain += chain->Add("/data/HZ/user.PaulineGagnon.EV-HZinv-HZinv-006387_tid10758-2.AANT0._00007.root");
      nfchain += chain->Add("/data/HZ/user.PaulineGagnon.EV-HZinv-HZinv-006387_tid10758-2.AANT0._00008.root");
      nfchain += chain->Add("/data/HZ/user.PaulineGagnon.EV-HZinv-HZinv-006387_tid10758-2.AANT0._00009.root");
      nfchain += chain->Add("/data/HZ/user.PaulineGagnon.EV-HZinv-HZinv-006387_tid10758-2.AANT0._00010.root");}
    
    // HZ130 sample - 50k filtered events
     if (n==0 && eventType[0] == "HZ130") {
       typeNumber = 0;
       nfchain += chain->Add("/data/ntuples/HZ130_ntuple.root");}
    
    // HZ140 sample - 50k filtered events
     if (n==0 && eventType[0] == "HZ140") {
       typeNumber = 0;
       nfchain += chain->Add("/data/ntuples/HZ140_ntuple.root");}
        
    // HZ150 sample - 50k filtered events
    if (n==0 && eventType[0] == "HZ150") {
      typeNumber = 0;
      nfchain += chain->Add("/data/ntuples/mh150_ntuple.root");}

    // HZ200 sample - 50k filtered events
    if (n==0 && eventType[0] == "HZ200") {
      typeNumber = 0;
      nfchain += chain->Add("/data/ntuples/mh200_ntuple.root");}

    // HZ250 sample - 50k filtered events
    if (n==0 && eventType[0] == "HZ250") {
      typeNumber = 0;
      nfchain += chain->Add("/data/ntuples/mh250_ntuple.root");}

//     //ZZllnunu sample - 50k filtered events
//     if (n==1) {
//       typeNumber = 1;
//       nfchain += chain->Add("/data/ZZ/user.PaulineGagnon.EV-HZinv-ZZllnunu_006388_tid10759-2.AANT0._00001.root");
//       nfchain += chain->Add("/data/ZZ/user.PaulineGagnon.EV-HZinv-ZZllnunu_006388_tid10759-2.AANT0._00002.root");
//       nfchain += chain->Add("/data/ZZ/user.PaulineGagnon.EV-HZinv-ZZllnunu_006388_tid10759-2.AANT0._00003.root");
//       nfchain += chain->Add("/data/ZZ/user.PaulineGagnon.EV-HZinv-ZZllnunu_006388_tid10759-2.AANT0._00004.root");
//       nfchain += chain->Add("/data/ZZ/user.PaulineGagnon.EV-HZinv-ZZllnunu_006388_tid10759-2.AANT0._00005.root");
//       nfchain += chain->Add("/data/ZZ/user.PaulineGagnon.EV-HZinv-ZZllnunu_006388_tid10759-2.AANT0._00006.root");
//       nfchain += chain->Add("/data/ZZ/user.PaulineGagnon.EV-HZinv-ZZllnunu_006388_tid10759-2.AANT0._00007.root");
//       nfchain += chain->Add("/data/ZZ/user.PaulineGagnon.EV-HZinv-ZZllnunu_006388_tid10759-2.AANT0._00008.root");
//       nfchain += chain->Add("/data/ZZ/user.PaulineGagnon.EV-HZinv-ZZllnunu_006388_tid10759-2.AANT0._00009.root");
//       nfchain += chain->Add("/data/ZZ/user.PaulineGagnon.EV-HZinv-ZZllnunu_006388_tid10759-2.AANT0._00010.root");}
    
    // ttbar sample - 120k filtered events
     if (n==2) {
      typeNumber = 2;
      //                      nfchain += chain->Add("tt_ntuple_METref.root");} 
      nfchain += chain->Add("/data/ttbar/user.PaulineGagnon.EV-HZinv-ttblnublnu_006811_tid010042-2.AANT0._00001.root.1");
      nfchain += chain->Add("/data/ttbar/user.PaulineGagnon.EV-HZinv-ttblnublnu_006811_tid010042-2.AANT0._00002.root.1");
      nfchain += chain->Add("/data/ttbar/user.PaulineGagnon.EV-HZinv-ttblnublnu_006811_tid010042-2.AANT0._00003.root");
      nfchain += chain->Add("/data/ttbar/user.PaulineGagnon.EV-HZinv-ttblnublnu_006811_tid010042-2.AANT0._00004.root");
      nfchain += chain->Add("/data/ttbar/user.PaulineGagnon.EV-HZinv-ttblnublnu_006811_tid010042-2.AANT0._00005.root");
      nfchain += chain->Add("/data/ttbar/user.PaulineGagnon.EV-HZinv-ttblnublnu_006811_tid010042-2.AANT0._00006.root");
      nfchain += chain->Add("/data/ttbar/user.PaulineGagnon.EV-HZinv-ttblnublnu_006811_tid010042-2.AANT0._00007.root");
      nfchain += chain->Add("/data/ttbar/user.PaulineGagnon.EV-HZinv-ttblnublnu_006811_tid010042-2.AANT0._00008.root");
      nfchain += chain->Add("/data/ntuples/tt6811_ntuple.root");}
    
   // WW sample - 35k, 200, 200, 35k
    if (n==3) {
      typeNumber = 3;
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_enuenu_006807_tid10040-2.AANT0._00001.root.1");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_enuenu_006807_tid10040-2.AANT0._00002.root");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_enuenu_006807_tid10040-2.AANT0._00003.root");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_enuenu_006807_tid10040-2.AANT0._00004.root.1");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_enuenu_006807_tid10040-2.AANT0._00005.root");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_enuenu_006807_tid10040-2.AANT0._00006.root");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_enuenu_006807_tid10040-2.AANT0._00007.root");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_enuenu_006807_tid10040-2.AANT0._00008.root");}

    if (n==4) {
      typeNumber = 3;
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_enumunu_006807_tid10764-2.AANT0._00001.root");}
    
    if (n==5) {
      typeNumber = 3;
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_munuenu_006809_tid010765-2.AANT0._00001.root");}
    
    if (n==6) {
      typeNumber = 3;
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_munumunu_006810_tid009813-2.AANT0._00001.root");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_munumunu_006810_tid009813-2.AANT0._00002.root.1");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_munumunu_006810_tid009813-2.AANT0._00003.root");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_munumunu_006810_tid009813-2.AANT0._00004.root");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_munumunu_006810_tid009813-2.AANT0._00005.root");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_munumunu_006810_tid009813-2.AANT0._00006.root");
      nfchain += chain->Add("/data/WW/user.PaulineGagnon.EV-HZinv-WpWm_munumunu_006810_tid009813-2.AANT0._00007.root");}
    
    // ZZtautaununu sample - 35k
    if (n==7) {
      typeNumber = 4;
      nfchain += chain->Add("/data/ntuples/ZZtautaununu_ntuple.root");}
    
    // ZZlltautau sample - 35k
         if (n==8) {
           typeNumber = 5;
           nfchain += chain->Add("/data/ntuples/ZZlltautau_ntuple.root");}
    
    // ZW+ sample - 50k
         if (n==9) {
           typeNumber = 6;
           nfchain += chain->Add("/data/ntuples/WpZ_ntuple.root");}
    
    // ZW- sample - 50k
         if (n==10) {
           typeNumber = 6;
           nfchain += chain->Add("/data/ntuples/WmZ_ntuple.root");}
    
     // Zincl sample -  run 6389 
         if (n==11) {
           typeNumber = 7;
	   nfchain += chain->Add("/data/ntuples/Zee-ljets_ntuple.root");}

     // Zincl sample -  run 6390 
         if (n==12) {
           typeNumber = 7;
	 nfchain += chain->Add("/data/ntuples/Zee-bjets_ntuple.root");
	 nfchain += chain->Add("/data/ntuples/Zee-bjets-2_ntuple.root");}

     // Zincl sample -  run 6595 
         if (n==13) {
           typeNumber = 7;
	   nfchain += chain->Add("/data/ntuples/Zmm-ljets_ntuple.root");}

     // Zincl sample -  run 6596
         if (n==14) {
           typeNumber = 7;
	   nfchain += chain->Add("/data/ntuples/Zmm-bjets_ntuple.root");
	   nfchain += chain->Add("/data/ntuples/Zmm-bjets-2_ntuple.root");}
    
    int EVEventAndInstance;  EVEventAndInstance = 0;
    int EVRunNumber;    EVRunNumber = 0;
    
    double MET_RefFinal_ex;  MET_RefFinal_ex = 0.0;
    double MET_RefFinal_ey;  MET_RefFinal_ey = 0.0;
    double MET_RefFinal_et;  MET_RefFinal_et = 0.0;
    
    //   double MissingEx;  MissingEx = 0.0;
    //   double MissingEy;  MissingEy = 0.0;
    //   double MissingEt;  MissingEt = 0.0;
    
    //   double MET_Truth_ex;  MET_Truth_ex = 0.0;
    //   double MET_Truth_ey;  MET_Truth_ey = 0.0;
    double MET_Truth_et;  MET_Truth_et = 0.0;
    
    int El_N;  El_N = 0;
    std::vector<double> * El_px;  El_px = new std::vector<double>();
    std::vector<double> * El_py;  El_py = new std::vector<double>();
    std::vector<double> * El_pz;  El_pz = new std::vector<double>();
    std::vector<double> * El_p_T; El_p_T = new std::vector<double>();
    std::vector<double> * El_eta; El_eta = new std::vector<double>();
    std::vector<int> * El_isEM;   El_isEM = new std::vector<int>();
    std::vector<int> * El_author; El_author = new std::vector<int>();
    std::vector<double> * El_pdgId; El_pdgId = new std::vector<double>();
    std::vector<double> * El_trEtCone10; El_trEtCone10 = new std::vector<double>();
    
    int TEl_N;  TEl_N = 0;
    std::vector<double> * TEl_p_T; TEl_p_T = new std::vector<double>();
    std::vector<double> * TEl_eta; TEl_eta = new std::vector<double>();
    
    int Mu_N;  Mu_N = 0;
    std::vector<double> * Mu_px;  Mu_px = new std::vector<double>();
    std::vector<double> * Mu_py;  Mu_py = new std::vector<double>();
    std::vector<double> * Mu_pz;  Mu_pz = new std::vector<double>();
    std::vector<double> * Mu_p_T; Mu_p_T = new std::vector<double>();
    std::vector<double> * Mu_eta; Mu_eta = new std::vector<double>();
    std::vector<int>    * Mu_isCombinedMuon; Mu_isCombinedMuon = new std::vector<int>();
    std::vector<int>    * Mu_bestMatch; Mu_bestMatch = new std::vector<int>();
    std::vector<double> * Mu_pdgId; Mu_pdgId = new std::vector<double>();
    std::vector<double> * Mu_trEtCone10; Mu_trEtCone10 = new std::vector<double>();
    
    int TMu_N;  TMu_N = 0;
    std::vector<double> * TMu_p_T; TMu_p_T = new std::vector<double>();
    std::vector<double> * TMu_eta; TMu_eta = new std::vector<double>();
    
    int Jet_N;  Jet_N = 0;
    std::vector<double> * Jet_eta;  Jet_eta = new std::vector<double>();
    std::vector<double> * Jet_E;    Jet_E = new std::vector<double>();
    std::vector<double> * Jet_px;   Jet_px = new std::vector<double>();
    std::vector<double> * Jet_py;   Jet_py= new std::vector<double>();
    std::vector<double> * Jet_pz;   Jet_pz= new std::vector<double>();
    std::vector<double> * Jet_p_T;  Jet_p_T= new std::vector<double>();
    
    std::vector<int> * Jet_BTagged;  Jet_BTagged= new std::vector<int>();
    
    // extract the trigger info
    bool PassedL1; PassedL1 = false;
    bool PassedL2; PassedL2 = false;
    bool PassedEF; PassedEF= false;
    bool PassedTrigger; PassedTrigger= false;
    
    bool PassedL1_EM25I; PassedL1_EM25I= false;
    bool PassedL1_2EM15I; PassedL1_2EM15I= false;
    
    bool PassedL1_MU06; PassedL1_MU06= false;
    bool PassedL1_MU08; PassedL1_MU08= false;
    bool PassedL1_MU10; PassedL1_MU10= false;
    bool PassedL1_MU11; PassedL1_MU11= false;
    bool PassedL1_MU20; PassedL1_MU20= false;
    bool PassedL1_MU40; PassedL1_MU40= false;
    bool PassedL1_2MU06; PassedL1_2MU06= false;
    
    bool PassedL1_XE100; PassedL1_XE100= false;
    bool PassedL1_XE50; PassedL1_XE50= false;
    
    bool PassedL2_e25i; PassedL2_e25i= false;
    bool PassedL2_mu6; PassedL2_mu6= false;
    bool PassedL2_mu6l; PassedL2_mu6l= false;
    bool PassedL2_mu20i; PassedL2_mu20i= false;
    
    bool PassedEF_e10; PassedEF_e10= false;
    bool PassedEF_e15iEF_e15i; PassedEF_e15iEF_e15i= false;
    bool PassedEF_e25i; PassedEF_e25i= false;
    bool PassedEF_mu6; PassedEF_mu6= false;
    bool PassedEF_mu6l; PassedEF_mu6l= false;
    bool PassedEF_mu20i; PassedEF_mu20i= false;
    
    double TrigEFMissingET_et; TrigEFMissingET_et = 0.0;
    
    int Z_N;  Z_N = 0;
    std::vector<int> * Z_Zee;  Z_Zee = new std::vector<int>();
    std::vector<int> * Z_Zmm;  Z_Zmm = new std::vector<int>();
    std::vector<double> * Z_m;    Z_m = new std::vector<double>();
    std::vector<double> * Z_E;    Z_E = new std::vector<double>();
    std::vector<double> * Z_p_T;  Z_p_T = new std::vector<double>();
    std::vector<double> * Z_px;   Z_px = new std::vector<double>();
    std::vector<double> * Z_py;   Z_py = new std::vector<double>();
    std::vector<double> * Z_eta;  Z_eta = new std::vector<double>();
    
    chain->SetBranchAddress("EVEventAndInstance",  &EVEventAndInstance);
    chain->SetBranchAddress("EVRunNumber", &EVRunNumber);
    
    //   chain->SetBranchAddress("MissingEx", &MissingEx);
    //   chain->SetBranchAddress("MissingEy", &MissingEy);
    //   chain->SetBranchAddress("MissingEt", &MissingEt);
    
    chain->SetBranchAddress("MET_RefFinal_ex", &MET_RefFinal_ex);
    chain->SetBranchAddress("MET_RefFinal_ey", &MET_RefFinal_ey);
    chain->SetBranchAddress("MET_RefFinal_et", &MET_RefFinal_et);
    
    chain->SetBranchAddress("MET_RefFinal_et", &MET_RefFinal_et);
    
    //   chain->SetBranchAddress("MET_Truth_ex", &MET_Truth_ex);
    //   chain->SetBranchAddress("MET_Truth_ey", &MET_Truth_ey);
    chain->SetBranchAddress("MET_Truth_et", &MET_Truth_et);
    
    chain->SetBranchAddress("El_N",&El_N);
    chain->SetBranchAddress("El_px",&El_px);
    chain->SetBranchAddress("El_py",&El_py);
    chain->SetBranchAddress("El_pz",&El_pz);
    chain->SetBranchAddress("El_p_T",&El_p_T);
    chain->SetBranchAddress("El_eta",&El_eta);
    chain->SetBranchAddress("El_pdgId",&El_pdgId);
    chain->SetBranchAddress("El_isEM",&El_isEM);
    chain->SetBranchAddress("El_author",&El_author);
    chain->SetBranchAddress("El_trEtCone10",&El_trEtCone10);
    
    chain->SetBranchAddress("TEl_N",&TEl_N);
    chain->SetBranchAddress("TEl_p_T",&TEl_p_T);
    chain->SetBranchAddress("TEl_eta",&TEl_eta);
    
    chain->SetBranchAddress("Mu_N",&Mu_N);
    chain->SetBranchAddress("Mu_px",&Mu_px);
    chain->SetBranchAddress("Mu_py",&Mu_py);
    chain->SetBranchAddress("Mu_pz",&Mu_pz);
    chain->SetBranchAddress("Mu_p_T",&Mu_p_T);
    chain->SetBranchAddress("Mu_eta",&Mu_eta);
    chain->SetBranchAddress("Mu_pdgId",&Mu_pdgId);
    chain->SetBranchAddress("Mu_isCombinedMuon",&Mu_isCombinedMuon);
    chain->SetBranchAddress("Mu_bestMatch",&Mu_bestMatch);
    chain->SetBranchAddress("Mu_trEtCone10",&Mu_trEtCone10);
    
    chain->SetBranchAddress("TMu_N",&TMu_N);
    chain->SetBranchAddress("TMu_p_T",&TMu_p_T);
    chain->SetBranchAddress("TMu_eta",&TMu_eta);
    
    chain->SetBranchAddress("Jet_N",&Jet_N);
    chain->SetBranchAddress("Jet_eta",&Jet_eta);
    chain->SetBranchAddress("Jet_E",&Jet_E);
    chain->SetBranchAddress("Jet_px",&Jet_px);
    chain->SetBranchAddress("Jet_py",&Jet_py);
    chain->SetBranchAddress("Jet_pz",&Jet_pz);
    chain->SetBranchAddress("Jet_p_T",&Jet_p_T);
    chain->SetBranchAddress("Jet_BTagged",&Jet_BTagged);
    
    chain->SetBranchAddress("PassedL1",&PassedL1);
    chain->SetBranchAddress("PassedL2",&PassedL2);
    chain->SetBranchAddress("PassedEF",&PassedEF);
    chain->SetBranchAddress("PassedTrigger",&PassedTrigger);
    
    chain->SetBranchAddress("PassedL1_EM25I",&PassedL1_EM25I);
    chain->SetBranchAddress("PassedL1_2EM15I",&PassedL1_2EM15I);
    
    chain->SetBranchAddress("PassedL1_MU06",&PassedL1_MU06);
    chain->SetBranchAddress("PassedL1_MU08",&PassedL1_MU08);
    chain->SetBranchAddress("PassedL1_MU10",&PassedL1_MU10);
    chain->SetBranchAddress("PassedL1_MU11",&PassedL1_MU11);
    chain->SetBranchAddress("PassedL1_MU20",&PassedL1_MU20);
    chain->SetBranchAddress("PassedL1_MU40",&PassedL1_MU40);
    chain->SetBranchAddress("PassedL1_2MU06",&PassedL1_2MU06);
    
    chain->SetBranchAddress("PassedL1_XE100",&PassedL1_XE100);
    chain->SetBranchAddress("PassedL1_XE50",&PassedL1_XE50);
    
    chain->SetBranchAddress("PassedL2_e25i",&PassedL2_e25i);
    chain->SetBranchAddress("PassedL2_mu6",&PassedL2_mu6);
    chain->SetBranchAddress("PassedL2_mu6l",&PassedL2_mu6l);
    chain->SetBranchAddress("PassedL2_mu20i",&PassedL2_mu20i);
    
    chain->SetBranchAddress("PassedEF_e10",&PassedEF_e10);
    chain->SetBranchAddress("PassedEF_e15iEF_e15i",&PassedEF_e15iEF_e15i);
    chain->SetBranchAddress("PassedEF_e25i",&PassedEF_e25i);
    chain->SetBranchAddress("PassedEF_mu6",&PassedEF_mu6);
    chain->SetBranchAddress("PassedEF_mu6l",&PassedEF_mu6l);
    chain->SetBranchAddress("PassedEF_mu20i",&PassedEF_mu20i);
    
    chain->SetBranchAddress("TrigEFMissingET_et",&TrigEFMissingET_et);
    
    chain->SetBranchAddress("Z_N",&Z_N);
    chain->SetBranchAddress("Z_Zee",&Z_Zee);
    chain->SetBranchAddress("Z_Zmm",&Z_Zmm);
    
    int nrecords = static_cast<int>(0.5 + chain->GetEntries());
    
    int nticker  = static_cast<int>(0.5 + nrecords/20.0);
    
    nrec[n] = nrecords;
    
    // The above is to find the number of events or entries
#endif // CHAIN 
    
    if (n == 0) {
      outClientFile << "      " <<  endl;
      outClientFile << "MET cut > " << METcut << endl;
      outClientFile << "pt cuts for leptons: e > " << eptmin << " and mu > " << mptmin <<" GeV with eta < " << etamin << endl;
      outClientFile << "nlep strictly equal to " << nlepcut << endl; //" and njet less or equal to " << njetcut << endl;	 	   
      outClientFile << "mZ within " << mZcut << " GeV of on-shell mass " << endl;
      outClientFile << endl;	  
    }
    
    double mZ = 0.;
    
    // evaluate the weight for this type of events
    generated[n] = nrec[n]/filterEff[n]; 
    cout << n << " generated: " << generated[n] << " nrec: " << nrec[n] << endl;
    weight[n] = (sigmaBR[n] * lumi)/generated[n];

    // ============  Starting the main loop over the events  ===============
    for (int i=0; i<nrec[n]; i++) {  
      
      if(div(i, nticker).rem == 0 || i >= nrec[n]-1) 
	{ 
	  portion=static_cast<float>(i) / static_cast<float>(nrec[n]-1);
	  printf("Done %4.1f percent of reading the NTuple  ", portion*100.0);
	  cout.flush();   cout << char(13);  
	}
      
      last_rec=(i>=nrec[n]-1); 
      
      if(!last_rec)  {
#ifdef CHAIN
	nb += chain->GetEvent(i+1); // Getting one event at a time
#else   /* Not CHAIN */ 
	nb += readtree->GetEvent(i+1);
#endif   /* CHAIN */ // The next record has been read for comparison
      }
      
      // Reading the i-th (current) record:
      
#ifdef CHAIN
      nb += chain->GetEvent(i); 
#else   /* Not CHAIN */ 
      nb += readtree->GetEvent(i);
#endif   /* CHAIN */
      
      int accepted_e = 0;
      double Zmasse[25];
      int elep1[25];
      int elep2[25];
      
      int accepted_m = 0;
      double Zmassm[25];
      int mlep1[25];
      int mlep2[25];
      
      int bestj = 0;
      int bestk = 0;
      int bestid = 0;
      
      //loop over all electrons and count the good ones

      nel = 0;
      for(int j=0; j<El_N; j++) {
	if (fabs((*El_eta)[j]) <= etamin && (*El_p_T)[j] > eptmin && ((*El_isEM)[j]&0x7)==0) {
	  nel++;
	}}
      
      //loop over all pairs of muons and only select the good ones
      nmu = 0;
      for(int j=0; j<Mu_N; j++) {
	if (fabs((*Mu_eta)[j]) <= etamin && (*Mu_p_T)[j] > mptmin && (*Mu_isCombinedMuon)[j] == 1 && (*Mu_bestMatch)[j] == 1) {
	  nmu++;
	}}
      
      // store the total number of good leptons
      nlep = nel + nmu;
      
      MET_RefFinal_ex /=1000.;
      MET_RefFinal_ey /=1000.; 
      MET_RefFinal_et /=1000.;
      
      MET_Truth_et /=1000.;

// unsmeared electrons
      for(int j=0; j<El_N; j++) {
	(*El_px)[j] /= 1000.;
	(*El_py)[j] /= 1000.;
	(*El_pz)[j] /= 1000.;
	(*El_p_T)[j] /= 1000.;
	(*El_trEtCone10)[j] /= 1000.;
      }

// unsmeared muons
      for(int j=0; j<Mu_N; j++) {
	(*Mu_px)[j] /= 1000.;
	(*Mu_py)[j] /= 1000.;
	(*Mu_pz)[j] /= 1000.;
	(*Mu_p_T)[j] /= 1000.;
	(*Mu_trEtCone10)[j] /= 1000.;
      }

// unsmeared jets
      for(int j=0; j<Jet_N; j++) {
	(*Jet_px)[j] /= 1000.;
	(*Jet_py)[j] /= 1000.;
	(*Jet_pz)[j] /= 1000.;
	(*Jet_p_T)[j] /= 1000.;
	(*Jet_E)[j] /= 1000.;
      }

   for(int j=0; j<El_N; j++) {
     (*TEl_p_T)[j] /= 1000.;
   }
   for(int j=0; j<Mu_N; j++) {
     (*TMu_p_T)[j] /= 1000.;
   }
      
   if (!PassedEF_e15iEF_e15i && !PassedEF_e25i && !PassedEF_mu6 && !PassedEF_mu6l && !PassedEF_mu20i && !PassedL1_XE100) {
     reject_trigger[n]++;}
   if (PassedEF_e15iEF_e15i || PassedEF_e25i || PassedEF_mu6 || PassedEF_mu6l || PassedEF_mu20i || PassedL1_XE100) {
     
     if (!PassedEF_e15iEF_e15i && !PassedEF_e25i && !PassedEF_mu6 && !PassedEF_mu6l && !PassedEF_mu20i) L1XET100[n]++;
     
	// count Zee and Zmm before and after the MET cut
	for(int j=0; j<Z_N; j++) {
	  if ((*Z_Zee)[j] == 1) zee0[n]++;
	  if ((*Z_Zmm)[j] == 1) zmm0[n]++;
	}
	hMET[typeNumber]->Fill(MET_RefFinal_et,weight[n]);
	if (MET_RefFinal_et < METcut) reject_MET[n]++;
	if (MET_RefFinal_et >= METcut) {
	  
	  for(int j=0; j<Z_N; j++) {
	    if ((*Z_Zee)[j] == 1) zee1[n]++;
	    if ((*Z_Zmm)[j] == 1) zmm1[n]++;
	  }
	  
	  int twoelectrons=0;
	  //loop over all pairs of electrons 
	  for(int j=0; j<El_N-1; j++) {
	    if (fabs((*El_eta)[j]) <= etamin && (*El_p_T)[j] > eptmin && ((*El_isEM)[j]&0x7)==0) {
	      for(int k=j+1; k<El_N; k++) {
		if (fabs((*El_eta)[k]) <= etamin && (*El_p_T)[k] > eptmin && ((*El_isEM)[k]&0x7)==0) {
		  
		  //lepid cut
		  //	       if((*El_pdgId)[j] != -(*El_pdgId)[k]) {cout << "electron id: " << (*El_pdgId)[j] << "\t" << (*El_pdgId)[k] << endl;}
		  if((*El_pdgId)[j] == -(*El_pdgId)[k]) {
		    twoelectrons=1;
		    
		    px1 = (*El_px)[j];
		    py1 = (*El_py)[j];
		    pz1 = (*El_pz)[j];
		    pt1 = (*El_p_T)[j];
		    eta1 = (*El_eta)[j];
		    px2 = (*El_px)[k];
		    py2 = (*El_py)[k];
		    pz2 = (*El_pz)[k];
		    pt2 = (*El_p_T)[k];
		    eta2 = (*El_eta)[k];
		    
		    // calculate the Z invariant mass 
		    double px = px1+px2;
		    double py = py1+py2;
		    double pz = pz1+pz2;
		    double pZ = px*px +py*py +pz*pz;
		    double ml = mass_e;
		    double e1 = ml*ml + px1*px1 +py1*py1 +pz1*pz1;
		    double e2 = ml*ml + px2*px2 +py2*py2 +pz2*pz2;
		    double eZ = sqrt(e1) + sqrt(e2);
		    mZ = eZ*eZ - pZ;
		    if (mZ > 0.0) {mZ = sqrt(mZ);}
		    else {mZ = -1.0;}
// 		    if (typeNumber == 6 && counter < 10) cout << "mZ = " << mZ << endl;
// 		    if (typeNumber == 6) counter++;
		    
		    //count the number of accepted pairs
		    //		 cout << "reconstructed mZ = " << mZ << endl;
		    if (abs(mZ - mZ0) <= mZcut) {
		      
		      if (accepted_e > 24) {cout << " too many accepted_e pairs, stop!!! accepted_e = " << accepted_e << endl;exit(-1);}
		      Zmasse[accepted_e] = mZ;
		      elep1[accepted_e]  = j;
		      elep2[accepted_e]  = k;
		      accepted_e++;}
		    
		  } //  end-if on lepid
		} // end if acceptance criteria for second electron
	      } // end of for-loop on second electron
	    } // end if acceptance criteria for first electron
	  } // end of for-loop on first electron
	  
	  
	  //loop over all pairs of muons 
	  int twomuons = 0;
	  for(int j=0; j<Mu_N-1; j++) {
	    if (fabs((*Mu_eta)[j]) <= etamin && (*Mu_p_T)[j] > mptmin && (*Mu_isCombinedMuon)[j] == 1 && (*Mu_bestMatch)[j] == 1) {
	      for(int k=j+1; k<Mu_N; k++) {
		if (fabs((*Mu_eta)[k]) <= etamin && (*Mu_p_T)[k] > mptmin && (*Mu_isCombinedMuon)[k] == 1 && (*Mu_bestMatch)[k] == 1) {
		  
		  //lepid cut
		  if((*Mu_pdgId)[j] == -(*Mu_pdgId)[k]) {
		    twomuons = 1;
		    px1 =  (*Mu_px)[j];
		    py1 =  (*Mu_py)[j];
		    pz1 =  (*Mu_pz)[j];
		    pt1 =  (*Mu_p_T)[j];
		    eta1 = (*Mu_eta)[j];
		    px2 =  (*Mu_px)[k];
		    py2 =  (*Mu_py)[k];
		    pz2 =  (*Mu_pz)[k];
		    pt2 =  (*Mu_p_T)[k];
		    eta2 = (*Mu_eta)[k];
		    
		    // calculate the Z invariant mass 
		    double px = px1+px2;
		    double py = py1+py2;
		    double pz = pz1+pz2;
		    double pZ = px*px +py*py +pz*pz;
		    double ml = mass_mu;
		    double e1 = ml*ml + px1*px1 +py1*py1 +pz1*pz1;
		    double e2 = ml*ml + px2*px2 +py2*py2 +pz2*pz2;
		    double eZ = sqrt(e1) + sqrt(e2);
		    mZ = eZ*eZ - pZ;
		    if (mZ > 0.0) {mZ = sqrt(mZ);}
		    else {mZ = -1.0;}

// add a Breit-Wigner distribution to ZW events
		    if (typeNumber == 6) {
		      double mZmean = mZ;
		      double mZwidth = 2.4952;
		      mZ = random->BreitWigner(mZmean, mZwidth);
		      //		      if (counter < 10) cout << "mZ = " << mZ << endl;
		      counter++;
		    }
		    
		    //count the number of accepted_m pairs
		    //		 cout << "reconstructed mZ = " << mZ << endl;
		    if (abs(mZ - mZ0) <= mZcut) {
		      
		      if (accepted_m > 24) {cout << " too many accepted_m pairs, stop!!! accepted_m = " << accepted_m << endl;exit(-1);}
		      Zmassm[accepted_m] = mZ;
		      mlep1[accepted_m]  = j;
		      mlep2[accepted_m]  = k;
		      accepted_m++;}
		    
		  } //  end-if on lepid
		} // end if acceptance criteria for second muon
	      } // end of for-loop on second muon
	    } // end if acceptance criteria for first muon
	  } // end of for-loop on first muon
	  
	  //	   cout << "found " << electrons << " good electrons and  " << nmu << " good muons  "<< endl;
	  
	  // sanity check: make sure we count only one per event
	  sanity[n]++;
	  
	  // nlepcut
	  if ((twoelectrons == 0 && twomuons == 0) || nlep != 2) {
	    reject_nlep[n]++;}
	  
	  if ((twoelectrons == 1 || twomuons == 1) && nlep == 2) {
	    if (twoelectrons == 1 && twomuons == 1) doublepair[n]++;
	    
	    //         afterNlep[n]++;
	    
	    // continue only when a good pair has been found
	    if (accepted_m == 0 && accepted_e == 0 ) {reject_mZ[n]++;;}
	    if (accepted_m+accepted_e > 0) {
	      if (nlep == 1) cout << "nlep == 1 and nmu = " << nmu << " nmel :" << nel << " accepted_mu = " << accepted_m << endl;  
	      
	      
	      //	   afterMz[n]++;
	      //retain only the best pair

	      mZ=0.;
	      for (int m=0; m<accepted_e; m++) {
		mZ = (( abs(mZ0 - mZ) < abs(mZ0 - Zmasse[m])) ? mZ : Zmasse[m]);
		if (mZ == Zmasse[m]) {bestj = elep1[m]; bestk = elep2[m]; bestid=11;}
	      }
	      for (int m=0; m<accepted_m; m++) {
		mZ = (( abs(mZ0 - mZ) < abs(mZ0 - Zmassm[m])) ? mZ : Zmassm[m]);
		if (mZ == Zmassm[m]) {bestj = mlep1[m]; bestk = mlep2[m]; bestid=13;}
	      }
	      
	      // apply an anti-btag on events containing a jet

	      int btagOK=1;
	      for(int l=0; l<Jet_N; l++) {
		//	     cout << (*Jet_BTagged)[l] << endl;
		if ((*Jet_BTagged)[l] == 1) {
		  //   	       cout << "(*Jet_BTagged)[l]: " << (*Jet_BTagged)[l] << endl;
		  btagOK=0;}
	      }
	      
	      if (btagOK == 0) {reject_btag[n]++;}
	      if (btagOK == 1) {
		
		// count how many ee and mm pairs are found	 
		if (bestid == 11) zee[n]++;
		if (bestid == 13) zmm[n]++;
		
		// done with selection
		found[n]++;
		
// do the smearing here for systematics error

// 		double new_MET_ex = MET_RefFinal_ex;
// 		double new_MET_ey = MET_RefFinal_ey;
// 		double new_MET_et = MET_RefFinal_et;
// 		double old_MET_et = MET_RefFinal_et;
      

// // add 0.73% gaussian smearing to ptlep for electrons and readjust MET
// // gaussian distribution
// 		double mean = 1.0;
// 		double sigma = 0.0073;
// 		double gaussian = random->Gaus(mean, sigma);
// // 		for(int j=0; j<El_N; j++) {
// 		  gaussian = random->Gaus(mean, sigma);
// 		  double new_px = (*El_px)[j] * gaussian;
// //       	cout << "pxlep before: " << (*El_px)[j] << " pxlep after: " << new_px << " % delta px: " << 100.0*((*El_px)[j] - new_px)/(*El_px)[j] << endl;
// 		  new_MET_ex = new_MET_ex + (*El_px)[j] - new_px;
// 		  (*El_px)[j] = new_px;

// 		  gaussian = random->Gaus(mean, sigma);
// 		  double new_py = (*El_py)[j] * gaussian;
// //       	cout << "pylep before: " << (*El_py)[j] << " pylep after: " << new_py << " % delta py: " << 100.0*((*El_py)[j] - new_py)/(*El_py)[j] << endl;
// 		  new_MET_ey = new_MET_ey + (*El_py)[j] - new_py;
// 		  (*El_py)[j] = new_py;

// 		  gaussian = random->Gaus(mean, sigma);
// 		  (*El_pz)[j] *= gaussian;

// 		  (*El_p_T)[j] = sqrt((new_px*new_px)+(new_py*new_py));
// 		}

// 		if (El_N > 0) {
// 		  new_MET_et = sqrt(new_MET_ex*new_MET_ex + new_MET_ey*new_MET_ey);
// 		  MET_RefFinal_ex = new_MET_ex;
// 		  MET_RefFinal_ey = new_MET_ey;
// 		  MET_RefFinal_et = new_MET_et;
// 		  hgauss1->Fill(old_MET_et);
// 		  hgauss2->Fill(new_MET_et);
// 		}

// add 0.5% scaling factor to the electron energy scale

// 		for(int j=0; j<El_N; j++) {
// 		  double new_px = (*El_px)[j]*1.005;
// //       	cout << "pxlep before: " << (*El_px)[j] << " pxlep after: " << new_px << " % delta px: " << 100.0*((*El_px)[j] - new_px)/(*El_px)[j] << endl;
// 		  new_MET_ex = new_MET_ex + (*El_px)[j] - new_px;
// 		  (*El_px)[j] = new_px;

// 		  double new_py = (*El_py)[j]*1.005;
// //       	cout << "pylep before: " << (*El_py)[j] << " pylep after: " << new_py << " % delta py: " << 100.0*((*El_py)[j] - new_py)/(*El_py)[j] << endl;
// 		  new_MET_ey = new_MET_ey + (*El_py)[j] - new_py;
// 		  (*El_py)[j] = new_py;

// 		  (*El_pz)[j] *= 1.005;

// 		  double new_pt = sqrt((new_px*new_px)+(new_py*new_py));
// 		  double delta = 100.0*((*El_p_T)[j] - new_pt)/(*El_p_T)[j];
// 		  (*El_p_T)[j] = new_pt;
// 		}

// 		if (El_N > 0) {
// 		  new_MET_et = sqrt(new_MET_ex*new_MET_ex + new_MET_ey*new_MET_ey);
// 		  MET_RefFinal_ex = new_MET_ex;
// 		  MET_RefFinal_ey = new_MET_ey;
// 		  MET_RefFinal_et = new_MET_et;
// 		  hgauss1->Fill(old_MET_et);
// 		  hgauss2->Fill(new_MET_et);
// 		}


// add smearing to ptlep for muons according to special formula
// sigma = 0.011*pT + 0.00017*pT^2 added in quadrature
  
//      mean = 1.0;
//       for(int j=0; j<Mu_N; j++) {
// 	sigma = sqrt(0.011*(*Mu_p_T)[j]*0.011*(*Mu_p_T)[j] + 0.00017*0.00017*(*Mu_p_T)[j]*(*Mu_p_T)[j]*(*Mu_p_T)[j]*(*Mu_p_T)[j]);

//         if ((*Mu_p_T)[j] > 0.0) sigma /= (*Mu_p_T)[j];
//         if ((*Mu_p_T)[j] <= 0.0) sigma = 0.0;
// 	gaussian = random->Gaus(mean, sigma);
// 	//	cout << sigma << "\t" << (*Mu_p_T)[j] << "\t" << gaussian << endl;
// 	new_MET_ex = new_MET_ex + (*Mu_px)[j];
// 	(*Mu_px)[j] *= gaussian;
// 	new_MET_ex = new_MET_ex - (*Mu_px)[j];

// 	gaussian = random->Gaus(mean, sigma);
// 	new_MET_ey = new_MET_ey + (*Mu_py)[j];
// 	(*Mu_py)[j] *= gaussian;
// 	new_MET_ey = new_MET_ey - (*Mu_py)[j];

// 	gaussian = random->Gaus(mean, sigma);
// 	(*Mu_pz)[j] *= gaussian;

// 	(*Mu_p_T)[j] = sqrt((*Mu_px)[j]*(*Mu_px)[j]+(*Mu_py)[j]*(*Mu_py)[j]);
//       }

//       if (Mu_N > 0) {
// 	new_MET_et = sqrt(new_MET_ex*new_MET_ex + new_MET_ey*new_MET_ey);
// 	MET_RefFinal_ex = new_MET_ex;
// 	MET_RefFinal_ey = new_MET_ey;
// 	MET_RefFinal_et = new_MET_et;
// 	hgauss1->Fill(old_MET_et);
// 	hgauss2->Fill(new_MET_et);
//       }

// scaling factor of 1% on the muon energy scale
// 		for(int j=0; j<Mu_N; j++) {
// 		  double new_px = (*Mu_px)[j]*0.99;
// //       	cout << "pxlep before: " << (*Mu_px)[j] << " pxlep after: " << new_px << " % delta px: " << 100.0*((*Mu_px)[j] - new_px)/(*Mu_px)[j] << endl;
// 		  new_MET_ex = new_MET_ex + (*Mu_px)[j] - new_px;
// 		  (*Mu_px)[j] = new_px;

// 		  double new_py = (*Mu_py)[j]*0.99;
// //       	cout << "pylep before: " << (*Mu_py)[j] << " pylep after: " << new_py << " % delta py: " << 100.0*((*Mu_py)[j] - new_py)/(*Mu_py)[j] << endl;
// 		  new_MET_ey = new_MET_ey + (*Mu_py)[j] - new_py;
// 		  (*Mu_py)[j] = new_py;

// 		  (*Mu_pz)[j] *= 0.99;

// 		  double new_pt = sqrt((new_px*new_px)+(new_py*new_py));
// 		  double delta = 100.0*((*Mu_p_T)[j] - new_pt)/(*Mu_p_T)[j];
// 		  (*Mu_p_T)[j] = new_pt;
// 		}

// 		if (Mu_N > 0) {
// 		  new_MET_et = sqrt(new_MET_ex*new_MET_ex + new_MET_ey*new_MET_ey);
// 		  MET_RefFinal_ex = new_MET_ex;
// 		  MET_RefFinal_ey = new_MET_ey;
// 		  MET_RefFinal_et = new_MET_et;
// 		  hgauss1->Fill(old_MET_et);
// 		  hgauss2->Fill(new_MET_et);
// 		}

// add 7% or 15% scaling factor to the jet energy scale - Higgs group approach

// 		for(int j=0; j<Jet_N; j++) {
// 		  double new_px = 0.0;
// 		  double new_py = 0.0;
// 		  double new_pz = 0.0;
// // 		  new_MET_ex = new_MET_ex + 0.05*(*Jet_px)[j];
// // 		  if (fabs((*Jet_eta)[j]) <= 3.2) new_px = (*Jet_px)[j]*0.93;
// // 		  if (fabs((*Jet_eta)[j]) <  3.2) new_px = (*Jet_px)[j]*0.85;
// // 		  (*Jet_px)[j] = new_px;

// // 		  new_MET_ey = new_MET_ey + 0.05*(*Jet_py)[j];
// // 		  if (fabs((*Jet_eta)[j]) <= 3.2) new_py = (*Jet_py)[j]*0.93;
// // 		  if (fabs((*Jet_eta)[j]) <= 3.2) new_py = (*Jet_py)[j]*0.85;
// // 		  (*Jet_py)[j] = new_py;

// // 		  if (fabs((*Jet_eta)[j]) <= 3.2) new_pz = (*Jet_pz)[j]*0.93;
// // 		  if (fabs((*Jet_eta)[j]) <= 3.2) new_pz = (*Jet_pz)[j]*0.85;
// // 		  (*Jet_pz)[j] = new_pz;

// 		  new_MET_ex = new_MET_ex - 0.05*(*Jet_px)[j];
// 		  if (fabs((*Jet_eta)[j]) <= 3.2) new_px = (*Jet_px)[j]*1.07;
// 		  if (fabs((*Jet_eta)[j]) <  3.2) new_px = (*Jet_px)[j]*1.15;
// 		  (*Jet_px)[j] = new_px;

// 		  new_MET_ey = new_MET_ey - 0.05*(*Jet_py)[j];
// 		  if (fabs((*Jet_eta)[j]) <= 3.2) new_py = (*Jet_py)[j]*1.07;
// 		  if (fabs((*Jet_eta)[j]) <= 3.2) new_py = (*Jet_py)[j]*1.15;
// 		  (*Jet_py)[j] = new_py;

// 		  if (fabs((*Jet_eta)[j]) <= 3.2) new_pz = (*Jet_pz)[j]*1.07;
// 		  if (fabs((*Jet_eta)[j]) <= 3.2) new_pz = (*Jet_pz)[j]*1.15;
// 		  (*Jet_pz)[j] = new_pz;

// 		  double new_pt = sqrt((new_px*new_px)+(new_py*new_py));
// 		  double delta = 100.0*((*Jet_p_T)[j] - new_pt)/(*Jet_p_T)[j];
// 		  (*Jet_p_T)[j] = new_pt;
//  		}

// 		if (Jet_N > 0) {
// 		  new_MET_et = sqrt(new_MET_ex*new_MET_ex + new_MET_ey*new_MET_ey);
// 		  //		  cout << new_MET_et << "\t " << new_MET_ex << "\t " << new_MET_ey << endl;
// 		  MET_RefFinal_ex = new_MET_ex;
// 		  MET_RefFinal_ey = new_MET_ey;
// 		  MET_RefFinal_et = new_MET_et;
// 		  hgauss1->Fill(old_MET_et);
// 		  hgauss2->Fill(new_MET_et);
// 		}

// jet energy resolution: add gaussian smearing to jets and readjust MET
// gaussian distribution 
// May 5, 2008: use the same correction factor in x, y, and z , not two independant ones
// 		double mean = 1.0;
// 		double sigma = 0.0;
// 		double gaussian = 1.0;

// 		for(int j=0; j<Jet_N; j++) {
// 		  if (fabs((*Jet_eta)[j]) <= 3.2) sigma = (0.45 * sqrt((*Jet_E)[j]))/(*Jet_E)[j];
// 		  if (fabs((*Jet_eta)[j]) > 3.2)  sigma = (0.63 * sqrt((*Jet_E)[j]))/(*Jet_E)[j];
// 		  gaussian = random->Gaus(mean, sigma);

// 		  //		  cout << (*Jet_E)[j] << "\t" << sigma << "\t" << gaussian << endl;

// 		  double new_px = (*Jet_px)[j] * gaussian;
// //        	cout << "Jet px before: " << (*Jet_px)[j] << " jet px after: " << new_px << " % delta px: " << 100.0*((*Jet_px)[j] - new_px)/(*Jet_px)[j] << endl;
// 		  new_MET_ex = new_MET_ex + (*Jet_px)[j] - new_px;
// 		  (*Jet_px)[j] = new_px;

// 		  //		  gaussian = random->Gaus(mean, sigma);
// 		  double new_py = (*Jet_py)[j] * gaussian;
// //        	cout << "Jet py before: " << (*Jet_py)[j] << " jet py after: " << new_py << " % delta py: " << 100.0*((*Jet_py)[j] - new_py)/(*Jet_py)[j] << endl;
// 		  new_MET_ey = new_MET_ey + (*Jet_py)[j] - new_py;
// 		  (*Jet_py)[j] = new_py;

// 		  //		  gaussian = random->Gaus(mean, sigma);
// 		  (*Jet_pz)[j] *= gaussian;

// 		  (*Jet_p_T)[j] = sqrt((new_px*new_px)+(new_py*new_py));
// 		}

// 		if (Jet_N > 0) {
// 		  new_MET_et = sqrt(new_MET_ex*new_MET_ex + new_MET_ey*new_MET_ey);
// 		  MET_RefFinal_ex = new_MET_ex;
// 		  MET_RefFinal_ey = new_MET_ey;
// 		  MET_RefFinal_et = new_MET_et;
// 		  hgauss1->Fill(old_MET_et);
// 		  hgauss2->Fill(new_MET_et);
// 		}

		px1  = -999.;
		py1  = -999.;
		pz1  = -999.;
		pt1  = -999.;
		eta1 = -999.;
		Cone1= -999.;
		px2  = -999.;
		py2  = -999.;
		pz2  = -999.;
		pt2  = -999.;
		eta2 = -999.;
		Cone2= -999.;
		
		// reload info on the best pair of leptons
		if (abs(bestid) == 11) {
		  px1  = (*El_px)[bestj];
		  py1  = (*El_py)[bestj];
		  pz1  = (*El_pz)[bestj];
		  pt1  = (*El_p_T)[bestj];
		  eta1 = (*El_eta)[bestj];
		  Cone1= float((*El_trEtCone10)[bestj]);
		  px2  = (*El_px)[bestk];
		  py2  = (*El_py)[bestk];
		  pz2  = (*El_pz)[bestk];
		  pt2  = (*El_p_T)[bestk];
		  eta2 = (*El_eta)[bestk];
		  Cone2= float((*El_trEtCone10)[bestk]);
		}
		if (abs(bestid) == 13) {
		  px1  = (*Mu_px)[bestj];
		  py1  = (*Mu_py)[bestj];
		  pz1  = (*Mu_pz)[bestj];
		  pt1  = (*Mu_p_T)[bestj];
		  eta1 = (*Mu_eta)[bestj];
		  Cone1= float((*Mu_trEtCone10)[bestj]);
		  px2  = (*Mu_px)[bestk];
		  py2  = (*Mu_py)[bestk];
		  pz2  = (*Mu_pz)[bestk];
		  pt2  = (*Mu_p_T)[bestk];
		  eta2 = (*Mu_eta)[bestk];
		  Cone2= float((*Mu_trEtCone10)[bestk]);
		}
		// calculate a few more variables for likelihood function for the best lepton pair
		double px = px1+px2;
		double py = py1+py2;
		modulus = sqrt(px1*px1 + py1*py1 + pz1*pz1) * sqrt(px2*px2 + py2*py2 + pz2*pz2);
		cosll3D = (px1*px2 +py1*py2 + pz1*pz2)/ modulus;
		if (abs(cosll3D) > 1.0 ) {cout << "cosll3d > 1: " << "\t " << cosll3D << "\t " << px1 << "\t " 
					       << pz1 << "\t " << py1 << "\t " << px2 << "\t " << py2 << "\t " << pz2 << "\t " << modulus << endl;}
		modulus = sqrt(px1*px1 + py1*py1) * sqrt(px2*px2 + py2*py2);
		cosll2D = (px1*px2 + py1*py2)/ modulus;
		if (abs(cosll2D) > 1.0 ) {cout << "cosll2d > 1: " << "\t " << cosll2D << "\t " << px1 << "\t " 
					       << py1 << "\t " << px2 << "\t " << py2 << "\t " << modulus << endl;}
		pt2l    = sqrt(px*px + py*py);
		
		// calculate the angle between MET_RefFinal_Et and PtZ 
		cosPhi  = (px*MET_RefFinal_ex + py*MET_RefFinal_ey)/(pt2l*MET_RefFinal_et);
		
		// calculate the transverse mass
		mt      = sqrt(2.0*pt2l*MET_RefFinal_et*(1.0-cosPhi));
		
		// calculate cosJetMET: cosine between promptest jet and MET direction in transverse plane
		cosjetmet = 1.1;
		if (Jet_N > 0) cosjetmet = ((*Jet_px)[0]*MET_RefFinal_ex + (*Jet_py)[0]*MET_RefFinal_ey)/((*Jet_p_T)[0]*MET_RefFinal_et);
		
		// select the promptest lepton for this last variable
		if (pt1 >= pt2)  {PX=px1; PY=py1; PT=pt1;}
		if (pt1 <  pt2)  {PX=px2; PY=py2; PT=pt2;}
		
		cosptlepMET = (PX*MET_RefFinal_ex +PY*MET_RefFinal_ey)/ (PT * MET_RefFinal_et);
		
		// fill the jet info
		ejet1 = -99.99;
		ejet2 = -99.99;
		ejet3 = -99.99;
		
		njet = Jet_N;
		// add info on the 3 most energetic jets
		if (njet > 0) ejet1 = (*Jet_E)[0];
		if (njet > 1) ejet2 = (*Jet_E)[1];
		if (njet > 2) ejet3 = (*Jet_E)[2];
		
		ptlep1 = float(pt1);
		ptlep2 = float(pt2);
		cone1 = float(Cone1);
		cone2 = float(Cone2);
		lepID = abs(bestid);
		ptZ0 = pt2l;
		massZ = float(mZ);
		cosll2d = float(cosll2D);
		cosll3d = float(cosll3D);
		cosJetMET = float(cosjetmet);
		mT = float(mt);
		MET = float(MET_RefFinal_et);
		cosMETptlep = float(cosptlepMET);
		cosphi = float(cosPhi);
		
		runnum = EVRunNumber;
		evtnum  = EVEventAndInstance;
		type   = typeNumber;
		w      = float(weight[n]);
		
		// fill the preselection tree from the values calculated above
		if (n == 0 ) treeS.Fill();
		if (n != 0 ) treeB.Fill();
		
		//	      if (typeNumber == 6) cout << " got here 7 " << endl;
		// fill the histograms
		hptlep1[typeNumber]->Fill(pt1,weight[n]);
		hptlep2[typeNumber]->Fill(pt2,weight[n]);
		hcosll2D[typeNumber]->Fill(cosll2D,weight[n]);
		hcosll3D[typeNumber]->Fill(cosll3D,weight[n]);
		//		hMET[typeNumber]->Fill(MET,weight[n]);
		hmt[typeNumber]->Fill(mt,weight[n]);
		hcosMETPtlep[typeNumber]->Fill(cosMETptlep,weight[n]);
		hmZll[typeNumber]->Fill(massZ,weight[n]);
		hcosphi[typeNumber]->Fill(cosphi,weight[n]);
		hnjet[typeNumber]->Fill(njet,weight[n]);
		hcone1[typeNumber]->Fill(Cone1,weight[n]);
		hcone2[typeNumber]->Fill(Cone2,weight[n]);
		hejet1[typeNumber]->Fill(ejet1,weight[n]);
		hejet2[typeNumber]->Fill(ejet2,weight[n]);
		hejet3[typeNumber]->Fill(ejet3,weight[n]);
		hcosJetMET[typeNumber]->Fill(cosjetmet,weight[n]);
		hnel[typeNumber]->Fill(nel,weight[n]);
		hnmu[typeNumber]->Fill(nmu,weight[n]);
		hnlep[typeNumber]->Fill(nlep,weight[n]);
		hlepid[typeNumber]->Fill(lepID,weight[n]);

// 		hptlep1[typeNumber]->Fill(pt1);
// 		hptlep2[typeNumber]->Fill(pt2);
// 		hcosll2D[typeNumber]->Fill(cosll2D);
// 		hcosll3D[typeNumber]->Fill(cosll3D);
// 		hMET[typeNumber]->Fill(MET);
// 		hmt[typeNumber]->Fill(mt);
// 		hcosMETPtlep[typeNumber]->Fill(cosMETptlep);
// 		hmZll[typeNumber]->Fill(massZ);
// 		hcosphi[typeNumber]->Fill(cosphi);
// 		hnjet[typeNumber]->Fill(njet);
// 		hcone1[typeNumber]->Fill(Cone1);
// 		hcone2[typeNumber]->Fill(Cone2);
// 		hejet1[typeNumber]->Fill(ejet1);
// 		hejet2[typeNumber]->Fill(ejet2);
// 		hejet3[typeNumber]->Fill(ejet3);
// 		hcosJetMET[typeNumber]->Fill(cosjetmet);

	      } // end if on btag
	    } // end if on mZ
	  } // end if on nlep > nlepcut
	} // end on METcut
      } // end on trigger cut
      
    } // end of for loop over events   
    delete random;
  } //end loop over ndatasets
  
  double cut0[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double cut1[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double cut2[8]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double cut3[8]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double cut4[8]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double cut5[8]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double cut6[8]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  
  for (int n=0; n<ndatasets; n++) {
    
    if (n==0) typeNumber=0;
    if (n==1) typeNumber=1;
    if (n==2) typeNumber=2;
    if (n >= 3 && n <=6 ) typeNumber=3;
    if (n==7) typeNumber=4;
    if (n==8) typeNumber=5;
    if (n==9 || n==10) typeNumber=6;
    if (n==11) typeNumber=7;
    if (n==12) typeNumber=7;
    if (n==13) typeNumber=7;
    if (n==14) typeNumber=7;

    outClientFile << "              " << endl;
    outClientFile << "Cut flow for: " << eventType[n] << endl;
    outClientFile << "-----------------------------------------------------------" << endl;
    outClientFile << nrec[n] << " records found in the chain. "<< endl;
    
    cout << endl;
    cout << "METcut " << METcut << endl;
    cout << "pt cuts for leptons: e > " << eptmin << " and mu: " << mptmin << " GeV and eta < " << etamin << endl;	  
    cout << "nlep strictly equal to "<< nlepcut << " of the same flavor" << endl; 
    cout << "mZ within " <<  mZcut << " GeV of on-shell mass " << endl;
    cout << "total # of selected events in file: \t" << found[n] << endl;	  
    cout << endl;
    cout << "Cut flow for: " << eventType[n] << endl;
    cout << "-----------------------------------------------------------" << endl;
    cout << nrec[n] << " records found in the chain. "<< endl;
    cout << endl;
    
    passed[n] = nrec[n];
    totalRejected[n] = (reject_MET[n] + reject_nlep[n] + reject_trigger[n]  + reject_mZ[n] + reject_btag[n]); 
    
    int places = 2;
    cout <<fixed;                 
    cout.precision(places);
    
    cout << "  " << endl;
    cout << "pairs of electrons before MET cut: " << int(zee0[n]) << " muons: " << int(zmm0[n]) << endl;
    if (zmm0[n] > 0) cout << " ratio: " << zee0[n]/zmm0[n] << endl;
    cout << "pairs of electrons after MET cut:  " << int(zee1[n]) << " muons: " << int(zmm1[n]) << endl;
    if (zmm1[n] > 0) cout << " ratio: " << zee1[n]/zmm1[n] << endl;
    cout << "accepted pairs of electrons: " << int(zee[n]) << " muons: " << int(zmm[n]) << endl;
    double denom = (( zee[n] > zmm[n]) ? zee[n] : zmm[n]);
    double fraction = (( zee[n] < zmm[n]) ? zee[n]/zmm[n] : zmm[n]/zee[n]);
    
    double error = sqrt(fraction*(1.0-fraction)/denom);
    cout << "ratio e/mu: " << 100.0*fraction << " +/- " << 100.0*error <<" %" <<endl;
    cout << "  " << endl;
    
    outClientFile << "  " << endl;
    outClientFile << "pairs of electrons before MET cut: " << int(zee0[n]) << " muons: " << int(zmm0[n]) << " ratio: " << zee0[n]/zmm0[n] << endl;
    outClientFile << "pairs of electrons after MET cut:  " << int(zee1[n]) << " muons: " << int(zmm1[n]) << " ratio: " << zee1[n]/zmm1[n] << endl;
    outClientFile << "accepted pairs of electrons: " << zee[n] << " muons: " << zmm[n] << endl;
    outClientFile << "ratio e/mu: " << 100.0*fraction << " +/- " << 100.0*error <<" %" <<endl;
    outClientFile << "  " << endl;
    
    cout << "L1XET100 only events: " << L1XET100[n] << endl;
    cout << "double pair: " << doublepair[n]<< endl;
    cout << " " << endl;
    
    outClientFile << "L1XET100 only events: " << L1XET100[n] << endl;
    outClientFile << "double pair: " << doublepair[n]<< endl;
    outClientFile << " " << endl;
    
    //  cout << "before trigger: " << beforeTrigger[n]<< endl;
    //  cout << "after trigger: " << afterTrigger[n]<< endl;
    //  cout << "after MET: " << afterMET[n]<< endl;
    //  cout << "after nlep: " << afterNlep[n]<< endl;
    //  cout << "after MZ: " << afterMz[n]<< endl;
    //  cout << " " << endl;
    
    //cout << "     ************ trigger is turned off *****************************                    " <<   endl;
    cout << "                         " <<   endl;
    cout << "                         " << " cross-section              raw #                 percentage"    <<   endl;
    cout << "cross-section*BR in fb   " << 1000.*sigmaBR[n]   <<"\t" << generated[n] <<"\t" <<  100.*(generated[n]/generated[n])<< "%" << endl;
    cut0[typeNumber] += 1000.*sigmaBR[n]; 
    cout << "xs after filter:         " << filterEff[n]*1000.*sigmaBR[n] <<"\t" << passed[n]  <<"\t" <<  100.*(passed[n]/generated[n])<< "%" << endl;
    cut1[typeNumber] += filterEff[n]*1000.*sigmaBR[n]; 
    float temp = passed[n];
    passed[n] = (passed[n] - reject_trigger[n]);
    cout << "xs after trigger:        " << 1000.*sigmaBR[n]*passed[n]/generated[n] <<"\t" << passed[n] <<"\t" <<  100.*(passed[n]/generated[n])<<"\t" << "%; rel. eff: " <<  100.*(passed[n]/temp)<< "%" << endl;
    cut2[typeNumber] += 1000.*sigmaBR[n]*passed[n]/generated[n];
    temp = passed[n];
    passed[n] = (passed[n] - reject_MET[n]);
    cout << "xs after MET cut         " << 1000.*sigmaBR[n]*passed[n]/generated[n] <<"\t" << passed[n] <<"\t" <<  100.*(passed[n]/generated[n])<<"\t" << "%; rel. eff: " <<  100.*(passed[n]/temp)<< "%" << endl;
    cut3[typeNumber] += 1000.*sigmaBR[n]*passed[n]/generated[n];
    temp = passed[n];
    passed[n] = (passed[n] - reject_nlep[n]);
    cout << "xs after nlep cut:       " << 1000.*sigmaBR[n]*passed[n]/generated[n] <<"\t" << passed[n] <<"\t" <<  100.*(passed[n]/generated[n])<<"\t" << "%; rel. eff: " <<  100.*(passed[n]/temp)<< "%" << endl;
    cut4[typeNumber] += 1000.*sigmaBR[n]*passed[n]/generated[n];
    temp = passed[n];
    passed[n] = (passed[n] - reject_mZ[n]);
    cout << "xs after mZ cut:         " << 1000.*sigmaBR[n]*passed[n]/generated[n] <<"\t" << passed[n] <<"\t" <<  100.*(passed[n]/generated[n])<<"\t" << "%; rel. eff: " <<  100.*(passed[n]/temp)<< "%" << endl;
    cut5[typeNumber] += 1000.*sigmaBR[n]*passed[n]/generated[n];
    temp = passed[n];
    passed[n] = (passed[n] - reject_btag[n]);
    cout << "xs after b-tag cut:      " << 1000.*sigmaBR[n]*passed[n]/generated[n] <<"\t" << passed[n] <<"\t" <<  100.*(passed[n]/generated[n])<<"\t" << "%; rel. eff: " <<  100.*(passed[n]/temp)<< "%" << endl;
    cut6[typeNumber] += 1000.*sigmaBR[n]*passed[n]/generated[n];
    Total[n] = passed[n]*filterEff[n]; 
    cout << endl;
    cout << "Total rejected (reweighted):  " << lumi*sigmaBR[n] - 1000.*sigmaBR[n]*passed[n]/generated[n] << " out of " << generated[n]*filterEff[n] << endl;
    cout << "Total rejected (raw)       :  " << generated[n] - passed[n] << " out of " << generated[n]<< endl;
    cout << "Fraction retained          :  " << 100.*(passed[n]/generated[n]) << "% " << endl;
    places = 5;
    cout.precision(places);
    cout << endl;
    cout << "Events weight of " << weight[n] << endl;
    cout << " "<< endl;
    cout <<fixed;                 
    places = 2;
    cout.precision(places);
    
    // cout << "sanity check: # of events found before lepton cuts: " << sanity[n] << endl;
    // cout << endl;
    passed[n] = nrec[n];
    
    outClientFile << "                         " <<   endl;
    outClientFile << "                         " << " cross-section              raw #                 percentage"    <<   endl;
    outClientFile << "cross-section*BR in fb   " << 1000.*sigmaBR[n]   <<"\t" << generated[n] <<"\t" <<  100.*(generated[n]/generated[n])<< "%" << endl;
    outClientFile << "xs after filter:         " << filterEff[n]*1000.*sigmaBR[n] <<"\t" << passed[n]  <<"\t" <<  100.*(passed[n]/generated[n])<< "%" << endl;
    temp = passed[n];
    passed[n] = (passed[n] - reject_trigger[n]);
    outClientFile << "xs after trigger:        " << 1000.*sigmaBR[n]*passed[n]/generated[n] <<"\t" << passed[n] <<"\t" <<  100.*(passed[n]/generated[n])<<"\t" << "%; rel. eff: " <<  100.*(passed[n]/temp)<< "%" << endl;
    temp = passed[n];
    passed[n] = (passed[n] - reject_MET[n]);
    outClientFile << "xs after MET cut         " << 1000.*sigmaBR[n]*passed[n]/generated[n] <<"\t" << passed[n] <<"\t" <<  100.*(passed[n]/generated[n])<<"\t" << "%; rel. eff: " <<  100.*(passed[n]/temp)<< "%" << endl;
    temp = passed[n];
    passed[n] = (passed[n] - reject_nlep[n]);
    outClientFile << "xs after nlep cut:       " << 1000.*sigmaBR[n]*passed[n]/generated[n] <<"\t" << passed[n] <<"\t" <<  100.*(passed[n]/generated[n])<<"\t" << "%; rel. eff: " <<  100.*(passed[n]/temp)<< "%" << endl;
    temp = passed[n];
    passed[n] = (passed[n] - reject_mZ[n]);
    outClientFile << "xs after mZ cut:         " << 1000.*sigmaBR[n]*passed[n]/generated[n] <<"\t" << passed[n] <<"\t" <<  100.*(passed[n]/generated[n])<<"\t" << "%; rel. eff: " <<  100.*(passed[n]/temp)<< "%" << endl;
    temp = passed[n];
    passed[n] = (passed[n] - reject_btag[n]);
    outClientFile << "xs after b-tag cut:      " << 1000.*sigmaBR[n]*passed[n]/generated[n] <<"\t" << passed[n] <<"\t" <<  100.*(passed[n]/generated[n])<<"\t" << "%; rel. eff: " <<  100.*(passed[n]/temp)<< "%" << endl;
    Total[n] = passed[n]*filterEff[n]; 
    outClientFile << endl;
    outClientFile << "Total rejected (reweighted):  " << lumi*sigmaBR[n] - 1000.*sigmaBR[n]*passed[n]/generated[n] << " out of " << generated[n]*filterEff[n] << endl;
    outClientFile << "Total rejected (raw)       :  " << generated[n] - passed[n] << " out of " << generated[n]<< endl;
    outClientFile << "Fraction retained          :  " << 100.*(passed[n]/generated[n]) << "% " << endl;
    places = 5;
    outClientFile.precision(places);
    outClientFile << endl;
    outClientFile << "Events weight of " << weight[n] << endl;
    outClientFile << " "<< endl;
    outClientFile <<fixed;                 
    places = 2;
    outClientFile.precision(places);
    
    if (n != 0) TotalBgnd += Total[n]; 
    
    //end second loop over ndatasets of events
  }
  
  //    h_MET_final->Write();
  //    h_MET_ref->Write();
  
  // check what the tree looks like
  // treeS.Print();
  // treeB.Print();
  
  double fraction = 0.0;
  for (int n=0; n<ndatasets; n++) {fraction +=TotalUM[n];}
  cout << fraction << endl;
  fraction = 100.0*fraction/(Total[0]+TotalBgnd);
  // get info on S/sqrt(B)
  double SoverB = Total[0]/sqrt(TotalBgnd);
  cout << "*************************************************************************** " << endl;
  cout << "Number of files used         = " << ndatasets  << endl;
  for(int n=0; n<ndatasets; n++) { cout << "\t" << eventType[n] << "\t " << passed[n] << endl;}
  cout << "Total preselected signal     = " << Total[0]  << endl;
  cout << "Total preselected background = " << TotalBgnd << endl;
  cout << "Signal over sqrt(B)          = " << SoverB << endl;
  cout << "*************************************************************************** " << endl;
  
  SoverB = Total[0]/sqrt(TotalBgnd);
  outClientFile << "*************************************************************************** " << endl;
  outClientFile << "Number of files used         = " << ndatasets  << endl;
  for(int n=0; n<ndatasets; n++) { outClientFile << "\t" << eventType[n] << "\t " << passed[n] << endl;}
  outClientFile << "*************************************************************************** " << endl;
  outClientFile << "Total preselected signal     = " << Total[0]  << endl;
  outClientFile << "Total preselected background = " << TotalBgnd << endl;
  outClientFile << "Signal over sqrt(B)          = " << SoverB << endl;
  outClientFile << "*************************************************************************** " << endl;
  
  int places = 2;
  cout <<fixed;                 
  cout.precision(places);
  
  hs1.SetMinimum(1);
  hs2.SetMinimum(1);
  hs3.SetMinimum(1);
  hs4.SetMinimum(1);
  hs5.SetMinimum(1);
  hs6.SetMinimum(1);
  hs7.SetMinimum(1);
  hs8.SetMinimum(1);
  hs9.SetMinimum(1);
  hs10.SetMinimum(1);
  hs11.SetMinimum(1);
  hs12.SetMinimum(1);
  hs13.SetMinimum(1);
  hs14.SetMinimum(1);
  hs15.SetMinimum(1);
  hs16.SetMinimum(1);
  hs17.SetMinimum(1);
  hs18.SetMinimum(1);
  hs19.SetMinimum(1);
  hs20.SetMinimum(1);
  
  TLegend *leg1 = new TLegend(0.6, 0.6, 0.9, 0.95);
  TLegend *leg2 = new TLegend(0.6, 0.6, 0.9, 0.95);
  TLegend *leg3 = new TLegend(0.6, 0.6, 0.9, 0.95);
  TLegend *leg4 = new TLegend(0.65, 0.6, 0.9, 0.95);
  TLegend *leg5 = new TLegend(0.65, 0.6, 0.9, 0.95);
  
//     c6->cd();
//     pad61->cd(); 
//     hgauss1->Draw();
//     pad62->cd(); 
//     hgauss2->Draw();

  for (int m=0; m<ntype; m++) {

    c1->cd();
    hs17.Add(hnel[m]);
    pad13->cd(); 
    hs17.Draw("");
    hs17.Draw("");
    if (m==0) {leg1->AddEntry(hnel[m], "  HZ", "f"); }
    //    if (m==1) {leg1->AddEntry(hnel[m], "  ChL #rightarrow ZW #rightarrow l l l #nu", "f"); }
    if (m==1) {leg1->AddEntry(hnel[m], "  ZZ #rightarrow l l #nu #nu", "f"); }
    if (m==2) {leg1->AddEntry(hnel[m], "  t#bar t", "f");} 
    if (m==3) {leg1->AddEntry(hnel[m], "  WW", "f"); }
//     if (m==4) {leg1->AddEntry(hnel[m], "  ZZ #rightarrow #tau #tau #nu #nu", "f"); }
    if (m==5) {leg1->AddEntry(hnel[m], "  ZZ #rightarrow l l #tau #tau", "f"); }
    if (m==6) {leg1->AddEntry(hnel[m], "  ZW", "f"); }
    if (m==7) {leg1->AddEntry(hnel[m], "  Z + jets", "f");} 
    leg1->Draw();

    hs18.Add(hnmu[m]);
    pad14->cd(); 
    hs18.Draw("");
    hs18.Draw("");

    hs19.Add(hnlep[m]);
    pad11->cd(); 
    hs19.Draw("");
    hs19.Draw("");

    hs20.Add(hlepid[m]);
    pad12->cd(); 
    hs20.Draw("");
    hs20.Draw("");

    c3->cd();
    hs1.Add(hptlep1[m]);
    pad33->cd(); 
    //   pad33->SetLogy(1);
    hs1.Draw("");
    hs1.Draw("");
    if (m==0) {leg3->AddEntry(hptlep1[m], "  HZ", "f"); }
    //    if (m==1) {leg3->AddEntry(hptlep1[m], "  ChL #rightarrow ZW #rightarrow l l l #nu", "f"); }
    if (m==1) {leg3->AddEntry(hptlep1[m], "  ZZ #rightarrow l l #nu #nu", "f"); }
    if (m==2) {leg3->AddEntry(hptlep1[m], "  t#bar t", "f");} 
    if (m==3) {leg3->AddEntry(hptlep1[m], "  WW", "f"); }
//     if (m==4) {leg3->AddEntry(hptlep1[m], "  ZZ #rightarrow #tau #tau #nu #nu", "f"); }
    if (m==5) {leg3->AddEntry(hptlep1[m], "  ZZ #rightarrow l l #tau #tau", "f"); }
    if (m==6) {leg3->AddEntry(hptlep1[m], "  ZW", "f"); }
    if (m==7) {leg3->AddEntry(hptlep1[m], "  Z + jets", "f");} 
    leg3->Draw();
    
    hs2.Add(hptlep2[m]);
    pad34->cd(); 
    //   pad34->SetLogy(1);
    hs2.Draw("");
    hs2.Draw("");
    
    hs3.Add(hcosll2D[m]);
    pad31->cd(); 
    //     pad31->SetLogy(1);
    hs3.Draw("");
    hs3.Draw("");
    
    hs4.Add(hcosll3D[m]);
    pad32->cd(); 
    //     pad32->SetLogy(1);
    hs4.Draw("");
    hs4.Draw("");
    
    c2->cd();
    hs5.Add(hMET[m]);
    pad23->cd(); 
    //      pad23->SetLogy(1);
    hs5.Draw("");
    hs5.Draw("");
    if (m==0) {leg2->AddEntry(hMET[m], "  HZ", "f"); }
    //    if (m==1) {leg2->AddEntry(hMET[m], "  ChL #rightarrow ZW #rightarrow l l l #nu", "f"); }
    if (m==1) {leg2->AddEntry(hMET[m], "  ZZ #rightarrow l l #nu #nu", "f"); }
    if (m==2) {leg2->AddEntry(hMET[m], "  t#bar t", "f");} 
    if (m==3) {leg2->AddEntry(hMET[m], "  WW", "f"); }
//     if (m==4) {leg2->AddEntry(hMET[m], "  ZZ #rightarrow #tau #tau #nu #nu", "f"); }
    if (m==5) {leg2->AddEntry(hMET[m], "  ZZ #rightarrow l l #tau #tau", "f"); }
    if (m==6) {leg2->AddEntry(hMET[m], "  ZW", "f"); }
    if (m==7) {leg2->AddEntry(hMET[m], "  Z + jets", "f");} 
    leg2->Draw();
    
    hs6.Add(hmt[m]);
    pad24->cd(); 
    //    pad24->SetLogy(1);
    hs6.Draw("");
    hs6.Draw("");
    
    hs7.Add(hcosMETPtlep[m]);
    pad21->cd(); 
    //    pad21->SetLogy(1);
    hs7.Draw("");
    hs7.Draw("");
    
    hs8.Add(hmZll[m]);
    pad22->cd(); 
    //    pad22->SetLogy(1);
    hs8.Draw("");
    hs8.Draw("");
    
    c4->cd();
    hs9.Add(hcosphi[m]);
    pad43->cd(); 
    pad43->SetLogy(1);
    hs9.Draw("");
    hs9.Draw("");
    if (m==0) {leg4->AddEntry(hcosphi[m], "  HZ", "f"); }
    //    if (m==1) {leg4->AddEntry(hcosphi[m], "  ChL #rightarrow ZW #rightarrow l l l #nu", "f"); }
    if (m==1) {leg4->AddEntry(hcosphi[m], "  ZZ #rightarrow l l #nu #nu", "f"); }
    if (m==2) {leg4->AddEntry(hcosphi[m], "  t#bar t", "f");} 
    if (m==3) {leg4->AddEntry(hcosphi[m], "  WW", "f"); }
//     if (m==4) {leg4->AddEntry(hcosphi[m], "  ZZ #rightarrow #tau #tau #nu #nu", "f"); }
    if (m==5) {leg4->AddEntry(hcosphi[m], "  ZZ #rightarrow l l #tau #tau", "f"); }
    if (m==6) {leg4->AddEntry(hcosphi[m], "  ZW", "f"); }
    if (m==7) {leg4->AddEntry(hcosphi[m], "  Z + jets", "f");} 
    leg4->Draw();
    
    hs10.Add(hcosJetMET[m]);
    pad44->cd(); 
    pad44->SetLogy(1);
    hs10.Draw("");
    hs10.Draw("");
    
    hs11.Add(hcone1[m]);
    pad41->cd(); 
    pad41->SetLogy(1);
    hs11.Draw("");
    hs11.Draw("");
    
    hs12.Add(hcone2[m]);
    pad42->cd(); 
    pad42->SetLogy(1);
    hs12.Draw("");
    hs12.Draw("");
    
    c5->cd();
    hs13.Add(hejet1[m]);
    pad53->cd(); 
    //   pad53->SetLogy(1);
    hs13.Draw("");
    hs13.Draw("");
    
    hs14.Add(hejet2[m]);
    pad54->cd(); 
    //    pad54->SetLogy(1);
    hs14.Draw("");
    hs14.Draw("");
    
    hs15.Add(hejet3[m]);
    pad51->cd(); 
    //    pad51->SetLogy(1);
    hs15.Draw("");
    hs15.Draw("");
    
    hs16.Add(hnjet[m]);
    pad52->cd(); 
    //    pad52->SetLogy(1);
    hs16.Draw("");
    hs16.Draw("");
    if (m==0) {leg5->AddEntry(hnjet[m], "  HZ", "f"); }
    //    if (m==1) {leg5->AddEntry(hnjet[m], "  ChL #rightarrow ZW #rightarrow l l l #nu", "f"); }
    if (m==1) {leg5->AddEntry(hnjet[m], "  ZZ #rightarrow l l #nu #nu", "f"); }
    if (m==2) {leg5->AddEntry(hnjet[m], "  t#bar t", "f");} 
    if (m==3) {leg5->AddEntry(hnjet[m], "  WW", "f"); }
//     if (m==4) {leg5->AddEntry(hnjet[m], "  ZZ #rightarrow #tau #tau #nu #nu", "f"); }
    if (m==5) {leg5->AddEntry(hnjet[m], "  ZZ #rightarrow l l #tau #tau", "f"); }
    if (m==6) {leg5->AddEntry(hnjet[m], "  ZW", "f"); }
    if (m==7) {leg5->AddEntry(hnjet[m], "  Z + jets", "f");} 
    leg5->Draw();

 } //end loop over all types of events
  
  for (int m=0; m<ntype; m++) {
    cout << "                          " << endl;
    cout << "combined cutflow for type " << m << endl;
    cout << "cross-section*BR in fb    " << cut0[m] << endl;
    cout << "xs after filter:          " << cut1[m] << endl;
    cout << "xs after trigger:         " << cut2[m] << endl;
    cout << "xs after MET cut          " << cut3[m] << endl;
    cout << "xs after nlep cut:        " << cut4[m] << endl;
    cout << "xs after mZ cut:          " << cut5[m] << endl;
    cout << "xs after b-tag cut:       " << cut6[m] << endl;
  }

    cout <<fixed;                 
    places = 1;
    cout.precision(places);

    cout << "                          " << endl;
    cout << "$\sigma$*BR in fb    &        " << cut0[0] << " & " << cut0[1] << " & " << cut0[2] << " & " << cut0[3] << " & " << cut0[4] << " & " << cut0[5] << " & " << cut0[6] << " & " << cut0[7] << " \\ " << endl;
    cout << "$\sigma$ after filter:          & " << cut1[0] << " & " << cut1[1] << " & " << cut1[2] << " & " << cut1[3] << " & " << cut1[4] << " & " << cut1[5] << " & " << cut1[6] << " & " << cut1[7] << " \\ " << endl;
    cout << "$\sigma$ after trigger:         & " << cut2[0] << " & " << cut2[1] << " & " << cut2[2] << " & " << cut2[3] << " & " << cut2[4] << " & " << cut2[5] << " & " << cut2[6] << " & " << cut2[7] << " \\ " << endl;
    cout << "$\sigma$ after MET cut          & " << cut3[0] << " & " << cut3[1] << " & " << cut3[2] << " & " << cut3[3] << " & " << cut3[4] << " & " << cut3[5] << " & " << cut3[6] << " & " << cut3[7] << " \\ " << endl;
    cout << "$\sigma$ after nlep cut:        & " << cut4[0] << " & " << cut4[1] << " & " << cut4[2] << " & " << cut4[3] << " & " << cut4[4] << " & " << cut4[5] << " & " << cut4[6] << " & " << cut4[7] << " \\ " << endl;
    cout << "$\sigma$ after mZ cut:          & " << cut5[0] << " & " << cut5[1] << " & " << cut5[2] << " & " << cut5[3] << " & " << cut5[4] << " & " << cut5[5] << " & " << cut5[6] << " & " << cut5[7] << " \\ " << endl;
    cout << "$\sigma$ after b-tag cut:       & " << cut6[0] << " & " << cut6[1] << " & " << cut6[2] << " & " << cut6[3] << " & " << cut6[4] << " & " << cut6[5] << " & " << cut6[6] << " & " << cut6[7] << " \\ " << endl;

  for (int m=0; m<ntype; m++) {
    outClientFile << "                          " << endl;
    outClientFile << "combined cutflow for type " << m << endl;
    outClientFile << "cross-section*BR in fb    " << cut0[m] << endl;
    outClientFile << "xs after filter:          " << cut1[m] << endl;
    outClientFile << "xs after trigger:         " << cut2[m] << endl;
    outClientFile << "xs after MET cut          " << cut3[m] << endl;
    outClientFile << "xs after nlep cut:        " << cut4[m] << endl;
    outClientFile << "xs after mZ cut:          " << cut5[m] << endl;
    outClientFile << "xs after b-tag cut:       " << cut6[m] << endl;
  }

    outClientFile <<fixed;                 
    places = 1;
    outClientFile.precision(places);

    outClientFile << "                          " << endl;
    outClientFile << "$\sigma$*BR in fb    &        " << cut0[0] << " & " << cut0[1] << " & " << cut0[2] << " & " << cut0[3] << " & " << cut0[4] << " & " << cut0[5] << " & " << cut0[6] << " & " << cut0[7] << " \\ " << endl;
    outClientFile << "$\sigma$ after filter:          & " << cut1[0] << " & " << cut1[1] << " & " << cut1[2] << " & " << cut1[3] << " & " << cut1[4] << " & " << cut1[5] << " & " << cut1[6] << " & " << cut1[7] << " \\ " << endl;
    outClientFile << "$\sigma$ after trigger:         & " << cut2[0] << " & " << cut2[1] << " & " << cut2[2] << " & " << cut2[3] << " & " << cut2[4] << " & " << cut2[5] << " & " << cut2[6] << " & " << cut2[7] << " \\ " << endl;
    outClientFile << "$\sigma$ after MET cut          & " << cut3[0] << " & " << cut3[1] << " & " << cut3[2] << " & " << cut3[3] << " & " << cut3[4] << " & " << cut3[5] << " & " << cut3[6] << " & " << cut3[7] << " \\ " << endl;
    outClientFile << "$\sigma$ after nlep cut:        & " << cut4[0] << " & " << cut4[1] << " & " << cut4[2] << " & " << cut4[3] << " & " << cut4[4] << " & " << cut4[5] << " & " << cut4[6] << " & " << cut4[7] << " \\ " << endl;
    outClientFile << "$\sigma$ after mZ cut:          & " << cut5[0] << " & " << cut5[1] << " & " << cut5[2] << " & " << cut5[3] << " & " << cut5[4] << " & " << cut5[5] << " & " << cut5[6] << " & " << cut5[7] << " \\ " << endl;
    outClientFile << "$\sigma$ after b-tag cut:       & " << cut6[0] << " & " << cut6[1] << " & " << cut6[2] << " & " << cut6[3] << " & " << cut6[4] << " & " << cut6[5] << " & " << cut6[6] << " & " << cut6[7] << " \\ " << endl;

  f.Write();   // write to the file histograms and trees
  
  cout << " " << endl;
  cout << "All done!" << endl;
  
  theApp.Run();                            // concluding ROOT requirements.
  
  return 0;
} //  End of main(...)

