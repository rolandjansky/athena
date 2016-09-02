/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Original: Darren.Price@CERN.CH
// Modified: Maximiliano.Sioli@CERN.CH
//           Stefanos.Leontsinis@CERN.CH
//

#include <TROOT.h>
#include "TMinuit.h"
#include <TChain.h>
#include <TFile.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <sstream>
#include <stdexcept>
#include <TH2.h>
#include <TH3.h>
#include <TF1.h>
#include <TCut.h>
#include <TMath.h>
#include <TStyle.h>
#include <TFitResult.h>
#include <TCanvas.h>
#include <TColor.h>
#include "TLegend.h"
#include "TGraph.h"
#include "stdio.h"
#include <TLatex.h>
#include <TString.h>
#include "TGraphAsymmErrors.h"
#include "TEfficiency.h"
#include <vector>
#include <TRandom3.h>
#include <TGaxis.h>
#include <algorithm>
#include <TSystem.h>

#include "RooGlobalFunc.h"
#include "RooExtendPdf.h"
#include "RooRealVar.h"
#include "RooStats/SPlot.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooChebychev.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"
#include "RooPolynomial.h"
#include "RooDataHist.h"
#include "RooAddition.h"
#include "RooProduct.h"
#include "TCanvas.h"
#include "RooAbsPdf.h"
#include "RooCategory.h"
#include "RooSimultaneous.h"
#include "RooFit.h"
#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "RooConstVar.h"
#include "RooStats/ModelConfig.h"

using namespace std;
using namespace RooFit;
using namespace RooStats;

bool verbose = true;

bool makePDFfiles = false;
bool makeEPSfiles = false;
bool makePNGfiles = true;
bool makeROOTfiles = false;

bool massPlots = true;
bool useTree = true;
bool integral = true;

bool plotQEta(true);
bool plotEtaPosMu(false);
bool plotEtaNegMu(false);

// Binning as from MuonTPTool
/* float pt_bins[] = {0., 0.5, 1., 1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 5.5, 6., 6.5, 7., 7.5, 8., 9., 10., 12., 14., 16., 18., 20.}; */
/* float eta_bins[] = {-2.5, -2.25, -2., -1.75, -1.5, -1.25, -1., -0.75, -0.5, -0.25, 0., 0.25, 0.5, 0.75, 1., 1.25, 1.5, 1.75, 2., 2.25, 2.5}; */

// Temporary binning for week-1
/* float pt_bins[] = {3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5}; */
/* float eta_bins[] = {-2.5, -1.5, -0.5, 0.0, 0.5, 1.5, 2.5}; */

// Temporary binning for period C
/* float pt_bins[] = {3.5, 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 15.}; */
/* float eta_bins[] = {-2.5, -2., -1.5, -1., -0.5, -0.25, 0., 0.25, 0.5, 1., 1.5, 2., 2.5}; */

// Binning for integral
/* float pt_bins[] = {3.5, 15.}; */
/* float eta_bins[] = {-2.5, 2.5}; */

// Binning to match Z analysis
/* float pt_bins[] = {3.5, 4., 5., 6., 7., 8., 9., 10., 12., 14., 16., 18., 20.}; */
/* float eta_bins[] = {-2.5, -2.0, -1.5, -1.2, -0.6, -0.15, 0.0, 0.15, 0.6, 1.2, 1.5, 2.0, 2.5}; */

// Binning for Z+Jpsi matching studies
/* float pt_bins[] = {10., 20.}; */
/* float eta_bins[] = {-2.5, -2.0, -1.3, -1.05, -0.1, 0.0, 0.1, 1.05, 1.3, 2.0, 2.5}; */
/* float pt_bins[] = {15., 20.}; */
/* float pt_bins[] = {3.5, 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 15., 20., 30., 40., 50.}; */
/* float eta_bins[] = {0.0, 0.1, 1.05, 1.3, 2.0, 2.5}; */

float pt_bins[] = {3.5, 4., 5., 6., 7., 8., 10., 12., 15., 20.};
//float pt_bins[] = {12., 15.};
//float eta_bins[] = {0.1, 2.5};
float eta_bins[] = {-2.5, -2.0, -1.3, -1.05, -0.1, 0.0, 0.1, 1.05, 1.3, 2.0, 2.5};

int pt_num;
int eta_num;

///////////////////////////////////////////////////////////////////////////////////////////////

inline TString stringify(Double_t x)
{
  std::ostringstream o;
  if (!(o << x))
    cout << "stringify(Double_t)" << endl;
  return o.str();
}

void set_plot_style();
vector<double> findefficiency(TChain * chain, TString fitfunction, TString mode, int index1, int index2, TString dataSample, TString probeType, TString muType);
vector<double> findefficiency(TChain * chain, TString fitfunction, TString mode, int index1, TString dataSample, TString probeType, TString muType);
vector<TString> translateBoundaries(TString mode, int index1, int index2);

TString resultlabel = "NULL";

// ntuple variables

UInt_t runNumber;
ULong64_t eventNumber;
UInt_t lumiblock;
float average_mu;
float actual_mu;
int PV_n;
float mcEventWeight;

float probe_pt;
float probe_eta;
float probe_phi;
int probe_type;
int probe_author;
int probe_quality;
int probe_fineEtaPhi;
int probe_detRegion;
int probe_q;
float probe_d0;
float probe_d0err;
float probe_z0;
float probe_dRJet;

float tag_pt;
float tag_eta;
float tag_phi;
int tag_q;
float tag_d0;
float tag_d0err;
float tag_z0;
float tag_dRJet;

float dilep_mll;
float dilep_dphi;
float dilep_deta;
float dilep_dphi_exTP;
float dilep_deta_exTP;
float dilep_pt;
float dilep_tof;

// matched

bool matched_LooseMuons;
bool matched_MediumMuons;
bool matched_TightMuons;
bool matched_IDTracks;
float probe_dRMatch_LooseMuons;
float probe_dRMatch_MediumMuons;
float probe_dRMatch_TightMuons;
float probe_dRMatch_IDTracks;

// trigger

bool HLT_mu4_bJpsi_Trkloose;
bool HLT_mu6_bJpsi_Trkloose;
bool HLT_mu10_bJpsi_Trkloose;
bool HLT_mu18_bJpsi_Trkloose;
bool tag_matched_HLT_mu4_bJpsi_Trkloose;
bool tag_matched_HLT_mu6_bJpsi_Trkloose;
bool tag_matched_HLT_mu10_bJpsi_Trkloose;
bool tag_matched_HLT_mu18_bJpsi_Trkloose;
float tag_dRMatch_HLT_mu4_bJpsi_Trkloose;
float tag_dRMatch_HLT_mu6_bJpsi_Trkloose;
float tag_dRMatch_HLT_mu10_bJpsi_Trkloose;
float tag_dRMatch_HLT_mu18_bJpsi_Trkloose;

bool HLT_mu4;
bool HLT_mu6;
bool HLT_mu6_idperf;
bool HLT_mu14;
bool tag_matched_HLT_mu4;
bool tag_matched_HLT_mu6;
bool tag_matched_HLT_mu6_idperf;
bool tag_matched_HLT_mu14;
float tag_dRMatch_HLT_mu4;
float tag_dRMatch_HLT_mu6;
float tag_dRMatch_HLT_mu6_idperf;
float tag_dRMatch_HLT_mu14;

bool L1_MU4;
bool L1_MU6;
bool tag_matched_L1_MU4;
bool tag_matched_L1_MU6;
float tag_dRMatch_L1_MU4;
float tag_dRMatch_L1_MU6;

bool HLT_mu4_mu4_idperf_bJpsimumu_noid;
bool tag_matched_HLT_mu4_mu4_idperf_bJpsimumu_noid;
float tag_dRMatch_HLT_mu4_mu4_idperf_bJpsimumu_noid;

// dataSample = data, mc-p, mc-np
// probeType = IDProbes, CaloProbes (MSProbes_ID)
// muType = LooseMuons, MediumMuons, TightMuons (IDTracks)

