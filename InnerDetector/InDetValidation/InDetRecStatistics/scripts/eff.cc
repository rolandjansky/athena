/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File: $Id: eff.cc,v 1.12 2008-09-02 22:15:30 tompkins Exp $
#include <cfloat>
#include <iostream>
#include <string>

#include "TROOT.h"
#include "TString.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TCut.h"
#include "TLegend.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;


void eff_setstyle (TH1 *h, int hstyle)
{
  const int n_styles= 3;
  const int width[n_styles]= {3,3,3};
  const int style[n_styles]= {1,2,3};
  const int color[n_styles]= {1,2,4};
  if (hstyle>n_styles) hstyle= 0;
  h->SetLineWidth (width[hstyle]);
  h->SetLineColor (color[hstyle]);  
  h->SetLineStyle (style[hstyle]);  
}


TH1* eff_hist (TDirectory* dir, const char* name)
{
  if (!dir) return 0;
  TH1* h= dynamic_cast<TH1*>(dir->Get(name));
  if (!h) {
    cerr << __FILE__ << ":" << __LINE__ << ": No histogram " << name << " in directory " << dir->GetPath() << endl;
    return 0;
    //  } else {
    //    cout << "Got histogram " << name << " from directory " << dir->GetPath() << endl;
  }
  return h;
}


void eff_draw (TDirectory* dir, const char* name, Option_t* option = "")
{
  TH1* h= eff_hist(dir,name);
  if (!h) return;
  h->Draw(option);
}


void eff_drawall (const int ntrack_finders, TDirectory** dir, const char* name, TLegend* legend= 0)
{
  Option_t* option = "";
  int n= 0;
  bool makeleg= false;
  if (legend) {
    TList* list= legend->GetListOfPrimitives();
    makeleg= !(list && list->GetSize());
  }

  Double_t maxval= -DBL_MAX;
  for (int i= 0; i<ntrack_finders; i++) {
    if (dir[i]) {
      TH1* h= eff_hist(dir[i], name);
      if (h && h->GetMaximum() > maxval) maxval= h->GetMaximum();
    }
  }

  for (int i= 0; i<ntrack_finders; i++) {
    if (dir[i]) {
      TH1* h= eff_hist(dir[i], name);
      if (h) {
#if DUPOLD!=1
        if (maxval > h->GetMaximum()) h->SetMaximum (1.1*maxval);
#endif
        if (makeleg) legend->AddEntry(h, dir[i]->GetName(), "l");
        h->Draw(option);
        option= "same";
        n++;
      }
    }
  }
  if (legend && n>=2) legend->Draw();
}

Double_t unique_truth (Long64_t entry, Int_t mctrack, Double_t cut= 1.0)
{
  static const int s_n_maxtracksMC = 10000;   // copied from InDetRecStatisticsAlg.cxx
  static int nselected= 0, kilroy= 0;
  static Long64_t lastentry= -99;
  static Int_t selected[s_n_maxtracksMC];

  if (entry != lastentry) {
    if (entry == 0) kilroy= 0;
    nselected= 0;
    lastentry= entry;
  }
  if (mctrack < 0 || cut == 0) return 0.0;
  for (int i= 0; i<nselected; i++) {
    if (selected[i] == mctrack) {
//    cout << entry << ": duplicate mctrack " << mctrack << " dropped" << endl;
      return 0.0;
    }
  }
  if (nselected < s_n_maxtracksMC) {
    selected[nselected++]= mctrack;
  } else if (kilroy++) {
    cerr << "more than " << s_n_maxtracksMC << " unique selected mctracks - can't check for duplicates in the rest" << endl;
  }
  return cut;
}

#ifdef __MAKECINT__
#pragma link C++ function unique_truth;
#endif

