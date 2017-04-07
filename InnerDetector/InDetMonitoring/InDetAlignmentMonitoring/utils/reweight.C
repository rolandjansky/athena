/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TFile.h"
#include<string.h>
#include<stdio.h>
#include "TCanvas.h"
#include "TString.h"
#include "TProfile2D.h"
#include <iostream>

using namespace std;



void reweight(string  mcname, string  dtname)
{

  TFile *mcfile = new TFile(mcname.c_str());
  TFile *dtfile = new TFile(dtname.c_str());

  string histoName("trk_pT_vs_eta");
  string path("IDAlignMon/ExtendedTracks_noTrigSel_15GeV/GenericTracks/");
  string hh = path + histoName;
  cout << "name " << hh << endl;

  TH2F * mchisto = (TH2F*) mcfile->GetObjectUnchecked( hh.c_str() );
  TH2F * dthisto = (TH2F*) dtfile->GetObjectUnchecked( hh.c_str() );

  // mchisto->Scale(1./mchisto->Integral());
  // dthisto->Scale(1./dthisto->Integral());

  //  
  // 
  int nbins = mchisto->GetNbinsX();
  TH1D*  mcp = mchisto->ProjectionX("mcp", nbins-10, nbins+10);
  TH1D*  dtp = dthisto->ProjectionX("dtp", nbins-10, nbins+10);

  TProfile*   mcpp = mchisto->ProfileX("mcpp", nbins-10, nbins+10);
  TProfile*   dtpp = dthisto->ProfileX("dtpp", nbins-10, nbins+10);


  mcp->Scale(1./mcp->Integral());
  dtp->Scale(1./dtp->Integral());

  dtp->SetLineColor(kRed);
  mcp->Draw("histo");
  dtp->Draw("histo same");

  TCanvas *Bpm_canvas = new TCanvas("Bpm_fit","Bpm_fit", 2);
  dtpp->SetLineColor(kRed);
  mcpp->Draw("histo");
  dtpp->Draw("histo same");


  TH2F * weight = (TH2F*) mchisto->Clone();

  for (int ix = 0; ix < mchisto->GetNbinsX()+1; ix++){
    for(int iy = 0; iy < mchisto->GetNbinsY()+1; iy++){

      double mcbin = mchisto->GetBinContent(ix,iy);
      double dtbin = dthisto->GetBinContent(ix,iy);
      if ( mcbin != 0 ) weight->SetBinContent( ix, iy, dtbin / mcbin );
      else              weight->SetBinContent( ix, iy, 0 );
    }
  }

  TH1D* wp = weight->ProjectionX("wp", nbins-1, nbins+1);
  wp->Scale(wp->Integral());
  wp->Draw();
 
  TFile *wfile = new TFile("hWeight.root", "recreate");
  weight->Write();

  return;





}
