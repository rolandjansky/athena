/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// program used to apply the final selection based on the chosen TMVA output variable. (previously called stack.C)
// 
// *************    Written by Pauline Gagnon - 14 mars 2007  *********************
// updated on nov 21 2007
// 
// first, one must run TMVAnalysis.C (root .L TMVAnalysis.C) to obtain the weights for a specific method
// this will read the preselected events from preselection-6types.root tree
// second, run TMVApplication.C (root .L TMVApplication.C) to read in the weights 
// reads in the TMVA weights obtained for several backgrounds separately from final.root tree
// display the distribution of the TMVA output variable to determine the best place to set the cuts.
// 

using namespace std;

#define CHAIN

#include <fstream>
using std::ofstream;

#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <string.h>
#include <vector>
#include <list>  
#include <stdlib.h>
// Regular include files to make ROOT work with our C++ program
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

// - - - Initiates the GUI interface of ROOT - - -
using std::string;
using std::setprecision;
Int_t places = 2;
extern void InitGui();

//============================================================================

int main(int argc, char **argv)
{
  // execute rootlogon.C to get ATLAS style setup.
#include "rootlogon.C"


   // initialization of all counters for statistics

  // enter the number of different types of events and the # of background weight files used
  Int_t const ntype = 8;
  Int_t const nbgndw = 7;
  Float_t mvaBdt[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  // give the desired cut value
  string suffix[8] = {("HZ-ZZ"), ("HZ-tt"), ("HZ-WW"), ("HZ-ZZtautaununu"), ("HZ-ZZlltautau"), ("HZ-ZW"), ("HZ-incl")}; 

  //           float minCutValue[7] = {-1.0, 0.84, 0.74, -1.0, 0.68, 0.44, 0.62};  // HZ120 with half the event - 3 june

  //  float minCutValue[7] = {-1.0, 0.66, 0.30, -1.0, 0.72, 0.24, 0.56};  // HZ110 - 10 June 95% CL
  //  float minCutValue[7] = {-1.0, 0.72, 0.58, -1.0, 0.74, 0.72, 0.68};  // HZ120 -eff - 0.496 - 10 juin 95% CL
  //  float minCutValue[7] = {-1.0, 0.52, -0.08, -1.0, 0.42, 0.20, 0.62}; // HZ130 with half the event - 3 june
    float minCutValue[7] = {-1.0, 0.62, 0.38, -1.0, 0.66, 0.24, 0.58};  // HZ140 with half the event - 3 june
  //  float minCutValue[7] = {-1.0, -1.0, 0.64, -1.0, -1.0, 0.44, -1.0};  // HZ150 - 10 June 95% CL
  //  float minCutValue[7] = {-1.0, 0.50, 0.44, -1.0, 0.64, 0.46, -1.0};  // HZ200 - 10 June 95% CL
  //  float minCutValue[7] = {-1.0, 0.72, 0.34, -1.0, -1.0, 0.62, -1.0};  // HZ250 - 10 June 95% CL

	// float minCutValue[7] = {-1.0, 0.84, 0.74, -1.0, 0.68, 0.44, 0.62};  // HZ120 with half the event - 3 june

  Float_t weight[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  // Float_t preselected[8] = {3574., 13427., 3955., 5555., 56., 636., 3832., 663.};  // standard preselection cuts mH = 110 GeV
  // Float_t preselected[8] = {18799., 13427., 3955., 5555., 56., 636., 3832., 663.};  // standard preselection cuts mH = 120 GeV
  // Float_t preselected[8] = {2674., 13427., 3955., 5555., 56., 636., 3832., 663.};  // standard preselection cuts mH = 130 GeV
   Float_t preselected[8] = {2972., 13427., 3955., 5555., 56., 636., 3832., 663.};  // standard preselection cuts mH = 140 GeV
  // Float_t preselected[8] = {4333., 13427., 3955., 5555., 56., 636., 3832., 663.};  // standard preselection cuts mH = 150 GeV
  // Float_t preselected[8] = {4957., 13427., 3955., 5555., 56., 636., 3832., 663.};  // standard preselection cuts mH = 200 GeV
  // Float_t preselected[8] = {5415., 13427., 3955., 5555., 56., 636., 3832., 663.};  // standard preselection cuts mH = 250 GeV

  Float_t selected[8] =   {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  Float_t rejected[8] =   {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  Float_t efficiency[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  Float_t rejection[8] =  {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  Float_t Total[8]  =     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  Float_t TotalBgnd = 0.0;

  string eventType[8]  = {("HZ"), ("ZZ"), ("tt"), ("WW"), ("ZZtautaununu"),("ZZlltautau"),("ZW"),("Zincl")}; 

  float NSignal = 0.0;
  float NBgnd = 0.0;

  int passed[7] = {0,0,0,0,0,0,0};
  float failed[8][7] = {
    {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0,0.0,0.0,0.0}
  };

  // open output file to save the cutflow results
  ofstream outClientFile( "stats.dat", ios::out );

  // exit program if unable to open file
  if ( !outClientFile ) {
    cerr << "File could not be opened" << endl;
    exit (1);
  } // end if

  VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
  TROOT root("","", initfuncs);

  TApplication theApp("App", &argc, argv);
  gROOT->Reset();
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();

  // initialisation: setup all the histograms, stacks, pads, canvases
  //----------------------------------------------------------------
  //     - - - Creating canvases with several pads - - -
  //-------------------------------------------------------------

  const unsigned int XC1=0, YC1=0, XC2=800, YC2=800;
  const double G=0.004;
  
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
    hcone1[m] = new TH1F(histName11.c_str(), ";electron isolation cone 0.10 ;@# of entries", 50, 0.0, 50.0);
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

    Color_t color = kWhite;
    if (m==0) color = kRed;
    if (m==1) color = kYellow;
    if (m==2) color = kBlue;
    if (m==3) color = kGreen;
    if (m==4) color = kBlack;
    if (m==5) color = kWhite;
    if (m==6) color = kMagenta;
    if (m==7) color = kBlack;

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
  }

  TCanvas* c1 = new TCanvas("c1","BDT output value before cuts", XC1, YC1, XC2, YC2);
  c1->cd();
  c1->SetFillColor(10);
  c1->GetFrame()->SetFillColor(10);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);
  TPad* pad11= new TPad("pad11", "Pad11",  G,       G,       1.0-G,  0.17-G);
  TPad* pad12= new TPad("pad12", "Pad12",  G,  0.17+G,       1.0-G,  0.33-G);
  TPad* pad13= new TPad("pad13", "Pad13",  G,  0.33+G,       1.0-G,  0.50-G);
  TPad* pad14= new TPad("pad14", "Pad14",  G,  0.50+G,       1.0-G,  0.67-G);
  TPad* pad15= new TPad("pad15", "Pad15",  G,  0.67+G,       1.0-G,  0.83-G);
  TPad* pad16= new TPad("pad16", "Pad16",  G,  0.83+G,       1.0-G,  1.0-G);
  pad11->SetFillColor(10); pad11->Draw();
  pad12->SetFillColor(10); pad12->Draw();   
  pad13->SetFillColor(10); pad13->Draw();
  pad14->SetFillColor(10); pad14->Draw();
  pad15->SetFillColor(10); pad15->Draw();
  pad16->SetFillColor(10); pad16->Draw();

//   TCanvas* c2 = new TCanvas("c2","BDT output value after cuts", XC1, YC1, XC2, YC2);
//   c2->cd();
//   c2->SetFillColor(10);
//   c2->GetFrame()->SetFillColor(10);
//   c2->GetFrame()->SetBorderSize(6);
//   c2->GetFrame()->SetBorderMode(-1);
//   TPad* pad21= new TPad("pad21", "Pad21",  G,       G,       1.0-G,  0.17-G);
//   TPad* pad22= new TPad("pad22", "Pad22",  G,  0.17+G,       1.0-G,  0.33-G);
//   TPad* pad23= new TPad("pad23", "Pad23",  G,  0.33+G,       1.0-G,  0.50-G);
//   TPad* pad24= new TPad("pad24", "Pad24",  G,  0.50+G,       1.0-G,  0.67-G);
//   TPad* pad25= new TPad("pad25", "Pad25",  G,  0.67+G,       1.0-G,  0.83-G);
//   TPad* pad26= new TPad("pad26", "Pad26",  G,  0.83+G,       1.0-G,  1.0-G);
//   pad21->SetFillColor(10); pad21->Draw();
//   pad22->SetFillColor(10); pad22->Draw();   
//   pad23->SetFillColor(10); pad23->Draw();
//   pad24->SetFillColor(10); pad24->Draw();
//   pad25->SetFillColor(10); pad25->Draw();
//   pad26->SetFillColor(10); pad26->Draw();

  // -----------------------------------------------------------
  //      - - -  Creating three stacks - - -  
  // -----------------------------------------------------------
  THStack hs1("hs1","ZZ BDT output"); 
  THStack hs2("hs2","tt BDT output");
  THStack hs3("hs3","WW BDT output");
  THStack hs4("hs4","ZZ #rightarrow #tau #tau #nu #nu BDT output");
  THStack hs5("hs5","ZZ #rightarrow l l #tau #tau BDT output");
  THStack hs6("hs6","ZW BDT output");
  THStack hs7("hs7","Zincl BDT output");

  THStack hs11("hs11","ZZ BDT output");
  THStack hs12("hs12","tt BDT output");
  THStack hs13("hs13","WW BDT output");
  // THStack hs14("hs14","tautaununu");
  THStack hs15("hs15","lltautau BDT output");
  THStack hs16("hs16","ZW BDT output");
  THStack hs17("hs17","Zincl BDT output");

  THStack hs21("hs21"," BDT input"); 
  THStack hs22("hs22"," BDT input");
  THStack hs23("hs23"," BDT input");
  THStack hs24("hs24"," BDT input");
  THStack hs25("hs25"," BDT input");
  THStack hs26("hs26"," BDT input");
  THStack hs27("hs27"," BDT input");
  THStack hs28("hs28"," BDT input");


  // -----------------------------------------------------------
  //      - - -  Creating many histograms - - -  
  // -----------------------------------------------------------
  TLegend *leg11 = new TLegend(0.1, 0.3, 0.25, 1.0);
  TLegend *leg1 = new TLegend(0.6, 0.6, 0.9, 0.95);
  TLegend *leg3 = new TLegend(0.6, 0.6, 0.9, 0.95);
  TLegend *leg4 = new TLegend(0.6, 0.6, 0.9, 0.95);

  // //create some histograms
  // TH1F* hbdtZZ0 = new TH1F("hbdtZZ0", "BDT ZZ for signal", 100, -2.0, 1.0);
  // TH1F* hbdtZZ1 = new TH1F("hbdtZZ1", "BDT ZZ for ZZ", 100, -2.0, 1.0);
  // TH1F* hbdtZZ2 = new TH1F("hbdtZZ2", "BDT ZZ for ttbar", 100, -2.0, 1.0);
  // TH1F* hbdtZZ3 = new TH1F("hbdtZZ3", "BDT ZZ for WW", 100, -2.0, 1.0);
  // TH1F* hbdtZZ4 = new TH1F("hbdtZZ4", "BDT ZZ for ZZtautaununu", 100, -2.0, 1.0);
  // TH1F* hbdtZZ5 = new TH1F("hbdtZZ5", "BDT ZZ for ZZlltautau", 100, -2.0, 1.0);
  // TH1F* hbdtZZ6 = new TH1F("hbdtZZ6", "BDT ZZ for ZW", 100, -2.0, 1.0);

  TH1F* hbdtZZ0 = new TH1F("hbdtZZ0", ";BDT ZZ", 100, -2.0, 1.0);
  TH1F* hbdtZZ1 = new TH1F("hbdtZZ1", ";BDT ZZ", 100, -2.0, 1.0);
  TH1F* hbdtZZ2 = new TH1F("hbdtZZ2", ";BDT ZZ", 100, -2.0, 1.0);
  TH1F* hbdtZZ3 = new TH1F("hbdtZZ3", ";BDT ZZ", 100, -2.0, 1.0);
  // TH1F* hbdtZZ4 = new TH1F("hbdtZZ4", ";BDT ZZ", 100, -2.0, 1.0);
  TH1F* hbdtZZ5 = new TH1F("hbdtZZ5", ";BDT ZZ", 100, -2.0, 1.0);
  TH1F* hbdtZZ6 = new TH1F("hbdtZZ6", ".BDT ZZ", 100, -2.0, 1.0);
  TH1F* hbdtZZ7 = new TH1F("hbdtZZ7", ".BDT ZZ", 100, -2.0, 1.0);

  TH1F* hbdttt0 = new TH1F("hbdttt0", "BDT tt for signal", 100, -2.0, 1.0);
  TH1F* hbdttt1 = new TH1F("hbdttt1", "BDT tt for ZZ", 100, -2.0, 1.0);
  TH1F* hbdttt2 = new TH1F("hbdttt2", "BDT tt for ttbar", 100, -2.0, 1.0);
  TH1F* hbdttt3 = new TH1F("hbdttt3", "BDT tt for WW", 100, -2.0, 1.0);
  // TH1F* hbdttt4 = new TH1F("hbdttt4", "BDT tt for ZZtautaununu", 100, -2.0, 1.0);
  TH1F* hbdttt5 = new TH1F("hbdttt5", "BDT tt for ZZlltautau", 100, -2.0, 1.0);
  TH1F* hbdttt6 = new TH1F("hbdttt6", "BDT tt for ZW", 100, -2.0, 1.0);
  TH1F* hbdttt7 = new TH1F("hbdttt7", "BDT tt for Zincl", 100, -2.0, 1.0);

  TH1F* hbdtWW0 = new TH1F("hbdtWW0", "BDT WW for signal", 100, -2.0, 1.0);
  TH1F* hbdtWW1 = new TH1F("hbdtWW1", "BDT WW for ZZ", 100, -2.0, 1.0);
  TH1F* hbdtWW2 = new TH1F("hbdtWW2", "BDT WW for ttbar", 100, -2.0, 1.0);
  TH1F* hbdtWW3 = new TH1F("hbdtWW3", "BDT WW for WW", 100, -2.0, 1.0);
  // TH1F* hbdtWW4 = new TH1F("hbdtWW4", "BDT WW for ZZtautaununu", 100, -2.0, 1.0);
  TH1F* hbdtWW5 = new TH1F("hbdtWW5", "BDT WW for ZZlltautau", 100, -2.0, 1.0);
  TH1F* hbdtWW6 = new TH1F("hbdtWW6", "BDT WW for ZW", 100, -2.0, 1.0);
  TH1F* hbdtWW7 = new TH1F("hbdtWW7", "BDT WW for Zincl", 100, -2.0, 1.0);

  // TH1F* hbdttn0 = new TH1F("hbdttn0", "BDT ZZtautaununu for signal", 100, -2.0, 1.0);
  // TH1F* hbdttn1 = new TH1F("hbdttn1", "BDT ZZtautaununu for ZZ", 100, -2.0, 1.0);
  // TH1F* hbdttn2 = new TH1F("hbdttn2", "BDT ZZtautaununu for ttbar", 100, -2.0, 1.0);
  // TH1F* hbdttn3 = new TH1F("hbdttn3", "BDT ZZtautaununu for WW", 100, -2.0, 1.0);
  // TH1F* hbdttn4 = new TH1F("hbdttn4", "BDT ZZtautaununu for ZZtautaununu", 100, -2.0, 1.0);
  // TH1F* hbdttn5 = new TH1F("hbdttn5", "BDT ZZtautaununu for ZZlltautau", 100, -2.0, 1.0);
  // TH1F* hbdttn6 = new TH1F("hbdttn6", "BDT ZZtautaununu for ZW", 100, -2.0, 1.0);
  // TH1F* hbdttn7 = new TH1F("hbdttn7", "BDT ZZtautaununu for Zincl", 100, -2.0, 1.0);

  TH1F* hbdtlt0 = new TH1F("hbdtlt0", "BDT ZZlltautau for signal", 100, -2.0, 1.0);
  TH1F* hbdtlt1 = new TH1F("hbdtlt1", "BDT ZZlltautau for ZZ", 100, -2.0, 1.0);
  TH1F* hbdtlt2 = new TH1F("hbdtlt2", "BDT ZZlltautau for ttbar", 100, -2.0, 1.0);
  TH1F* hbdtlt3 = new TH1F("hbdtlt3", "BDT ZZlltautau for WW", 100, -2.0, 1.0);
  // TH1F* hbdtlt4 = new TH1F("hbdtlt4", "BDT ZZlltautau for ZZtautaununu", 100, -2.0, 1.0);
  TH1F* hbdtlt5 = new TH1F("hbdtlt5", "BDT ZZlltautau for ZZlltautau", 100, -2.0, 1.0);
  TH1F* hbdtlt6 = new TH1F("hbdtlt6", "BDT ZZlltautau for ZW", 100, -2.0, 1.0);
  TH1F* hbdtlt7 = new TH1F("hbdtlt7", "BDT ZZlltautau for Zincl", 100, -2.0, 1.0);

  TH1F* hbdtZW0 = new TH1F("hbdtZW0", "BDT ZW for signal", 100, -2.0, 1.0);
  TH1F* hbdtZW1 = new TH1F("hbdtZW1", "BDT ZW for ZZ", 100, -2.0, 1.0);
  TH1F* hbdtZW2 = new TH1F("hbdtZW2", "BDT ZW for ttbar", 100, -2.0, 1.0);
  TH1F* hbdtZW3 = new TH1F("hbdtZW3", "BDT ZW for WW", 100, -2.0, 1.0);
  // TH1F* hbdtZW4 = new TH1F("hbdtZW4", "BDT ZW for ZZtautaununu", 100, -2.0, 1.0);
  TH1F* hbdtZW5 = new TH1F("hbdtZW5", "BDT ZW for ZZtautaununu", 100, -2.0, 1.0);
  TH1F* hbdtZW6 = new TH1F("hbdtZW6", "BDT ZW for ZW", 100, -2.0, 1.0);
  TH1F* hbdtZW7 = new TH1F("hbdtZW7", "BDT ZW for Zincl", 100, -2.0, 1.0);

  TH1F* hbdtZi0 = new TH1F("hbdtZi0", "BDT Zi for signal", 100, -2.0, 1.0);
  TH1F* hbdtZi1 = new TH1F("hbdtZi1", "BDT Zi for ZZ", 100, -2.0, 1.0);
  TH1F* hbdtZi2 = new TH1F("hbdtZi2", "BDT Zi for ttbar", 100, -2.0, 1.0);
  TH1F* hbdtZi3 = new TH1F("hbdtZi3", "BDT Zi for WW", 100, -2.0, 1.0);
  // TH1F* hbdtZi4 = new TH1F("hbdtZi4", "BDT Zi for ZZtautaununu", 100, -2.0, 1.0);
  TH1F* hbdtZi5 = new TH1F("hbdtZi5", "BDT Zi for ZZtautaununu", 100, -2.0, 1.0);
  TH1F* hbdtZi6 = new TH1F("hbdtZi6", "BDT Zi for ZW", 100, -2.0, 1.0);
  TH1F* hbdtZi7 = new TH1F("hbdtZi7", "BDT Zi for Zincl", 100, -2.0, 1.0);


  TH1F* h2bdtZZ0 = new TH1F("h2bdtZZ0", "BDT ZZ for signal", 100, -1.0, 1.0);
  TH1F* h2bdtZZ1 = new TH1F("h2bdtZZ1", "BDT ZZ for ZZ", 100, -1.0, 1.0);
  TH1F* h2bdtZZ2 = new TH1F("h2bdtZZ2", "BDT ZZ for ttbar", 100, -1.0, 1.0);
  TH1F* h2bdtZZ3 = new TH1F("h2bdtZZ3", "BDT ZZ for WW", 100, -1.0, 1.0);
  // TH1F* h2bdtZZ4 = new TH1F("h2bdtZZ4", "BDT ZZ for ZZtautaununu", 100, -1.0, 1.0);
  TH1F* h2bdtZZ5 = new TH1F("h2bdtZZ5", "BDT ZZ for ZZlltautau", 100, -1.0, 1.0);
  TH1F* h2bdtZZ6 = new TH1F("h2bdtZZ6", "BDT ZZ for ZW", 100, -1.0, 1.0);
  TH1F* h2bdtZZ7 = new TH1F("h2bdtZZ7", "BDT ZZ for Zincl", 100, -1.0, 1.0);

  TH1F* h2bdttt0 = new TH1F("h2bdttt0", "BDT tt for signal", 100, -1.0, 1.0);
  TH1F* h2bdttt1 = new TH1F("h2bdttt1", "BDT tt for ZZ", 100, -1.0, 1.0);
  TH1F* h2bdttt2 = new TH1F("h2bdttt2", "BDT tt for ttbar", 100, -1.0, 1.0);
  TH1F* h2bdttt3 = new TH1F("h2bdttt3", "BDT tt for WW", 100, -1.0, 1.0);
  // TH1F* h2bdttt4 = new TH1F("h2bdttt4", "BDT tt for ZZtautaununu", 100, -1.0, 1.0);
  TH1F* h2bdttt5 = new TH1F("h2bdttt5", "BDT tt for ZZlltautau", 100, -1.0, 1.0);
  TH1F* h2bdttt6 = new TH1F("h2bdttt6", "BDT tt for ZW", 100, -1.0, 1.0);
  TH1F* h2bdttt7 = new TH1F("h2bdttt7", "BDT tt for Zincl", 100, -1.0, 1.0);

  TH1F* h2bdtWW0 = new TH1F("h2bdtWW0", "BDT WW for signal", 100, -1.0, 1.0);
  TH1F* h2bdtWW1 = new TH1F("h2bdtWW1", "BDT WW for ZZ", 100, -1.0, 1.0);
  TH1F* h2bdtWW2 = new TH1F("h2bdtWW2", "BDT WW for ttbar", 100, -1.0, 1.0);
  TH1F* h2bdtWW3 = new TH1F("h2bdtWW3", "BDT WW for WW", 100, -1.0, 1.0);
  // TH1F* h2bdtWW4 = new TH1F("h2bdtWW4", "BDT WW for ZZtautaununu", 100, -1.0, 1.0);
  TH1F* h2bdtWW5 = new TH1F("h2bdtWW5", "BDT WW for ZZlltautau", 100, -1.0, 1.0);
  TH1F* h2bdtWW6 = new TH1F("h2bdtWW6", "BDT WW for ZW", 100, -1.0, 1.0);
  TH1F* h2bdtWW7 = new TH1F("h2bdtWW7", "BDT WW for Zincl", 100, -1.0, 1.0);

  // TH1F* h2bdttn0 = new TH1F("h2bdttn0", "BDT tn for signal", 100, -1.0, 1.0);
  // TH1F* h2bdttn1 = new TH1F("h2bdttn1", "BDT tn for ZZ", 100, -1.0, 1.0);
  // TH1F* h2bdttn2 = new TH1F("h2bdttn2", "BDT tn for ttbar", 100, -1.0, 1.0);
  // TH1F* h2bdttn3 = new TH1F("h2bdttn3", "BDT tn for WW", 100, -1.0, 1.0);
  // TH1F* h2bdttn4 = new TH1F("h2bdttn4", "BDT tn for ZZtautaununu", 100, -1.0, 1.0);
  // TH1F* h2bdttn5 = new TH1F("h2bdttn5", "BDT tn for ZZlltautau", 100, -1.0, 1.0);
  // TH1F* h2bdttn6 = new TH1F("h2bdttn6", "BDT tn for ZW", 100, -1.0, 1.0);
  // TH1F* h2bdttn7 = new TH1F("h2bdttn7", "BDT tn for Zincl", 100, -1.0, 1.0);

  TH1F* h2bdtlt0 = new TH1F("h2bdtlt0", "BDT lt for signal", 100, -1.0, 1.0);
  TH1F* h2bdtlt1 = new TH1F("h2bdtlt1", "BDT lt for ZZ", 100, -1.0, 1.0);
  TH1F* h2bdtlt2 = new TH1F("h2bdtlt2", "BDT lt for ttbar", 100, -1.0, 1.0);
  TH1F* h2bdtlt3 = new TH1F("h2bdtlt3", "BDT lt for WW", 100, -1.0, 1.0);
  // TH1F* h2bdtlt4 = new TH1F("h2bdtlt4", "BDT lt for ZZtautaununu", 100, -1.0, 1.0);
  TH1F* h2bdtlt5 = new TH1F("h2bdtlt5", "BDT lt for ZZlltautau", 100, -1.0, 1.0);
  TH1F* h2bdtlt6 = new TH1F("h2bdtlt6", "BDT lt for ZW", 100, -1.0, 1.0);
  TH1F* h2bdtlt7 = new TH1F("h2bdtlt7", "BDT lt for Zincl", 100, -1.0, 1.0);

  TH1F* h2bdtZW0 = new TH1F("h2bdtZW0", "BDT ZW for signal", 100, -1.0, 1.0);
  TH1F* h2bdtZW1 = new TH1F("h2bdtZW1", "BDT ZW for ZZ", 100, -1.0, 1.0);
  TH1F* h2bdtZW2 = new TH1F("h2bdtZW2", "BDT ZW for ttbar", 100, -1.0, 1.0);
  TH1F* h2bdtZW3 = new TH1F("h2bdtZW3", "BDT ZW for WW", 100, -1.0, 1.0);
  // TH1F* h2bdtZW4 = new TH1F("h2bdtZW4", "BDT ZW for ZZtautaununu", 100, -1.0, 1.0);
  TH1F* h2bdtZW5 = new TH1F("h2bdtZW5", "BDT ZW for ZZlltautau", 100, -1.0, 1.0);
  TH1F* h2bdtZW6 = new TH1F("h2bdtZW6", "BDT ZW for ZW", 100, -1.0, 1.0);
  TH1F* h2bdtZW7 = new TH1F("h2bdtZW7", "BDT ZW for Zincl", 100, -1.0, 1.0);

  TH1F* h2bdtZi0 = new TH1F("h2bdtZi0", "BDT Zi for signal", 100, -1.0, 1.0);
  TH1F* h2bdtZi1 = new TH1F("h2bdtZi1", "BDT Zi for ZZ", 100, -1.0, 1.0);
  TH1F* h2bdtZi2 = new TH1F("h2bdtZi2", "BDT Zi for ttbar", 100, -1.0, 1.0);
  TH1F* h2bdtZi3 = new TH1F("h2bdtZi3", "BDT Zi for WW", 100, -1.0, 1.0);
  // TH1F* h2bdtZi4 = new TH1F("h2bdtZi4", "BDT Zi for ZZtautaununu", 100, -1.0, 1.0);
  TH1F* h2bdtZi5 = new TH1F("h2bdtZi5", "BDT Zi for ZZlltautau", 100, -1.0, 1.0);
  TH1F* h2bdtZi6 = new TH1F("h2bdtZi6", "BDT Zi for ZW", 100, -1.0, 1.0);
  TH1F* h2bdtZi7 = new TH1F("h2bdtZi7", "BDT Zi for Zincl", 100, -1.0, 1.0);

  hbdtZZ0->SetFillColor(kRed);
  hbdttt0->SetFillColor(kRed);
  hbdtWW0->SetFillColor(kRed);
  //  hbdttn0->SetFillColor(kRed);
  hbdtlt0->SetFillColor(kRed);
  hbdtZW0->SetFillColor(kRed);
  hbdtZi0->SetFillColor(kRed);

  hbdtZZ1->SetFillColor(kYellow);
  hbdttt1->SetFillColor(kYellow);
  hbdtWW1->SetFillColor(kYellow);
  //  hbdttn1->SetFillColor(kYellow);
  hbdtlt1->SetFillColor(kYellow);
  hbdtZW1->SetFillColor(kYellow);
  hbdtZi1->SetFillColor(kYellow);

  hbdtZZ2->SetFillColor(kBlue);
  hbdttt2->SetFillColor(kBlue);
  hbdtWW2->SetFillColor(kBlue);
  //  hbdttn2->SetFillColor(kBlue);
  hbdtlt2->SetFillColor(kBlue);
  hbdtZW2->SetFillColor(kBlue);
  hbdtZi2->SetFillColor(kBlue);

  hbdtZZ3->SetFillColor(kGreen);
  hbdttt3->SetFillColor(kGreen);
  hbdtWW3->SetFillColor(kGreen);
  //  hbdttn3->SetFillColor(kGreen);
  hbdtlt3->SetFillColor(kGreen);
  hbdtZW3->SetFillColor(kGreen);
  hbdtZi3->SetFillColor(kGreen);

  // hbdtZZ4->SetFillColor(kBlack);
  // hbdttt4->SetFillColor(kBlack);
  // hbdtWW4->SetFillColor(kBlack);
  // hbdttn4->SetFillColor(kBlack);
  // hbdtlt4->SetFillColor(kBlack);
  // hbdtZW4->SetFillColor(kBlack);
  // hbdtZi4->SetFillColor(kBlack);

  hbdtZZ5->SetFillColor(kWhite);
  hbdttt5->SetFillColor(kWhite);
  hbdtWW5->SetFillColor(kWhite);
  hbdtZW5->SetFillColor(kWhite);
  hbdtZi5->SetFillColor(kWhite);
  //  hbdttn5->SetFillColor(kWhite);
  hbdtlt5->SetFillColor(kWhite);

  hbdtZZ6->SetFillColor(kMagenta);
  hbdttt6->SetFillColor(kMagenta);
  hbdtWW6->SetFillColor(kMagenta);
  hbdtZW6->SetFillColor(kMagenta);
  hbdtZi6->SetFillColor(kMagenta);
  //  hbdttn6->SetFillColor(kMagenta);
  hbdtlt6->SetFillColor(kMagenta);

  hbdtZZ7->SetFillColor(kBlack);
  hbdttt7->SetFillColor(kBlack);
  hbdtWW7->SetFillColor(kBlack);
  hbdtZW7->SetFillColor(kBlack);
  hbdtZi7->SetFillColor(kBlack);
  //  hbdttn7->SetFillColor(kBlack);
  hbdtlt7->SetFillColor(kBlack);
 
  h2bdtZZ0->SetFillColor(kRed);
  h2bdttt0->SetFillColor(kRed);
  h2bdtWW0->SetFillColor(kRed);
  //  h2bdttn0->SetFillColor(kRed);
  h2bdtlt0->SetFillColor(kRed);
  h2bdtZW0->SetFillColor(kRed);
  h2bdtZi0->SetFillColor(kRed);
 
  h2bdtZZ1->SetFillColor(kYellow);
  h2bdttt1->SetFillColor(kYellow);
  h2bdtWW1->SetFillColor(kYellow);
  //  h2bdttn1->SetFillColor(kYellow);
  h2bdtlt1->SetFillColor(kYellow);
  h2bdtZW1->SetFillColor(kYellow);
  h2bdtZi1->SetFillColor(kYellow);

  h2bdtZZ2->SetFillColor(kBlue);
  h2bdttt2->SetFillColor(kBlue);
  h2bdtWW2->SetFillColor(kBlue);
  //  h2bdttn2->SetFillColor(kBlue);
  h2bdtlt2->SetFillColor(kBlue);
  h2bdtZW2->SetFillColor(kBlue);
  h2bdtZi2->SetFillColor(kBlue);

  h2bdtZZ3->SetFillColor(kGreen);
  h2bdttt3->SetFillColor(kGreen);
  h2bdtWW3->SetFillColor(kGreen);
  //  h2bdttn3->SetFillColor(kGreen);
  h2bdtlt3->SetFillColor(kGreen);
  h2bdtZW3->SetFillColor(kGreen);
  h2bdtZi3->SetFillColor(kGreen);

  // h2bdtZZ4->SetFillColor(kBlack);
  // h2bdttt4->SetFillColor(kBlack);
  // h2bdtWW4->SetFillColor(kBlack);
  // h2bdttn4->SetFillColor(kBlack);
  // h2bdtlt4->SetFillColor(kBlack);
  // h2bdtZW4->SetFillColor(kBlack);
  // h2bdtZi4->SetFillColor(kBlack);

  h2bdtZZ5->SetFillColor(kWhite);
  h2bdttt5->SetFillColor(kWhite);
  h2bdtWW5->SetFillColor(kWhite);
  //  h2bdttn5->SetFillColor(kWhite);
  h2bdtlt5->SetFillColor(kWhite);
  h2bdtZW5->SetFillColor(kWhite);
  h2bdtZi5->SetFillColor(kWhite);

  h2bdtZZ6->SetFillColor(kMagenta);
  h2bdttt6->SetFillColor(kMagenta);
  h2bdtWW6->SetFillColor(kMagenta);
  //  h2bdttn6->SetFillColor(kMagenta);
  h2bdtlt6->SetFillColor(kMagenta);
  h2bdtZW6->SetFillColor(kMagenta);
  h2bdtZi6->SetFillColor(kMagenta);

  h2bdtZZ7->SetFillColor(kBlack);
  h2bdttt7->SetFillColor(kBlack);
  h2bdtWW7->SetFillColor(kBlack);
  //  h2bdttn7->SetFillColor(kBlack);
  h2bdtlt7->SetFillColor(kBlack);
  h2bdtZW7->SetFillColor(kBlack);
  h2bdtZi7->SetFillColor(kBlack);

  Float_t bdtZZ = 0.0;
  Float_t bdttt = 0.0;
  Float_t bdtWW = 0.0;
  Float_t bdttn = 0.0;
  Float_t bdtlt = 0.0;
  Float_t bdtZW = 0.0;
  Float_t bdtZi = 0.0;
  Float_t w = 0.0;

  Int_t runnum;
  Int_t evtnum, type;

  //  int lepID, nel, nmu, nlep;
  //  float ejet1, ejet2, ejet3, ptlep1, ptlep2, cone1, cone2;

  float massZ;
  float cosJetMET;
  float cosll2d;
  float cosll3d;
  float mT;
  float MET;
  float cosMETptlep;
  float cosphi;

  Int_t nb=0;
  float portion;
  bool last_rec=0;

  //              - - - Getting tree from the disk - - - 

  // We define the chain, which is basically a tree to add root files with the same tree name.
  // If you have two or more files, just keep going as nfchain += chain->Add("filename.root")

#ifdef CHAIN

  TChain* chain = new TChain("finalTree");
  Int_t nfchain=0;

  //  nfchain += chain->Add("/data/TMVA/final/final-HZ110.root");
  //  nfchain += chain->Add("/data/TMVA/final/final-HZ120.root");
  //  nfchain += chain->Add("/data/TMVA/final/final-HZ130-3jun.root");
    nfchain += chain->Add("/data/TMVA/final/final-HZ140-3jun.root");
  //  nfchain += chain->Add("/data/TMVA/final/final-HZ150.root");
  //  nfchain += chain->Add("/data/TMVA/final/final-HZ200.root");
  //  nfchain += chain->Add("/data/TMVA/final/final-HZ250.root");

  chain->SetBranchAddress("runnum", &runnum);
  chain->SetBranchAddress("evtnum", &evtnum);
  chain->SetBranchAddress("type",  &type);   
  chain->SetBranchAddress("bdtZZ", &bdtZZ);
  chain->SetBranchAddress("bdttt", &bdttt);
  chain->SetBranchAddress("bdtWW", &bdtWW);
  chain->SetBranchAddress("bdttn", &bdttn);
  chain->SetBranchAddress("bdtlt", &bdtlt);
  chain->SetBranchAddress("bdtZW", &bdtZW);
  chain->SetBranchAddress("bdtZi", &bdtZi);
  chain->SetBranchAddress("massZ",&massZ);	         
  chain->SetBranchAddress("cosll2d",&cosll2d);	         
  chain->SetBranchAddress("cosll3d",&cosll3d);	         
  chain->SetBranchAddress("cosphi",&cosphi);	         
  chain->SetBranchAddress("mT",&mT);	         
  chain->SetBranchAddress("MET",&MET);	         
  chain->SetBranchAddress("cosMETptlep",&cosMETptlep);	         
  chain->SetBranchAddress("cosJetMET",&cosJetMET);	         
  chain->SetBranchAddress("w", &w);

//     chain->SetBranchAddress( "njet", &njet );
//     chain->SetBranchAddress( "ejet1", &ejet1 );
//     chain->SetBranchAddress( "ejet2", &ejet2 );
//     chain->SetBranchAddress( "ejet3", &ejet3 );
//     chain->SetBranchAddress( "ptlep1", &ptlep1 );
//     chain->SetBranchAddress( "ptlep2", &ptlep2 );
//     chain->SetBranchAddress( "cone1", &cone1 );
//     chain->SetBranchAddress( "cone2", &cone2 );
//     chain->SetBranchAddress( "nel", &nel );
//     chain->SetBranchAddress( "nmu", &nmu );
//     chain->SetBranchAddress( "nlep", &nlep );
//     chain->SetBranchAddress( "lepID", &lepID );
//     chain->SetBranchAddress( "massZ", &massZ );
//     chain->SetBranchAddress( "cosll2d", &cosll2d );
//     chain->SetBranchAddress( "cosll3d", &cosll3d );
//     chain->SetBranchAddress( "mT", &mT );
//     chain->SetBranchAddress( "MET", &MET );
//     chain->SetBranchAddress( "cosMETptlep", &cosMETptlep);
//     chain->SetBranchAddress( "cosJetMET", &cosJetMET);
//     chain->SetBranchAddress( "cosphi", &cosphi );
//     chain->SetBranchAddress( "runnum", &runnum );
//     chain->SetBranchAddress( "evtnum", &evtnum );
//     chain->SetBranchAddress( "type", &type );
//     chain->SetBranchAddress( "w", &w );

  // Arrays in general have their address at the beginning, so no need for "&"

  Int_t nrec = static_cast<int>(0.5 + chain->GetEntries());
  Int_t nticker= static_cast<int>(0.5 + nrec/20.0);
  cout << nrec << " records found in the chain. "<< endl;

  // The above is to find the number of events or entries

#endif // CHAIN 

  // ============  Starting the main loop over the events  ===============

  for (Int_t i=0; i<nrec; i++) {        
    if(div(i, nticker).rem == 0 || i >= nrec-1) // this section is just for fun
      { 
	portion=static_cast<float>(i) / static_cast<float>(nrec-1);
	printf("Done %4.1f percent of reading the NTuple  ", portion*100.0);
	cout.flush();   cout << char(13);  
      }

    last_rec=(i>=nrec-1); 

    if(!last_rec)
      {
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

    // only take every other event, rejecting the half of events used in training
    if ((i%2) != 0) {

      // store the weight for each event type
      weight[type] = w;

      if (type == 0) {
	hbdtZZ0->Fill(bdtZZ,weight[type]);  
	hbdttt0->Fill(bdttt,weight[type]);  
	hbdtWW0->Fill(bdtWW,weight[type]);  
	//  hbdttn0->Fill(bdttn,weight[type]);  
	hbdtlt0->Fill(bdtlt,weight[type]);  
	hbdtZW0->Fill(bdtZW,weight[type]);  
	hbdtZi0->Fill(bdtZi,weight[type]);  
      }

      if (type ==  1) {
	hbdtZZ1->Fill(bdtZZ,weight[type]);  
	hbdttt1->Fill(bdttt,weight[type]);  
	hbdtWW1->Fill(bdtWW,weight[type]);  
	//  hbdttn1->Fill(bdttn,weight[type]);  
	hbdtlt1->Fill(bdtlt,weight[type]);  
	hbdtZW1->Fill(bdtZW,weight[type]);  
	hbdtZi1->Fill(bdtZi,weight[type]);  
      }

      if (type ==  2) {
	hbdtZZ2->Fill(bdtZZ,weight[type]);  
	hbdttt2->Fill(bdttt,weight[type]);  
	hbdtWW2->Fill(bdtWW,weight[type]);  
	//  hbdttn2->Fill(bdttn,weight[type]);  
	hbdtlt2->Fill(bdtlt,weight[type]);  
	hbdtZW2->Fill(bdtZW,weight[type]);  
	hbdtZi2->Fill(bdtZi,weight[type]);  
      }

      if (type ==  3) {
	hbdtZZ3->Fill(bdtZZ,weight[type]);  
	hbdttt3->Fill(bdttt,weight[type]);  
	hbdtWW3->Fill(bdtWW,weight[type]);  
	//  hbdttn3->Fill(bdttn,weight[type]);  
	hbdtlt3->Fill(bdtlt,weight[type]);  
	hbdtZW3->Fill(bdtZW,weight[type]);  
	hbdtZi3->Fill(bdtZi,weight[type]);  
      }

      //  if (type ==  4) {
      //    hbdtZZ4->Fill(bdtZZ,weight[type]);  
      //    hbdttt4->Fill(bdttt,weight[type]);  
      //    hbdtWW4->Fill(bdtWW,weight[type]);  
      //    hbdttn4->Fill(bdttn,weight[type]);  
      //    hbdtlt4->Fill(bdtlt,weight[type]);  
      //    hbdtZW4->Fill(bdtZW,weight[type]);  
      //    hbdtZi4->Fill(bdtZi,weight[type]);  
      //  }

      if (type == 5) {
	hbdtZZ5->Fill(bdtZZ,weight[type]);  
	hbdttt5->Fill(bdttt,weight[type]);  
	hbdtWW5->Fill(bdtWW,weight[type]);  
	//  hbdttn5->Fill(bdttn,weight[type]);  
	hbdtlt5->Fill(bdtlt,weight[type]);  
	hbdtZW5->Fill(bdtZW,weight[type]);  
	hbdtZi5->Fill(bdtZi,weight[type]);  
      }

      if (type == 6) {
	hbdtZZ6->Fill(bdtZZ,weight[type]);  
	hbdttt6->Fill(bdttt,weight[type]);  
	hbdtWW6->Fill(bdtWW,weight[type]);  
	//  hbdttn6->Fill(bdttn,weight[type]);  
	hbdtlt6->Fill(bdtlt,weight[type]);  
	hbdtZW6->Fill(bdtZW,weight[type]);  
	hbdtZi6->Fill(bdtZi,weight[type]);  
      }

      if (type == 7) {
	hbdtZZ7->Fill(bdtZZ,weight[type]);  
	hbdttt7->Fill(bdttt,weight[type]);  
	hbdtWW7->Fill(bdtWW,weight[type]);  
	//  hbdttn7->Fill(bdttn,weight[type]);  
	hbdtlt7->Fill(bdtlt,weight[type]);  
	hbdtZW7->Fill(bdtZW,weight[type]);  
	hbdtZi7->Fill(bdtZi,weight[type]);  
      }

      if (bdtZZ > minCutValue[0] && bdttt > minCutValue[1] && bdtWW > minCutValue[2] && bdttn > minCutValue[3] && bdtlt > minCutValue[4] 
	  && bdtZW > minCutValue[5] && bdtZi > minCutValue[6]){

	if (type == 0) {
	  h2bdtZZ0->Fill(bdtZZ,weight[type]);  
	  h2bdttt0->Fill(bdttt,weight[type]);  
	  h2bdtWW0->Fill(bdtWW,weight[type]);  
	  //  h2bdttn0->Fill(bdttn,weight[type]);  
	  h2bdtlt0->Fill(bdtlt,weight[type]);  
	  h2bdtZW0->Fill(bdtZW,weight[type]);  
	  h2bdtZi0->Fill(bdtZi,weight[type]);  
	}

	if (type == 1) {
	  h2bdtZZ1->Fill(bdtZZ,weight[type]);  
	  h2bdttt1->Fill(bdttt,weight[type]);  
	  h2bdtWW1->Fill(bdtWW,weight[type]);  
	  //  h2bdttn1->Fill(bdttn,weight[type]);  
	  h2bdtlt1->Fill(bdtlt,weight[type]);  
	  h2bdtZW1->Fill(bdtZW,weight[type]);  
	  h2bdtZi1->Fill(bdtZi,weight[type]);  
	}

	if (type == 2) {
	  h2bdtZZ2->Fill(bdtZZ,weight[type]);  
	  h2bdttt2->Fill(bdttt,weight[type]);  
	  h2bdtWW2->Fill(bdtWW,weight[type]);  
	  //  h2bdttn2->Fill(bdttn,weight[type]);  
	  h2bdtlt2->Fill(bdtlt,weight[type]);  
	  h2bdtZW2->Fill(bdtZW,weight[type]);  
	  h2bdtZi2->Fill(bdtZi,weight[type]);  
	}

	if (type == 3) {
	  h2bdtZZ3->Fill(bdtZZ,weight[type]);  
	  h2bdttt3->Fill(bdttt,weight[type]);  
	  h2bdtWW3->Fill(bdtWW,weight[type]);  
	  //  h2bdttn3->Fill(bdttn,weight[type]);  
	  h2bdtlt3->Fill(bdtlt,weight[type]);  
	  h2bdtZW3->Fill(bdtZW,weight[type]);  
	  h2bdtZi3->Fill(bdtZi,weight[type]);  
	}

	//    if (type == 4) {
	//      h2bdtZZ4->Fill(bdtZZ,weight[type]);  
	//      h2bdttt4->Fill(bdttt,weight[type]);  
	//      h2bdtWW4->Fill(bdtWW,weight[type]);  
	//      h2bdttn4->Fill(bdttn,weight[type]);  
	//      h2bdtlt4->Fill(bdtlt,weight[type]);  
	//      h2bdtZW4->Fill(bdtZW,weight[type]);  
	//      h2bdtZi4->Fill(bdtZi,weight[type]);  
	//    }
	if (type == 5) {
	  h2bdtZZ5->Fill(bdtZZ,weight[type]);  
	  h2bdttt5->Fill(bdttt,weight[type]);  
	  h2bdtWW5->Fill(bdtWW,weight[type]);  
	  //  h2bdttn5->Fill(bdttn,weight[type]);  
	  h2bdtlt5->Fill(bdtlt,weight[type]);  
	  h2bdtZW5->Fill(bdtZW,weight[type]);  
	  h2bdtZi5->Fill(bdtZi,weight[type]);  
	}
	if (type == 6) {
	  h2bdtZZ6->Fill(bdtZZ,weight[type]);  
	  h2bdttt6->Fill(bdttt,weight[type]);  
	  h2bdtWW6->Fill(bdtWW,weight[type]);  
	  //  h2bdttn6->Fill(bdttn,weight[type]);  
	  h2bdtlt6->Fill(bdtlt,weight[type]);  
	  h2bdtZW6->Fill(bdtZW,weight[type]);  
	  h2bdtZi6->Fill(bdtZi,weight[type]);  
	}
	if (type == 7) {
	  h2bdtZZ7->Fill(bdtZZ,weight[type]);  
	  h2bdttt7->Fill(bdttt,weight[type]);  
	  h2bdtWW7->Fill(bdtWW,weight[type]);  
	  //  h2bdttn7->Fill(bdttn,weight[type]);  
	  h2bdtlt7->Fill(bdtlt,weight[type]);  
	  h2bdtZW7->Fill(bdtZW,weight[type]);  
	  h2bdtZi7->Fill(bdtZi,weight[type]);  
	}

	// fill in the input variables after cuts
	hcosll2D[type]->Fill(cosll2d,weight[type]);
	hmt[type]->Fill(mT,weight[type]);
	hmZll[type]->Fill(massZ,weight[type]);
	hcosphi[type]->Fill(cosphi,weight[type]);

	hcosll3D[type]->Fill(cosll3d,weight[type]);
	hMET[type]->Fill(MET,weight[type]);
	hcosMETPtlep[type]->Fill(cosMETptlep,weight[type]);
	hcosJetMET[type]->Fill(cosJetMET,weight[type]);
      } // end if on cut values for each set of weights

      mvaBdt[0] = bdtZZ;
      mvaBdt[1] = bdttt;
      mvaBdt[2] = bdtWW;
      mvaBdt[3] = bdttn;
      mvaBdt[4] = bdtlt;
      mvaBdt[5] = bdtZW;
      mvaBdt[6] = bdtZi;

      int success = 1;
     
      for (int i = 0; i < nbgndw; i++) {
	passed[i] = 0;
	//   if (mvaBdt[i] > minCutValue[i]  && mvaBdt[i] < maxCutValue[i]) {passed[i] = 1;}
	if (mvaBdt[i] > minCutValue[i]) {passed[i] = 1;}
	success *= passed[i];
	if (passed[i] == 0.0) failed[type][i] += 2.0;
      }
  
      // count how many events pass all selections; count each event as 2 since only half the events are used
      if (success == 1) {
	Total[type] += 2.0*weight[type];
	selected[type] += 2.0;
	if (type == 0) NSignal += 2.0;
	if (type >  0) NBgnd += 2.0;
      }
 
      if (success == 0) {
	rejected[type] += 2.0;
      }
 
    } // end of if loop keeping only every other event; reject half the events used in the training
  } // end of for loop over events   

  places = 1;
  cout <<fixed;                 
  cout.precision(places);
   
  // print out the cut values used for each background
  cout << " " << endl;	  
  outClientFile << " " << endl;	  

  places = 2;
  cout <<fixed;                 
  cout.precision(places);

  // loop over all types of events to get the efficiencies; 

  for (int type=1; type<ntype; type++) {
    TotalBgnd += Total[type];
    //    PTotalBgnd += PTotal[type];
    //    STotalBgnd += STotal[type];
  }

  for (int type=0; type<ntype; type++) {

    if (preselected[type]>0) {
      efficiency[type] =  selected[type]/preselected[type];
    }

    else {cout << "preselected = 0 for type " << type << endl;}

    //   cout << efficiency[type] << "\t" <<selected[type] << "\t" << preselected[type] << endl;
    rejection[type] = 1.0 - efficiency[type];

    places = 1;
    cout <<fixed;                 
    cout.precision(places); 
    outClientFile <<fixed;                 
    outClientFile.precision(places);

    cout << "Type of event: " << eventType[type] << endl;	  
    cout << "# events preselected: " << preselected[type] << ", # selected: " << selected[type] 
	 << " with efficiency in % " << 100.0*efficiency[type] << endl;
    cout << "# events selected after reweighting: " << Total[type] << endl;	  
    cout << "   " << endl;	  
  }

  // get info on S/sqrt(B) and display it after doubling the numbers since only half of the events were used
  // use reweighted number
 
  float TotalBgnd0 = 0.0;
  for (int j=1; j<ntype; j++) {TotalBgnd0 += weight[j]*preselected[j];}

  Float_t xi2;
  //  if (Total[0] > 0.0) xi2 = 100.0*2.33*sqrt(0.071*TotalBgnd*0.071*TotalBgnd+TotalBgnd)/Total[0];  //optimize xi2 - 99% CL
  if (Total[0] > 0.0) xi2 = 100.0*1.64*sqrt(0.071*TotalBgnd*0.071*TotalBgnd+TotalBgnd)/Total[0];  //optimize xi2 - 95% CL
  Float_t sigmaB = sqrt(0.071*TotalBgnd*0.071*TotalBgnd+TotalBgnd);
  Float_t SoverB = Total[0]/sqrt(TotalBgnd);
  //  Float_t SoverB0 = weight[0]*preselected[0]/sqrt(TotalBgnd0);
  Float_t SoverSB = Total[0]/sqrt(TotalBgnd+Total[0]);
  //  Float_t SoverSB0 = weight[0]*preselected[0]/sqrt(TotalBgnd0+weight[0]*preselected[0]);
  cout << " " << endl;

  cout << "Number of files used         = " << ntype  << endl;
  for(int type=0; type<ntype; type++) { cout << "\t" << eventType[type] << endl;}

  places = 2;
  cout <<fixed;                 
  cout.precision(places); 
  outClientFile <<fixed;                 
  outClientFile.precision(places);

  for (int j=0; j<nbgndw; j++) {
    cout << "set min cut value against " << suffix[j] << " background: " << minCutValue[j] << endl;
    //   cout << "set max cut value against " << suffix[j] << " background: " << maxCutValue[j] << endl;
    outClientFile << "set min cut value against " << suffix[j] << " background: " << minCutValue[j] << endl;
    //   outClientFile << "set max cut value against " << suffix[j] << " background: " << maxCutValue[j] << endl;
  }

  places = 2;
  cout <<fixed;                 
  cout.precision(places); 
  outClientFile <<fixed;                 
  outClientFile.precision(places);
  //  float null = 0.0;

  cout << "*************************************************************************** " << endl;
  cout << "*    Total signal @ 30fb-1 with cuts = " << Total[0]  << endl;
  cout << "*    Total background @ 30fb-1       = " << TotalBgnd << "\t" << Total[1] << "\t" 
       << Total[2] << "\t" << Total[3] << "\t" << Total[4] << "\t" << Total[5] << "\t" << Total[6] << "\t" << Total[7] << endl;
  cout << "*    Total raw samples               = " << selected[0] << "\t" << selected[1] << "\t" 
       << selected[2] << "\t" << selected[3] << "\t" << selected[4] << "\t" << selected[5] << "\t" << selected[6] << "\t" << selected[7] << endl;
  cout << "*    xi2:                                  = " << xi2 << endl;
  cout << "*    sigmaB                                = " << sigmaB << endl;
  cout << "*    Signal over sqrt(B) @ 30fb-1          = " << SoverB << endl;
  cout << "*    Signal over sqrt(S+B) @ 30fb-1        = " << SoverSB << endl;
  cout << "* " << endl;
  cout << "*    Total preselected signal @ 30fb-1     = " << weight[0]*preselected[0]  << endl;
  cout << "*    Total preselected background @ 30fb-1 = " << TotalBgnd0 << "\t" << weight[1]*preselected[1] << "\t" 
       << weight[2]*preselected[2] << "\t" <<weight[3]*preselected[3] << "\t" << weight[4]*preselected[4] << "\t" << weight[5]*preselected[5] 
       << "\t" << weight[6]*preselected[6] << "\t" << weight[7]*preselected[7] << endl;
  cout << "*    Total raw preselected samples         = " << preselected[0] << "\t" << preselected[1] << "\t" << preselected[2] << "\t" 
       <<preselected[3] << "\t" << preselected[4] << "\t" << preselected[5] << "\t" << preselected[7] << "\t" << preselected[7] << endl;
  cout << "*************************************************************************** " << endl;

  cout << "*************************************************************************** " << endl;
  cout << "*    Total x-section in fb-1 with cuts = " << Total[0]/30.  << endl;
  cout << "*    Total background @ x-section in fb-1       = " << TotalBgnd/30. << "\t" << Total[1]/30. << "\t" 
       << Total[2]/30. << "\t" << Total[3]/30. << "\t" << Total[4]/30. << "\t" << Total[5]/30. << "\t" << Total[6]/30. << "\t" << Total[7]/30. << endl;
  cout << "*************************************************************************** " << endl;

  outClientFile << "*************************************************************************** " << endl;
  outClientFile << "*    Total signal @ 30fb-1 with cuts = " << Total[0]  << endl;
  outClientFile << "*    Total background @ 30fb-1       = " << TotalBgnd << "\t" << Total[1] << "\t" 
		<< Total[2] << "\t" << Total[3] << "\t" << Total[4] << "\t" << Total[5] << "\t" << Total[6] << "\t" << Total[7] << endl;
  outClientFile << "*    Total raw samples               = " << selected[0] << "\t" << selected[1] << "\t" 
		<< selected[2] << "\t" << selected[3] << "\t" << selected[4] << "\t" << selected[5] << "\t" << selected[6] << "\t" << selected[7] << endl;
  outClientFile << "*    xi2:                            = " << xi2 << endl;
  outClientFile << "*    sigmaB                          = " << sigmaB << endl;
  outClientFile << "*    Signal over sqrt(B) @ 30fb-1    = " << SoverB << endl;
  outClientFile << "*    Signal over sqrt(S+B) @ 30fb-1  = " << SoverSB << endl;
  outClientFile << "* " << endl;
  outClientFile << "*    Total preselected signal @ 30fb-1     = " << weight[0]*preselected[0]  << endl;
  outClientFile << "*    Total preselected background @ 30fb-1 = " << TotalBgnd0 << "\t" << weight[1]*preselected[1] << "\t" 
		<< weight[2]*preselected[2] << "\t" <<weight[3]*preselected[3] << "\t" << weight[4]*preselected[4] << "\t" << weight[5]*preselected[5] 
		<< "\t" << weight[6]*preselected[6] << "\t" << weight[7]*preselected[7] << endl;
  outClientFile << "*    Total raw preselected samples         = " << preselected[0] << "\t" << preselected[1] << "\t" 
		<< preselected[2] << "\t" <<preselected[3] << "\t" << preselected[4] << "\t" << preselected[5] << "\t" << preselected[6] << "\t" << preselected[7] << endl;
  outClientFile << "*************************************************************************** " << endl;

  outClientFile << "*************************************************************************** " << endl;
  outClientFile << "*    Total x-section in fb-1 with cuts = " << Total[0]/30.  << endl;
  outClientFile << "*    Total background @ x-section in fb-1       = " << TotalBgnd/30. << "\t" << Total[1]/30. << "\t" 
		<< Total[2]/30. << "\t" << Total[3]/30. << "\t" << Total[4]/30. << "\t" << Total[5]/30. << "\t" << Total[6]/30. << "\t" << Total[7]/30. << endl;
  outClientFile << "*************************************************************************** " << endl;
 
  outClientFile << endl;

  // done with statistics; display the histograms

  hs1.SetMinimum(0.01); 
  hs2.SetMinimum(0.01); 
  hs3.SetMinimum(0.01); 
  hs4.SetMinimum(0.01); 

  hs5.SetMinimum(0.01); 
  hs6.SetMinimum(0.01); 
  hs7.SetMinimum(0.01); 

  hs21.SetMinimum(0.01); 
  hs22.SetMinimum(0.01); 
  hs23.SetMinimum(0.01); 
  hs24.SetMinimum(0.01); 

  hs25.SetMinimum(0.01); 
  hs26.SetMinimum(0.01); 
  hs27.SetMinimum(0.01); 
  hs28.SetMinimum(0.01); 

  hs11.SetMinimum(0.01); 
  hs12.SetMinimum(0.01); 
  hs13.SetMinimum(0.01); 
  hs15.SetMinimum(0.01); 
  hs16.SetMinimum(0.01); 
  hs17.SetMinimum(0.01); 


  c1->cd();
  hs11.Add(hbdtZZ0);
  pad16->cd();
  hs11.Draw();
  leg11->AddEntry(hbdtZZ0, "HZ", "f"); 
  hs11.Add(hbdtZZ1);
  hs11.Draw();
  leg11->AddEntry(hbdtZZ1, "ZZ #rightarrow l l #nu #nu", "f"); 
  hs11.Add(hbdtZZ2);
  hs11.Draw();
  leg11->AddEntry(hbdtZZ2,  "t#bar t", "f"); 
  hs11.Add(hbdtZZ3);
  hs11.Draw();
  leg11->AddEntry(hbdtZZ3,"WW", "f");
  leg11->Draw();
  //    hs11.Add(hbdtZZ4);
  //    hs11.Draw();
  //    leg11->AddEntry(hbdtZZ4,"ZZ #rightarrow #tau #tau #nu #nu", "f");
  //    leg11->Draw();
  hs11.Add(hbdtZZ5);
  hs11.Draw();
  leg11->AddEntry(hbdtZZ5,"ZZ #rightarrow l l #tau #tau", "f");
  leg11->Draw();
  hs11.Add(hbdtZZ6);
  hs11.Draw();
  leg11->AddEntry(hbdtZZ6,"ZW", "f");
  leg11->Draw();
  hs11.Add(hbdtZZ7);
  hs11.Draw();
  leg11->AddEntry(hbdtZZ7,"Z+jets", "f");
  leg11->Draw();

  hs12.Add(hbdttt0);
  pad15->cd();
  hs12.Draw();
  hs12.Add(hbdttt1);
  hs12.Draw();
  hs12.Add(hbdttt2);
  hs12.Draw();
  hs12.Add(hbdttt3);
  hs12.Draw();
  //    hs12.Add(hbdttt4);
  //    hs12.Draw();
  hs12.Add(hbdttt5);
  hs12.Draw();
  hs12.Add(hbdttt6);
  hs12.Draw();
  hs12.Add(hbdttt7);
  hs12.Draw();

  TLine* line1 = new TLine(minCutValue[1],0.0,minCutValue[1],34.0);
  line1->SetLineColor(1);
  line1->SetLineWidth(2);
  line1->SetLineStyle(2);
  line1->Draw();

  hs13.Add(hbdtWW0);
  pad14->cd();
  hs13.Draw();
  hs13.Add(hbdtWW1);
  hs13.Draw();
  hs13.Add(hbdtWW2);
  hs13.Draw();
  hs13.Add(hbdtWW3);
  hs13.Draw();
  //    hs13.Add(hbdtWW4);
  //    hs13.Draw();
  hs13.Add(hbdtWW5);
  hs13.Draw();
  hs13.Add(hbdtWW6);
  hs13.Draw();
  hs13.Add(hbdtWW7);
  hs13.Draw();

  TLine* line2 = new TLine(minCutValue[2],0.0,minCutValue[2],34.0);
  line2->SetLineColor(1);
  line2->SetLineWidth(2);
  line2->SetLineStyle(2);
  line2->Draw();

  hs15.Add(hbdtlt0);
  pad13->cd();
  hs15.Draw();
  hs15.Add(hbdtlt1);
  hs15.Draw();
  hs15.Add(hbdtlt2);
  hs15.Draw();
  hs15.Add(hbdtlt3);
  hs15.Draw();
  //    hs15.Add(hbdtlt4);
  //    hs15.Draw();
  hs15.Add(hbdtlt5);
  hs15.Draw();
  hs15.Add(hbdtlt6);
  hs15.Draw();
  hs15.Add(hbdtlt7);
  hs15.Draw();

  TLine* line4 = new TLine(minCutValue[4],0.0,minCutValue[4],42.0);
  line4->SetLineColor(1);
  line4->SetLineWidth(2);
  line4->SetLineStyle(2);
  line4->Draw();

  hs16.Add(hbdtZW0);
  pad12->cd();
  hs16.Draw();
  hs16.Add(hbdtZW1);
  hs16.Draw();
  hs16.Add(hbdtZW2);
  hs16.Draw();
  hs16.Add(hbdtZW3);
  hs16.Draw();
  //    hs16.Add(hbdtZW4);
  //    hs16.Draw();
  hs16.Add(hbdtZW5);
  hs16.Draw();
  hs16.Add(hbdtZW6);
  hs16.Draw();
  hs16.Add(hbdtZW7);
  hs16.Draw();

  TLine* line5 = new TLine(minCutValue[5],0.0,minCutValue[5],32.0);
  line5->SetLineColor(1);
  line5->SetLineWidth(2);
  line5->SetLineStyle(2);
  line5->Draw();

  hs17.Add(hbdtZi0);
  pad11->cd();
  hs17.Draw();
  hs17.Add(hbdtZi1);
  hs17.Draw();
  hs17.Add(hbdtZi2);
  hs17.Draw();
  hs17.Add(hbdtZi3);
  hs17.Draw();
  //    hs17.Add(hbdtZi4);
  //    hs17.Draw();
  hs17.Add(hbdtZi5);
  hs17.Draw();
  hs17.Add(hbdtZi6);
  hs17.Draw();
  hs17.Add(hbdtZi7);
  hs17.Draw();

  TLine* line6 = new TLine(minCutValue[6],0.0,minCutValue[6],52.0);
  line6->SetLineColor(1);
  line6->SetLineWidth(2);
  line6->SetLineStyle(2);
  line6->Draw();

  //==========================================


  for (int m=0; m<ntype; m++) {
    c3->cd();
    hs21.Add(hmt[m]);
    pad33->cd(); 
    hs21.Draw("");
    hs21.Draw("");
    if (m==0) {leg3->AddEntry(hmt[m], "  HZ", "f"); }
    if (m==1) {leg3->AddEntry(hmt[m], "  ZZ #rightarrow l l #nu #nu", "f"); }
    if (m==2) {leg3->AddEntry(hmt[m], "  t#bar t", "f");} 
    if (m==3) {leg3->AddEntry(hmt[m], "  WW", "f"); }
    if (m==5) {leg3->AddEntry(hmt[m], "  ZZ #rightarrow l l #tau #tau", "f"); }
    if (m==6) {leg3->AddEntry(hmt[m], "  ZW", "f"); }
    if (m==7) {leg3->AddEntry(hmt[m], "  Z + jets", "f");} 
    leg3->Draw();
    
    hs22.Add(hmZll[m]);
    pad34->cd(); 
    hs22.Draw("");
    hs22.Draw("");
    
    hs23.Add(hcosll2D[m]);
    pad31->cd(); 
    //    pad31->SetLogy(1);
    hs23.Draw("");
    hs23.Draw("");
    
    hs24.Add(hcosphi[m]);
    pad32->cd(); 
    pad32->SetLogy(1);
    hs24.Draw("");
    hs24.Draw("");

    c4->cd();
    hs25.Add(hMET[m]);
    pad43->cd(); 
    hs25.Draw("");
    hs25.Draw("");
    if (m==0) {leg4->AddEntry(hMET[m], "  HZ", "f"); }
    if (m==1) {leg4->AddEntry(hMET[m], "  ZZ #rightarrow l l #nu #nu", "f"); }
    if (m==2) {leg4->AddEntry(hMET[m], "  t#bar t", "f");} 
    if (m==3) {leg4->AddEntry(hMET[m], "  WW", "f"); }
    if (m==5) {leg4->AddEntry(hMET[m], "  ZZ #rightarrow l l #tau #tau", "f"); }
    if (m==6) {leg4->AddEntry(hMET[m], "  ZW", "f"); }
    if (m==7) {leg4->AddEntry(hMET[m], "  Z + jets", "f");} 
    leg4->Draw();
    
    hs26.Add(hcosll3D[m]);
    pad44->cd(); 
    hs26.Draw("");
    hs26.Draw("");
    
//     hs27.Add(hcosJetMET[m]);
//     pad41->cd(); 
//     pad41->SetLogy(1);
//     hs27.Draw("");
//     hs27.Draw("");
    
    hs28.Add(hcosMETPtlep[m]);
    pad42->cd(); 
    pad42->SetLogy(1);
    hs28.Draw("");
    hs28.Draw("");

//      c2->cd();
//      hs1.Add(h2bdtZZ0);
//      pad26->cd();
//      hs1.Draw();
//      leg1->AddEntry(h2bdtZZ0, "HZ", "f"); 
//      hs1.Add(h2bdtZZ1);
//      hs1.Draw();
//      leg1->AddEntry(h2bdtZZ1, "ZZ #rightarrow l l #nu #nu", "f"); 
//      hs1.Add(h2bdtZZ2);
//      hs1.Draw();
//      leg1->AddEntry(h2bdtZZ2,  "t#bar t", "f"); 
//      hs1.Add(h2bdtZZ3);
//      hs1.Draw();
//      leg1->AddEntry(h2bdtZZ3,"WW", "f");
//      leg1->Draw();
//    //   hs1.Add(h2bdtZZ4);
//   //    hs1.Draw();
//   //    leg1->AddEntry(h2bdtZZ4,"ZZ #rightarrow #tau #tau #nu #nu", "f");
//      leg1->Draw();
//      hs1.Add(h2bdtZZ5);
//      hs1.Draw();
//      leg1->AddEntry(h2bdtZZ5,"ZZ #rightarrow l l #tau #tau", "f");
//      leg1->Draw();
//      hs1.Add(h2bdtZZ6);
//      hs1.Draw();
//      leg1->AddEntry(h2bdtZZ6,"ZW", "f");
//      leg1->Draw();
//      hs1.Add(h2bdtZZ7);
//      hs1.Draw();
//      leg1->AddEntry(h2bdtZZ7,"Zincl", "f");
//      leg1->Draw();

//      hs2.Add(h2bdttt0);
//      pad25->cd();
//      hs2.Draw();
//      hs2.Add(h2bdttt1);
//      hs2.Draw();
//      hs2.Add(h2bdttt2);
//      hs2.Draw();
//      hs2.Add(h2bdttt3);
//      hs2.Draw();
//   //    hs2.Add(h2bdttt4);
//   //    hs2.Draw();
//      hs2.Add(h2bdttt5);
//      hs2.Draw();
//      hs2.Add(h2bdttt6);
//      hs2.Draw();
//      hs2.Add(h2bdttt7);
//      hs2.Draw();

//      hs3.Add(h2bdtWW0);
//      pad24->cd();
//      hs3.Draw();
//      hs3.Add(h2bdtWW1);
//      hs3.Draw();
//      hs3.Add(h2bdtWW2);
//      hs3.Draw();
//      hs3.Add(h2bdtWW3);
//      hs3.Draw();
//   //    hs3.Add(h2bdtWW4);
//   //    hs3.Draw();
//      hs3.Add(h2bdtWW5);
//      hs3.Draw();
//      hs3.Add(h2bdtWW6);
//      hs3.Draw();
//      hs3.Add(h2bdtWW7);
//      hs3.Draw();

//      hs5.Add(h2bdtlt0);
//      pad23->cd();
//      hs5.Draw();
//      hs5.Add(h2bdtlt1);
//      hs5.Draw();
//      hs5.Add(h2bdtlt2);
//      hs5.Draw();
//      hs5.Add(h2bdtlt3);
//      hs5.Draw();
//   //    hs5.Add(h2bdtlt4);
//   //    hs5.Draw();
//      hs5.Add(h2bdtlt5);
//      hs5.Draw();
//      hs5.Add(h2bdtlt6);
//      hs5.Draw();
//      hs5.Add(h2bdtlt7);
//      hs5.Draw();

//      hs6.Add(h2bdtZW0);
//      pad22->cd();
//      hs6.Draw();
//      hs6.Add(h2bdtZW1);
//      hs6.Draw();
//      hs6.Add(h2bdtZW2);
//      hs6.Draw();
//      hs6.Add(h2bdtZW3);
//      hs6.Draw();
//   //    hs6.Add(h2bdtZW4);
//   //    hs6.Draw();
//      hs6.Add(h2bdtZW5);
//      hs6.Draw();
//      hs6.Add(h2bdtZW6);
//      hs6.Draw();
//      hs6.Add(h2bdtZW7);
//      hs6.Draw();

//      hs7.Add(h2bdtZi0);
//      pad21->cd();
//      hs7.Draw();
//      hs7.Add(h2bdtZi1);
//      hs7.Draw();
//      hs7.Add(h2bdtZi2);
//      hs7.Draw();
//      hs7.Add(h2bdtZi3);
//      hs7.Draw();
//   //    hs7.Add(h2bdtZi4);
//   //    hs7.Draw();
//      hs7.Add(h2bdtZi5);
//      hs7.Draw();
//      hs7.Add(h2bdtZi6);
//      hs7.Draw();
//      hs7.Add(h2bdtZi7);
//      hs7.Draw();

  //==========================================


  }
    
  cout << endl;
  cout << "All done!" << endl;

  theApp.Run();                            // concluding ROOT requirements.

  return 0;
} //  End of main(...)


