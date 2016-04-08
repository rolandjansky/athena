/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void SCT_trkProp() {

  if (!PixelOnly) {

    cout << " SCT - Track properties..." << endl;

    TH2F *hSCTResVsPhi0;
    TH2F *hSCTResVsZ0;
    TH2F *hSCTResVsSector;
    TH2F *hSCTResVsRing;
    TProfile *hprofGlobSide0;
    TProfile *hprofGlobSide1;
    TProfile *hprofGlob;

    cTrkSCTProp = new TCanvas("cTrkSCTProp","SCT - Track properties",
			      CanvasSizeX6[0],CanvasSizeX6[1]);
    cTrkSCTProp->Divide(3,2);

    cTrkSCTProp->cd(1);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hSCTResVsPhi0 = (TH2F*)file->Get("trkana/Track_Parameters/hSCTRes_RecoPhi0_0");
    hSCTResVsPhi0->SetLineColor(ColorForAll+2);
    hSCTResVsPhi0->ProfileX()->Draw();
    hSCTResVsPhi0->ProfileX()->SetTitle("SCT Residuals Vs Trk #phi0");
    hSCTResVsPhi0->ProfileX()->GetYaxis()->SetTitle("average SCT residual mean (#mum)");
    hSCTResVsPhi0->ProfileX()->GetXaxis()->SetTitle("Trk #phi0 (rad)");
    hSCTResVsPhi0->ProfileX()->GetYaxis()->SetRangeUser(-40.0,40.0);

    setTitleFont(font);
    setTitleSize(0.67);

    cTrkSCTProp->cd(2);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hSCTResVsZ0 = (TH2F*)file->Get("trkana/Track_Parameters/hSCTRes_RecoZ0_0");
    hSCTResVsZ0->SetLineColor(ColorForAll+2);
    hSCTResVsZ0->ProfileX()->Draw();
    hSCTResVsZ0->ProfileX()->SetTitle("SCT Residuals Vs Trk z0");
    hSCTResVsZ0->ProfileX()->GetYaxis()->SetTitle("average SCT residual mean (#mum)");
    hSCTResVsZ0->ProfileX()->GetXaxis()->SetTitle("Trk z0 (mm)");
    hSCTResVsZ0->ProfileX()->GetYaxis()->SetRangeUser(-60.0,60.0);
 
    setTitleFont(font);
    setTitleSize(0.67);

    cTrkSCTProp->cd(3);
   
    hTrk_delzr = (TH2F*)file->Get("trkana/SCTRes/hSCTDelzr_0");
    hTrk_delzr->SetTitle("SCT LocalY residual (delzr)");
    hTrk_delzr->GetXaxis()->SetTitle("SCT Z residual (mm)");
    hTrk_delzr->SetLineColor(ColorForAll+2);
    if (normalizePlots) hTrk_delzr->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrk_delzr->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete hTrk_delzr;
    
    
    cTrkSCTProp->cd(4);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hSCTResVsSector = (TH2F*)file->Get("trkana/Track_Parameters/hSCTRes_Sector_0");
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hSCTResVsSector->SetLineColor(ColorForAll+2);
    hSCTResVsSector->ProfileX()->Draw();
    hSCTResVsSector->ProfileX()->SetTitle("SCT Residuals Vs Sector");
    hSCTResVsSector->ProfileX()->GetYaxis()->SetRangeUser(-40.0,40.0);
    hSCTResVsSector->ProfileX()->GetYaxis()->SetTitle("average SCT residual mean (#mum)");
    hSCTResVsSector->ProfileX()->GetXaxis()->SetTitle("Sector (#phi)");
    setTitleFont(font);
    setTitleSize(0.67);
    
    cTrkSCTProp->cd(5);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hSCTResVsRing = (TH2F*)file->Get("trkana/Track_Parameters/hSCTRes_Ring_0");
    hSCTResVsRing->SetLineColor(ColorForAll+2);
    hSCTResVsRing->ProfileX()->Draw();
    hSCTResVsRing->ProfileX()->SetTitle("SCT Residuals Vs Ring");
    hSCTResVsRing->ProfileX()->GetYaxis()->SetTitle("average SCT residual mean (#mum)");
    hSCTResVsRing->ProfileX()->GetXaxis()->SetTitle("Ring (#eta)");
    hSCTResVsRing->ProfileX()->GetYaxis()->SetRangeUser(-60.0,60.0);
    setTitleFont(font);
    setTitleSize(0.67);
     
    cTrkSCTProp->cd(6);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    hprofGlob = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResBaLay_0");
    hprofGlob->Draw();
    hprofGlob->SetTitle("SCT Residuals Vs Layer");
    hprofGlob->GetYaxis()->SetTitle("average SCT residual mean (#mum)");
    hprofGlob->GetXaxis()->SetTitle("Trk layer");
    hprofGlob->GetYaxis()->SetRangeUser(-70.0,70.0);
    hprofGlob->SetStats(kFALSE);

    hprofSide0 = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResBaLaySide0_0");
    hprofSide0->SetLineColor(kBlue);
    hprofSide0->Draw("SAME");

    hprofSide1 = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResBaLaySide1_0");
    hprofSide1->SetLineColor(kRed);
    hprofSide1->Draw("SAME");
    setTitleFont(font);
    setTitleSize(0.60);

    TLegend *leg0 = new TLegend(0.65,0.8,0.95,0.95);
    sprintf(name,"All sides");
    leg0->AddEntry(hprofGlob,name,"F");
    sprintf(name,"Side 0");
    leg0->AddEntry(hprofSide0,name,"F");
    sprintf(name,"Side 1");
    leg0->AddEntry(hprofSide1,name,"F");
    leg0->SetBorderSize(1);
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw("P");

    cTrkSCTProp->Update();
  }
  else cout << " --> No Pixel hitmap because this is a PixelOnly sample" << endl;
}
