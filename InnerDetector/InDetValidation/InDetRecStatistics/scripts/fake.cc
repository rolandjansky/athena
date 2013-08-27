/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File: $Id: fake.cc,v 1.8 2008-07-23 20:51:23 adye Exp $
#include <cfloat>
#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TCut.h"
#include "TLegend.h"

using std::cerr;
using std::endl;


void fake_setstyle (TH1 *h, int hstyle)
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


TH1* fake_hist (TDirectory* dir, const char* name)
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


void fake_drawall (const int ntrack_finders, TDirectory** dir, const char* name, TLegend* legend= 0)
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
      TH1* h= fake_hist(dir[i], name);
      if (h && h->GetMaximum() > maxval) maxval= h->GetMaximum();
    }
  }

  for (int j= 0; j<ntrack_finders; j++) {
#if DUPOLD==1
    const int draw_order[3]= {1,2,0};
    int i= ntrack_finders==3 ? draw_order[j] : j;
#else
    int i= j;
#endif
    if (dir[i]) {
      TH1* h= fake_hist(dir[i], name);
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


TH1F * hist_fake (char * name, char * title, TH1F * numerator, TH1F * denominator, char *axistitle)
{
  TH1F * fake = (TH1F*) denominator->Clone(name);
  fake->SetTitle(title);
  fake->Reset();  
  fake->Divide(numerator,denominator,1,1);
  fake->GetXaxis()->SetTitle(axistitle);
  return fake;
}


void fake_part (TTree* tree, const int hstyle=0, const TCut data_cut="", double match=-1, const int s_bins=20)
{
  char * pt_title  = "Track Fake-rate Versus Reconstructed P_{T}, |#eta|<0.8";
  char * eta_title = "Track Fake-rate Versus Reconstruced #eta, P_{T}>5GeV/c";
  char * phi_title = "Track Fake-rate Versus Reconstruced #phi, P_{T}>5GeV/c, |#eta|<0.8";
  char * d0_title  = "Track Fake-rate Versus Reconstruced d0, P_{T}>2GeV/c, |#eta|<0.8";
  char * z0_title  = "Track Fake-rate Versus Reconstruced z0, P_{T}>2GeV/c, |#eta|<0.8";

  TCut truth_cut            ("track_truth_prob<"+TString::Format("%g",(match>=0?match:0.5)));
  TCut cut_barrel         = "(abs(track_eta) < 0.80)";
  TCut cut_endcap         = "(1.6 < abs(track_eta) < 2.50)";
  TCut cut_pt             = "(abs(1./track_qoverpt)>5000)";
  TCut cut_pt2            = "(abs(1./track_qoverpt)>2000)";

  TCut cut_eta = cut_barrel; 

  TCut fake_cut = data_cut && truth_cut;
  if (*data_cut.GetTitle()) cout << "data_cut: " << data_cut.GetTitle() << endl;
  if (*fake_cut.GetTitle()) cout << "fake_cut: " << fake_cut.GetTitle() << endl;

  TH1F *etarec     = new TH1F("etarec",     "eta rec",              s_bins, -3.00,    3.00);
  TH1F *etafakerec = new TH1F("etafakerec", "eta fake-rec",         s_bins, -3.00,    3.00);
 
  TH1F *phirec    = new TH1F("phirec",    "phi rec",                s_bins, -3.50,    3.50);
  TH1F *phifakerec = new TH1F("phifakerec", "phi gen+rec",            s_bins, -3.50,    3.50);

  TH1F *ptrec     = new TH1F("ptrec",     "pt rec ",                      s_bins,   0.0,    5.00);
  TH1F *ptfakerec = new TH1F("ptfakerec", "pt gen+rec",                     s_bins,   0.0,    5.00);

  TH1F *d0rec     = new TH1F("d0rec",     "d0 rec",                 s_bins, -40.,      40.);
  TH1F *d0fakerec = new TH1F("d0fakerec", "d0 fake-rec",            s_bins, -40.,      40.);

  TH1F *z0rec     = new TH1F("z0rec",     "z0 rec",                 s_bins, -350.,     350.);
  TH1F *z0fakerec = new TH1F("z0fakerec", "z0 fake-rec",            s_bins, -350.,     350.);

  ptrec->GetXaxis()->SetTitle("P_T [GeV/c]"); 
  etarec->GetXaxis()->SetTitle("#eta"); 
  phirec->GetXaxis()->SetTitle("#phi"); 
  d0rec->GetXaxis()->SetTitle("d0 [mm]"); 
  z0rec->GetXaxis()->SetTitle("z0 [mm]"); 

  tree->Project("ptrec",      "abs(1./track_qoverpt)/1000",             cut_barrel && data_cut);
  tree->Project("ptfakerec",  "abs(1./track_qoverpt)/1000",             cut_barrel && fake_cut);
  tree->Project("etarec",     "track_eta",                              cut_pt && data_cut );
  tree->Project("etafakerec", "track_eta",                              cut_pt && fake_cut);
  tree->Project("phirec",     "track_phi",                              cut_pt && cut_eta && data_cut);
  tree->Project("phifakerec", "track_phi",                              cut_pt && cut_eta && fake_cut);
  tree->Project("d0rec",      "track_d0",                               cut_pt2 && cut_barrel && data_cut );
  tree->Project("d0fakerec",  "track_d0",                               cut_pt2 && cut_barrel && fake_cut);
  tree->Project("z0rec",      "track_z0",                               cut_pt2 && cut_barrel && data_cut );
  tree->Project("z0fakerec",  "track_z0",                               cut_pt2 && cut_barrel && fake_cut);

   // z0 histograms
  TH1F *z0fake = hist_fake ("z0fake", z0_title, z0fakerec, z0rec, "z0 [mm]");

   // d0 histograms
  TH1F *d0fake = hist_fake ("d0fake", d0_title, d0fakerec, d0rec, "d0 [mm]");

  // eta histograms
  TH1F *etafake = hist_fake ("etafake", eta_title, etafakerec, etarec, "#eta");

  // phi histograms
  TH1F *phifake = hist_fake ("phifake", phi_title, phifakerec, phirec, "#phi");

  // pt histograms
  TH1F *ptfake = hist_fake ("ptfake", pt_title, ptfakerec, ptrec, "P_{T} [GeV/c]");

  fake_setstyle(z0fake,hstyle);
  fake_setstyle(d0fake,hstyle);
  fake_setstyle(etafake,hstyle);
  fake_setstyle(phifake,hstyle);
  fake_setstyle(ptfake,hstyle);
}


void fake (TTree* tracktree[3], const char* name[3],
           const string plotprefix="fake_", const string plotext="",
           const int ntrack_finders=3,
           const TCut data_cut="", const double match=-1, const int s_bins=20)
{
  TDirectory *parent= gDirectory;
  TDirectory *dirs[ntrack_finders], *firstdir= 0;
  for (int i= 0; i<ntrack_finders; i++) {
    if (tracktree[i]) {
      if (gDirectory->Get(name[i])) gDirectory->rmdir(name[i]);
      gDirectory= dirs[i]= gDirectory->mkdir(name[i]);
      if (!firstdir) firstdir= gDirectory;
      fake_part (tracktree[i], i, data_cut, match, s_bins);
      gDirectory= parent;
    } else
      dirs[i]= 0;
  }

#ifdef DOALL
  // Make some copies for dCube compatibility
  copyObject ("newTracking", "d0fake",  parent, "d0fakent");
  copyObject ("newTracking", "z0fake",  parent, "z0fakent");
  copyObject ("newTracking", "phifake", parent, "phifakent");
  copyObject ("newTracking", "etafake", parent, "etafakent");
  copyObject ("newTracking", "ptfake",  parent, "ptfakent");
#endif

  // draw summary histograms

  TStyle plotStyle ("PlotStyle", "My Plot Style");
  plotStyle.cd();

  TCanvas *fake_pt  =  new TCanvas("fake_pt" ,"Fake-rate Versus P_T");
  TCanvas *fake_eta =  new TCanvas("fake_eta","Fake-rate Versus Eta");
  TCanvas *fake_phi =  new TCanvas("fake_phi","Fake-rate Versus Phi");
  TCanvas *fake_d0  =  new TCanvas("fake_d0"    ,"Fake-rate Versus d0");
  TCanvas *fake_z0  =  new TCanvas("fake_z0"    ,"Fake-rate Versus z0");

  plotStyle.SetOptStat(""); 
  plotStyle.SetLineWidth(2);
  
  fake_d0->cd(); gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified(); 
  fake_drawall(ntrack_finders,dirs,"d0fake");
  
  fake_z0->cd(); gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified(); 
  fake_drawall(ntrack_finders,dirs,"z0fake");

  fake_eta->cd(); gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified(); 
  fake_drawall(ntrack_finders,dirs,"etafake");

  fake_phi->cd(); gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified(); 
  fake_drawall(ntrack_finders,dirs,"phifake");

  fake_pt->cd();  gPad->SetGridx(1); gPad->SetGridy(1); gPad->Modified(); 
  fake_drawall(ntrack_finders,dirs,"ptfake");

  if (plotext != "") {
    string plotfile;
    plotfile = plotprefix+"pt" +plotext; fake_pt ->Print(plotfile.c_str());
    plotfile = plotprefix+"eta"+plotext; fake_eta->Print(plotfile.c_str());
    plotfile = plotprefix+"phi"+plotext; fake_phi->Print(plotfile.c_str());
    plotfile = plotprefix+"d0" +plotext; fake_d0 ->Print(plotfile.c_str());
    plotfile = plotprefix+"z0" +plotext; fake_z0 ->Print(plotfile.c_str());
  }

  gROOT->SetStyle("Default");

  delete fake_pt;
  delete fake_eta;
  delete fake_phi;
  delete fake_d0;
  delete fake_z0;
}
