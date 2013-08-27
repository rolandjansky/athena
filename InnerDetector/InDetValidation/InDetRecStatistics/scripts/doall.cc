/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File: $Id: doall.cc,v 1.14 2008-07-23 20:51:23 adye Exp $
#define DUPOLD 0  // 0=normal - if set to 1 or 2, duplicate old styles exactly
#include <iostream>
#include <string>

#include "TROOT.h"
#include "TMath.h"
#include "TObject.h"
#include "TDirectory.h"
#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TCut.h"

bool dummy;   // somewhere to put unused variables and stop stupid warnings

#define DOALL 1
using std::cout;
using std::endl;
using std::string;
void doall_setup (const char* name, const int compile= 0, const char* subdir= 0);
bool doall_doit (const string& doplot, const string& routine);
string doall_filename (const string& dir, const string& name, const string& ext);
string doall_filename (const string& dir, const string& name);
TObject* copyObject (const char* olddirname, const char* oldname, TDirectory* newdir, const char* newname);

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "eff.cc"
#include "fake.cc"
#include "vertex.cc"
#include "interaction.cc"
#include "resolution_phi.cc"
#include "resolution_pt.cc"
#include "resolution_cot.cc"
#include "resolution_d0.cc"
#include "resolution_z0.cc"
#include "pulls.cc"
#include "residuals.cc"
#include "hits.cc"
#endif

string doall_doplot;
Long64_t doall_nentries= 0;
int doall_cutset= 1;   // 0=none, 1=no TRTStandalong, 2=standard cuts,...

