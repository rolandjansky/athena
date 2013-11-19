/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
//=========Macro generated from canvas: C_FMVCommon_Hit_NameID_1/MVCommon_Hit_NameID_1: Hit_NameID
//=========  (Thu Jun 22 01:12:37 2006) by ROOT version5.10/00
   TCanvas *C_FMVCommon_Hit_NameID_1 = new TCanvas("C_FMVCommon_Hit_NameID_1", "MVCommon_Hit_NameID_1: Hit_NameID",344,77,855,680);
   gStyle->SetOptStat(0);
   C_FMVCommon_Hit_NameID_1->ToggleEventStatus();
   C_FMVCommon_Hit_NameID_1->SetHighLightColor(2);
   C_FMVCommon_Hit_NameID_1->Range(-2.68779,-1.68839,24.517,10.7157);
   C_FMVCommon_Hit_NameID_1->SetFillColor(0);
   C_FMVCommon_Hit_NameID_1->SetBorderMode(0);
   C_FMVCommon_Hit_NameID_1->SetBorderSize(1);
   C_FMVCommon_Hit_NameID_1->SetRightMargin(0.0373832);
   C_FMVCommon_Hit_NameID_1->SetTopMargin(0.0580762);
   C_FMVCommon_Hit_NameID_1->SetBottomMargin(0.136116);
   C_FMVCommon_Hit_NameID_1->SetFrameFillStyle(0);
   C_FMVCommon_Hit_NameID_1->SetFrameLineStyle(0);
   C_FMVCommon_Hit_NameID_1->SetFrameBorderMode(0);
   C_FMVCommon_Hit_NameID_1->SetFrameFillStyle(0);
   C_FMVCommon_Hit_NameID_1->SetFrameLineStyle(0);
   C_FMVCommon_Hit_NameID_1->SetFrameBorderMode(0);
   
   TH1 *MVCommon_Hit_NameID_1 = new TH1F("MVCommon_Hit_NameID_1","Hit_NameID",120,0,60);
   MVCommon_Hit_NameID_1->SetBinContent(1,8.64735);
   MVCommon_Hit_NameID_1->SetBinContent(3,9.51933);
   MVCommon_Hit_NameID_1->SetBinContent(5,6.6956);
   MVCommon_Hit_NameID_1->SetBinContent(7,6.26295);
   MVCommon_Hit_NameID_1->SetBinContent(9,6.23895);
   MVCommon_Hit_NameID_1->SetBinContent(11,6.21782);
   MVCommon_Hit_NameID_1->SetBinContent(13,7.89368);
   MVCommon_Hit_NameID_1->SetBinContent(15,7.83116);
   MVCommon_Hit_NameID_1->SetBinContent(17,7.25893);
   MVCommon_Hit_NameID_1->SetBinContent(19,6.45052);
   MVCommon_Hit_NameID_1->SetBinContent(21,6.17111);
   MVCommon_Hit_NameID_1->SetBinContent(27,7.61259);
   MVCommon_Hit_NameID_1->SetBinContent(35,6.52498);
   MVCommon_Hit_NameID_1->SetBinContent(37,6.59659);
   MVCommon_Hit_NameID_1->SetBinContent(41,6.18493);
   MVCommon_Hit_NameID_1->SetBinContent(43,5.98427);
   MVCommon_Hit_NameID_1->SetBinContent(99,8.87007);
   MVCommon_Hit_NameID_1->SetBinError(1,8.64735);
   MVCommon_Hit_NameID_1->SetBinError(3,9.51933);
   MVCommon_Hit_NameID_1->SetBinError(5,6.6956);
   MVCommon_Hit_NameID_1->SetBinError(7,6.26295);
   MVCommon_Hit_NameID_1->SetBinError(9,6.23895);
   MVCommon_Hit_NameID_1->SetBinError(11,6.21782);
   MVCommon_Hit_NameID_1->SetBinError(13,7.89368);
   MVCommon_Hit_NameID_1->SetBinError(15,7.83116);
   MVCommon_Hit_NameID_1->SetBinError(17,7.25893);
   MVCommon_Hit_NameID_1->SetBinError(19,6.45052);
   MVCommon_Hit_NameID_1->SetBinError(21,6.17111);
   MVCommon_Hit_NameID_1->SetBinError(27,7.61259);
   MVCommon_Hit_NameID_1->SetBinError(35,6.52498);
   MVCommon_Hit_NameID_1->SetBinError(37,6.59659);
   MVCommon_Hit_NameID_1->SetBinError(41,6.18493);
   MVCommon_Hit_NameID_1->SetBinError(43,5.98427);
   MVCommon_Hit_NameID_1->SetBinError(99,8.87007);
   MVCommon_Hit_NameID_1->SetEntries(52);

   Int_t ci;   // for color index setting
   ci = TColor::GetColor("#59ace0");
   MVCommon_Hit_NameID_1->SetFillColor(ci);
   MVCommon_Hit_NameID_1->SetMarkerStyle(7);
   MVCommon_Hit_NameID_1->GetXaxis()->SetTitle("Station Name ID");
   MVCommon_Hit_NameID_1->GetXaxis()->SetRange(1,47);
   MVCommon_Hit_NameID_1->GetXaxis()->SetLabelSize(0.05);
   MVCommon_Hit_NameID_1->GetXaxis()->SetTitleSize(0.06);
   MVCommon_Hit_NameID_1->GetXaxis()->SetTitleOffset(0.91);
   MVCommon_Hit_NameID_1->GetYaxis()->SetTitle("Mean Number Of Digits");
   MVCommon_Hit_NameID_1->GetYaxis()->SetLabelSize(0.05);
   MVCommon_Hit_NameID_1->GetYaxis()->SetTitleSize(0.06);
   MVCommon_Hit_NameID_1->GetYaxis()->SetTitleOffset(0.7);
   MVCommon_Hit_NameID_1->GetZaxis()->SetLabelSize(0.03);
   MVCommon_Hit_NameID_1->GetZaxis()->SetTitleSize(0.03);
   MVCommon_Hit_NameID_1->Draw("hist");
   
   TH1 *MVCommon_Digit_NameID_1 = new TH1F("MVCommon_Digit_NameID_1","Digit_NameID",120,0,60);
   MVCommon_Digit_NameID_1->SetBinContent(1,8.65294);
   MVCommon_Digit_NameID_1->SetBinContent(3,9.23567);
   MVCommon_Digit_NameID_1->SetBinContent(5,6.24509);
   MVCommon_Digit_NameID_1->SetBinContent(7,6.57886);
   MVCommon_Digit_NameID_1->SetBinContent(9,6.29988);
   MVCommon_Digit_NameID_1->SetBinContent(11,6.19765);
   MVCommon_Digit_NameID_1->SetBinContent(13,8.65805);
   MVCommon_Digit_NameID_1->SetBinContent(15,7.5248);
   MVCommon_Digit_NameID_1->SetBinContent(17,7.45635);
   MVCommon_Digit_NameID_1->SetBinContent(19,8.48155);
   MVCommon_Digit_NameID_1->SetBinContent(21,8.89889);
   MVCommon_Digit_NameID_1->SetBinContent(27,7.96783);
   MVCommon_Digit_NameID_1->SetBinContent(35,6.5262);
   MVCommon_Digit_NameID_1->SetBinContent(37,7.75523);
   MVCommon_Digit_NameID_1->SetBinContent(41,6.02282);
   MVCommon_Digit_NameID_1->SetBinContent(43,6.22193);
   MVCommon_Digit_NameID_1->SetBinContent(99,8.75411);
   MVCommon_Digit_NameID_1->SetBinError(1,8.65294);
   MVCommon_Digit_NameID_1->SetBinError(3,9.23567);
   MVCommon_Digit_NameID_1->SetBinError(5,6.24509);
   MVCommon_Digit_NameID_1->SetBinError(7,6.57886);
   MVCommon_Digit_NameID_1->SetBinError(9,6.29988);
   MVCommon_Digit_NameID_1->SetBinError(11,6.19765);
   MVCommon_Digit_NameID_1->SetBinError(13,8.65805);
   MVCommon_Digit_NameID_1->SetBinError(15,7.5248);
   MVCommon_Digit_NameID_1->SetBinError(17,7.45635);
   MVCommon_Digit_NameID_1->SetBinError(19,8.48155);
   MVCommon_Digit_NameID_1->SetBinError(21,8.89889);
   MVCommon_Digit_NameID_1->SetBinError(27,7.96783);
   MVCommon_Digit_NameID_1->SetBinError(35,6.5262);
   MVCommon_Digit_NameID_1->SetBinError(37,7.75523);
   MVCommon_Digit_NameID_1->SetBinError(41,6.02282);
   MVCommon_Digit_NameID_1->SetBinError(43,6.22193);
   MVCommon_Digit_NameID_1->SetBinError(99,8.75411);
   MVCommon_Digit_NameID_1->SetEntries(52);
   MVCommon_Digit_NameID_1->SetDirectory(0);
   MVCommon_Digit_NameID_1->SetFillStyle(0);
   MVCommon_Digit_NameID_1->SetLineColor(2);
   MVCommon_Digit_NameID_1->SetLineWidth(2);
   MVCommon_Digit_NameID_1->SetMarkerStyle(7);
   MVCommon_Digit_NameID_1->GetXaxis()->SetRange(1,52);
   MVCommon_Digit_NameID_1->GetXaxis()->SetLabelSize(0.03);
   MVCommon_Digit_NameID_1->GetXaxis()->SetTitleSize(0.03);
   MVCommon_Digit_NameID_1->GetYaxis()->SetLabelSize(0.03);
   MVCommon_Digit_NameID_1->GetYaxis()->SetTitleSize(0.03);
   MVCommon_Digit_NameID_1->GetZaxis()->SetLabelSize(0.03);
   MVCommon_Digit_NameID_1->GetZaxis()->SetTitleSize(0.03);
   MVCommon_Digit_NameID_1->Draw("histhistsame");
   C_FMVCommon_Hit_NameID_1->Modified();
   C_FMVCommon_Hit_NameID_1->cd();
   C_FMVCommon_Hit_NameID_1->SetSelected(C_FMVCommon_Hit_NameID_1);
   C_FMVCommon_Hit_NameID_1->SaveAs("MV_Hit_NameID_R_Rpp.RTT.ps")
}
