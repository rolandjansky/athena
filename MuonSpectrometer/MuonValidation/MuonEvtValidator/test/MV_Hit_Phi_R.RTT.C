/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
//=========Macro generated from canvas: C_FMVCommon_Hit_Phi_1/MVCommon_Hit_Phi_1: Hit_Phi
//=========  (Thu Jun 22 01:10:48 2006) by ROOT version5.10/00
   TCanvas *C_FMVCommon_Hit_Phi_1 = new TCanvas("C_FMVCommon_Hit_Phi_1", "MVCommon_Hit_Phi_1: Hit_Phi",338,95,854,702);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   C_FMVCommon_Hit_Phi_1->ToggleEventStatus();
   C_FMVCommon_Hit_Phi_1->SetHighLightColor(2);
   C_FMVCommon_Hit_Phi_1->Range(-1.18971,-1.48111,10.836,8.45103);
   C_FMVCommon_Hit_Phi_1->SetFillColor(0);
   C_FMVCommon_Hit_Phi_1->SetBorderMode(0);
   C_FMVCommon_Hit_Phi_1->SetBorderSize(1);
   C_FMVCommon_Hit_Phi_1->SetRightMargin(0.0695187);
   C_FMVCommon_Hit_Phi_1->SetTopMargin(0.0561404);
   C_FMVCommon_Hit_Phi_1->SetBottomMargin(0.149123);
   C_FMVCommon_Hit_Phi_1->SetFrameFillStyle(0);
   C_FMVCommon_Hit_Phi_1->SetFrameLineStyle(0);
   C_FMVCommon_Hit_Phi_1->SetFrameBorderMode(0);
   C_FMVCommon_Hit_Phi_1->SetFrameFillStyle(0);
   C_FMVCommon_Hit_Phi_1->SetFrameLineStyle(0);
   C_FMVCommon_Hit_Phi_1->SetFrameBorderMode(0);
   
   TH1 *MVCommon_Hit_Phi_1 = new TH1F("MVCommon_Hit_Phi_1","Hit_Phi",30,0,10);
   MVCommon_Hit_Phi_1->SetBinContent(4,6.88233);
   MVCommon_Hit_Phi_1->SetBinContent(7,6.8379);
   MVCommon_Hit_Phi_1->SetBinContent(10,7.14025);
   MVCommon_Hit_Phi_1->SetBinContent(13,7.51756);
   MVCommon_Hit_Phi_1->SetBinContent(16,7.06422);
   MVCommon_Hit_Phi_1->SetBinContent(19,6.94495);
   MVCommon_Hit_Phi_1->SetBinContent(22,7.17403);
   MVCommon_Hit_Phi_1->SetBinContent(25,7.21547);
   MVCommon_Hit_Phi_1->SetBinError(4,6.88233);
   MVCommon_Hit_Phi_1->SetBinError(7,6.8379);
   MVCommon_Hit_Phi_1->SetBinError(10,7.14025);
   MVCommon_Hit_Phi_1->SetBinError(13,7.51756);
   MVCommon_Hit_Phi_1->SetBinError(16,7.06422);
   MVCommon_Hit_Phi_1->SetBinError(19,6.94495);
   MVCommon_Hit_Phi_1->SetBinError(22,7.17403);
   MVCommon_Hit_Phi_1->SetBinError(25,7.21547);
   MVCommon_Hit_Phi_1->SetEntries(9);

   Int_t ci;   // for color index setting
   ci = TColor::GetColor("#59ace0");
   MVCommon_Hit_Phi_1->SetFillColor(ci);
   MVCommon_Hit_Phi_1->SetMarkerStyle(7);
   MVCommon_Hit_Phi_1->GetXaxis()->SetTitle("Phi-Sector");
   MVCommon_Hit_Phi_1->GetXaxis()->SetLabelSize(0.05);
   MVCommon_Hit_Phi_1->GetXaxis()->SetTitleSize(0.06);
   MVCommon_Hit_Phi_1->GetXaxis()->SetTitleOffset(0.86);
   MVCommon_Hit_Phi_1->GetYaxis()->SetTitle("Mean Number Of Digits");
   MVCommon_Hit_Phi_1->GetYaxis()->SetLabelSize(0.05);
   MVCommon_Hit_Phi_1->GetYaxis()->SetTitleSize(0.06);
   MVCommon_Hit_Phi_1->GetYaxis()->SetTitleOffset(0.69);
   MVCommon_Hit_Phi_1->GetZaxis()->SetLabelSize(0.03);
   MVCommon_Hit_Phi_1->GetZaxis()->SetTitleSize(0.03);
   MVCommon_Hit_Phi_1->Draw("hist");
   C_FMVCommon_Hit_Phi_1->Modified();
   C_FMVCommon_Hit_Phi_1->cd();
   C_FMVCommon_Hit_Phi_1->SetSelected(C_FMVCommon_Hit_Phi_1);
   C_FMVCommon_Hit_Phi_1->SaveAs("MV_Hit_Phi_R.RTT.ps")
}