int doall (const string file= "InDetRecStatistics.root",
           const string plotdir= ".",
           const string plotext= ".eps",
           const string histfilename= "InDetRTTHistos.root",
           const string doplot= doall_doplot,
           const int compile= 0,
           const int abseta = 0,   //added by SCZ; abseta = 1 for old default behavior
           const int cutset = doall_cutset,
           const char* datacut=  "",   // track data  selection
           const char* truthcut= "",   // track truth selection
           const char* mccut=    "",   // mctrack     selection
           const char* hitcut=   "",   // hit         selection
           const Long64_t nentries = doall_nentries)
{
  TCut cutset_data;   // track data  selection
  TCut cutset_truth;  // track truth selection
  TCut cutset_mc;     // mctrack     selection
  TCut cutset_hit;    // hit         selection
  TCut hitmap_cut;    // hit map     selection (usually none)
  bool primary= true;
  double match= -1;   // use defaults

  switch (cutset) {
  case 1:    // No TRTStandalone tracks
    cutset_data=  "track_npixhits+track_nscthits>0";
    cutset_truth= "";
    cutset_mc=    "";
    cutset_hit=   "hit_recindex>=0&&track_npixhits[hit_recindex]+track_nscthits[hit_recindex]>0";
    break;
  case 2:    // TrackSelector cuts for data
    cutset_data=  "track_passSelectorCuts>0";
    cutset_truth= "abs(track_truth_eta)<2.5&&track_truth_barcode<100000&&abs(1/track_truth_qoverpt)>1000";
    cutset_mc=    "abs(mctrack_eta)<2.5&&mctrack_barcode<100000&&abs(1/mctrack_qoverpt)>1000";
    cutset_hit=   "hit_recindex>=0&&track_passSelectorCuts[hit_recindex]>0";
    primary=      true;
    match=        0.8;
    break;
  case 3:    // "standard cuts" from InDetTrackingPerformanceCuts TWiki page, but no impact cuts, since those should be WRT PV, which we don't have. Use IDRS primary cuts instead.
    cutset_data=  "track_npixhits+track_nscthits>=7";
    cutset_truth= "abs(track_truth_eta)<2.5&&track_truth_barcode<100000&&abs(1/track_truth_qoverpt)>1000";
    cutset_mc=    "abs(mctrack_eta)<2.5&&mctrack_barcode<100000&&abs(1/mctrack_qoverpt)>1000";
    cutset_hit=   "hit_recindex>=0&&track_npixhits[hit_recindex]+track_nscthits[hit_recindex]>=7";
    primary=      true;
    match=        0.8;
    break;
  case 4:    // "standard cuts" closer to InDetTrackingPerformanceCuts TWiki page, but which primary vertex to use? track_truth_beginvertex is not right for cutset_data, because there may be no associated truth
    cutset_data=  "track_npixhits+track_nscthits>=7&&abs(track_d0+delta_d0(track_truth_phi,track_truth_beginvertexx,track_truth_beginvertexy))<2&&abs((track_z0-track_truth_beginvertexz)*sin(track_theta))<10";
    cutset_truth= "abs(track_truth_eta)<2.5&&track_truth_barcode<100000&&abs(track_truth_d0+delta_d0(track_truth_phi,track_truth_beginvertexx,track_truth_beginvertexy))<2&&abs(1/track_truth_qoverpt)>1000";
    cutset_mc=    "abs(mctrack_eta)<2.5&&mctrack_barcode<100000&&abs(mctrack_d0+delta_d0(mctrack_phi,mctrack_beginvertexx,mctrack_beginvertexy))<2&&abs(1/mctrack_qoverpt)>1000";
    cutset_hit=   "hit_recindex>=0&&track_npixhits[hit_recindex]+track_nscthits[hit_recindex]>=7&&abs(track_d0[hit_recindex]+delta_d0(track_truth_phi[hit_recindex],track_truth_beginvertexx[hit_recindex],track_truth_beginvertexy[hit_recindex]))<2&&abs((track_z0[hit_recindex]-track_truth_beginvertexz[hit_recindex])*sin(track_theta[hit_recindex]))<10";
    primary=      false;
    match=        0.8;
    break;
  case 5:
    cutset_data=  "abs(track_d0)<10";
    cutset_truth= "abs(track_truth_d0)<10";
    cutset_mc=    "abs(mctrack_d0)<10";
    cutset_hit=   "hit_recindex>=0&&abs(track_d0[hit_recindex])<10";
    break;
  case 6:
    cutset_data=  "track_author!=30";
    cutset_truth= "abs(track_truth_d0)<10";
    cutset_mc=    "abs(mctrack_d0)<10";
    cutset_hit=   "hit_recindex>=0&&track_author[hit_recindex]!=30";
    break;
  }

  TCut data_cut=  cutset_data  && TCut(datacut);
  TCut truth_cut= cutset_truth && TCut(truthcut);
  TCut mc_cut=    cutset_mc    && TCut(mccut);
  TCut hit_cut=   cutset_hit   && TCut(hitcut);
  TCut truedata_cut= truth_cut && data_cut;

  if (*data_cut  .GetTitle()) cout << "data_cut:   " << data_cut .GetTitle() << endl;
  if (*truth_cut .GetTitle()) cout << "truth_cut:  " << truth_cut.GetTitle() << endl;
  if (*mc_cut    .GetTitle()) cout << "mc_cut:     " << mc_cut   .GetTitle() << endl;
  if (*hit_cut   .GetTitle()) cout << "hit_cut:    " << hit_cut  .GetTitle() << endl;
  if (*hitmap_cut.GetTitle()) cout << "hitmap_cut: " << hit_cut  .GetTitle() << endl;
  if (nentries > 0)           cout << "nentries=   " << nentries             << endl;

  // ---------------- set up chains -------------
  // replace new with "0" to disable a tree.
  TChain *ip = new TChain("ConvertedIPatTracks");
  TChain *xk = new TChain("ConvertedXKalmanTracks");
  TChain *nt = new TChain("Tracks");

  if (ip) ip->Add(file.c_str());
  if (xk) xk->Add(file.c_str());
  if (nt) nt->Add(file.c_str());

  if (ip && !ip->GetEntries()) { delete ip; ip = 0; }
  if (xk && !xk->GetEntries()) { delete xk; xk = 0; }
  if (nt && !nt->GetEntries()) { delete nt; nt = 0; }

  int ntree = 0;
  TTree* trees[3] = { 0, 0, 0 };
  const char* names[3];
  if (ip) {
    trees[ntree]= ip; names[ntree]= "iPatRec";     ntree++;
  }
  if (xk) {
    trees[ntree]= xk; names[ntree]= "xKalman";     ntree++;
  }
  if (nt) {
    trees[ntree]= nt; names[ntree]= "newTracking"; ntree++;
  }

  if (ntree==0) {
    cout << "No trees found" << endl;
    return 1;
  }

  cout << "InDetRecStatistics trees:-" << endl;
  for (int i= 0; i<ntree; i++) {
    trees[i]->ls();
    if (nentries>0) trees[i]->SetMaxEntryLoop(nentries);
  }

  TFile* histfile= 0;
  if (histfilename != "") {
    histfile= TFile::Open (histfilename.c_str(), "recreate");
  }

  const string plotprefix = plotdir+"/";

  // --------------------- create different sets of plots

  if (doall_doit (doplot, "eff")) {
    doall_setup ("eff", compile);
    eff (trees, names, doall_filename(plotdir,"efficiency_"), plotext, ntree, truedata_cut, mc_cut, primary, match);
  }
  if (doall_doit (doplot, "fake")) {
    doall_setup ("fake", compile);
    fake (trees, names, doall_filename(plotdir,"fake_"), plotext, ntree, data_cut, match);
  }
  if (doall_doit (doplot, "vtx")) {
    doall_setup ("vertex", compile);
    vertex (trees[0], doall_filename(plotdir,"vertex_z0",plotext), truedata_cut, mc_cut, match);
  }
  if (doall_doit (doplot, "int")) {
    doall_setup ("interaction", compile);
    interaction (trees[ntree-1], doall_filename(plotdir,"interaction_"),plotext, truth_cut);
  }
  if (doall_doit (doplot, "resphi")) {
    doall_setup ("resolution_phi", compile, "resolution");
    resolution_phi (trees, names, doall_filename(plotdir,"resolution_phi_vs_eta",plotext),abseta,0,2.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "respt")) {
    doall_setup ("resolution_pt", compile, "resolution");
    resolution_pt (trees, names,  doall_filename(plotdir,"resolution_pt_vs_eta",plotext),abseta,0,2.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "rescot")) {
    doall_setup ("resolution_cot", compile, "resolution");
    resolution_cot (trees, names, doall_filename(plotdir,"resolution_cot_vs_eta",plotext),abseta,0,2.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "resd0")) {
    doall_setup ("resolution_d0", compile, "resolution");
    resolution_d0  (trees, names, doall_filename(plotdir,"resolution_d0_vs_eta",plotext),abseta,0,2.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "resz0")) {
    doall_setup ("resolution_z0", compile, "resolution");
    resolution_z0  (trees, names, doall_filename(plotdir,"resolution_z0_vs_eta",plotext),abseta,0,2.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "pullphi")) {
    doall_setup ("resolution_phi", compile, "pull");
    resolution_phi (trees, names, doall_filename(plotdir,"pull_phi_vs_eta",plotext),abseta,1,2.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "pullpt")) {
    doall_setup ("resolution_pt", compile, "pull");
    resolution_pt (trees, names,  doall_filename(plotdir,"pull_pt_vs_eta",plotext),abseta,1,2.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "pullcot")) {
    doall_setup ("resolution_cot", compile, "pull");
    resolution_cot (trees, names, doall_filename(plotdir,"pull_cot_vs_eta",plotext),abseta,1,2.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "pulld0")) {
    doall_setup ("resolution_d0", compile, "pull");
    resolution_d0  (trees, names, doall_filename(plotdir,"pull_d0_vs_eta",plotext),abseta,1,2.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "pullz0")) {
    doall_setup ("resolution_z0", compile, "pull");
    resolution_z0  (trees, names, doall_filename(plotdir,"pull_z0_vs_eta",plotext),abseta,1,2.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "narresphi")) {
    doall_setup ("resolution_phi", compile, "res_narrow");
    resolution_phi (trees, names, doall_filename(plotdir,"res_narrow_phi_vs_eta",plotext),abseta,0,1.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "narrespt")) {
    doall_setup ("resolution_pt", compile, "res_narrow");
    resolution_pt (trees, names,  doall_filename(plotdir,"res_narrow_pt_vs_eta",plotext),abseta,0,1.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "narrescot")) {
    doall_setup ("resolution_cot", compile, "res_narrow");
    resolution_cot (trees, names, doall_filename(plotdir,"res_narrow_cot_vs_eta",plotext),abseta,0,1.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "narresd0")) {
    doall_setup ("resolution_d0", compile, "res_narrow");
    resolution_d0  (trees, names, doall_filename(plotdir,"res_narrow_d0_vs_eta",plotext),abseta,0,1.5,ntree,truedata_cut,match);
  }
  if (doall_doit (doplot, "narresz0")) {
    doall_setup ("resolution_z0", compile, "res_narrow");
    resolution_z0  (trees, names, doall_filename(plotdir,"res_narrow_z0_vs_eta",plotext),abseta,0,1.5,ntree,truedata_cut,match);
  }
  if (ip && doall_doit (doplot, "pullip")) {
    doall_setup ("pulls", compile, "iPatRec");
    pulls (ip, 0, 0, doall_filename(plotdir,"pulls_iPatRec",plotext), truedata_cut, match);
  }
  if (xk && doall_doit (doplot, "pullxk")) {
    doall_setup ("pulls", compile, "xKalman");
    pulls (xk, 0, 0, doall_filename(plotdir,"pulls_xKalman",plotext), truedata_cut, match);
  }
  if (nt && doall_doit (doplot, "pullnt")) {
    doall_setup ("pulls", compile, "newTracking");
    pulls (nt, 0, 0, doall_filename(plotdir,"pulls_newTracking",plotext), truedata_cut, match);
  }
  if (ip && doall_doit (doplot, "resip")) {
    doall_setup ("residuals", compile, "iPatRec");
    residuals (ip, plotprefix, doall_filename("","_iPatRec",plotext), hit_cut);
  }
  if (xk && doall_doit (doplot, "resxk")) {
    doall_setup ("residuals", compile, "xKalman");
    residuals (xk, plotprefix, doall_filename("","_xKalman",plotext), hit_cut);
  }
  if (nt && doall_doit (doplot, "resnt")) {
    doall_setup ("residuals", compile, "newTracking");
    residuals (nt, plotprefix, doall_filename("","_newTracking",plotext), hit_cut);
  }
  if (nt && doall_doit (doplot, "hits")) {
    doall_setup ("hits", compile);
    hits (nt, plotprefix, plotext, hitmap_cut, data_cut, true);
  }

  gDirectory->cd("/");
  if (histfile) {
    histfile->Write();
    histfile->Close();
  }
  delete ip;
  delete xk;
  delete nt;
  return 0;
}

