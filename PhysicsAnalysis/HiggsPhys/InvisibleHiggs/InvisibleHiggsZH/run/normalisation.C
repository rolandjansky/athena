/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// program adapted from EVmva.C to select the variables needed for the Boosted Tree analysis for the normalisation
// - reads in the events after using EV as a ntuple dumper
// - applies all preselection cuts to select events with 4 leptons from two Z 
// - turns two leptons into invisible neutrinos
// - recalculates the missing energy adding the 2 missing electrons
// - apply the missing energy cut
// - apply the anti b-tag rejection cut

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
#include <time.h>

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

// - - - Initiates the GUI interface of ROOT - - -
using std::string;
using std::setprecision;
extern void InitGui();

//============================================================================

int main(int argc, char **argv)
 {

VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
TROOT root("","", initfuncs);

TApplication theApp("App", &argc, argv);
gROOT->Reset();

   // bool EF_passed = true;

   double generated = 0.0;

   string eventType = ("ZZllll"); 
   double sigmaBR = 0.1509; 
   double filterEff = 0.219;

   int found =0;
   double Total = 0.0;
   double weight = 0.0; 
   double totalRejected = 0.0;
   int nrec = 0;
   double passed = 0.0;

   double reject_MET  = 0.0;
   double reject_nlep  = 0.0;
   double reject_trigger  = 0.0;
   double reject_AODtrigger  = 0.0;
   double reject_masse  = 0.0;
   double reject_massm  = 0.0;
   double reject_2Z  = 0.0;
   double reject_leppid  = 0.0;
   double reject_btag  = 0.0;
   float epair = 0.0; 
   float mpair = 0.0; 
   int seed = 77;
   //   srand ( time(0) ); // for a random seed
   srand(seed);  // for a fixed start

 string histName1 = ("ptlep1");
 string histName2 = ("ptlep2");
 string histName3 = ("cosll2D");
 string histName4 = ("cosll3D");
  
 string histName5 = ("MET");
 string histName6 = ("mt");
 string histName7 = ("cosMETPtlep");

 string histName8 = ("mZ");
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

 double lumi = 30000.0;

// open output file to save the cutflow results
ofstream outClientFile( "normalisation.dat", ios::out );

// exit program if unable to open file
 if ( !outClientFile ) {
   cerr << "File could not be opened" << endl;
   exit (1);
 } // end if

const unsigned int XC1=0, YC1=0, XC2=800, YC2=800;
const double G=0.004;

TCanvas* c1 = new TCanvas("c1","pvariables set 1", XC1, YC1, XC2, YC2);
c1->cd();
c1->SetFillColor(30);
c1->GetFrame()->SetFillColor(10);
c1->GetFrame()->SetBorderSize(6);
c1->GetFrame()->SetBorderMode(-1);
TPad* pad11= new TPad("pad11", "Pad11",       G,     G,       0.5-G,   0.5-G);
TPad* pad12= new TPad("pad12", "Pad12",   0.5+G,     G,       1.0-G,   0.5-G);
TPad* pad13= new TPad("pad13", "Pad13",       G, 0.5+G,       0.5-G,   1.0-G);
TPad* pad14= new TPad("pad14", "Pad14",   0.5+G, 0.5+G,       1.0-G,   1.0-G);
pad11->SetFillColor(10); pad11->SetGrid(); pad11->Draw();
pad12->SetFillColor(10); pad12->SetGrid(); pad12->Draw();   
pad13->SetFillColor(10); pad13->SetGrid(); pad13->Draw();
pad14->SetFillColor(10); pad14->SetGrid(); pad14->Draw();

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
pad21->SetFillColor(10); pad21->SetGrid(); pad21->Draw();
pad22->SetFillColor(10); pad22->SetGrid(); pad22->Draw();   
pad23->SetFillColor(10); pad23->SetGrid(); pad23->Draw();
pad24->SetFillColor(10); pad24->SetGrid(); pad24->Draw();

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
pad31->SetFillColor(10); pad31->SetGrid(); pad31->Draw();
pad32->SetFillColor(10); pad32->SetGrid(); pad32->Draw();   
pad33->SetFillColor(10); pad33->SetGrid(); pad33->Draw();
pad34->SetFillColor(10); pad34->SetGrid(); pad34->Draw();

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
pad41->SetFillColor(10); pad41->SetGrid(); pad41->Draw();
pad42->SetFillColor(10); pad42->SetGrid(); pad42->Draw();   
pad43->SetFillColor(10); pad43->SetGrid(); pad43->Draw();
pad44->SetFillColor(10); pad44->SetGrid(); pad44->Draw();

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
pad51->SetFillColor(10); pad51->SetGrid(); pad51->Draw();
pad52->SetFillColor(10); pad52->SetGrid(); pad52->Draw();   
pad53->SetFillColor(10); pad53->SetGrid(); pad53->Draw();
pad54->SetFillColor(10); pad54->SetGrid(); pad54->Draw();

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

// // -----------------------------------------------------------
// //      - - -  Creating many histograms - - -  
// // -----------------------------------------------------------
 TH1F* hptlep1;
 TH1F* hptlep2;
 TH1F* hcosll2D;
 TH1F* hcosll3D;
 TH1F* hMET;
 TH1F* hmt;
 TH1F* hcosMETPtlep;
 TH1F* hmZll;
//  TH1F* hmZ1;
//  TH1F* hmZ2;
 TH1F* hcosphi;
 TH1F* hcosJetMET;
 TH1F* hcone1;
 TH1F* hcone2;
 TH1F* hejet1;
 TH1F* hejet2;
 TH1F* hejet3;
 TH1F* hnjet;
 TH1F* hnel;
 TH1F* hnmu;
 TH1F* hnlep;
 TH1F* hlepid;

// variables of the ntuples are being declared below.  

 // cuts for AOD 
 const double eptmin2  = 15.0;
 const double mptmin2  = 10.0;
 const double eptmin1  = 25.0;
 const double mptmin1  = 20.0;
 const double eptmin  =  10.0;  
 const double mptmin  =  10.0;  
 const double etamin   = 2.5;
 const int    nlepcut  = 2;
 const double METcut   = 90.0;
 double mZcut    = 30.0;  
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
 
// create a new ROOT file
 TFile f("normalisation.root","recreate");

// create one TTree
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
 float cosll2d;
 float cosll3d;
 float mT;
 float MET;
 float cosJetMET = 0.;
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

 reject_MET =0.;
 reject_nlep =0.;
 reject_trigger =0.;
 reject_AODtrigger =0.;
 reject_leppid =0.;
 reject_2Z =0.;
 reject_btag =0.;
 totalRejected =0.;
 
 found=0;

 hptlep1 = new TH1F("ptlep1", "p_T^{lepton 1} ", 50, 0, 300.0);
 hptlep2 = new TH1F("ptlep2", "p_T^{lepton 2} ", 50, 0, 100.0);
 hcosll2D = new TH1F("cosll2D", "cos_{ll} (xy plane)", 50, -1.1, 1.1);
 hcosll3D = new TH1F("cosll3D", "cos_{ll}", 50, -1.1, 1.1);
 hMET = new TH1F("MET", "p_{T} missing per event ", 50, 50, 500);
 hmt = new TH1F("mT", "m_T ", 50, 0, 700.0);
 hcosMETPtlep = new TH1F("cosMETptlep", "cos MET-Ptlep ", 50, -1.1, 1.1);
 hmZll = new TH1F("mZ", "M_{Z #rightarrow ll} 1", 50, 71.19, 111.19);
//  hmZ2 = new TH1F("mZ2", "M_{Z #rightarrow ll} 2", 50, 51.19, 131.19);
 hcosphi = new TH1F("cosphi", "cos #phi ", 50, -1.1, 1.1);
 hcosJetMET = new TH1F("cosJetMET", "cos #phi (between p_{T} miss and jet", 50, -1.0, 1.2);
 hcone1 = new TH1F("cone1", "electron isolation cone 0.10 ", 50, 0.0, 50.0);
 hcone2 = new TH1F("cone2", "muon isolation cone 0.10 ", 50, 0.0, 300.0);
 hejet1 = new TH1F("ejet1", "energy jet 1 ", 50, 0, 300.0);
 hejet2 = new TH1F("ejet2", "energy jet 2 ", 50, 0, 300.0);
 hejet3 = new TH1F("ejet3", "energy jet 3 ", 50, 0, 300.0);
 hnjet = new TH1F("njet", "njet ", 11, 0, 10);
 hnel = new TH1F("nel", "nel ", 6, 0, 5);
 hnmu = new TH1F("nmu", "nmu ", 6, 0, 5);
 hnlep = new TH1F("nlep", "nlep ", 11, 0, 10);
 hlepid = new TH1F("lepid", "lepid ", 26, -0, 25);

 Color_t color = kYellow;

 hptlep1->SetFillColor(color);
 hptlep2->SetFillColor(color);
 hcosll2D->SetFillColor(color);
 hcosll3D->SetFillColor(color);
 hMET->SetFillColor(color);     
 hmt->SetFillColor(color);     
 hcosMETPtlep->SetFillColor(color);     
 hmZll->SetFillColor(color);    
 hcosphi->SetFillColor(color);     
 hcosJetMET->SetFillColor(color);     
 hcone1->SetFillColor(color);
 hcone2->SetFillColor(color);
 hejet1->SetFillColor(color);
 hejet2->SetFillColor(color);
 hejet3->SetFillColor(color);
 hnjet->SetFillColor(color);
 hnel->SetFillColor(color);
 hnmu->SetFillColor(color);
 hnlep->SetFillColor(color);
 hlepid->SetFillColor(color);

  /***************** Getting file/tree from disk *********************/
  // We define chain, which is basically a tree to add root files with the
  // same tree name.  If you have two or more files, keep adding in the 
  // nfchain variable.

  TChain *chain=0;

#ifdef CHAIN
  //     chain = new TChain("EVCands");
     chain = new TChain("EV0");
     int nfchain=0;

// ZZ->llll sample - 50k filtered events with multilepton filter
    int typeNumber = 1;
    nfchain += chain->Add("/data/ntuples/ZZ4l_ntuple.root");
     
  int EVEventAndInstance;  EVEventAndInstance = 0;
  int EVRunNumber;    EVRunNumber = 0;

  double MET_RefFinal_ex;  MET_RefFinal_ex = 0.0;
  double MET_RefFinal_ey;  MET_RefFinal_ey = 0.0;
  double MET_RefFinal_et;  MET_RefFinal_et = 0.0;

  double MET_Truth_ex;  MET_Truth_ex = 0.0;
  double MET_Truth_ey;  MET_Truth_ey = 0.0;
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

  chain->SetBranchAddress("MET_RefFinal_ex", &MET_RefFinal_ex);
  chain->SetBranchAddress("MET_RefFinal_ey", &MET_RefFinal_ey);
  chain->SetBranchAddress("MET_RefFinal_et", &MET_RefFinal_et);

  chain->SetBranchAddress("MET_RefFinal_et", &MET_RefFinal_et);

  chain->SetBranchAddress("MET_Truth_ex", &MET_Truth_ex);
  chain->SetBranchAddress("MET_Truth_ey", &MET_Truth_ey);
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

nrec = nrecords;

// The above is to find the number of events or entries
#endif // CHAIN 

outClientFile << "      " <<  endl;
outClientFile << "MET cut > " << METcut << endl;
outClientFile << "pt cuts for leptons: e > " << eptmin << " and mu > " << mptmin <<" GeV with eta < " << etamin << endl;
outClientFile << "pt cuts for AOD trigger: 1e > " << eptmin1 << " and 2e > " << eptmin2 <<" GeV with eta < " << etamin << endl;
outClientFile << "pt cuts for AOD trigger: 1m > " << mptmin1 << " and 2m > " << mptmin2 <<" GeV with eta < " << etamin << endl;
outClientFile << "nlep strictly equal to " << nlepcut << endl; //" and njet less or equal to " << njetcut << endl;	 	   
outClientFile << "mZ within " << mZcut << " GeV of on-shell mass " << endl;
outClientFile << endl;	  

double mZ  = 0.0;
double mZ1 = 0.0;
double mZ2 = 0.0;

// evaluate the weight for this type of events
generated = nrec/filterEff; 
 cout << " generated: " << generated << " nrec: " << nrec << endl;
weight = (sigmaBR * lumi)/generated;

// =====================================================================
// ============  Starting the main loop over the events  ===============
// =====================================================================

 for (int i=0; i<nrec; i++) {  

   if(div(i, nticker).rem == 0 || i >= nrec-1) 
     { 
       portion=static_cast<float>(i) / static_cast<float>(nrec-1);
       printf("Done %4.1f percent of reading the NTuple  ", portion*100.0);
       cout.flush();   cout << char(13);  
     }

   last_rec=(i>=nrec-1); 

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

   int best1j = 0;
   int best1k = 0;
   int best1id = 0;
   int best2j = 0;
   int best2k = 0;
   int best2id = 0;
   int invlep1 = 0;
   int invlep2 = 0;
   int invid = 0;

   MET_RefFinal_ex /=1000.;
   MET_RefFinal_ey /=1000.; 
   MET_RefFinal_et /=1000.;

   MET_Truth_et /=1000.;

   for(int j=0; j<El_N; j++) {
     (*El_px)[j] /= 1000.;
     (*El_py)[j] /= 1000.;
     (*El_pz)[j] /= 1000.;
     (*El_p_T)[j] /= 1000.;
     (*El_trEtCone10)[j] /= 1000.;
   }
   for(int j=0; j<Mu_N; j++) {
     (*Mu_px)[j] /= 1000.;
     (*Mu_py)[j] /= 1000.;
     (*Mu_pz)[j] /= 1000.;
     (*Mu_p_T)[j] /= 1000.;
     (*Mu_trEtCone10)[j] /= 1000.;
   }

   for(int j=0; j<El_N; j++) {
     (*TEl_p_T)[j] /= 1000.;
   }
   for(int j=0; j<Mu_N; j++) {
     (*TMu_p_T)[j] /= 1000.;
   }

// apply the trigger selection
   if (!PassedEF_e15iEF_e15i && !PassedEF_e25i && !PassedEF_mu6 && !PassedEF_mu6l && !PassedEF_mu20i && !PassedL1_XE100) {
     reject_trigger++;}
   if (PassedEF_e15iEF_e15i || PassedEF_e25i || PassedEF_mu6 || PassedEF_mu6l || PassedEF_mu20i || PassedL1_XE100) {
     
//first loop over all pairs of electrons which have pt > 7 GeV
     if (El_N >= 2) epair++;
     if (El_N >= 4) epair++;
     for(int j=0; j<El_N-1; j++) {
       if (fabs((*El_eta)[j]) <= etamin && (*El_p_T)[j] > 7.0 && ((*El_isEM)[j]&0x7)==0) {
	 for(int k=j+1; k<El_N; k++) {
	   if (fabs((*El_eta)[k]) <= etamin && (*El_p_T)[k] >= 7.0 && ((*El_isEM)[k]&0x7)==0) {
//lepid cut
//	       if((*El_pdgId)[j] != -(*El_pdgId)[k]) {cout << "electron id: " << (*El_pdgId)[j] << "\t" << (*El_pdgId)[k] << endl;}
	     if((*El_pdgId)[j] == -(*El_pdgId)[k]) {

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

 //count the number of accepted pairs
	       if (abs(mZ - mZ0) > mZcut) reject_masse++;
	       if (abs(mZ - mZ0) <= mZcut) {

		 if (accepted_e > 24) {cout << " too many accepted_e pairs, stop!!! accepted_e = " << accepted_e << endl;exit(-1);}
		 Zmasse[accepted_e] = mZ;
		 elep1[accepted_e]  = j;
		 elep2[accepted_e]  = k;
		 accepted_e++;}
	       
	     } //  first end-if on lepid
	   } // first end if acceptance criteria for second electron
	 } // first end of for-loop on second electron
       } // first end if acceptance criteria for first electron
     } // first end of for-loop on first electron

     if (Mu_N >= 2) epair++;
     if (Mu_N >= 4) epair++;
// next step: first loop over all pairs of muons 
     for(int j=0; j<Mu_N-1; j++) {
       if (fabs((*Mu_eta)[j]) <= etamin && (*Mu_p_T)[j] > 7.0 && (*Mu_isCombinedMuon)[j] == 1 && (*Mu_bestMatch)[j] == 1) {
	 for(int k=j+1; k<Mu_N; k++) {

	   if (fabs((*Mu_eta)[k]) <= etamin && (*Mu_p_T)[k] >= 7.0 && (*Mu_isCombinedMuon)[k] == 1 && (*Mu_bestMatch)[k] == 1) {
	     
//lepid cut
	     if((*Mu_pdgId)[j] == -(*Mu_pdgId)[k]) {
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

//count the number of accepted_m pairs
		 //		 cout << "reconstructed mZ = " << mZ << endl;
	       if (abs(mZ - mZ0) > mZcut) reject_massm++;
	       if (abs(mZ - mZ0) <= mZcut) {
		 
		 if (accepted_m > 24) {cout << " too many accepted_m pairs, stop!!! accepted_m = " << accepted_m << endl;exit(-1);}
		 Zmassm[accepted_m] = mZ;
		 mlep1[accepted_m]  = j;
		 mlep2[accepted_m]  = k;
		 accepted_m++;}
	       
	     } //  first end-if on lepid for muons
	   } // first end if acceptance criteria for second muon
	 } // first end of for-loop on second muon
       } // first end if acceptance criteria for first muon
     } // first end of for-loop on first muon

// check to see if two Z are found:
//       cout << "accepted_e: " << accepted_e << " accepted_m: " << accepted_m << endl;
     if (accepted_m + accepted_e < 2 ) {reject_2Z++;;}
     if (accepted_m + accepted_e >= 2 ) {

//loop over all accepted pairs and retain only the two best ones
//   for (int n = 0; n < accepted_e + accepted_m; n++) {
       mZ =0.0;
       mZ1=0.0;
       mZ2=0.0;
       for (int m=0; m<accepted_e; m++) {
	 mZ1 = (( abs(mZ0 - mZ1) < abs(mZ0 - Zmasse[m])) ? mZ1 : Zmasse[m]);
	 if (mZ1 == Zmasse[m]) {best1j = elep1[m]; best1k = elep2[m]; best1id=11;}
       }
       for (int m=0; m<accepted_m; m++) {
	 mZ1 = (( abs(mZ0 - mZ1) < abs(mZ0 - Zmassm[m])) ? mZ1 : Zmassm[m]);
	 if (mZ1 == Zmassm[m]) {best1j = mlep1[m]; best1k = mlep2[m]; best1id=13;}
       }
       
       for (int m=0; m<accepted_e; m++) {
	 if ((best1id == 11 && (best1j != elep1[m] || best1k != elep2[m])) || best1id ==13) {
	   mZ2 = (( abs(mZ0 - mZ2) < abs(mZ0 - Zmasse[m])) ? mZ2 : Zmasse[m]);
	   if (mZ2 == Zmasse[m]) {best2j = elep1[m]; best2k = elep2[m]; best2id=11;}
	 }
       }
       for (int m=0; m<accepted_m; m++) {
	 if ((best1id == 13 && (best1j != mlep1[m] || best1k != mlep2[m])) || best1id ==11) {
	   mZ2 = (( abs(mZ0 - mZ2) < abs(mZ0 - Zmassm[m])) ? mZ2 : Zmassm[m]);
	   if (mZ2 == Zmassm[m]) {best2j = mlep1[m]; best2k = mlep2[m]; best2id=13;}
	 }
       }
//       cout << "mZ1: " << mZ1 << " mZ2: " << mZ2 << endl;
//        hmZ1->Fill(mZ1);
//        hmZ2->Fill(mZ2);

// pick randomly one Z and declare the two leptons invisible
       runnum = EVRunNumber;
       evtnum  = EVEventAndInstance/100;

  /* generate random number between 1 and 100: */
       int dice = rand() % 100 + 1;
   //   cout << "dice : " << dice << " for event # " << evtnum << "\t" << runnum << endl;
       if (dice <= 50) {
	 bestid  = best2id;
	 bestj   = best2j;
	 bestk   = best2k;
         mZ      = mZ2;
	 invlep1 = best1j;
	 invlep2 = best1k;
	 invid   = best1id;
  }
       if (dice > 50) {
	 bestid  = best1id;
	 bestj   = best1j;
	 bestk   = best1k;
         mZ      = mZ1;
	 invlep1 = best2j;
	 invlep2 = best2k;
	 invid   = best2id;
       }

       if (invlep1 == invlep2) cout << "wo les moteurs!" << dice << "\t" << best1id << "\t" << best2id << endl;

// check if the remaining Z passes the original selection cuts on momentum
       bool ok = true;
       if (bestid == 11) {
   	 if ((*El_p_T)[bestj] < eptmin || (*El_p_T)[bestk] < eptmin) {
	   ok = false;  
	   reject_nlep++;}
       }
       if (bestid == 13) {
	 if ((*Mu_p_T)[bestj] < mptmin || (*Mu_p_T)[bestk] < mptmin) {
	   ok = false;  
	   reject_nlep++;}
       }

//second loop over all electrons to count the good ones
	 nel = 0;
	 for(int j=0; j<El_N; j++) {
	   if (fabs((*El_eta)[j]) <= etamin && (*El_p_T)[j] > eptmin && ((*El_isEM)[j]&0x7)==0) {
//  	     cout << "j for electrons: " << j << " invid: " << invid << " invlep1: " << invlep1 << " invlep2: " << invlep2 << endl;
	     if (invid == 13 || (invid == 11 && (j != invlep1 && j != invlep2))) {
//  	       cout << " this electron got accepted in event: " << evtnum << endl;
	       nel++;
	     }
	   }
	 }

// 	 if (nel == 3) {
// 	   for(int j=0; j<El_N; j++) {
// 	     if (fabs((*El_eta)[j]) <= etamin && (*El_p_T)[j] > eptmin && ((*El_isEM)[j]&0x7)==0) {
// 	       if (invid == 13 || (invid == 11 && (j != invlep1 && j != invlep2))) {
// 		 cout << "nel == 3 in event: " << evtnum << " El_N = : " << El_N << " j for electrons: " << j << " invid: " << invid                               << " invlep1: " << invlep1 << " invlep2: " << invlep2 << endl;
// 	       }
// 	     }
// 	   }
// 	 }
// second loop over all muons to select the good ones after removing the leptons declared invisible
	 nmu = 0;
	 for(int j=0; j<Mu_N; j++) {
	   if (fabs((*Mu_eta)[j]) <= etamin && (*Mu_p_T)[j] > mptmin && (*Mu_isCombinedMuon)[j] == 1 && (*Mu_bestMatch)[j] == 1) {
//	  cout << "j for muons: " << j << " invid: " << invid << " invlep1: " << invlep1 << " invlep2: " << invlep2 << endl;
	     if (invid == 11 || (invid == 13 && (j != invlep1 && j != invlep2))) {
//	    cout << " this muon got accepted" << endl;
	       nmu++;
	     }
	   }
	 }

// 	 if (nmu == 3) {
// 	   for(int j=0; j<Mu_N; j++) {
// 	     if (fabs((*Mu_eta)[j]) <= etamin && (*Mu_p_T)[j] > mptmin && (*Mu_isCombinedMuon)[j] == 1 && (*Mu_bestMatch)[j] == 1) {
// 	       if (invid == 11 || (invid == 13 && (j != invlep1 && j != invlep2))) {
// 		 cout << "Nmu == 3 in event: " << evtnum << " Mu_N = : " << Mu_N << " j for muons: " << j << " invid: " << invid << " invlep1: " <<                invlep1 << " invlep2: " << invlep2 << endl;
// 		 nmu++;
// 	       }
// 	     }
// 	   }
// 	 }
// store the total number of good leptons
	 nlep = nel + nmu;

	 if (nlep < 2) reject_nlep++;
	 if (ok && nlep >= 2) {

// apply the trigger cuts at the AOD level; reset the counters for the trigger 
	 int etrigger1 = 0;
	 int etrigger2 = 0;
	 int mtrigger1 = 0;
	 int mtrigger2 = 0;

//check the trigger requirements for leptons at AOD level
	 for(int j=0;j<El_N;j++){
	   if (fabs((*El_eta)[j]) <= etamin && (*El_p_T)[j] > eptmin1) etrigger1++;
	   if (fabs((*El_eta)[j]) <= etamin && (*El_p_T)[j] > eptmin2) etrigger2++;
	 }

	 for(int j=0;j<Mu_N;j++){
	   if (fabs((*Mu_eta)[j]) <= etamin && (*Mu_p_T)[j] > mptmin1) mtrigger1++;
	   if (fabs((*Mu_eta)[j]) <= etamin && (*Mu_p_T)[j] > mptmin2) mtrigger2++;
	 }

//apply the MET cut after re-evaluating it:
	   double new_MET_ex = 0.0;
	   double new_MET_ey = 0.0;
	   double new_MET_et = 0.0;

	   if (invid == 11) {
	   if ((*El_px)[invlep1] == 0.0) cout << "(*El_px)[invlep1] =0: invlep1 = " << invlep1 << "\t" << invid << endl;
	   if ((*El_py)[invlep1] == 0.0) cout << "(*El_py)[invlep1] =0: invlep1 = " << invlep1 << "\t" << invid << endl;
	   if ((*El_px)[invlep2] == 0.0) cout << "(*El_px)[invlep2] =0: invlep2 = " << invlep2 << "\t" << invid << endl;
	   if ((*El_py)[invlep2] == 0.0) cout << "(*El_py)[invlep2] =0: invlep2 = " << invlep2 << "\t" << invid << endl;
	   }
	   if (invid == 13) {
	   if ((*Mu_px)[invlep1] == 0.0) cout << "(*Mu_px)[invlep1] =0: invlep1 = " << invlep1 << "\t" << invid << endl;
	   if ((*Mu_py)[invlep1] == 0.0) cout << "(*Mu_py)[invlep1] =0: invlep1 = " << invlep1 << "\t" << invid << endl;
	   if ((*Mu_px)[invlep2] == 0.0) cout << "(*Mu_px)[invlep2] =0: invlep2 = " << invlep2 << "\t" << invid << endl;
	   if ((*Mu_py)[invlep2] == 0.0) cout << "(*Mu_py)[invlep2] =0: invlep2 = " << invlep2 << "\t" << invid << endl;
	   }

	   if (invid == 11) {
	     new_MET_ex = MET_RefFinal_ex + (*El_px)[invlep1] + (*El_px)[invlep2];
	     new_MET_ey = MET_RefFinal_ey + (*El_py)[invlep1] + (*El_py)[invlep2];
	     new_MET_et = new_MET_ex*new_MET_ex + new_MET_ey*new_MET_ey;
	     if (new_MET_et <= 0.0) cout << new_MET_ex << "\t" << new_MET_ey << "\t" << new_MET_et << endl;
	     new_MET_et = sqrt(new_MET_et);
	     MET_RefFinal_et = new_MET_et;
	 }
	   if (invid == 13) {
	     new_MET_ex = MET_RefFinal_ex + (*Mu_px)[invlep1] + (*Mu_px)[invlep2];
	     new_MET_ey = MET_RefFinal_ey + (*Mu_py)[invlep1] + (*Mu_py)[invlep2];
	     new_MET_et = new_MET_ex*new_MET_ex + new_MET_ey*new_MET_ey;
	     if (new_MET_et <= 0.0) cout << new_MET_ex << "\t" << new_MET_ey << "\t" << new_MET_et << endl;
	     new_MET_et = sqrt(new_MET_et);
	     MET_RefFinal_et = new_MET_et;
	   }

//	   cout << "MET_RefFinal_et: " << MET_RefFinal_et << " new_MET_et: " << new_MET_et << " difference: " <<  new_MET_et - MET_RefFinal_et << endl;

           hMET->Fill(new_MET_et,weight);
	   if (MET_RefFinal_et < METcut) reject_MET++;
	   if (MET_RefFinal_et >= METcut) {

// apply the trigger cuts, including the MET trigger:
	 if (etrigger1 < 1  && etrigger2  < 2  && mtrigger1 < 1   && mtrigger2  < 2 && new_MET_et < METcut ) {
	   reject_AODtrigger++;}
	 if (etrigger1 >= 1 || etrigger2 >= 2  || mtrigger1 >= 1  || mtrigger2  >= 2 || new_MET_et >= METcut ) {

// apply an anti-btag on events containing a jet
	     int btagOK=1;
	     for(int l=0; l<Jet_N; l++) {
	       if ((*Jet_BTagged)[l] == 1) {
		 btagOK=0;}
	     }
	   
	     if (btagOK == 0) {reject_btag++;}
	     if (btagOK == 1) {
	     
// done with selection
	       found++;

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
	       if (abs(bestid) == 11 ) {
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
	       cosPhi  = (px*new_MET_ex + py*new_MET_ey)/(pt2l*new_MET_et);

// calculate the transverse mass
	       mt      = sqrt(2.0*pt2l*new_MET_et*(1.0-cosPhi));

// calculate cosJetMET: cosine between promptest jet and MET direction in transverse plane
	       cosjetmet = 1.1;
	       if (Jet_N > 0) cosjetmet = float(((*Jet_px)[0]*new_MET_ex + (*Jet_py)[0]*new_MET_ey)/((*Jet_p_T)[0]*new_MET_et));
		   
// select the promptest lepton for this last variable
	       if (pt1 >= pt2)  {PX=px1; PY=py1; PT=pt1;}
	       if (pt1 <  pt2)  {PX=px2; PY=py2; PT=pt2;}
	     
	       cosptlepMET = (PX*new_MET_ex +PY*new_MET_ey)/ (PT * new_MET_et);

// fill the jet info
	       ejet1 = -99.99;
	       ejet2 = -99.99;
	       ejet3 = -99.99;
	       
	       njet = Jet_N;
// add info on the 3 most energetic jets
	       if (njet > 0) ejet1 = (*Jet_E)[0]/1000.;
	       if (njet > 1) ejet2 = (*Jet_E)[1]/1000.;
	       if (njet > 2) ejet3 = (*Jet_E)[2]/1000.;
	       
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
	       MET = float(new_MET_et);
	       cosMETptlep = float(cosptlepMET);
	       cosphi = float(cosPhi);
	       
	       runnum = EVRunNumber;
	       evtnum  = EVEventAndInstance/100;
	       w      = float(weight);
	     
// // fill the preselection tree from the values calculated above
	       type = typeNumber;
	       treeB.Fill();

// fill the histograms
	       hptlep1->Fill(pt1,weight);
	       hptlep2->Fill(pt2,weight);
	       hcosll2D->Fill(cosll2D,weight);
	       hcosll3D->Fill(cosll3D,weight);
	       //	       hMET->Fill(MET,weight);
	       hmt->Fill(mt,weight);
	       hcosMETPtlep->Fill(cosMETptlep,weight);
	       hmZll->Fill(massZ,weight);
	       hcosphi->Fill(cosphi,weight);
	       hnjet->Fill(njet,weight);
	       hcone1->Fill(cone1,weight);
	       hcone2->Fill(cone2,weight);
	       hejet1->Fill(ejet1,weight);
	       hejet2->Fill(ejet2,weight);
	       hejet3->Fill(ejet3,weight);
	       hcosJetMET->Fill(cosJetMET,weight);
	       hnel->Fill(nel,weight);
	       hnmu->Fill(nmu,weight);
	       hnlep->Fill(nlep,weight);
	       hlepid->Fill(lepID,weight);

	     
	     } // end if on btag
	   } // end on METcut
	 } // end if on "trigger cuts" at AOD level
       } // end if on nlep > nlepcut
     } // end if on finding 2 Z
   } // end on trigger cut   
 } // end of for loop over events   
  
outClientFile << "              " << endl;
outClientFile << "Cut flow for: " << eventType << endl;
outClientFile << "-----------------------------------------------------------" << endl;
outClientFile << nrec << " records found in the chain. "<< endl;
       
cout << endl;
cout << "METcut " << METcut << endl;
cout << "pt cuts for leptons: e > " << eptmin << " and mu: " << mptmin << " GeV and eta < " << etamin << endl;	  
cout << "nlep strictly equal to "<< nlepcut << " of the same flavor" << endl; 
cout << "mZ within " <<  mZcut << " GeV of on-shell mass " << endl;
cout << "total # of selected events in file: \t" << found << endl;	  
cout << endl;
cout << "Cut flow for: " << eventType << endl;
 cout << "-----------------------------------------------------------" << endl;
cout << nrec << " records found in the chain. "<< endl;
cout << endl;

passed = nrec;
totalRejected = (reject_MET + reject_nlep + reject_trigger  + reject_AODtrigger  + reject_2Z  + reject_btag); 

int places = 2;
cout <<fixed;                 
cout.precision(places);

 cout << "  " << endl;
 outClientFile << "  " << endl;

 cout << "epair " << "\t" << "reject_masse" << "\t" << "mpair" << "\t" << "reject_massm" << endl; 
 cout << epair  << "\t" << reject_masse << "\t" << mpair << "\t" << reject_massm << endl; 

cout << "                                   " <<   endl;
cout << "                                   " << " reweighted \t"   << "raw # \t"   << "\t  percentage"    <<   endl;
cout << "Events at 30 fb-1                  " << lumi*sigmaBR   <<"\t" << generated <<"\t" <<  100.*(generated/generated)<< "%" << endl;
cout << "Events after filter:               " << weight*passed <<"\t" << passed  <<"\t" <<  100.*(passed/generated)<< "%" << endl;
float temp = passed;
passed = (passed - reject_trigger);
cout << "Events after trigger:              " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
temp = passed;
passed = (passed - reject_2Z);
cout << "Events after 2Z cut:               " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
temp = passed;
passed = (passed - reject_nlep);
cout << "Events after 2 high pt lepton cut: " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
temp = passed;
passed = (passed - reject_MET);
cout << "Events after MET cut               " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
temp = passed;
passed = (passed - reject_AODtrigger);
cout << "Events after AOD trigger cut       " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
temp = passed;
passed = (passed - reject_btag);
cout << "Events after b-tag cut:            " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
Total = passed*weight; 
cout << endl;
cout << "Total rejected (reweighted):  " << lumi*sigmaBR - weight*passed << " out of " << generated*weight << endl;
cout << "Total rejected (raw)       :  " << generated - passed << " out of " << generated<< endl;
cout << "Fraction retained          :  " << 100.*(passed/generated) << "% " << endl;
places = 5;
cout.precision(places);
cout << endl;
cout << "Events weight of " << weight << endl;
cout << " "<< endl;
cout <<fixed;                 
places = 2;
cout.precision(places);

passed = nrec;

outClientFile << "                         " <<   endl;
outClientFile <<fixed;                 
outClientFile.precision(places);
outClientFile << "                         " << " reweighted \t"   << "raw # \t"   << "\t  percentage"    <<   endl;
outClientFile << "Events at 30 fb-1         " << lumi*sigmaBR   <<"\t" << generated <<"\t" <<  100.*(generated/generated)<< "%" << endl;
outClientFile << "Events after filter:      " << weight*passed <<"\t" << passed      <<"\t" <<  100.*(passed/generated)<< "%" << endl;
temp = passed;
passed = (passed - reject_trigger);
outClientFile << "Events after trigger:              " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
temp = passed;
passed = (passed - reject_2Z);
outClientFile << "Events after 2Z cut:               " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
temp = passed;
passed = (passed - reject_nlep);
outClientFile << "Events after 2 high pt lepton cut: " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
temp = passed;
passed = (passed - reject_MET);
outClientFile << "Events after MET cut               " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
temp = passed;
passed = (passed - reject_AODtrigger);
outClientFile << "Events after AOD trigger cut       " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
temp = passed;
passed = (passed - reject_btag);
outClientFile << "Events after b-tag cut:            " << weight*passed <<"\t" << passed <<"\t" <<  100.*(passed/generated)<< "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
Total = passed*weight; 
outClientFile << endl;
outClientFile << "Total rejected (reweighted):  " << lumi*sigmaBR - weight*passed << " out of " << generated*weight << endl;
outClientFile << "Total rejected (raw)       :  " << generated - passed << " out of " << generated<< endl;
outClientFile << "Fraction retained          :  " << 100.*(passed/generated) << "% " << endl;
places = 5;
outClientFile.precision(places);
outClientFile << endl;
outClientFile << "Events weight of " << weight << endl;
outClientFile << " "<< endl;
outClientFile <<fixed;                 
places = 2;
outClientFile.precision(places);

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

TLegend *leg1 = new TLegend(0.6, 0.6, 0.9, 0.95);
TLegend *leg2 = new TLegend(0.6, 0.6, 0.9, 0.95);
TLegend *leg3 = new TLegend(0.6, 0.6, 0.9, 0.95);
TLegend *leg4 = new TLegend(0.65, 0.6, 0.9, 0.95);
TLegend *leg5 = new TLegend(0.65, 0.6, 0.9, 0.95);

   c3->cd();
   hs1.Add(hptlep1);
   pad33->cd(); 
   //   pad33->SetLogy(1);
   hs1.Draw();
   hs1.Draw();
   leg3->AddEntry(hptlep1, "  ZZ #rightarrow l l l l", "f"); 
   leg3->Draw();

   hs2.Add(hptlep2);
   pad34->cd(); 
   //  pad34->SetLogy(1);
   hs2.Draw();
   hs2.Draw();

   hs3.Add(hcosll2D);
   pad31->cd(); 
   //   pad31->SetLogy(1);
   hs3.Draw();
   hs3.Draw();

   hs4.Add(hcosll3D);
   pad32->cd(); 
   //   pad32->SetLogy(1);
   hs4.Draw();
   hs4.Draw();

   c2->cd();
   hs5.Add(hMET);
   pad23->cd(); 
   //   pad24->SetLogy(1);
   hs5.Draw();
   hs5.Draw();
   leg2->AddEntry(hptlep1, "  ZZ #rightarrow l l l l", "f"); 
   leg2->Draw();

   hs6.Add(hmt);
   pad24->cd(); 
   //   pad24->SetLogy(1);
   hs6.Draw();
   hs6.Draw();

   hs7.Add(hcosMETPtlep);
   pad21->cd(); 
   //   pad21->SetLogy(1);
   hs7.Draw();
   hs7.Draw();

   hs8.Add(hmZll);
   pad22->cd(); 
   //   pad22->SetLogy(1);
   hs8.Draw();
   hs8.Draw();

   c4->cd();
   hs9.Add(hcosphi);
   pad43->cd(); 
   //   pad43->SetLogy(1);
   hs9.Draw();
   hs9.Draw();
   leg4->AddEntry(hptlep1, "  ZZ #rightarrow l l l l", "f"); 
   leg4->Draw();

   hs10.Add(hcosJetMET);
   pad44->cd(); 
   //   pad44->SetLogy(1);
   hs10.Draw();
   hs10.Draw();

   hs11.Add(hcone1);
   pad41->cd(); 
   //   pad41->SetLogy(1);
   hs11.Draw();
   hs11.Draw();

   hs12.Add(hcone2);
   pad42->cd(); 
   //   pad42->SetLogy(1);
   hs12.Draw();
   hs12.Draw();

   c5->cd();
   hs13.Add(hejet1);
   pad53->cd(); 
   //   pad53->SetLogy(1);
   hs13.Draw();
   hs13.Draw();

   hs14.Add(hejet2);
   pad54->cd(); 
   //   pad54->SetLogy(1);
   hs14.Draw();
   hs14.Draw();

   hs15.Add(hejet3);
   pad51->cd(); 
   //   pad51->SetLogy(1);
   hs15.Draw();
   hs15.Draw();

   hs16.Add(hnjet);
   pad52->cd(); 
   //   pad52->SetLogy(1);
   hs16.Draw();
   hs16.Draw();
   leg5->AddEntry(hptlep1, "  ZZ #rightarrow l l l l", "f"); 
   leg5->Draw();

   c1->cd();
   hs17.Add(hnel);
   pad13->cd(); 
   hs17.Draw();
   hs17.Draw();
   leg1->AddEntry(hptlep1, "  ZZ #rightarrow l l l l", "f"); 
   leg1->Draw();

   hs18.Add(hnmu);
   pad14->cd(); 
   hs18.Draw();
   hs18.Draw();

   hs19.Add(hnlep);
   pad11->cd(); 
   hs19.Draw();
   hs19.Draw();

   hs20.Add(hlepid);
   pad12->cd(); 
   hs20.Draw();
   hs20.Draw();

   //----------------------------------
   //insert EVmva.C here

   srand(seed);  // for a fixed start

  int counter = 0;
  typeNumber = 0;
  generated = 0.0;
  
  eventType = ("HZ130");
  sigmaBR = 0.0359;
  filterEff = 0.754;
  
// mH = 130 GeV
  sigmaBR = 0.0359;
  filterEff = 0.754;

  lumi = 30000.0;
  
  // cuts for AOD 
  mZcut    = 20.;
  
  modulus =0.;
  cosll2D = 0.;
  cosll3D = 0.;
  cosPhi = 0.;
  cosptlepMET = 0.;
  mt = 0.;
  pt2l = 0.;
  PX = 0.;
  PY = 0.;
  PT = 0.;
  
  ptZ0 = 0.;
  eta1 = 0.;
  Cone1 = 0.;
  pt1 = 0.;
  px1 = 0.;
  py1 = 0.;
  pz1 = 0.;
  eta2 = 0.;
  Cone2 = 0.;
  px2 = 0.;
  py2 = 0.;
  pz2 = 0.;
  pt2 = 0.;
  cosjetmet = 0.;
  
  nb=0;
  last_rec=0;
  
  nlep = 0;
  nel = 0;;
  nmu = 0;
  ejet1 = 0.0;
  ejet2 = 0.0;
  ejet3 = 0.0;
  
  
  // -----------------------------------------------------------
  // ----------- loop over the various datasets ----------
  // -----------------------------------------------------------
    reject_MET =0.;
    reject_nlep =0.;
    reject_trigger =0.;
    reject_leppid =0.;
    double reject_mZ =0.;
    reject_btag =0.;
    totalRejected =0.;
  
  // loop over all datasets
    found=0;
    TRandom * random = new TRandom3(seed);

    
    /***************** Getting file/tree from disk *********************/
    // We define chain, which is basically a tree to add root files with the
    // same tree name.  If you have two or more files, keep adding in the 
    // nfchain variable.
    
    chain=0;
    
#ifdef CHAIN
    chain = new TChain("EV0");
    nfchain=0;
    
    // HZ130 sample - 50k filtered events
       nfchain += chain->Add("/data/ntuples/HZ130_ntuple.root");
    
    
       EVEventAndInstance = 0;
       EVRunNumber = 0;
    
       MET_RefFinal_ex = 0.0;
       MET_RefFinal_ey = 0.0;
       MET_RefFinal_et = 0.0;
    
       MET_Truth_et = 0.0;
    
       El_N = 0;
       TEl_N = 0;
       Mu_N = 0;
       TMu_N = 0;
       Jet_N = 0;
 
    // extract the trigger info
       PassedL1 = false;
       PassedL2 = false;
       PassedEF= false;
       PassedTrigger= false;
       PassedL1_EM25I= false;
       PassedL1_2EM15I= false;
    
       PassedL1_MU06= false;
       PassedL1_MU08= false;
       PassedL1_MU10= false;
       PassedL1_MU11= false;
       PassedL1_MU20= false;
       PassedL1_MU40= false;
       PassedL1_2MU06= false;
    
       PassedL1_XE100= false;
       PassedL1_XE50= false;
    
       PassedL2_e25i= false;
       PassedL2_mu6= false;
       PassedL2_mu6l= false;
       PassedL2_mu20i= false;
    
       PassedEF_e10= false;
       PassedEF_e15iEF_e15i= false;
       PassedEF_e25i= false;
       PassedEF_mu6= false;
       PassedEF_mu6l= false;
       PassedEF_mu20i= false;
    
       TrigEFMissingET_et = 0.0;
    
       Z_N = 0;
    
    chain->SetBranchAddress("EVEventAndInstance",  &EVEventAndInstance);
    chain->SetBranchAddress("EVRunNumber", &EVRunNumber);
    
    chain->SetBranchAddress("MET_RefFinal_ex", &MET_RefFinal_ex);
    chain->SetBranchAddress("MET_RefFinal_ey", &MET_RefFinal_ey);
    chain->SetBranchAddress("MET_RefFinal_et", &MET_RefFinal_et);
    
    chain->SetBranchAddress("MET_RefFinal_et", &MET_RefFinal_et);
    
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

    nrecords = static_cast<int>(0.5 + chain->GetEntries());
    
    nticker  = static_cast<int>(0.5 + nrecords/20.0);
    
    nrec = nrecords;
    
    // The above is to find the number of events or entries
#endif // CHAIN 
    
      outClientFile << "      " <<  endl;
      outClientFile << "MET cut > " << METcut << endl;
      outClientFile << "pt cuts for leptons: e > " << eptmin << " and mu > " << mptmin <<" GeV with eta < " << etamin << endl;
      outClientFile << "nlep strictly equal to " << nlepcut << endl; //" and njet less or equal to " << njetcut << endl;	 	   
      outClientFile << "mZ within " << mZcut << " GeV of on-shell mass " << endl;
      outClientFile << endl;	  
    
    mZ = 0.;
    
    // evaluate the weight for this type of events
    generated = nrec/filterEff; 
    cout << eventType << " generated: " << generated << " nrec: " << nrec << endl;
    weight = (sigmaBR * lumi)/generated;

    // ============  Starting the main loop over the events  ===============

    for (int i=0; i<nrec; i++) {  
      
      if(div(i, nticker).rem == 0 || i >= nrec-1) 
	{ 
	  portion=static_cast<float>(i) / static_cast<float>(nrec-1);
	  printf("Done %4.1f percent of reading the NTuple  ", portion*100.0);
	  cout.flush();   cout << char(13);  
	}
      
      last_rec=(i>=nrec-1); 
      
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
     reject_trigger++;}
   if (PassedEF_e15iEF_e15i || PassedEF_e25i || PassedEF_mu6 || PassedEF_mu6l || PassedEF_mu20i || PassedL1_XE100) {
     
	if (MET_RefFinal_et < METcut) reject_MET++;
	if (MET_RefFinal_et >= METcut) {
	  
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
	  
	  // nlepcut
	  if ((twoelectrons == 0 && twomuons == 0) || nlep != 2) {
	    reject_nlep++;}
	  
	  if ((twoelectrons == 1 || twomuons == 1) && nlep == 2) {
	    
	    //         afterNlep++;
	    
	    // continue only when a good pair has been found
	    if (accepted_m == 0 && accepted_e == 0 ) {reject_mZ++;;}
	    if (accepted_m+accepted_e > 0) {
	      if (nlep == 1) cout << "nlep == 1 and nmu = " << nmu << " nmel :" << nel << " accepted_mu = " << accepted_m << endl;  
	      
	      
	      //	   afterMz++;
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
	      
	      if (btagOK == 0) {reject_btag++;}
	      if (btagOK == 1) {
		
		// done with selection
		found++;
		
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
		w      = float(weight);
		
		// fill the preselection tree from the values calculated above
		treeS.Fill();
		
	      } // end if on btag
	    } // end if on mZ
	  } // end if on nlep > nlepcut
	} // end on METcut
      } // end on trigger cut
      
    } // end of for loop over events   
    delete random;
  
  double cut0 = 0.0;
  double cut1 = 0.0;
  double cut2= 0.0;
  double cut3= 0.0;
  double cut4= 0.0;
  double cut5= 0.0;
  double cut6= 0.0;
  
    outClientFile << "              " << endl;
    outClientFile << "Cut flow for: " << eventType << endl;
    outClientFile << "-----------------------------------------------------------" << endl;
    outClientFile << nrec << " records found in the chain. "<< endl;
    
    cout << endl;
    cout << "METcut " << METcut << endl;
    cout << "pt cuts for leptons: e > " << eptmin << " and mu: " << mptmin << " GeV and eta < " << etamin << endl;	  
    cout << "nlep strictly equal to "<< nlepcut << " of the same flavor" << endl; 
    cout << "mZ within " <<  mZcut << " GeV of on-shell mass " << endl;
    cout << "total # of selected events in file: \t" << found << endl;	  
    cout << endl;
    cout << "Cut flow for: " << eventType << endl;
    cout << "-----------------------------------------------------------" << endl;
    cout << nrec << " records found in the chain. "<< endl;
    cout << endl;
    
    passed = nrec;
    totalRejected = (reject_MET + reject_nlep + reject_trigger  + reject_mZ + reject_btag); 
    
    places = 2;
    cout <<fixed;                 
    cout.precision(places);
    
    cout << "                         " <<   endl;
    cout << "                         " << " cross-section              raw #                 percentage"    <<   endl;
    cout << "cross-section*BR in fb   " << 1000.*sigmaBR   <<"\t" << generated <<"\t" <<  100.*(generated/generated)<< "%" << endl;
    cut0 += 1000.*sigmaBR; 
    cout << "xs after filter:         " << filterEff*1000.*sigmaBR <<"\t" << passed  <<"\t" <<  100.*(passed/generated)<< "%" << endl;
    cut1 += filterEff*1000.*sigmaBR; 
    temp = passed;
    passed = (passed - reject_trigger);
    cout << "xs after trigger:        " << 1000.*sigmaBR*passed/generated <<"\t" << passed <<"\t" <<  100.*(passed/generated)<<"\t" << "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
    cut2 += 1000.*sigmaBR*passed/generated;
    temp = passed;
    passed = (passed - reject_MET);
    cout << "xs after MET cut         " << 1000.*sigmaBR*passed/generated <<"\t" << passed <<"\t" <<  100.*(passed/generated)<<"\t" << "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
    cut3 += 1000.*sigmaBR*passed/generated;
    temp = passed;
    passed = (passed - reject_nlep);
    cout << "xs after nlep cut:       " << 1000.*sigmaBR*passed/generated <<"\t" << passed <<"\t" <<  100.*(passed/generated)<<"\t" << "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
    cut4 += 1000.*sigmaBR*passed/generated;
    temp = passed;
    passed = (passed - reject_mZ);
    cout << "xs after mZ cut:         " << 1000.*sigmaBR*passed/generated <<"\t" << passed <<"\t" <<  100.*(passed/generated)<<"\t" << "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
    cut5 += 1000.*sigmaBR*passed/generated;
    temp = passed;
    passed = (passed - reject_btag);
    cout << "xs after b-tag cut:      " << 1000.*sigmaBR*passed/generated <<"\t" << passed <<"\t" <<  100.*(passed/generated)<<"\t" << "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
    cut6 += 1000.*sigmaBR*passed/generated;
    Total = passed*filterEff; 
    cout << endl;
    cout << "Total rejected (reweighted):  " << lumi*sigmaBR - 1000.*sigmaBR*passed/generated << " out of " << generated*filterEff << endl;
    cout << "Total rejected (raw)       :  " << generated - passed << " out of " << generated<< endl;
    cout << "Fraction retained          :  " << 100.*(passed/generated) << "% " << endl;
    places = 5;
    cout.precision(places);
    cout << endl;
    cout << "Events weight of " << weight << endl;
    cout << " "<< endl;
    cout <<fixed;                 
    places = 2;
    cout.precision(places);
    
    passed = nrec;
    
    outClientFile << "                         " <<   endl;
    outClientFile << "                         " << " cross-section              raw #                 percentage"    <<   endl;
    outClientFile << "cross-section*BR in fb   " << 1000.*sigmaBR   <<"\t" << generated <<"\t" <<  100.*(generated/generated)<< "%" << endl;
    outClientFile << "xs after filter:         " << filterEff*1000.*sigmaBR <<"\t" << passed  <<"\t" <<  100.*(passed/generated)<< "%" << endl;
    temp = passed;
    passed = (passed - reject_trigger);
    outClientFile << "xs after trigger:        " << 1000.*sigmaBR*passed/generated <<"\t" << passed <<"\t" <<  100.*(passed/generated)<<"\t" << "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
    temp = passed;
    passed = (passed - reject_MET);
    outClientFile << "xs after MET cut         " << 1000.*sigmaBR*passed/generated <<"\t" << passed <<"\t" <<  100.*(passed/generated)<<"\t" << "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
    temp = passed;
    passed = (passed - reject_nlep);
    outClientFile << "xs after nlep cut:       " << 1000.*sigmaBR*passed/generated <<"\t" << passed <<"\t" <<  100.*(passed/generated)<<"\t" << "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
    temp = passed;
    passed = (passed - reject_mZ);
    outClientFile << "xs after mZ cut:         " << 1000.*sigmaBR*passed/generated <<"\t" << passed <<"\t" <<  100.*(passed/generated)<<"\t" << "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
    temp = passed;
    passed = (passed - reject_btag);
    outClientFile << "xs after b-tag cut:      " << 1000.*sigmaBR*passed/generated <<"\t" << passed <<"\t" <<  100.*(passed/generated)<<"\t" << "%; rel. eff: " <<  100.*(passed/temp)<< "%" << endl;
    Total = passed*filterEff; 
    outClientFile << endl;
    outClientFile << "Total rejected (reweighted):  " << lumi*sigmaBR - 1000.*sigmaBR*passed/generated << " out of " << generated*filterEff << endl;
    outClientFile << "Total rejected (raw)       :  " << generated - passed << " out of " << generated<< endl;
    outClientFile << "Fraction retained          :  " << 100.*(passed/generated) << "% " << endl;
    places = 5;
    outClientFile.precision(places);
    outClientFile << endl;
    outClientFile << "Events weight of " << weight << endl;
    outClientFile << " "<< endl;
    outClientFile <<fixed;                 
    places = 2;
    outClientFile.precision(places);
    
  // check what the tree looks like
  treeS.Print();
  treeB.Print();
  

  f.Write();   // write to the file histograms and trees
  
  cout << " " << endl;
  cout << "All done!" << endl;
  
  theApp.Run();                            // concluding ROOT requirements.
  
//---------------------------------- end insertion of EVmva.C


   f.Write();   // write to the file histograms and trees

cout << " " << endl;
cout << "All done!" << endl;

  theApp.Run();                            // concluding ROOT requirements.


return 0;
} //  End of main(...)