void JPsiFitter(TString dataSample = "data", TString probeType = "IDProbes", TString muType = "MediumMuons"){ 

  if (!verbose) RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL) ; // DEBUG=0, INFO=1, PROGRESS=2, WARNING=3, ERROR=4, FATAL=5

  pt_num = sizeof (pt_bins) / sizeof (float) - 1;
  eta_num = sizeof (eta_bins) / sizeof (float) - 1;

  cout  << pt_num << " bins in pt " << endl;
  cout  << eta_num << " bins in eta " << endl;
  
  // BUILD DIRECTORY STRUCTURES
  cout << "BUILDING NECESSARY DIRECTORY STRUCTURES" << endl;
  gSystem->Exec("mkdir EffResults/");
  gSystem->Exec("mkdir EffResults/"+probeType+"_"+muType+"");
  gSystem->Exec("mkdir EffResults/"+probeType+"_"+muType+"/SummaryPlots");

  if ( massPlots ) {
    gSystem->Exec("mkdir EffResults/"+probeType+"_"+muType+"/MassPlots");
    gSystem->Exec("mkdir EffResults/"+probeType+"_"+muType+"/MassPlots/gp2");
    gSystem->Exec("mkdir EffResults/"+probeType+"_"+muType+"/MassPlots/gp3");
    gSystem->Exec("mkdir EffResults/"+probeType+"_"+muType+"/MassPlots/doublegp2");
    gSystem->Exec("mkdir EffResults/"+probeType+"_"+muType+"/MassPlots/gp2/1D");
    gSystem->Exec("mkdir EffResults/"+probeType+"_"+muType+"/MassPlots/gp3/1D");
    gSystem->Exec("mkdir EffResults/"+probeType+"_"+muType+"/MassPlots/doublegp2/1D");
  }
    
  gStyle->SetTitleBorderSize(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetPadColor(kWhite);
  gStyle->SetHistLineWidth(2);
  gStyle->SetTextFont(42);
    
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetPadColor(kWhite);
    
  gROOT->LoadMacro("AtlasUtils.C");
  set_plot_style();
  TGaxis::SetMaxDigits(3);
    
  resultlabel= "recoeff";

  TString dataBase = "/gpfs_data/local/atlas/sioli/NTuples/v009/";

  TChain * chain = new TChain("JPsiTPReco/Trees/"+probeType+"_JPsi_OC/TPTree_"+probeType+"_JPsi_OC");

  if (dataSample == "dataC") {
    chain->Add(dataBase+"periodC/270806/*");
    chain->Add(dataBase+"periodC/270953/*");
    chain->Add(dataBase+"periodC/271048/*");
    chain->Add(dataBase+"periodC/271298/*");
    chain->Add(dataBase+"periodC/271421/*");
    chain->Add(dataBase+"periodC/271516/*");
    chain->Add(dataBase+"periodC/271595/*");
    chain->Add(dataBase+"periodC/271744/*");
  }
  
  if (dataSample == "dataDE") {
    chain->Add(dataBase+"periodD/276262/*");
    chain->Add(dataBase+"periodD/276329/*");
    chain->Add(dataBase+"periodD/276330/*");
    chain->Add(dataBase+"periodD/276336/*");
    chain->Add(dataBase+"periodD/276416/*");
    chain->Add(dataBase+"periodD/276511/*");
    chain->Add(dataBase+"periodD/276689/*");
    chain->Add(dataBase+"periodD/276778/*");
    chain->Add(dataBase+"periodD/276790/*");
    chain->Add(dataBase+"periodD/276952/*");
    chain->Add(dataBase+"periodD/276954/*");

    chain->Add(dataBase+"periodE/278880/*");
    chain->Add(dataBase+"periodE/278912/*");
    chain->Add(dataBase+"periodE/278968/*");
    chain->Add(dataBase+"periodE/279169/*");
    chain->Add(dataBase+"periodE/279259/*");
    chain->Add(dataBase+"periodE/279279/*");
    chain->Add(dataBase+"periodE/279284/*");
    chain->Add(dataBase+"periodE/279598/*");
  }
  
  if (dataSample == "dataDspecial") {
    chain->Add(dataBase+"periodD/276731/*");
  }

  if (dataSample == "mc50") chain->Add(dataBase+"Jpsimu2mu2-prompt-50ns/*");
  if (dataSample == "mc50-np") chain->Add(dataBase+"Jpsimu2p5mu2p5-nonprompt-50ns/*");
  if (dataSample == "mc25") chain->Add(dataBase+"Jpsimu4mu4-prompt-25ns/*");

  //  TFile *mainfile = new TFile(dataBase+dataSample+".root","READ");

  cout << " " << endl;
  cout << "Efficiencies of " << muType << " using " << probeType << " in " << dataSample << endl;
  cout << " " << endl;

  chain->SetBranchAddress("runNumber",&runNumber);
  chain->SetBranchAddress("eventNumber",&eventNumber);
  chain->SetBranchAddress("lumiblock",&lumiblock);
  chain->SetBranchAddress("average_mu",&average_mu);
  chain->SetBranchAddress("actual_mu",&actual_mu);
  chain->SetBranchAddress("PV_n",&PV_n);
  chain->SetBranchAddress("mcEventWeight",&mcEventWeight);
  
  chain->SetBranchAddress("probe_pt",&probe_pt);
  chain->SetBranchAddress("probe_eta",&probe_eta);
  chain->SetBranchAddress("probe_phi",&probe_phi);
  chain->SetBranchAddress("probe_type",&probe_type);
  chain->SetBranchAddress("probe_author",&probe_author);
  chain->SetBranchAddress("probe_quality",&probe_quality);
  chain->SetBranchAddress("probe_fineEtaPhi",&probe_fineEtaPhi);
  chain->SetBranchAddress("probe_detRegion",&probe_detRegion);
  chain->SetBranchAddress("probe_q",&probe_q);
  chain->SetBranchAddress("probe_d0",&probe_d0);
  chain->SetBranchAddress("probe_d0err",&probe_d0err);
  chain->SetBranchAddress("probe_z0",&probe_z0);
  chain->SetBranchAddress("probe_dRJet",&probe_dRJet);
    
  chain->SetBranchAddress("tag_pt",&tag_pt);
  chain->SetBranchAddress("tag_eta",&tag_eta);
  chain->SetBranchAddress("tag_phi",&tag_phi);
  chain->SetBranchAddress("tag_q",&tag_q);
  chain->SetBranchAddress("tag_d0",&tag_d0);
  chain->SetBranchAddress("tag_d0err",&tag_d0err);
  chain->SetBranchAddress("tag_z0",&tag_z0);
  chain->SetBranchAddress("tag_dRJet",&tag_dRJet);

  chain->SetBranchAddress("dilep_mll",&dilep_mll);
  chain->SetBranchAddress("dilep_dphi",&dilep_dphi);
  chain->SetBranchAddress("dilep_deta",&dilep_deta);
  chain->SetBranchAddress("dilep_dphi_exTP",&dilep_dphi_exTP);
  chain->SetBranchAddress("dilep_deta_exTP",&dilep_deta_exTP);
  chain->SetBranchAddress("dilep_pt",&dilep_pt);
  chain->SetBranchAddress("dilep_tof",&dilep_tof);

  if (probeType == "IDProbes" || probeType == "CaloProbes") {
    chain->SetBranchAddress("HLT_mu4",&HLT_mu4);
    chain->SetBranchAddress("HLT_mu6",&HLT_mu6);
    chain->SetBranchAddress("HLT_mu6_idperf",&HLT_mu6_idperf);
    chain->SetBranchAddress("HLT_mu14",&HLT_mu14);
    chain->SetBranchAddress("tag_matched_HLT_mu4",&tag_matched_HLT_mu4);
    chain->SetBranchAddress("tag_matched_HLT_mu6",&tag_matched_HLT_mu6);
    chain->SetBranchAddress("tag_matched_HLT_mu6_idperf",&tag_matched_HLT_mu6_idperf);
    chain->SetBranchAddress("tag_matched_HLT_mu14",&tag_matched_HLT_mu14);
    chain->SetBranchAddress("tag_dRMatch_HLT_mu4",&tag_dRMatch_HLT_mu4);
    chain->SetBranchAddress("tag_dRMatch_HLT_mu6",&tag_dRMatch_HLT_mu6);
    chain->SetBranchAddress("tag_dRMatch_HLT_mu6_idperf",&tag_dRMatch_HLT_mu6_idperf);
    chain->SetBranchAddress("tag_dRMatch_HLT_mu14",&tag_dRMatch_HLT_mu14);
    chain->SetBranchAddress("L1_MU4",&L1_MU4);
    chain->SetBranchAddress("L1_MU6",&L1_MU6);
    chain->SetBranchAddress("tag_matched_L1_MU4",&tag_matched_L1_MU4);
    chain->SetBranchAddress("tag_matched_L1_MU6",&tag_matched_L1_MU6);
    chain->SetBranchAddress("tag_dRMatch_L1_MU4",&tag_dRMatch_L1_MU4);
    chain->SetBranchAddress("tag_dRMatch_L1_MU6",&tag_dRMatch_L1_MU6);
  }

  if (probeType == "IDProbes" || probeType == "CaloProbes") {
    chain->SetBranchAddress("probe_matched_LooseMuons",&matched_LooseMuons);
    chain->SetBranchAddress("probe_matched_MediumMuons",&matched_MediumMuons);
    chain->SetBranchAddress("probe_matched_TightMuons",&matched_TightMuons);
    chain->SetBranchAddress("probe_dRMatch_LooseMuons",&probe_dRMatch_LooseMuons);
    chain->SetBranchAddress("probe_dRMatch_MediumMuons",&probe_dRMatch_MediumMuons);
    chain->SetBranchAddress("probe_dRMatch_TightMuons",&probe_dRMatch_TightMuons);
    chain->SetBranchAddress("HLT_mu4_bJpsi_Trkloose",&HLT_mu4_bJpsi_Trkloose);
    chain->SetBranchAddress("HLT_mu6_bJpsi_Trkloose",&HLT_mu6_bJpsi_Trkloose);
    chain->SetBranchAddress("HLT_mu10_bJpsi_Trkloose",&HLT_mu10_bJpsi_Trkloose);
    chain->SetBranchAddress("HLT_mu18_bJpsi_Trkloose",&HLT_mu18_bJpsi_Trkloose);
    chain->SetBranchAddress("tag_matched_HLT_mu4_bJpsi_Trkloose",&tag_matched_HLT_mu4_bJpsi_Trkloose);
    chain->SetBranchAddress("tag_matched_HLT_mu6_bJpsi_Trkloose",&tag_matched_HLT_mu6_bJpsi_Trkloose);
    chain->SetBranchAddress("tag_matched_HLT_mu10_bJpsi_Trkloose",&tag_matched_HLT_mu10_bJpsi_Trkloose);
    chain->SetBranchAddress("tag_matched_HLT_mu18_bJpsi_Trkloose",&tag_matched_HLT_mu18_bJpsi_Trkloose);
    chain->SetBranchAddress("tag_dRMatch_HLT_mu4_bJpsi_Trkloose",&tag_dRMatch_HLT_mu4_bJpsi_Trkloose);
    chain->SetBranchAddress("tag_dRMatch_HLT_mu6_bJpsi_Trkloose",&tag_dRMatch_HLT_mu6_bJpsi_Trkloose);
    chain->SetBranchAddress("tag_dRMatch_HLT_mu10_bJpsi_Trkloose",&tag_dRMatch_HLT_mu10_bJpsi_Trkloose);
    chain->SetBranchAddress("tag_dRMatch_HLT_mu18_bJpsi_Trkloose",&tag_dRMatch_HLT_mu18_bJpsi_Trkloose);
  }

  if (probeType == "MSProbes_ID") {
    chain->SetBranchAddress("probe_matched_IDTracks",&matched_IDTracks);
    chain->SetBranchAddress("probe_dRMatch_IDTracks",&probe_dRMatch_IDTracks);
    chain->SetBranchAddress("HLT_mu4_mu4_idperf_bJpsimumu_noid",&HLT_mu4_mu4_idperf_bJpsimumu_noid);
    chain->SetBranchAddress("tag_matched_HLT_mu4_mu4_idperf_bJpsimumu_noid",&tag_matched_HLT_mu4_mu4_idperf_bJpsimumu_noid);
    chain->SetBranchAddress("tag_dRMatch_HLT_mu4_mu4_idperf_bJpsimumu_noid",&tag_dRMatch_HLT_mu4_mu4_idperf_bJpsimumu_noid);
  }

  // summary efficiency plots (1D and 2D)
  TH2F * h_eta_pt = new TH2F("h_eta_pt",";;",eta_num,eta_bins,pt_num,pt_bins);
  TH1F * h_eta = new TH1F("h_eta",";;",eta_num,eta_bins);
  TH1F * h_pt = new TH1F("h_pt",";;",pt_num,pt_bins);
  TH2F * h_totalErrUp_eta_pt = new TH2F("h_totalErrUp_eta_pt",";;",eta_num,eta_bins,pt_num,pt_bins);
  TH1F * h_totalErrUp_eta = new TH1F("h_totalErrUp_eta",";;",eta_num,eta_bins);
  TH1F * h_totalErrUp_pt = new TH1F("h_totalErrUp_pt",";;",pt_num,pt_bins);
  TH2F * h_totalErrDown_eta_pt = new TH2F("h_totalErrDown_eta_pt",";;",eta_num,eta_bins,pt_num,pt_bins);
  TH1F * h_totalErrDown_eta = new TH1F("h_totalErrDown_eta",";;",eta_num,eta_bins);
  TH1F * h_totalErrDown_pt = new TH1F("h_totalErrDown_pt",";;",pt_num,pt_bins);
  TH2F * h_errSyst_eta_pt = new TH2F("h_errSyst_eta_pt",";;",eta_num,eta_bins,pt_num,pt_bins);
  TH1F * h_errSyst_eta = new TH1F("h_errSyst_eta",";;",eta_num,eta_bins);
  TH1F * h_errSyst_pt = new TH1F("h_errSyst_pt",";;",pt_num,pt_bins);

  //////////////////////////////////////////////////////////////////////
  /////////////////////////// FITTING DONE HERE ////////////////////////
  //////////////////////////////////////////////////////////////////////
    
  TString fitfunction = "gp2"; // gp2, doublegp2, gp3, cb

  vector<double> effAndError;
  vector<double> effAndError_signalSyst;
  vector<double> effAndError_bkgSyst;

  if ( integral ) {

    for (int i=0;i<eta_num;++i) {
      effAndError = findefficiency(chain, fitfunction, "eta", i, dataSample, probeType, muType);
      effAndError_signalSyst = findefficiency(chain, "doublegp2", "eta", i, dataSample, probeType, muType);
      effAndError_bkgSyst = findefficiency(chain, "gp3", "eta", i, dataSample, probeType, muType);
      Double_t maxSyst = 0.0;
      Double_t minSyst = 0.0;
      Double_t sigDiff = effAndError_signalSyst.at(0)-effAndError.at(0);
      Double_t bkgDiff = effAndError_bkgSyst.at(0)-effAndError.at(0);
      if (sigDiff>maxSyst) maxSyst = sigDiff;
      if (bkgDiff>maxSyst) maxSyst = bkgDiff;
      if (sigDiff<minSyst) minSyst = sigDiff;
      if (bkgDiff<minSyst) minSyst = bkgDiff;
      if (verbose) cout << "eta bin " << i  << " effAndError = " << setprecision(2) << effAndError.at(0) << " +/- " << setprecision(2) << effAndError.at(1) << endl;
      h_eta->SetBinContent(i+1,effAndError.at(0));
      h_eta->SetBinError(i+1,effAndError.at(1));
      h_totalErrUp_eta->SetBinContent(i+1,sqrt(maxSyst*maxSyst+effAndError.at(1)*effAndError.at(1)) );
      h_totalErrDown_eta->SetBinContent(i+1,sqrt(minSyst*minSyst+effAndError.at(1)*effAndError.at(1)) );
      Double_t errSyst = TMath::Max(fabs(maxSyst),fabs(minSyst));
      h_errSyst_eta->SetBinContent(i+1,effAndError.at(0));
      h_errSyst_eta->SetBinError(i+1,sqrt(errSyst*errSyst+effAndError.at(1)*effAndError.at(1)) );
    }

    for (int i=0;i<pt_num;++i) {
      effAndError = findefficiency(chain, fitfunction, "pt", i, dataSample, probeType, muType);
      effAndError_signalSyst = findefficiency(chain, "doublegp2", "pt", i, dataSample, probeType, muType);
      effAndError_bkgSyst = findefficiency(chain, "gp3", "pt", i, dataSample, probeType, muType);
      Double_t maxSyst = 0.0;
      Double_t minSyst = 0.0;
      Double_t sigDiff = effAndError_signalSyst.at(0)-effAndError.at(0);
      Double_t bkgDiff = effAndError_bkgSyst.at(0)-effAndError.at(0);
      if (sigDiff>maxSyst) maxSyst = sigDiff;
      if (bkgDiff>maxSyst) maxSyst = bkgDiff;
      if (sigDiff<minSyst) minSyst = sigDiff;
      if (bkgDiff<minSyst) minSyst = bkgDiff;
      if (verbose) cout << "pt bin " << i << " effAndError = " << setprecision(2) << effAndError.at(0) << " +/- " << setprecision(2) << effAndError.at(1) << endl;
      h_pt->SetBinContent(i+1,effAndError.at(0));
      h_pt->SetBinError(i+1,effAndError.at(1));
      h_totalErrUp_pt->SetBinContent(i+1,sqrt(maxSyst*maxSyst+effAndError.at(1)*effAndError.at(1)) );
      h_totalErrDown_pt->SetBinContent(i+1,sqrt(minSyst*minSyst+effAndError.at(1)*effAndError.at(1)) );
      Double_t errSyst = TMath::Max(fabs(maxSyst),fabs(minSyst));
      h_errSyst_pt->SetBinContent(i+1,effAndError.at(0));
      h_errSyst_pt->SetBinError(i+1,sqrt(errSyst*errSyst+effAndError.at(1)*effAndError.at(1)) );
    }

  }

  //2D
  for (int e=0;e<eta_num;++e) {
    for (int p=0;p<pt_num;++p) {
      effAndError = findefficiency(chain, fitfunction, "etapt", e, p, dataSample, probeType, muType);
      effAndError_signalSyst = findefficiency(chain, "doublegp2", "etapt", e,p, dataSample, probeType, muType);
      effAndError_bkgSyst = findefficiency(chain, "gp3", "etapt", e, p, dataSample, probeType, muType);
      Double_t maxSyst = 0.0;
      Double_t minSyst = 0.0;
      Double_t sigDiff = effAndError_signalSyst.at(0)-effAndError.at(0);
      Double_t bkgDiff = effAndError_bkgSyst.at(0)-effAndError.at(0);
      if (sigDiff>maxSyst) maxSyst = sigDiff;
      if (bkgDiff>maxSyst) maxSyst = bkgDiff;
      if (sigDiff<minSyst) minSyst = sigDiff;
      if (bkgDiff<minSyst) minSyst = bkgDiff;
      if (verbose) {
	cout << "eta:pT bin " << e << " , " << p << " effAndError = " << setprecision(2) << effAndError.at(0) << " +/- " << setprecision(2) << effAndError.at(1)
	     << " +" << setprecision(2) << maxSyst << " " << setprecision(2) << minSyst << " (syst)" << endl;
      }
      h_eta_pt->SetBinContent(e+1,p+1,effAndError.at(0));
      h_eta_pt->SetBinError(e+1,p+1,effAndError.at(1));
      h_totalErrUp_eta_pt->SetBinContent(e+1,p+1,sqrt(maxSyst*maxSyst+effAndError.at(1)*effAndError.at(1)) );
      h_totalErrDown_eta_pt->SetBinContent(e+1,p+1,sqrt(minSyst*minSyst+effAndError.at(1)*effAndError.at(1)) );
      Double_t errSyst = TMath::Max(fabs(maxSyst),fabs(minSyst));
      h_errSyst_eta_pt->SetBinContent(e+1,p+1,effAndError.at(0));
      h_errSyst_eta_pt->SetBinError(e+1,p+1,sqrt(errSyst*errSyst+effAndError.at(1)*effAndError.at(1)) );
    }
  }

  //////////////////////////////////////////////////////////////////////
    
  // eta plot

  TCanvas* c_eta = new TCanvas("c_eta","c_eta",800,600);
  c_eta->cd(1);
    
  h_eta->GetXaxis()->SetNdivisions(505);h_eta->GetYaxis()->SetNdivisions(505);
  h_eta->GetXaxis()->SetTitle("#eta^{#mu}");
  h_eta->GetYaxis()->SetTitle("Reco efficiency");
  h_eta->SetMaximum( h_eta->GetMaximum()*1.2);h_eta->SetMinimum(0);
  h_eta->SetLineColor(2);h_eta->SetLineWidth(2);h_eta->SetMarkerStyle(20);h_eta->SetMarkerColor(1);h_eta->SetMarkerSize(1.2);
  h_eta->Draw("E1P");
    
  TLegend *leg_eta = new TLegend(0.6,0.65,0.9,0.7);
  leg_eta->SetHeader( " p_{T}^{#mu} > 3.5 GeV " );
  leg_eta->SetTextSize(0.03);
  leg_eta->SetFillColor(10);leg_eta->SetLineColor(10);
  leg_eta->Draw("same");
    
  TGraphAsymmErrors *asymm_eta = new TGraphAsymmErrors(h_eta->GetNbinsX());
  for (int i=1;i<=h_eta->GetNbinsX()+1;++i) {
    double centre = h_eta->GetBinLowEdge(i)+(h_eta->GetBinLowEdge(i+1)-h_eta->GetBinLowEdge(i))/2.;
    asymm_eta->SetPoint(i-1,centre,h_eta->GetBinContent(i));
    asymm_eta->SetPointError(i-1,centre-h_eta->GetBinLowEdge(i),h_eta->GetBinLowEdge(i+1)-centre,fabs(h_totalErrDown_eta->GetBinContent(i)),fabs(h_totalErrUp_eta->GetBinContent(i)));
  }
  asymm_eta->SetLineColor(1);asymm_eta->SetLineWidth(3);asymm_eta->SetMarkerStyle(20);asymm_eta->SetMarkerColor(1);asymm_eta->SetMarkerSize(1.2);
  asymm_eta->SetFillColor(kBlue-3);asymm_eta->SetFillStyle(3004);
  asymm_eta->Draw("Pe2 same");
  h_eta->Draw("E1P same");
    
  if (makePDFfiles) c_eta->Print("EffResults/"+probeType+"_"+muType+"/SummaryPlots/"+resultlabel+"_eta.pdf");
  if (makeEPSfiles) c_eta->Print("EffResults/"+probeType+"_"+muType+"/SummaryPlots/"+resultlabel+"_eta.eps");
  if (makePNGfiles) c_eta->Print("EffResults/"+probeType+"_"+muType+"/SummaryPlots/"+resultlabel+"_eta.png");

  // pt plot
    
  TCanvas* c_pt = new TCanvas("c_pt","c_pt",800,600);
  c_pt->cd(1);
    
  h_pt->GetXaxis()->SetNdivisions(505);h_pt->GetYaxis()->SetNdivisions(505);
  h_pt->GetXaxis()->SetTitle("p_{T}^{#mu} [GeV]");
  h_pt->GetYaxis()->SetTitle("Reco efficiency");
  h_pt->SetMaximum( h_pt->GetMaximum()*1.2);h_pt->SetMinimum(0.0);
  h_pt->SetLineColor(2);h_pt->SetLineWidth(2);h_pt->SetMarkerStyle(20);h_pt->SetMarkerColor(1);h_pt->SetMarkerSize(1.2);
  //  gPad->SetLogx(1);
  h_pt->GetXaxis()->SetMoreLogLabels();
  h_pt->Draw("E1P");
    
  TLegend *leg_pt = new TLegend(0.6,0.6,0.9,0.65);
  leg_pt->SetHeader( "|#eta_{#mu}| < 2.5" );
  leg_pt->SetTextSize(0.03);
  leg_pt->SetFillColor(10);leg_pt->SetLineColor(10);
  leg_pt->Draw("same");
    
  TGraphAsymmErrors *asymm_pt = new TGraphAsymmErrors(h_pt->GetNbinsX());
  for (int i=1;i<=h_pt->GetNbinsX()+1;++i) {
    double centre = h_pt->GetBinLowEdge(i)+(h_pt->GetBinLowEdge(i+1)-h_pt->GetBinLowEdge(i))/2.;
    asymm_pt->SetPoint(i-1,centre,h_pt->GetBinContent(i));
    asymm_pt->SetPointError(i-1,centre-h_pt->GetBinLowEdge(i),h_pt->GetBinLowEdge(i+1)-centre,fabs(h_totalErrDown_pt->GetBinContent(i)),fabs(h_totalErrUp_pt->GetBinContent(i)));
  }
  asymm_pt->SetLineColor(1);asymm_pt->SetLineWidth(3);asymm_pt->SetMarkerStyle(20);asymm_pt->SetMarkerColor(1);asymm_pt->SetMarkerSize(1.2);
  asymm_pt->SetFillColor(kBlue-3);asymm_pt->SetFillStyle(3004);
  asymm_pt->Draw("Pe2 same");
  h_pt->Draw("E1P same");
    
  if (makePDFfiles) c_pt->Print("EffResults/"+probeType+"_"+muType+"/SummaryPlots/"+resultlabel+"_pt.pdf");
  if (makeEPSfiles) c_pt->Print("EffResults/"+probeType+"_"+muType+"/SummaryPlots/"+resultlabel+"_pt.eps");
  if (makePNGfiles) c_pt->Print("EffResults/"+probeType+"_"+muType+"/SummaryPlots/"+resultlabel+"_pt.png");

  // eta-pt plot

  TCanvas* c_eta_pt = new TCanvas("c_eta_pt","c_eta_pt",800,600);
  c_eta_pt->cd(1);
    
  h_eta_pt->GetXaxis()->SetNdivisions(505);h_eta_pt->GetYaxis()->SetNdivisions(505);
  h_eta_pt->GetXaxis()->SetTitle("#eta^{#mu}");
  h_eta_pt->GetYaxis()->SetTitle("p_{T}^{#mu} [GeV]");
  h_eta_pt->GetZaxis()->SetTitle("Reco efficiency");

  gPad->SetLeftMargin(0.17);
  gPad->SetRightMargin(0.17);
  h_eta_pt->GetYaxis()->SetMoreLogLabels();
  h_eta_pt->SetMaximum(1.0);
  h_eta_pt->SetMinimum(0.0);
  h_eta_pt->SetLineColor(2);h_eta_pt->SetLineWidth(2);h_eta_pt->SetMarkerStyle(20);h_eta_pt->SetMarkerColor(1);h_eta_pt->SetMarkerSize(1.2);
  h_eta_pt->Draw("colz");

  if (makePDFfiles) c_eta_pt->Print("EffResults/"+probeType+"_"+muType+"/SummaryPlots/"+resultlabel+"_etapt.pdf");
  if (makeEPSfiles) c_eta_pt->Print("EffResults/"+probeType+"_"+muType+"/SummaryPlots/"+resultlabel+"_etapt.eps");
  if (makePNGfiles) c_eta_pt->Print("EffResults/"+probeType+"_"+muType+"/SummaryPlots/"+resultlabel+"_etapt.png");
    
  // dump histograms into root file

  TFile *reco_eff_results_file;
  reco_eff_results_file = TFile::Open( "EffResults/"+probeType+"_"+muType+"/"+resultlabel+"_etapt_"+fitfunction+".root", "RECREATE" );
  std::cout <<"Writing reco_eff_results to: "<< reco_eff_results_file->GetName() << std::endl;

  h_eta->Write("hEta");
  h_totalErrUp_eta->Write(resultlabel+"_uptotalerror_eta_"+fitfunction);
  h_totalErrDown_eta->Write(resultlabel+"_downtotalerror_eta_"+fitfunction);  
  h_errSyst_eta->Write("hErrSystEta");

  h_pt->Write("hPt");
  h_totalErrUp_pt->Write(resultlabel+"_uptotalerror_pt_"+fitfunction);
  h_totalErrDown_pt->Write(resultlabel+"_downtotalerror_pt_"+fitfunction);  
  h_errSyst_pt->Write("hErrSystPt");

  h_eta_pt->Write(resultlabel+"_centralvalue_"+fitfunction);
  h_totalErrUp_eta_pt->Write(resultlabel+"_uptotalerror_etapt_"+fitfunction);
  h_totalErrDown_eta_pt->Write(resultlabel+"_downtotalerror_etapt_"+fitfunction);
  h_errSyst_eta_pt->Write("hErrSystEtaPt");

  reco_eff_results_file->Close();

  return;
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// end of main function
////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<double> findefficiency(TChain * chain, TString fitfunction, TString mode, int index, TString dataSample, TString probeType, TString muType) {
  return findefficiency(chain, fitfunction, mode, index, -1, dataSample, probeType, muType);
}