void eff_part (TTree* tree, const int hstyle=0,
               const TCut data_cut="", const TCut mc_cut="",
               const bool primary= true, double match=-1,
               const int s_bins=40, const Double_t upper_pt=5.0)
{
  const Double_t upper_d0 = 50.0; // mm
  const Double_t upper_z0 = 300.0; // mm

  TCut truth_cut          = data_cut && ("track_truth_prob>"+TString::Format("%g",(match>=0?match:0.5)));
  TCut unique_truth_cut    ("unique_truth(Entry$,track_truth_index,"+TString(truth_cut)+")");

  TCut cut_barrel         = "abs(track_truth_eta) < 0.8";
//TCut cut_endcap         = "1.6 < abs(track_truth_eta) && abs(track_truth_eta) < 2.5";
  TCut cut_pt             = "abs(1/track_truth_qoverpt)>5000";
  TCut cut_pt2            = "abs(1/track_truth_qoverpt)>3000";
  TCut cut_zinside_blayer = "abs(track_truth_beginvertexz) < 200";
  TCut cut_rinside_blayer = "sqrt(pow(track_truth_beginvertexx,2)+pow(track_truth_beginvertexy,2)) < 25";
  TCut cut_zoutside_sct   = "abs(track_truth_endvertexz) > 2300";
  TCut cut_routside_sct   = "sqrt(pow(track_truth_endvertexx,2)+pow(track_truth_endvertexy,2)) > 400";

  // check what 40 cm / 230 cm corresponds to!!!

  TCut cut_signal = unique_truth_cut;
  if (primary) cut_signal= (cut_rinside_blayer && cut_zinside_blayer && ( cut_zoutside_sct || cut_routside_sct)) && cut_signal;

  TCut mccut_barrel         = "abs(mctrack_eta) < 0.8";
//TCut mccut_endcap         = "1.6 < abs(mctrack_eta) && abs(mctrack_eta) < 2.5";
  TCut mccut_pt             = "abs(1/mctrack_qoverpt)>5000";
  TCut mccut_pt2            = "abs(1/mctrack_qoverpt)>3000";
  TCut mccut_zinside_blayer = "abs(mctrack_beginvertexz) < 200";
  TCut mccut_rinside_blayer = "sqrt(pow(mctrack_beginvertexx,2)+pow(mctrack_beginvertexy,2)) < 25";
  TCut mccut_zoutside_sct   = "abs(mctrack_endvertexz) > 2300";
  TCut mccut_routside_sct   = "sqrt(pow(mctrack_endvertexx,2)+pow(mctrack_endvertexy,2)) > 400";

  TCut mccut_signal = mc_cut;
  if (primary) mccut_signal= (mccut_rinside_blayer && mccut_zinside_blayer && ( mccut_zoutside_sct || mccut_routside_sct)) && mccut_signal;

  cout << "mc_cut:   " << mccut_signal.GetTitle() << endl;
  cout << "data_cut: " << cut_signal  .GetTitle() << endl;

  /*
    InDet::InDetRecStatisticsAlg     INFO Cuts
    InDet::InDetRecStatisticsAlg     INFO   Signal                  Pt > 0.10 GeV/c, |eta| < 2.70                   Primary track start     R < 25.00mm and |z| < 200.00mm
    InDet::InDetRecStatisticsAlg     INFO   Barrel                  0.00< |eta| < 0.80                              Primary track end       R > 400.00mm or |z| > 2300.00mm
    InDet::InDetRecStatisticsAlg     INFO   Transition Region       0.80< |eta| < 1.60                              Secondary track start   25.00mm < R < 360.00mm and 200.00mm < |z| < 2000.00 mm
    InDet::InDetRecStatisticsAlg     INFO   Endcap                  1.60< |eta| < 2.50                              Secondary track end     R > 1000.00mm or |z| > 1000.00mm
    InDet::InDetRecStatisticsAlg     INFO   Fake reco track         < 0.70 of hits from single Truth Track
    InDet::InDetRecStatisticsAlg     INFO   Good reco track         > 0.70 of hits from single Truth Track
  */

  TH1F *etagen      = new TH1F("etagen",      "eta generated;#eta",                  s_bins, -3.00,    3.00);
  TH1F *etagenrec   = new TH1F("etagenrec",   "eta gen+rec;#eta",                    s_bins, -3.00,    3.00);
  TH1F *phigen      = new TH1F("phigen",      "phi generated;#phi",                  s_bins, -3.50,    3.50);
  TH1F *phigenrec   = new TH1F("phigenrec",   "phi gen+rec;#phi",                    s_bins, -3.50,    3.50);
  TH1F *ptgen       = new TH1F("ptgen",       "p_T generated;P_T [GeV/c]",           s_bins,   0.0,    1.20);
  TH1F *ptgenrec    = new TH1F("ptgenrec",    "p_T gen+rec;P_T [GeV/c]",             s_bins,   0.0,    1.20);
  TH1F *ptgen2      = new TH1F("ptgen2",      "p_T generated;P_T [GeV/c]",           s_bins,   0.0,    upper_pt);
  TH1F *ptgenrec2   = new TH1F("ptgenrec2",   "p_T gen+rec;P_T [GeV/c]",             s_bins,   0.0,    upper_pt);

  TH1F *d0gen       = new TH1F("d0gen",       "d_0 generated;d_0 [mm]",              s_bins,   -upper_d0,    upper_d0);
  TH1F *d0genrec    = new TH1F("d0genrec",    "d_0 gen+rec;d_0 [mm]",                s_bins,   -upper_d0,    upper_d0);

  TH1F *z0gen       = new TH1F("z0gen",       "z_0 generated;z_0 [mm]",              s_bins,   -upper_z0,    upper_z0);
  TH1F *z0genrec    = new TH1F("z0genrec",    "z_0 gen+rec;z_0 [mm]",                s_bins,   -upper_z0,    upper_z0);

  tree->Project("etagen",       "mctrack_eta",                  mccut_pt && mccut_signal);
  tree->Project("phigen",       "mctrack_phi",                  mccut_pt && mccut_barrel && mccut_signal);
  tree->Project("d0gen",        "sqrt(pow(mctrack_beginvertexx,2)+pow(mctrack_beginvertexy,2))", mccut_pt2 && mccut_barrel && mccut_signal);
  tree->Project("z0gen",        "mctrack_z0",                   mccut_pt2 && mccut_barrel && mccut_signal);
  tree->Project("etagenrec",    "track_truth_eta",                  cut_pt && cut_signal);
  tree->Project("phigenrec",    "track_truth_phi",                  cut_pt && cut_signal && cut_barrel);
  tree->Project("d0genrec",     "sqrt(pow(track_truth_beginvertexx,2)+pow(track_truth_beginvertexy,2))", cut_pt2 && cut_barrel && cut_signal);
  tree->Project("z0genrec",     "track_truth_z0",                   cut_pt2 && cut_barrel && cut_signal);

  tree->Project("ptgen",        "abs(1/mctrack_qoverpt)/1000", mccut_barrel && mccut_signal);
  tree->Project("ptgenrec",     "abs(1/track_truth_qoverpt)/1000", cut_barrel && cut_signal);
  tree->Project("ptgen2",       "abs(1/mctrack_qoverpt)/1000", mccut_barrel && mccut_signal);
  tree->Project("ptgenrec2",    "abs(1/track_truth_qoverpt)/1000", cut_barrel && cut_signal);

  TH1F *etaeff = (TH1F*) etagenrec->Clone("etaeff");  etaeff->Reset();
  etaeff->SetTitle("Tracking Efficiency Versus #eta (P_{T}>5GeV/c);#eta");
  TH1F *phieff = (TH1F*) phigenrec->Clone("phieff");  phieff->Reset();
  phieff->SetTitle("Tracking Efficiency Versus #phi (P_{T}>5GeV/c, |#eta|<0.8);#phi");
  TH1F *d0eff  = (TH1F*) d0genrec ->Clone("d0eff");   d0eff->Reset();
  d0eff->SetTitle("Tracking Efficiency Versus d_0 (P_{T}>3GeV/c, |#eta|<0.8);d_{0} [mm]");
  TH1F *z0eff  = (TH1F*) z0genrec ->Clone("z0eff");   z0eff->Reset();
  z0eff->SetTitle("Tracking Efficiency Versus z_0 (P_{T}>3GeV/c, |#eta|<0.8);z_{0} [mm]");

  etaeff->Divide(etagenrec,etagen,1,1);
  phieff->Divide(phigenrec,phigen,1,1);
  d0eff ->Divide(d0genrec,d0gen,1,1);
  z0eff ->Divide(z0genrec,z0gen,1,1);


  TH1F *pteff = (TH1F*) ptgenrec->Clone("pteff");
  pteff->SetTitle("tracking efficiency versus P_{T};P_{T} [GeV/c]");
  pteff->Reset();
  pteff->Divide(ptgenrec,ptgen,1,1);

  TH1F *pteff2 = (TH1F*) ptgenrec2->Clone("pteff2");
  pteff2->SetTitle("tracking efficiency versus P_{T}, |#eta|<0.8;P_{T} [GeV/c]");
  pteff2->Reset();
  pteff2->Divide(ptgenrec2,ptgen2,1,1);

  //etaeff->Fit("pol0","R","",-2.0,2.0);

  eff_setstyle(z0eff,hstyle);
  eff_setstyle(d0eff,hstyle);
  eff_setstyle(etaeff,hstyle);
  eff_setstyle(phieff,hstyle);
  eff_setstyle(pteff2,hstyle);

  //pteff2->Fit("pol0","R","",3.0,10.0);
}

