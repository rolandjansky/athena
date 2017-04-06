/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
// root[0] .L make_eop_maps.cxx
// root[1] make_eop_maps("path/to/fitEoverP.root",zrange)
//   -- or --
// root[1] make_sagitta_maps("path/to/Scaling.root",zrange)
// 
// @author: will.di.clemente@cern.ch
//
/////////////////////////////////////////////////////////////

#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TString.h"

void make_eop_maps(TString path, Float_t range=0.2){ //zmin=0.8, Float_t zmax=1.2){

  // Make the ratio histogram from the input file
  TFile *f = new TFile(path);
  TH2D *h1 = (TH2D*)f->Get("meanPlotPos");
  TH2D *h2 = (TH2D*)f->Get("meanPlotNeg");
  TH2D *outhist = h1->Clone();
  outhist->Divide(h2);

  // Set up the output canvas
  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  c1->SetRightMargin(0.13);

  // Set up ranges
  Float_t zmin = 1.-range;
  Float_t zmax = 1.+range;

  // Plot aesthetics
  outhist->SetTitle("E/p ratio <+>/<->");
  outhist->GetZaxis()->SetRangeUser(zmin,zmax);
  outhist->GetXaxis()->SetTitle("#eta");
  outhist->GetXaxis()->SetTitleOffset(1);
  outhist->GetYaxis()->SetTitle("#phi [rad]");
  outhist->GetYaxis()->SetTitleOffset(.7);
  TPaletteAxis *palette = (TPaletteAxis*)outhist->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.87);
  palette->SetX2NDC(0.92);
  palette->SetY1NDC(0.1);
  palette->SetY2NDC(0.9);

  // Draw and print
  outhist->Draw("colz");
  c1->Print("EoverP.pdf","Portrait pdf");

  return;
}

void make_sagitta_maps(TString path, Float_t range=0.004){

  // Make the ratio histogram from the input file
  TFile *f = new TFile(path);
  TH2D *outhist = (TH2D*)f->Get("FinalCorrections");

  // Set up the output canvas
  TCanvas *c1 = new TCanvas("c1","c1",800,600);
  c1->SetRightMargin(0.16);

  // Set up ranges
  range = fabs(range);
  Float_t zmin = (-1.)*range;
  Float_t zmax = range;

  // Plot aesthetics
  outhist->SetTitle("Sagitta Bias");
  outhist->GetZaxis()->SetRangeUser(zmin,zmax);
  outhist->GetXaxis()->SetTitle("#eta");
  outhist->GetXaxis()->SetTitleOffset(1);
  outhist->GetYaxis()->SetTitle("#phi [rad]");
  outhist->GetYaxis()->SetTitleOffset(.7);
  TPaletteAxis *palette = (TPaletteAxis*)outhist->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.84);
  palette->SetX2NDC(0.89);
  palette->SetY1NDC(0.1);
  palette->SetY2NDC(0.9);
  TGaxis::SetMaxDigits(2);

  // Draw and print
  outhist->Draw("colz");
  c1->Print("Sagitta.pdf","Portrait pdf");

  return;
}