vector<double> findefficiency(TChain * chain, TString fitfunction, TString mode, int index1, int index2, TString dataSample, TString probeType, TString muType) {

  TString index1_long = "";
  TString index2_long = "";
  TString mainindex_long = "";
  if (mode=="pt" || mode =="eta") {
    if (index1<10) {
      index1_long = "_"+mode+"_00"+stringify(index1);
    }
    else if (index1<100) {
      index1_long = "_"+mode+"_0"+stringify(index1);
    }
    mainindex_long = index1_long;
  } else {
    if (index1<10) {
      index1_long = "_"+mode+"_00"+stringify(index1);
    }
    else if (index1<100) {
      index1_long = "_"+mode+"_0"+stringify(index1);
    }
    if (index2<10) {
      index2_long = "_"+mode+"_00"+stringify(index2);
    }
    else if (index2<100) {
      index2_long = "_"+mode+"_0"+stringify(index2);
    }
    mainindex_long = "_2D"+index1_long+index2_long;
  }
    
  vector<TString> boundaries = translateBoundaries(mode,index1,index2);
    
  TCanvas* canv = new TCanvas("HF1","HF1",50,50,600,600);
  canv->cd(1);
    
  int pt1 = 1, pt2 = -1, eta1 = 1, eta2 = -1;
  float ptmin, ptmax, etamin, etamax;
  if (mode == "eta") {

    pt1 =  1;
    pt2 = -1;
    eta1 = index1 + 1;
    eta2 = index1 + 1;

    ptmin = pt_bins[0];
    ptmax = pt_bins[pt_num];
    etamin = eta_bins[eta1-1];
    etamax = eta_bins[eta2];

  } else if ( mode == "pt" ) {

    pt1 = index1 + 1;
    pt2 = index1 + 1;
    eta1 =  1;
    eta2 = -1;

    ptmin = pt_bins[pt1-1];
    ptmax = pt_bins[pt2];
    etamin = eta_bins[0];
    etamax = eta_bins[eta_num];

  } else if ( mode == "etapt" ) { // index1 = eta, index2 = pt

    pt1 = index2 + 1;
    pt2 = index2 + 1;
    eta1 = index1 + 1;
    eta2 = index1 + 1;

    ptmin = pt_bins[pt1-1];
    ptmax = pt_bins[pt2];
    etamin = eta_bins[eta1-1];
    etamax = eta_bins[eta2];

  }

  cout << "Processing window index1, index2 = " << index1 << " " << index2 << " corresponding to ptmin, ptmax = " << ptmin << " " << ptmax << " and etamin, etamax " << etamin << " " << etamax << endl;


  double fit_rag_min = 2.7;
  double fit_rag_max = 3.5;

  RooRealVar *Mass = new RooRealVar("Mass", "Mass [GeV]", fit_rag_min, fit_rag_max) ;
  RooDataSet *data_mat = new RooDataSet("data_mat", "data_mat", RooArgSet(*Mass));
  RooDataSet *data_unm = new RooDataSet("data_unm", "data_num", RooArgSet(*Mass));

  if (!useTree) {

    // Will be used in the future when analysis and binning will be freezed

    /* TH3D *pem3D_total = (TH3D*)f->Get( "/"+probeType+"_All/"+probeType+"_OC/"+muType+"/Probes/"+probeType+"_All_"+probeType+"_OC_"+muType+"_Probes_pem_num" ); */
    /* TH3D *pem3D_match = (TH3D*)f->Get( "/"+probeType+"_All/"+probeType+"_OC/"+muType+"/Matches/"+probeType+"_All_"+probeType+"_OC_"+muType+"_Matches_pem_num" ); */
    
    /* h_tot_bin = pem3D_total->ProjectionZ(tag_hist,pt1,pt2,eta1,eta2); */
    /* h_unm_bin = pem3D_match->ProjectionZ(probe_hist,pt1,pt2,eta1,eta2); */

  } else {

    if (verbose) cout << "Total # of entries = " << chain->GetEntries() << endl;

    for (int i=0; i<chain->GetEntries(); i++) {
      chain->GetEntry(i);

      if (dataSample == "dataC" || dataSample == "dataDE") {

	bool checkGRL = false;

	if (runNumber == 270806 &&
	    ((lumiblock >=  352 && lumiblock <= 399)) ) checkGRL = true;

	if (runNumber == 270953 &&
	    ((lumiblock >=    3 && lumiblock <= 129) ||
	     (lumiblock >=  131 && lumiblock <= 161) ||
	     (lumiblock >=  169 && lumiblock <= 343)) ) checkGRL = true;

	if (runNumber == 271048 &&
	    ((lumiblock >=  102 && lumiblock <= 110) ||
	     (lumiblock >=  112 && lumiblock <= 135) ||
	     (lumiblock >=  139 && lumiblock <= 153) ||
	     (lumiblock >=  159 && lumiblock <= 181) ||
	     (lumiblock >=  183 && lumiblock <= 352) ||
	     (lumiblock >=  354 && lumiblock <= 392) ||
	     (lumiblock >=  398 && lumiblock <= 472) ||
	     (lumiblock >=  484 && lumiblock <= 617)) )  checkGRL = true;
	
	if (runNumber == 271298 &&
	    ((lumiblock >=  294 && lumiblock <= 294) ||
	     (lumiblock >=  296 && lumiblock <= 296) ||
	     (lumiblock >=  299 && lumiblock <= 299) ||
	     (lumiblock >=  301 && lumiblock <= 301) ||
	     (lumiblock >=  359 && lumiblock <= 359) ||
	     (lumiblock >=  365 && lumiblock <= 398) ||
	     (lumiblock >=  401 && lumiblock <= 639) ||
	     (lumiblock >=  641 && lumiblock <= 716)) ) checkGRL = true;

	if (runNumber == 271421 &&
	    ((lumiblock >=  64 && lumiblock <= 183) ||
	     (lumiblock >=  189 && lumiblock <= 240) ||
	     (lumiblock >=  242 && lumiblock <= 248) ||
	     (lumiblock >=  262 && lumiblock <= 334) ||
	     (lumiblock >=  338 && lumiblock <= 340) ||
	     (lumiblock >=  342 && lumiblock <= 401) ||
	     (lumiblock >=  403 && lumiblock <= 409)) ) checkGRL = true;

	if (runNumber == 271516 &&
	    ((lumiblock >=  181 && lumiblock <= 372) ||
	     (lumiblock >=  374 && lumiblock <= 444) ||
	     (lumiblock >=  446 && lumiblock <= 497)) ) checkGRL = true;

	if (runNumber == 271595 &&
	    ((lumiblock >=  331 && lumiblock <= 428) ||
	     (lumiblock >=  454 && lumiblock <= 582)) ) checkGRL = true;

	if (runNumber == 271744 &&
	    ((lumiblock >=   78 && lumiblock <= 219)) ) checkGRL = true;

	if (runNumber == 276262 &&
	    ((lumiblock >=   72 && lumiblock <=  84) ||
	     (lumiblock >=   86 && lumiblock <= 110) ||
	     (lumiblock >=  118 && lumiblock <= 288) ||
	     (lumiblock >=  292 && lumiblock <= 316) ||
	     (lumiblock >=  318 && lumiblock <= 378)) ) checkGRL = true;

	if (runNumber == 276329 &&
	    ((lumiblock >=  147 && lumiblock <= 194) ||
	     (lumiblock >=  196 && lumiblock <= 208) ||
	     (lumiblock >=  212 && lumiblock <= 494) ||
	     (lumiblock >=  497 && lumiblock <= 503) ||
	     (lumiblock >=  507 && lumiblock <= 558)) ) checkGRL = true;

	if (runNumber == 276336 &&
	    ((lumiblock >=    4 && lumiblock <=  15) ||
	     (lumiblock >=   36 && lumiblock <=  54)) ) checkGRL = true;

	if (runNumber == 276416 &&
	    ((lumiblock >=  373 && lumiblock <= 418) ||
	     (lumiblock >=  445 && lumiblock <= 507)) ) checkGRL = true;

	if (runNumber == 276511 &&
	    ((lumiblock >=  185 && lumiblock <= 450) ||
	     (lumiblock >=  452 && lumiblock <= 475)) ) checkGRL = true;

	if (runNumber == 276689 &&
	    ((lumiblock >=  198 && lumiblock <= 214) ||
	     (lumiblock >=  216 && lumiblock <= 463)) ) checkGRL = true;

	if (runNumber == 276731 &&
	    ((lumiblock >=   60 && lumiblock <= 468) ||
	     (lumiblock >=  470 && lumiblock <= 621) ||
	     (lumiblock >=  623 && lumiblock <= 673)) ) checkGRL = true;

	if (runNumber == 276778 &&
	    ((lumiblock >=  177 && lumiblock <= 193)) ) checkGRL = true;

	if (runNumber == 276790 &&
	    ((lumiblock >=  251 && lumiblock <= 285)) ) checkGRL = true;

	if (runNumber == 276952 &&
	    ((lumiblock >=   75 && lumiblock <= 214)) ) checkGRL = true;

	if (runNumber == 276954 &&
	    ((lumiblock >=  208 && lumiblock <= 224)) ) checkGRL = true;

	if (runNumber == 278880 &&
	    ((lumiblock >=  101 && lumiblock <= 149) ||
	     (lumiblock >=  152 && lumiblock <= 211) ||
	     (lumiblock >=  213 && lumiblock <= 391) ||
	     (lumiblock >=  404 && lumiblock <= 509) ||
	     (lumiblock >=  511 && lumiblock <= 578)) ) checkGRL = true;

	if (runNumber == 278912 &&
	    ((lumiblock >=  119 && lumiblock <= 515) ||
	     (lumiblock >=  518 && lumiblock <= 530)) ) checkGRL = true;

	if (runNumber == 278968 &&
	    ((lumiblock >=   40 && lumiblock <= 128) ||
	     (lumiblock >=  131 && lumiblock <= 160) ||
	     (lumiblock >=  163 && lumiblock <= 166)) ) checkGRL = true;

	if (runNumber == 279169 &&
	    ((lumiblock >=  292 && lumiblock <= 573) ||
	     (lumiblock >=  588 && lumiblock <= 588) ||
	     (lumiblock >=  615 && lumiblock <= 716) ||
	     (lumiblock >=  721 && lumiblock <= 909) ||
	     (lumiblock >=  911 && lumiblock <=1056) ||
	     (lumiblock >= 1058 && lumiblock <=1082)) ) checkGRL = true;

	if (runNumber == 279259 &&
	    ((lumiblock >=  425 && lumiblock <= 526)) ) checkGRL = true;

	if (runNumber == 279279 &&
	    ((lumiblock >=  291 && lumiblock <= 393) ||
	     (lumiblock >=  395 && lumiblock <= 399) ||
	     (lumiblock >=  404 && lumiblock <= 453) ||
	     (lumiblock >=  498 && lumiblock <= 510) ||
	     (lumiblock >=  515 && lumiblock <= 570)) ) checkGRL = true;

	if (runNumber == 279284 &&
	    ((lumiblock >=  427 && lumiblock <= 882)) ) checkGRL = true;

	if (runNumber == 279598 &&
	    ((lumiblock >=  605 && lumiblock <= 674) ||
	     (lumiblock >=  677 && lumiblock <= 767) ||
	     (lumiblock >=  769 && lumiblock <= 770) ||
	     (lumiblock >=  772 && lumiblock <= 906) ||
	     (lumiblock >=  909 && lumiblock <= 922) ||
	     (lumiblock >=  924 && lumiblock <=1236) ||
	     (lumiblock >= 1239 && lumiblock <=1302) ||
	     (lumiblock >= 1304 && lumiblock <=1306)) ) checkGRL = true;

	if (!checkGRL) continue;

      }

      float dRTrig = 0.005;
      float dRMatch = 0.005;

      bool checkTrigger = false;
      
      if ( (probeType == "IDProbes" || probeType == "CaloProbes") &&
	   (
	    ( HLT_mu4 && tag_matched_HLT_mu4 && tag_dRMatch_HLT_mu4 < dRTrig) ||
	    ( HLT_mu6 && tag_matched_HLT_mu6 && tag_dRMatch_HLT_mu6 < dRTrig) ||
	    ( HLT_mu6_idperf && tag_matched_HLT_mu6_idperf && tag_dRMatch_HLT_mu6_idperf < dRTrig) ||
	    ( HLT_mu14 && tag_matched_HLT_mu14 && tag_dRMatch_HLT_mu14 < dRTrig) ||
	    ( L1_MU4 && tag_matched_L1_MU4 && tag_dRMatch_L1_MU4 < dRTrig) ||
	    ( L1_MU6 && tag_matched_L1_MU6 && tag_dRMatch_L1_MU6 < dRTrig) )
	   ) checkTrigger = true;

      //      if ( (probeType == "IDProbes" || probeType == "CaloProbes") &&
	   /* ( */
	   /*  ( HLT_mu4_bJpsi_Trkloose && tag_matched_HLT_mu4_bJpsi_Trkloose && tag_dRMatch_HLT_mu4_bJpsi_Trkloose < dRTrig) || */
	   /*  ( HLT_mu6_bJpsi_Trkloose && tag_matched_HLT_mu6_bJpsi_Trkloose && tag_dRMatch_HLT_mu6_bJpsi_Trkloose < dRTrig) || */
	   /*  ( HLT_mu10_bJpsi_Trkloose && tag_matched_HLT_mu10_bJpsi_Trkloose && tag_dRMatch_HLT_mu10_bJpsi_Trkloose < dRTrig) || */
	   /*  ( HLT_mu18_bJpsi_Trkloose && tag_matched_HLT_mu18_bJpsi_Trkloose && tag_dRMatch_HLT_mu18_bJpsi_Trkloose < dRTrig) ) */
	   /* ) checkTrigger = true; */
      
      if ( (probeType == "MSProbes_ID") && 
	   ( HLT_mu4_mu4_idperf_bJpsimumu_noid && tag_matched_HLT_mu4_mu4_idperf_bJpsimumu_noid && tag_dRMatch_HLT_mu4_mu4_idperf_bJpsimumu_noid < dRTrig )
	   ) checkTrigger = true;

      if (!checkTrigger) continue;

      if (probe_pt<ptmin||probe_pt>ptmax||probe_eta<etamin||probe_eta>etamax) continue;
      //      if (probe_pt<ptmin||probe_pt>ptmax||(probe_q*probe_eta)<etamin||(probe_q*probe_eta)>etamax) continue;
      //      if (probe_pt<ptmin||probe_pt>ptmax||fabs(probe_eta)<etamin||fabs(probe_eta)>etamax) continue;

      if (fabs(tag_z0-probe_z0)>5) continue;
      
      if (sqrt(dilep_dphi_exTP*dilep_dphi_exTP+dilep_deta_exTP*dilep_deta_exTP)<0.2) continue;
      //      if (sqrt(dilep_dphi*dilep_dphi+dilep_deta*dilep_deta)<0.3) continue;
      
      float tempMass = dilep_mll/1000.;
      Mass->setVal(tempMass);
      if ( (muType == "LooseMuons" && matched_LooseMuons && probe_dRMatch_LooseMuons<dRMatch) ||
	   (muType == "MediumMuons" && matched_MediumMuons && probe_dRMatch_MediumMuons<dRMatch) ||
	   (muType == "TightMuons" && matched_TightMuons && probe_dRMatch_TightMuons<dRMatch) ||
	   (muType == "IDTracks" && matched_IDTracks && probe_dRMatch_IDTracks<dRMatch) ) { 
	data_mat->add(RooArgList(*Mass));
      } else {
	data_unm->add(RooArgList(*Mass));
      }
      
    }
  }

  // ****************
  //   FIT AND PLOT
  // ****************    

  // Simultaneous fit:

  /* double sigFG = 90000; */
  /* double bgmFG = 35000; */
  /* double bguFG = 500000; */
  double sigFG = 1000;
  double bgmFG = 500;
  double bguFG = 5000;

  RooRealVar *mean   = new RooRealVar("mean",  "mean",   3.096916, 3.0,    3.2); 
  RooRealVar *sigma  = new RooRealVar("sigma", "sigma",  0.050,    0.020,  0.080) ;
  RooRealVar *sigma2 = new RooRealVar("sigma2","sigma2", 0.200,    0.100,  0.300) ;
  RooRealVar *NgaussianTOT = new RooRealVar("NgaussianTOT", "NgaussianTOT", sigFG, 1, 1000000);

  RooRealVar *a0 = new RooRealVar("a0", "a0", -0.5,  -1,  1);
  RooRealVar *a1 = new RooRealVar("a1", "a1",  0.0,  -1,  1);
  //  RooRealVar *a2 = new RooRealVar("a2", "a2", -0.1,  -1,  1);
  RooRealVar *NpolMatched = new RooRealVar("NpolMatched", "NpolMatched", bgmFG, 1, 1000000);

  RooRealVar *a3 = new RooRealVar("a3", "a3", -0.3,  -1,  1) ;
  RooRealVar *a4 = new RooRealVar("a4", "a4",  0.0,  -1,  1) ;
  //  RooRealVar *a5 = new RooRealVar("a5", "a5", -0.1,  -1,  1) ;
  RooRealVar *NpolUnmatched = new RooRealVar("NpolUnmatched", "NpolUnmatched", bguFG, 1, 1000000);

  RooRealVar *efficiency = new RooRealVar("efficiency", "efficiency", 0., 1.);
  RooFormulaVar  *NgaussianMatched = new RooFormulaVar("NgaussianMatched", "@0*@1", RooArgList(*efficiency, *NgaussianTOT));
  RooFormulaVar  *NgaussianUnmatched = new RooFormulaVar("NgaussianUnmatched", "(1.-@0)*@1", RooArgList(*efficiency, *NgaussianTOT));

  RooExtendPdf *egaussianMatched;
  RooExtendPdf *epolMatched;
  RooExtendPdf *egaussianUnmatched;
  RooExtendPdf *epolUnmatched;

  if (fitfunction=="gp2") {

    // model for matched
    RooGaussian *gaussianMatched = new RooGaussian("gaussianMatched", "gaussianMatched", *Mass, *mean, *sigma) ;
    egaussianMatched = new RooExtendPdf("egaussianMatched", "egaussianMatched", *gaussianMatched, *NgaussianMatched);
    RooChebychev *polMatched = new RooChebychev("polMatched", "polMatched", *Mass, RooArgSet(*a0,*a1));
    epolMatched = new RooExtendPdf("epolMatched", "epolMatched", *polMatched, *NpolMatched);
	
    // model for unmatched
    RooGaussian *gaussianUnmatched = new RooGaussian("gaussianUnmatched", "gaussianUnmatched", *Mass, *mean, *sigma) ;
    egaussianUnmatched = new RooExtendPdf("egaussianUnmatched", "egaussianUnmatched", *gaussianUnmatched, *NgaussianUnmatched);
    RooChebychev *polUnmatched = new RooChebychev("polUnmatched", "polUnmatched", *Mass, RooArgSet(*a3,*a4)) ;
    epolUnmatched = new RooExtendPdf("epolUnmatched", "epolUnmatched", *polUnmatched, *NpolUnmatched);

  }

  if (fitfunction=="doublegp2") {

    // model for matched
    RooGaussian *gaussianMatched = new RooGaussian("gaussianMatched", "gaussianMatched", *Mass, *mean, *sigma) ;
    RooGaussian *gaussian2Matched = new RooGaussian("gaussian2Matched", "gaussian2Matched", *Mass, *mean, *sigma2) ;
    RooRealVar *fMatched = new RooRealVar("fMatched", "fMatched", 0.5, 0., 1.);
    RooAddPdf *gaussiansMatched = new RooAddPdf("gaussiansMatched", "gaussiansMatched", *gaussianMatched, *gaussian2Matched, *fMatched);
    egaussianMatched = new RooExtendPdf("egaussianMatched", "egaussianMatched", *gaussiansMatched, *NgaussianMatched);
    RooChebychev *polMatched = new RooChebychev("polMatched", "polMatched", *Mass, RooArgSet(*a0,*a1));
    epolMatched = new RooExtendPdf("epolMatched", "epolMatched", *polMatched, *NpolMatched);
	
    // model for unmatched
    RooGaussian *gaussianUnmatched = new RooGaussian("gaussianUnmatched", "gaussianUnmatched", *Mass, *mean, *sigma) ;
    RooGaussian *gaussian2Unmatched = new RooGaussian("gaussian2Unmatched", "gaussian2Unmatched", *Mass, *mean, *sigma2) ;
    RooRealVar *fUnmatched = new RooRealVar("fUnmatched", "fUnmatched", 0.5, 0., 1.);
    RooAddPdf *gaussiansUnmatched = new RooAddPdf("gaussiansUnmatched", "gaussiansUnmatched", *gaussianUnmatched, *gaussian2Unmatched, *fUnmatched);
    egaussianUnmatched = new RooExtendPdf("egaussianUnmatched", "egaussianUnmatched", *gaussiansUnmatched, *NgaussianUnmatched);
    RooChebychev *polUnmatched = new RooChebychev("polUnmatched", "polUnmatched", *Mass, RooArgSet(*a3,*a4)) ;
    epolUnmatched = new RooExtendPdf("epolUnmatched", "epolUnmatched", *polUnmatched, *NpolUnmatched);

  }

  if (fitfunction=="gp3") {

    // model for matched
    RooGaussian *gaussianMatched = new RooGaussian("gaussianMatched", "gaussianMatched", *Mass, *mean, *sigma) ;
    egaussianMatched = new RooExtendPdf("egaussianMatched", "egaussianMatched", *gaussianMatched, *NgaussianMatched);
    RooChebychev *polMatched = new RooChebychev("polMatched", "polMatched", *Mass, RooArgSet(*a0));
    epolMatched = new RooExtendPdf("epolMatched", "epolMatched", *polMatched, *NpolMatched);
	
    // model for unmatched
    RooGaussian *gaussianUnmatched = new RooGaussian("gaussianUnmatched", "gaussianUnmatched", *Mass, *mean, *sigma) ;
    egaussianUnmatched = new RooExtendPdf("egaussianUnmatched", "egaussianUnmatched", *gaussianUnmatched, *NgaussianUnmatched);
    RooChebychev *polUnmatched = new RooChebychev("polUnmatched", "polUnmatched", *Mass, RooArgSet(*a3)) ;
    epolUnmatched = new RooExtendPdf("epolUnmatched", "epolUnmatched", *polUnmatched, *NpolUnmatched);

  }
  
  RooAddPdf *model_mat = new RooAddPdf("model_mat", "model_mat", RooArgList(*egaussianMatched, *epolMatched));
  RooAddPdf *model_unm = new RooAddPdf("model_unm", "model_unm", RooArgList(*egaussianUnmatched, *epolUnmatched));

  RooCategory sample("sample","sample");
  sample.defineType("Matched");
  sample.defineType("Unmatched");

  RooDataSet *combData = new RooDataSet("combData", "combined data", *Mass, Index(sample), Link("Matched",*data_mat), Link("Unmatched", *data_unm));

  RooSimultaneous simPdf("simPdf","simultaneous pdf",sample);
  simPdf.addPdf(*model_mat, "Matched");
  simPdf.addPdf(*model_unm, "Unmatched");

  int printlev = 3;
  if (!verbose) printlev = -1;

  // Fit now:
  simPdf.fitTo(*combData, RooFit::Minimizer("Minuit2","Migrad"), Range(fit_rag_min, fit_rag_max), PrintLevel(printlev));

  double eff       = -1;
  double eff_err   = -1;
  bool resultisNan =  1;

  eff = efficiency->getVal(); // 1.0 - NgaussianUnmatched->getVal() / NgaussianALL->getVal();
  eff_err = efficiency->getError(); // (1./sqrt(tot_func->GetParameter(1)))*sqrt(eff*(1.-eff));
  resultisNan = TMath::IsNaN(eff_err);
    
  // Plot:

  RooPlot* frame_all = Mass->frame(Bins(40)); frame_all->SetAxisRange(fit_rag_min, fit_rag_max);
  combData->plotOn(frame_all, Cut("sample==sample::Matched"),   LineWidth(2), MarkerSize(1.2), MarkerStyle(24), MarkerColor(kBlack));
  combData->plotOn(frame_all, Cut("sample==sample::Unmatched"), LineWidth(2), MarkerSize(1.2), MarkerStyle(20), MarkerColor(kBlack));
  simPdf.plotOn(frame_all, Slice(sample, "Matched"),   ProjWData(sample, *combData), LineColor(kBlue), LineWidth(2), NumCPU(4,kTRUE));
  simPdf.plotOn(frame_all, Slice(sample, "Unmatched"), ProjWData(sample, *combData), LineColor(kRed),  LineWidth(2), NumCPU(4,kTRUE));

  frame_all->Draw();
  //  gPad->WaitPrimitive();
  
  double tlxmin = 0.2, tlxmax = 0.4;

  TLegend *leg2 = new TLegend(tlxmin,0.65,tlxmax,0.7);
  if (mode=="pt")  leg2->SetHeader( boundaries.at(1)+" < p_{T}^{#mu} < "+boundaries.at(2) );
  if (mode=="eta") leg2->SetHeader( " p_{T}^{#mu} > 3.5 GeV " );
  if (mode=="etapt") leg2->SetHeader( boundaries.at(3)+" < p_{T}^{#mu} < "+boundaries.at(4) );
  leg2->SetTextSize(0.03);
  leg2->SetFillColor(10);leg2->SetLineColor(10);
  leg2->Draw("same");
    
  TLegend *leg3 = new TLegend(tlxmin,0.6,tlxmax,0.65);
  if (mode=="pt") leg3->SetHeader( "|#eta_{#mu}| < 2.5" );
  if (mode=="eta") leg3->SetHeader( boundaries.at(1)+" < #eta_{#mu} < "+boundaries.at(2) );
  if (mode=="etapt") leg3->SetHeader( boundaries.at(1)+" < #eta_{#mu} < "+boundaries.at(2) );
  leg3->SetTextSize(0.03);
  leg3->SetFillColor(10);leg3->SetLineColor(10);
  leg3->Draw("same");
    
  TLegend *leg_fit = new TLegend(tlxmin,0.52,tlxmax,0.57);
  if (fitfunction=="gp2") leg_fit->SetHeader( "Gaussian + pol2 fit" );
  if (fitfunction=="doublegp2") leg_fit->SetHeader( "Double-Gaussian + pol2 fit" );
  if (fitfunction=="gp3") leg_fit->SetHeader( "Gaussian + pol3 fit" );
  if (fitfunction=="cb") leg_fit->SetHeader( "Crystal Ball + pol2 fit" );
  leg_fit->SetTextSize(0.03);
  leg_fit->SetFillColor(10);leg_fit->SetLineColor(10);
  leg_fit->Draw("same");

  TString Teff;
  Teff = " #epsilon";
  if (!resultisNan && fitfunction=="gp2") Teff += Form("^{GP2}_{reco} = %0.2f #pm %0.2f", eff, eff_err );
  if (!resultisNan && fitfunction=="doublegp2") Teff += Form("^{doubleGP2}_{reco} = %0.2f #pm %0.2f", eff, eff_err );
  if (!resultisNan && fitfunction=="gp3") Teff += Form("^{GP3}_{reco} = %0.2f #pm %0.2f", eff, eff_err );
  if (!resultisNan && fitfunction=="cb") Teff += Form("^{CB}_{reco} = %0.2f #pm %0.2f", eff, eff_err );
  if (resultisNan) Teff = "Fit failed!";
    
  TLegend *leg4 = new TLegend(tlxmin,0.47,tlxmax,0.52);
  leg4->SetHeader( Teff );
  leg4->SetTextSize(0.03);
  leg4->SetFillColor(10);
  leg4->SetLineColor(10);
  leg4->Draw("same");    

  if ( massPlots ) {

    if (mode=="pt" || mode=="eta") {

      if (makePDFfiles) canv->Print("EffResults/"+probeType+"_"+muType+"/MassPlots/"+fitfunction+"/1D/"+resultlabel+"_MassPlot_"+fitfunction+mainindex_long+".pdf");
      if (makeEPSfiles) canv->Print("EffResults/"+probeType+"_"+muType+"/MassPlots/"+fitfunction+"/1D/"+resultlabel+"_MassPlot_"+fitfunction+mainindex_long+".eps");
      if (makePNGfiles) canv->Print("EffResults/"+probeType+"_"+muType+"/MassPlots/"+fitfunction+"/1D/"+resultlabel+"_MassPlot_"+fitfunction+mainindex_long+".png");
      if (makeROOTfiles) canv->Print("EffResults/"+probeType+"_"+muType+"/MassPlots/"+fitfunction+"/1D/"+resultlabel+"_MassPlot_"+fitfunction+mainindex_long+".C");

    } else {

      if (makePDFfiles) canv->Print("EffResults/"+probeType+"_"+muType+"/MassPlots/"+fitfunction+"/"+resultlabel+"_MassPlot_"+fitfunction+mainindex_long+".pdf");
      if (makeEPSfiles) canv->Print("EffResults/"+probeType+"_"+muType+"/MassPlots/"+fitfunction+"/"+resultlabel+"_MassPlot_"+fitfunction+mainindex_long+".eps");
      if (makePNGfiles) canv->Print("EffResults/"+probeType+"_"+muType+"/MassPlots/"+fitfunction+"/"+resultlabel+"_MassPlot_"+fitfunction+mainindex_long+".png");
      if (makeROOTfiles) canv->Print("EffResults/"+probeType+"_"+muType+"/MassPlots/"+fitfunction+"/"+resultlabel+"_MassPlot_"+fitfunction+mainindex_long+".C");

    }
    
  }

  delete canv;

  Mass->Delete();
  data_mat->Delete();
  data_unm->Delete();
  combData->Delete();

  vector<double> effAndError;
  effAndError.push_back(eff);
  effAndError.push_back(eff_err);
  return effAndError;
    
}


