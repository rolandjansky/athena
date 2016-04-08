/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
//=========Macro generated from canvas: c_eff_muID/c_eff_muID
//=========  (Thu Jun 24 16:32:39 2010) by ROOT version5.26/00
   TCanvas *c_eff_muID = new TCanvas("c_eff_muID", "c_eff_muID",15,34,700,500);
   gStyle->SetOptFit(1);
   c_eff_muID->Range(-4.273418,0.7770854,12.43544,1.037297);
   c_eff_muID->SetFillColor(0);
   c_eff_muID->SetBorderMode(0);
   c_eff_muID->SetBorderSize(2);
   c_eff_muID->SetTickx(1);
   c_eff_muID->SetTicky(1);
   c_eff_muID->SetLeftMargin(0.16);
   c_eff_muID->SetRightMargin(0.05);
   c_eff_muID->SetTopMargin(0.08);
   c_eff_muID->SetBottomMargin(0.16);
   c_eff_muID->SetFrameBorderMode(0);
   c_eff_muID->SetFrameBorderMode(0);
   
   TGraphAsymmErrors *grae = new TGraphAsymmErrors(11);
   grae->SetName("");
   grae->SetTitle("Muon id. efficiency w.r.t. candidates passing FEX ;No. of associated hits;Efficiency");
   grae->SetFillColor(1);
   grae->SetMarkerStyle(20);
   grae->SetMarkerSize(1.2);
   grae->SetPoint(0,0,1);
   grae->SetPointError(0,0.5,0.5,0.003383212,0);
   grae->SetPoint(1,1,0.9615385);
   grae->SetPointError(1,0.5,0.5,0.01143171,0.009599082);
   grae->SetPoint(2,2,0.9585799);
   grae->SetPointError(2,0.5,0.5,0.01179856,0.009979226);
   grae->SetPoint(3,3,0.9585799);
   grae->SetPointError(3,0.5,0.5,0.01179856,0.009979226);
   grae->SetPoint(4,4,0.9556213);
   grae->SetPointError(4,0.5,0.5,0.01215085,0.01034459);
   grae->SetPoint(5,5,0.9497041);
   grae->SetPointError(5,0.5,0.5,0.01281693,0.01103627);
   grae->SetPoint(6,6,0.9497041);
   grae->SetPointError(6,0.5,0.5,0.01281693,0.01103627);
   grae->SetPoint(7,7,0.9349112);
   grae->SetPointError(7,0.5,0.5,0.01430087,0.01258235);
   grae->SetPoint(8,8,0.9201183);
   grae->SetPointError(8,0.5,0.5,0.01558788,0.01392997);
   grae->SetPoint(9,9,0.8846154);
   grae->SetPointError(9,0.5,0.5,0.01812826,0.0166131);
   grae->SetPoint(10,10,0.8550296);
   grae->SetPointError(10,0.5,0.5,0.01983024,0.01843275);
   
   TH1F *Graph10 = new TH1F("Graph10","Muon id. efficiency w.r.t. candidates passing FEX ",100,-1.6,11.6);
   Graph10->SetMinimum(0.8187193);
   Graph10->SetMaximum(1.01648);
   Graph10->SetDirectory(0);
   Graph10->SetStats(0);
   Graph10->SetLineWidth(2);
   Graph10->SetMarkerStyle(20);
   Graph10->SetMarkerSize(1.2);
   Graph10->GetXaxis()->SetTitle("No. of associated hits");
   Graph10->GetXaxis()->SetLabelFont(42);
   Graph10->GetXaxis()->SetLabelSize(0.05);
   Graph10->GetXaxis()->SetTitleSize(0.05);
   Graph10->GetXaxis()->SetTitleOffset(1.4);
   Graph10->GetXaxis()->SetTitleFont(42);
   Graph10->GetYaxis()->SetTitle("Efficiency");
   Graph10->GetYaxis()->SetLabelFont(42);
   Graph10->GetYaxis()->SetLabelSize(0.05);
   Graph10->GetYaxis()->SetTitleSize(0.05);
   Graph10->GetYaxis()->SetTitleOffset(1.4);
   Graph10->GetYaxis()->SetTitleFont(42);
   Graph10->GetZaxis()->SetLabelFont(42);
   Graph10->GetZaxis()->SetLabelSize(0.05);
   Graph10->GetZaxis()->SetTitleSize(0.05);
   Graph10->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph10);
   
   grae->Draw("ap");
   
   TPaveText *pt = new TPaveText(0.205,0.9390678,0.905,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   pt->SetFillStyle(0);
   pt->SetTextFont(42);
   TText *text = pt->AddText("Muon id. efficiency w.r.t. candidates passing FEX ");
   pt->Draw();
   c_eff_muID->Modified();
   c_eff_muID->cd();
   c_eff_muID->SetSelected(c_eff_muID);
}
