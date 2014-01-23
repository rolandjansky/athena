/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
//=========Macro generated from canvas: c2_Hit_Barrel_mdt_middle_dEta/c2_Hit_Barrel_mdt_middle_dEta
//=========  (Fri Jun 25 09:57:18 2010) by ROOT version5.26/00
   TCanvas *c2_Hit_Barrel_mdt_middle_dEta = new TCanvas("c2_Hit_Barrel_mdt_middle_dEta", "c2_Hit_Barrel_mdt_middle_dEta",15,34,700,500);
   gStyle->SetOptFit(1);
   c2_Hit_Barrel_mdt_middle_dEta->Range(584.8655,0.007389661,11792.89,0.07888665);
   c2_Hit_Barrel_mdt_middle_dEta->SetFillColor(0);
   c2_Hit_Barrel_mdt_middle_dEta->SetBorderMode(0);
   c2_Hit_Barrel_mdt_middle_dEta->SetBorderSize(2);
   c2_Hit_Barrel_mdt_middle_dEta->SetTickx(1);
   c2_Hit_Barrel_mdt_middle_dEta->SetTicky(1);
   c2_Hit_Barrel_mdt_middle_dEta->SetLeftMargin(0.16);
   c2_Hit_Barrel_mdt_middle_dEta->SetRightMargin(0.05);
   c2_Hit_Barrel_mdt_middle_dEta->SetTopMargin(0.08);
   c2_Hit_Barrel_mdt_middle_dEta->SetBottomMargin(0.16);
   c2_Hit_Barrel_mdt_middle_dEta->SetFrameBorderMode(0);
   c2_Hit_Barrel_mdt_middle_dEta->SetFrameBorderMode(0);
   
   TGraphErrors *gre = new TGraphErrors(4);
   gre->SetName("Graph");
   gre->SetTitle("MDT Middle Barrel;<p_{T}> (MeV);#sigma_{ #Delta#eta}");
   gre->SetFillColor(1);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(1.2);
   gre->SetPoint(0,3116.011,0.06793699);
   gre->SetPointError(0,0,0.0007017632);
   gre->SetPoint(1,4780.185,0.05047582);
   gre->SetPointError(1,0,0.0004233572);
   gre->SetPoint(2,6770.835,0.03710823);
   gre->SetPointError(2,0,0.000653494);
   gre->SetPoint(3,10494.63,0.02408306);
   gre->SetPointError(3,0,0.0007257387);
   
   TH1F *Graph1 = new TH1F("Graph1","MDT Middle Barrel",100,2378.149,11232.49);
   Graph1->SetMinimum(0.01882918);
   Graph1->SetMaximum(0.07316689);
   Graph1->SetDirectory(0);
   Graph1->SetStats(0);
   Graph1->SetLineWidth(2);
   Graph1->SetMarkerStyle(20);
   Graph1->SetMarkerSize(1.2);
   Graph1->GetXaxis()->SetTitle("<p_{T}> (MeV)");
   Graph1->GetXaxis()->SetLabelFont(42);
   Graph1->GetXaxis()->SetLabelSize(0.05);
   Graph1->GetXaxis()->SetTitleSize(0.05);
   Graph1->GetXaxis()->SetTitleOffset(1.4);
   Graph1->GetXaxis()->SetTitleFont(42);
   Graph1->GetYaxis()->SetTitle("#sigma_{ #Delta#eta}");
   Graph1->GetYaxis()->SetLabelFont(42);
   Graph1->GetYaxis()->SetLabelSize(0.05);
   Graph1->GetYaxis()->SetTitleSize(0.05);
   Graph1->GetYaxis()->SetTitleOffset(1.4);
   Graph1->GetYaxis()->SetTitleFont(42);
   Graph1->GetZaxis()->SetLabelFont(42);
   Graph1->GetZaxis()->SetLabelSize(0.05);
   Graph1->GetZaxis()->SetTitleSize(0.05);
   Graph1->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph1);
   
   
   TF1 *fitCone_3 = new TF1("fitCone_3","[0]/pow(x,[1])",2378.149,11232.49);
   fitCone_3->SetFillColor(19);
   fitCone_3->SetFillStyle(0);
   fitCone_3->SetMarkerStyle(20);
   fitCone_3->SetMarkerSize(1.2);
   fitCone_3->SetLineWidth(3);
   fitCone_3->SetChisquare(21.47156);
   fitCone_3->SetNDF(2);
   fitCone_3->GetXaxis()->SetLabelFont(42);
   fitCone_3->GetXaxis()->SetLabelSize(0.05);
   fitCone_3->GetXaxis()->SetTitleSize(0.05);
   fitCone_3->GetXaxis()->SetTitleOffset(1.4);
   fitCone_3->GetXaxis()->SetTitleFont(42);
   fitCone_3->GetYaxis()->SetLabelFont(42);
   fitCone_3->GetYaxis()->SetLabelSize(0.05);
   fitCone_3->GetYaxis()->SetTitleSize(0.05);
   fitCone_3->GetYaxis()->SetTitleOffset(1.4);
   fitCone_3->GetYaxis()->SetTitleFont(42);
   fitCone_3->SetParameter(0,43.72761);
   fitCone_3->SetParError(0,6.731766);
   fitCone_3->SetParLimits(0,0,0);
   fitCone_3->SetParameter(1,0.8012541);
   fitCone_3->SetParError(1,0.01831433);
   fitCone_3->SetParLimits(1,0,0);
   gre->GetListOfFunctions()->Add(fitCone_3);
   
   TPaveStats *ptstats = new TPaveStats(0.59,0.76,0.95,0.92,"brNDC");
   ptstats->SetName("stats");
   ptstats->SetBorderSize(1);
   ptstats->SetTextAlign(12);
   ptstats->SetTextFont(42);
   TText *text = ptstats->AddText("Prob  = 2.175e-05");
   text = ptstats->AddText("p0       = 43.73 #pm 6.732 ");
   text = ptstats->AddText("p1       = 0.8013 #pm 0.01831 ");
   ptstats->SetOptStat(0);
   ptstats->SetOptFit(1011);
   ptstats->Draw();
   gre->GetListOfFunctions()->Add(ptstats);
   ptstats->SetParent(gre->GetListOfFunctions());
   gre->Draw("pa");
   
   TPaveText *pt = new TPaveText(0.4142529,0.945,0.6957471,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   pt->SetFillStyle(0);
   pt->SetTextFont(42);
   text = pt->AddText("MDT Middle Barrel");
   pt->Draw();
   c2_Hit_Barrel_mdt_middle_dEta->Modified();
   c2_Hit_Barrel_mdt_middle_dEta->cd();
   c2_Hit_Barrel_mdt_middle_dEta->SetSelected(c2_Hit_Barrel_mdt_middle_dEta);
}
