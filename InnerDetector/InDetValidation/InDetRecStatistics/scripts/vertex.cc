/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File: $Id: vertex.cc,v 1.5 2008-07-23 20:51:23 adye Exp $
#include "TString.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TCut.h"
#include "TLegend.h"

void vertex (TTree *tree, const string & plotfile= "",
             const TCut data_cut="", const TCut mc_cut="", double match=-1,
             const int s_bins=20)
{
  // turn on overflow (outlier) bins

  TCanvas *eff = new TCanvas("eff","",1000,400);
  eff->Divide(2,1);
  
  TH1F *vertexgen    = new TH1F("vertexgen",       "z0 for tracks w/ Pt>1GeV",    s_bins, -200.00,    200.00);
  TH1F *vertexgenrec = new TH1F("vertexgenrec",    "z0 rec, Pt>1GeV",          s_bins, -200.00,    200.00);
                       new TH1F("vertexrec",       "z0 rec, Pt>1GeV",          s_bins, -200.00,    200.00);
  TH1F *vertexdiff   = new TH1F("vertexdiff",      "z0 rec - z0 gen, tracks w/ Pt>1GeV", s_bins, -0.5,       0.500);

  TCut truth_cut ("track_truth_prob>"+TString::Format("%g",(match>=0?match:0.7)));

  tree->Project("vertexgen",    "mctrack_z0",              "abs(1/mctrack_qoverpt)>1000" && mc_cut);
  //cppcheck-suppress incorrectStringBooleanError
  tree->Project("vertexgenrec", "track_truth_z0",          truth_cut && "abs(1/track_truth_qoverpt)>1000" && data_cut);
  tree->Project("vertexrec",    "track_z0",                "abs(1/track_qoverpt)>1000" && data_cut);
  tree->Project("vertexdiff",   "track_z0-track_truth_z0", "abs(1/track_qoverpt)>1000" && data_cut);
  
 
  
  vertexgen->GetXaxis()->SetTitle("z0 [mm]"); 
  vertexdiff->GetXaxis()->SetTitle("#Deltaz0 (gen-rec) [mm]"); 

  vertexgenrec->SetLineStyle(2);
  TLegend *leg=new TLegend (0.15,0.85,0.45,0.7);
  leg->AddEntry(vertexgen,"generated tracks","l");
  leg->AddEntry(vertexgenrec,"reconstructed tracks","l");

  eff->cd(1); vertexgen->Draw();


  eff->cd(1); vertexgenrec->Draw("same"); 
  leg->Draw();

  eff->cd(2); vertexdiff->Draw();

  if (plotfile != "") eff->Print(plotfile.c_str());
  delete leg;
  delete eff;
}