void doall_setup (const char* name, const int compile, const char* subdir)
{
  cout << "======================= ";
  if (subdir) cout << subdir << " ";
  cout << name << " =======================" << endl;
  gDirectory->cd("/");
  if (gDirectory->Get(name))
    gDirectory->cd(name);
  else
    gDirectory= gDirectory->mkdir(name);
  if (subdir) gDirectory= gDirectory->mkdir(subdir);
#if !(!defined(__CINT__) || defined(__MAKECINT__))
  string file = name+string(".cc");
  if (!gInterpreter->IsLoaded(file.c_str())) {
    string line = string(".L ")+file;
    if (compile>=1) line += "+";
    if (compile>=2) line += "+";
    gROOT->ProcessLine(line.c_str());
  }
#else
  dummy= compile;
#endif
}

bool doall_doit (const string& doplot, const string& routine)
{
  if (doplot == "") return true;
  const string doplotc = ","+doplot+",";
  return (doplotc.find(","+routine+",") != string::npos);
}

string doall_filename (const string& dir, const string& name, const string& ext)
{
  if (ext == "") return "";
  if (dir == "")
    return                 name+ext;
  else
    return dir+string("/")+name+ext;
}

string doall_filename (const string& dir, const string& name)
{
  if (dir == "")
    return                 name;
  else
    return dir+string("/")+name;
}

TObject* copyObject (const char* olddirname, const char* oldname, TDirectory* newdir, const char* newname)
{
  if (!(gDirectory && newdir && newdir->GetList())) return 0;
  TDirectory* olddir= dynamic_cast<TDirectory*>(gDirectory->Get(olddirname));
  if (!olddir) return 0;
  const TObject* oldobj= olddir->Get(oldname);
  if (!oldobj) return 0;
  Bool_t oldstat= TH1::AddDirectoryStatus();
  TH1::AddDirectory (kFALSE);
  TObject* newobj= oldobj->Clone(newname);
  TH1::AddDirectory (oldstat);
  if (!newobj) return 0;
  newdir->Append(newobj);
  return newobj;
}

Double_t delta_d0 (Double_t phi, Double_t dx, Double_t dy)
{
  return TMath::Sqrt (dx*dx + dy*dy) * TMath::Sin (phi - TMath::ATan2 (dy, dx));
}
#ifdef __MAKECINT__
#pragma link C++ function delta_d0;
#endif
