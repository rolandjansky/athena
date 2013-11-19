/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
//=========Macro generated from canvas: C_FMVCommon_Hit_Eta_1/MVCommon_Hit_Eta_1: Hit_Eta
//=========  (Thu Jun 22 01:09:19 2006) by ROOT version5.10/00
   TCanvas *C_FMVCommon_Hit_Eta_1 = new TCanvas("C_FMVCommon_Hit_Eta_1", "MVCommon_Hit_Eta_1: Hit_Eta",203,119,854,702);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   C_FMVCommon_Hit_Eta_1->ToggleEventStatus();
   C_FMVCommon_Hit_Eta_1->SetHighLightColor(2);
   C_FMVCommon_Hit_Eta_1->Range(-12.926,-2.32572,11.1254,16.8868);
   C_FMVCommon_Hit_Eta_1->SetFillColor(0);
   C_FMVCommon_Hit_Eta_1->SetBorderMode(0);
   C_FMVCommon_Hit_Eta_1->SetBorderSize(1);
   C_FMVCommon_Hit_Eta_1->SetLeftMargin(0.121658);
   C_FMVCommon_Hit_Eta_1->SetRightMargin(0.0467914);
   C_FMVCommon_Hit_Eta_1->SetTopMargin(0.054386);
   C_FMVCommon_Hit_Eta_1->SetBottomMargin(0.121053);
   C_FMVCommon_Hit_Eta_1->SetFrameFillStyle(0);
   C_FMVCommon_Hit_Eta_1->SetFrameLineStyle(0);
   C_FMVCommon_Hit_Eta_1->SetFrameBorderMode(0);
   C_FMVCommon_Hit_Eta_1->SetFrameFillStyle(0);
   C_FMVCommon_Hit_Eta_1->SetFrameLineStyle(0);
   C_FMVCommon_Hit_Eta_1->SetFrameBorderMode(0);
   
   TH1 *MVCommon_Hit_Eta_1 = new TH1F("MVCommon_Hit_Eta_1","Hit_Eta",60,-10,10);
   MVCommon_Hit_Eta_1->SetBinContent(7,5.51042);
   MVCommon_Hit_Eta_1->SetBinContent(10,11.0536);
   MVCommon_Hit_Eta_1->SetBinContent(13,7.51058);
   MVCommon_Hit_Eta_1->SetBinContent(16,7.16576);
   MVCommon_Hit_Eta_1->SetBinContent(19,7.16827);
   MVCommon_Hit_Eta_1->SetBinContent(22,6.50492);
   MVCommon_Hit_Eta_1->SetBinContent(25,7.21926);
   MVCommon_Hit_Eta_1->SetBinContent(28,6.87286);
   MVCommon_Hit_Eta_1->SetBinContent(31,5.83333);
   MVCommon_Hit_Eta_1->SetBinContent(34,7.62967);
   MVCommon_Hit_Eta_1->SetBinContent(37,7.29222);
   MVCommon_Hit_Eta_1->SetBinContent(40,6.32039);
   MVCommon_Hit_Eta_1->SetBinContent(43,6.55961);
   MVCommon_Hit_Eta_1->SetBinContent(46,6.9728);
   MVCommon_Hit_Eta_1->SetBinContent(49,7.26838);
   MVCommon_Hit_Eta_1->SetBinContent(52,15.0875);
   MVCommon_Hit_Eta_1->SetBinContent(55,4.625);
   MVCommon_Hit_Eta_1->SetBinError(7,5.51042);
   MVCommon_Hit_Eta_1->SetBinError(10,11.0536);
   MVCommon_Hit_Eta_1->SetBinError(13,7.51058);
   MVCommon_Hit_Eta_1->SetBinError(16,7.16576);
   MVCommon_Hit_Eta_1->SetBinError(19,7.16827);
   MVCommon_Hit_Eta_1->SetBinError(22,6.50492);
   MVCommon_Hit_Eta_1->SetBinError(25,7.21926);
   MVCommon_Hit_Eta_1->SetBinError(28,6.87286);
   MVCommon_Hit_Eta_1->SetBinError(31,5.83333);
   MVCommon_Hit_Eta_1->SetBinError(34,7.62967);
   MVCommon_Hit_Eta_1->SetBinError(37,7.29222);
   MVCommon_Hit_Eta_1->SetBinError(40,6.32039);
   MVCommon_Hit_Eta_1->SetBinError(43,6.55961);
   MVCommon_Hit_Eta_1->SetBinError(46,6.9728);
   MVCommon_Hit_Eta_1->SetBinError(49,7.26838);
   MVCommon_Hit_Eta_1->SetBinError(52,15.0875);
   MVCommon_Hit_Eta_1->SetBinError(55,4.625);
   MVCommon_Hit_Eta_1->SetEntries(17);

   Int_t ci;   // for color index setting
   ci = TColor::GetColor("#59ace0");
   MVCommon_Hit_Eta_1->SetFillColor(ci);
   MVCommon_Hit_Eta_1->SetMarkerStyle(7);
   MVCommon_Hit_Eta_1->GetXaxis()->SetTitle("Eta-Section");
   MVCommon_Hit_Eta_1->GetXaxis()->SetLabelSize(0.05);
   MVCommon_Hit_Eta_1->GetXaxis()->SetTitleSize(0.05);
   MVCommon_Hit_Eta_1->GetXaxis()->SetTitleOffset(0.99);
   MVCommon_Hit_Eta_1->GetYaxis()->SetTitle("Mean Number Of Digits");
   MVCommon_Hit_Eta_1->GetYaxis()->SetLabelSize(0.05);
   MVCommon_Hit_Eta_1->GetYaxis()->SetTitleSize(0.06);
   MVCommon_Hit_Eta_1->GetYaxis()->SetTitleOffset(0.83);
   MVCommon_Hit_Eta_1->GetZaxis()->SetLabelSize(0.03);
   MVCommon_Hit_Eta_1->GetZaxis()->SetTitleSize(0.03);
   MVCommon_Hit_Eta_1->Draw("hist");
   
   TH1 *MVCommon_Digit_Eta_1 = new TH1F("MVCommon_Digit_Eta_1","Digit_Eta",60,-10,10);
   MVCommon_Digit_Eta_1->SetBinContent(7,5.61905);
   MVCommon_Digit_Eta_1->SetBinContent(10,10.3867);
   MVCommon_Digit_Eta_1->SetBinContent(13,6.66477);
   MVCommon_Digit_Eta_1->SetBinContent(16,7.2359);
   MVCommon_Digit_Eta_1->SetBinContent(19,6.98558);
   MVCommon_Digit_Eta_1->SetBinContent(22,7.1348);
   MVCommon_Digit_Eta_1->SetBinContent(25,7.42556);
   MVCommon_Digit_Eta_1->SetBinContent(28,7.71773);
   MVCommon_Digit_Eta_1->SetBinContent(31,5.25);
   MVCommon_Digit_Eta_1->SetBinContent(34,7.28016);
   MVCommon_Digit_Eta_1->SetBinContent(37,7.86849);
   MVCommon_Digit_Eta_1->SetBinContent(40,6.63446);
   MVCommon_Digit_Eta_1->SetBinContent(43,6.88662);
   MVCommon_Digit_Eta_1->SetBinContent(46,7.01352);
   MVCommon_Digit_Eta_1->SetBinContent(49,7.06493);
   MVCommon_Digit_Eta_1->SetBinContent(52,14.2007);
   MVCommon_Digit_Eta_1->SetBinContent(55,4.71429);
   MVCommon_Digit_Eta_1->SetBinError(7,5.61905);
   MVCommon_Digit_Eta_1->SetBinError(10,10.3867);
   MVCommon_Digit_Eta_1->SetBinError(13,6.66477);
   MVCommon_Digit_Eta_1->SetBinError(16,7.2359);
   MVCommon_Digit_Eta_1->SetBinError(19,6.98558);
   MVCommon_Digit_Eta_1->SetBinError(22,7.1348);
   MVCommon_Digit_Eta_1->SetBinError(25,7.42556);
   MVCommon_Digit_Eta_1->SetBinError(28,7.71773);
   MVCommon_Digit_Eta_1->SetBinError(31,5.25);
   MVCommon_Digit_Eta_1->SetBinError(34,7.28016);
   MVCommon_Digit_Eta_1->SetBinError(37,7.86849);
   MVCommon_Digit_Eta_1->SetBinError(40,6.63446);
   MVCommon_Digit_Eta_1->SetBinError(43,6.88662);
   MVCommon_Digit_Eta_1->SetBinError(46,7.01352);
   MVCommon_Digit_Eta_1->SetBinError(49,7.06493);
   MVCommon_Digit_Eta_1->SetBinError(52,14.2007);
   MVCommon_Digit_Eta_1->SetBinError(55,4.71429);
   MVCommon_Digit_Eta_1->SetEntries(17);
   MVCommon_Digit_Eta_1->SetDirectory(0);
   MVCommon_Digit_Eta_1->SetFillStyle(0);
   MVCommon_Digit_Eta_1->SetLineColor(2);
   MVCommon_Digit_Eta_1->SetLineWidth(2);
   MVCommon_Digit_Eta_1->SetMarkerStyle(3);
   MVCommon_Digit_Eta_1->GetXaxis()->SetLabelSize(0.03);
   MVCommon_Digit_Eta_1->GetXaxis()->SetTitleSize(0.03);
   MVCommon_Digit_Eta_1->GetYaxis()->SetLabelSize(0.03);
   MVCommon_Digit_Eta_1->GetYaxis()->SetTitleSize(0.03);
   MVCommon_Digit_Eta_1->GetZaxis()->SetLabelSize(0.03);
   MVCommon_Digit_Eta_1->GetZaxis()->SetTitleSize(0.03);
   MVCommon_Digit_Eta_1->Draw("histhistsame");
   C_FMVCommon_Hit_Eta_1->Modified();
   C_FMVCommon_Hit_Eta_1->cd();
   C_FMVCommon_Hit_Eta_1->SetSelected(C_FMVCommon_Hit_Eta_1);
   C_FMVCommon_Hit_Eta_1->SaveAs("MV_Hit_Eta_R_Rpp.RTT.ps")
}