vector<TString> translateBoundaries(TString mode, int index1, int index2) {
    
  vector<TString> boundaries;
  boundaries.push_back(mode);

  if (mode=="eta") {
    TString lowedge = stringify(eta_bins[index1]);
    TString highedge = stringify(eta_bins[index1+1]);
    boundaries.push_back(lowedge);
    boundaries.push_back(highedge);
  }

  if (mode=="pt") {
    TString lowedge = stringify(pt_bins[index1]);
    TString highedge = stringify(pt_bins[index1+1]);
    boundaries.push_back(lowedge);
    boundaries.push_back(highedge);
  }

  if (mode=="etapt") {
    TString eta_lowedge = stringify(eta_bins[index1]);
    TString eta_highedge = stringify(eta_bins[index1+1]);
    TString pt_lowedge = stringify(pt_bins[index2]);
    TString pt_highedge = stringify(pt_bins[index2+1]);
    boundaries.push_back(eta_lowedge);
    boundaries.push_back(eta_highedge);
    boundaries.push_back(pt_lowedge);
    boundaries.push_back(pt_highedge);
  }
    
  return boundaries;
    
}


void set_plot_style() {
  
  const Int_t nRGBs = 5;
  Double_t stops[nRGBs] = {0.00, 0.25, 0.50, 0.75, 1.00};
  Double_t red[nRGBs]   = {1.00, 1.00, 1.00, 0.75, 0.00};
  Double_t green[nRGBs] = {1.00, 1.00, 0.55, 0.00, 0.00};
  Double_t blue[nRGBs]  = {1.00, 0.00, 0.00, 0.00, 0.00};
  
  TColor::CreateGradientColorTable(nRGBs, stops, red, green, blue, 255);
  gStyle->SetNumberContours(255);

  gStyle->SetOptStat(0);

}