void eff (TTree* tracktree[3], const char* name[3],
          const string plotprefix="efficiency_", const string plotext="",
          const int ntrack_finders=3,
          const TCut data_cut="", const TCut mc_cut="",
          const bool primary= true, const double match=-1,
          const int s_bins=40, const Double_t upper_pt=5.0)
{
  TDirectory *parent= gDirectory;
  TDirectory *dirs[ntrack_finders], *firstdir= 0;
  for (int i= 0; i<ntrack_finders; i++) {
    if (tracktree[i]) {
      if (gDirectory->Get(name[i])) gDirectory->rmdir(name[i]);
      gDirectory= dirs[i]= gDirectory->mkdir(name[i]);
      if (!firstdir) firstdir= gDirectory;
      eff_part (tracktree[i], i, data_cut, mc_cut, primary, match, s_bins, upper_pt);
      gDirectory= parent;
    }  else
      dirs[i]= 0;
  }

#ifdef DOALL
  // Make some copies for dCube compatibility
  copyObject ("newTracking", "phieff",   parent, "phieffnt");
  copyObject ("newTracking", "etaeff",   parent, "etaeffnt");
#  copyObject ("iPatRec",     "pteff2",   parent, "pteff2");
  copyObject ("newTracking", "pteff2", parent, "pteff2nt");
#endif

  if (plotext != "") {
    TStyle plotStyle ("PlotStyle", "My Plot Style");
    plotStyle.cd();

    TCanvas *distributions =  new TCanvas("distributions","various distributions for current data sample");
    TCanvas *eff_eta       =  new TCanvas("eff_eta");
    TCanvas *eff_pt        =  new TCanvas("eff_pt");
    TCanvas *eff_sum_pt    =  new TCanvas("eff_sum_pt" ,"Efficiency Versus P_T");
    TCanvas *eff_sum_eta   =  new TCanvas("eff_sum_eta","Efficiency Versus Eta");
    TCanvas *eff_sum_phi   =  new TCanvas("eff_sum_phi","Efficiency Versus Phi");
    TCanvas *eff_sum_z0    =  new TCanvas("eff_sum_z0" ,"Efficiency Versus z0");
    TCanvas *eff_sum_d0    =  new TCanvas("eff_sum_d0" ,"Efficiency Versus d0");

    distributions -> Divide(2,2);
    eff_eta       -> Divide(2,2);
    eff_pt        -> Divide(2,2);

    eff_eta->cd(1); eff_draw(firstdir,"etagen");
    eff_eta->cd(2); eff_draw(firstdir,"etagenrec");

    eff_eta->cd(3); eff_draw(firstdir,"etagen");
    eff_eta->cd(3); eff_draw(firstdir,"etagenrec","same");
    eff_eta->cd(4); eff_draw(firstdir,"etaeff");

    eff_pt->cd(1); eff_draw(firstdir,"ptgen");
    eff_pt->cd(1); eff_draw(firstdir,"ptgenrec","same");
    eff_pt->cd(2); eff_draw(firstdir,"pteff");

    eff_pt->cd(3); eff_draw(firstdir,"ptgen2");
    eff_pt->cd(3); eff_draw(firstdir,"ptgenrec2","same");
    eff_pt->cd(4); eff_drawall(ntrack_finders, dirs, "pteff2");


//  TLegend* legend= new TLegend(0.6,0.7,0.89,0.89);
    TLegend* legend= new TLegend(0.3,0.4,0.59,0.59);

    // draw summary histograms

    plotStyle.SetOptStat("");
    plotStyle.SetLineWidth(2);

    eff_sum_eta->cd(); gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified();
    eff_drawall(ntrack_finders,dirs,"etaeff", legend);

    eff_sum_phi->cd(); gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified();
    eff_drawall(ntrack_finders,dirs,"phieff", legend);

    eff_sum_pt ->cd();gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified();
    eff_drawall(ntrack_finders,dirs,"pteff2", legend);

    eff_sum_d0 ->cd(); gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified();
    eff_drawall(ntrack_finders,dirs,"d0eff", legend);

    eff_sum_z0 ->cd(); gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified();
    eff_drawall(ntrack_finders,dirs,"z0eff", legend);

    distributions->cd(1); gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified();
    eff_draw(firstdir,"d0gen");    eff_draw(firstdir,"d0genrec","same");

    distributions->cd(2); gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified();
    eff_draw(firstdir,"z0gen");   eff_draw(firstdir,"z0genrec","same");

    string plotfile = plotprefix + string("eta") + plotext;
    eff_sum_eta->Print(plotfile.c_str());

    plotfile = plotprefix + string("phi") + plotext;
    eff_sum_phi->Print(plotfile.c_str());

    plotfile = plotprefix + string("pt") + plotext;
    eff_sum_pt->Print(plotfile.c_str());

    gROOT->SetStyle("Default");

    delete legend;
    delete distributions;
    delete eff_eta;
    delete eff_pt;
    delete eff_sum_pt;
    delete eff_sum_eta;
    delete eff_sum_phi;
    delete eff_sum_z0;
    delete eff_sum_d0;
  }
}
