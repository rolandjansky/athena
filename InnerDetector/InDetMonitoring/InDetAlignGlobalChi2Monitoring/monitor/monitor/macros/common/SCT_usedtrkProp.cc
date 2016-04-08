/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void SCT_usedtrkProp(int level=1) {

  if (!PixelOnly) {

    cout << " SCT - Used Track properties..." << endl;

    TH2F *hUSedSCTResVsPhi0;
    TH2F *hUsedSCTResVsZ0;
    TH2F *hUsedSCTResVsSector;
    TH2F *hUsedSCTResVsRing;
    TProfile *hprofUsedGlobSide0;
    TProfile *hprofUsedGlobSide1;
    TProfile *hprofUsedGlob;
    
    cUsedTrkSCTProp = new TCanvas("cUsedTrkSCTProp","SCT  - Used Track properties",
				  CanvasSizeX6[0],CanvasSizeX6[1]);
    cUsedTrkSCTProp->Divide(3,2);

    cUsedTrkSCTProp->cd(1);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    sprintf(name,"trkana/Track_Parameters/hSCTRes_RecoPhi0_%d",level);
    hUsedSCTResVsPhi0 = (TH2F*)file->Get(name);
    hUsedSCTResVsPhi0->SetLineColor(ColorForUsed+2);
    hUsedSCTResVsPhi0->ProfileX()->Draw();
    hUsedSCTResVsPhi0->ProfileX()->SetTitle("SCT Processed Residuals Vs Trk #phi1");
    hUsedSCTResVsPhi0->ProfileX()->GetYaxis()->SetTitle("average SCT residual mean (#mum)");
    hUsedSCTResVsPhi0->ProfileX()->GetXaxis()->SetTitle("Trk #phi0 (rad)");
    hUsedSCTResVsPhi0->ProfileX()->GetYaxis()->SetRangeUser(-40.0,40.0);
    setTitleFont(font);
    setTitleSize(0.67);
    
    cUsedTrkSCTProp->cd(2);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    sprintf(name,"trkana/Track_Parameters/hSCTRes_RecoZ0_%d",level);
    hUsedSCTResVsZ0 = (TH2F*)file->Get(name);
    hUsedSCTResVsZ0->SetLineColor(ColorForUsed+2);
    hUsedSCTResVsZ0->ProfileX()->Draw();
    hUsedSCTResVsZ0->ProfileX()->SetTitle("Processed SCT Residuals Vs Trk z0");
    hUsedSCTResVsZ0->ProfileX()->GetYaxis()->SetTitle("average SCT residual mean (#mum)");
    hUsedSCTResVsZ0->ProfileX()->GetXaxis()->SetTitle("Trk z0 (mm)");
    hUsedSCTResVsZ0->ProfileX()->GetYaxis()->SetRangeUser(-60.0,60.0);
    setTitleFont(font);
    setTitleSize(0.67);
    
    cUsedTrkSCTProp->cd(3);
    
    hUsedTrk_delzr = (TH2F*)file->Get("trkana/SCTRes/hSCTDelzr_1");
    hUsedTrk_delzr->SetTitle("SCT Processed LocalY Res (delzr)");
    hUsedTrk_delzr->GetXaxis()->SetTitle("SCT Z residual (mm)");
    hUsedTrk_delzr->SetLineColor(ColorForUsed+2);
    if (normalizePlots) hUsedTrk_delzr->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hUsedTrk_delzr->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
 
    delete hUsedTrk_delzr;
    

    cUsedTrkSCTProp->cd(4);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    sprintf(name,"trkana/Track_Parameters/hSCTRes_Sector_%d",level);
    hUsedSCTResVsSector = (TH2F*)file->Get(name);
    hUsedSCTResVsSector->SetLineColor(ColorForUsed+2);
    hUsedSCTResVsSector->ProfileX()->Draw();
    hUsedSCTResVsSector->ProfileX()->SetTitle("SCT Processed Residuals Vs Sector");
    hUsedSCTResVsSector->ProfileX()->GetYaxis()->SetRangeUser(-40.0,40.0);
    hUsedSCTResVsSector->ProfileX()->GetYaxis()->SetTitle("average SCT residual mean (#mum)");
    hUsedSCTResVsSector->ProfileX()->GetXaxis()->SetTitle("Sector (#phi)");
    setTitleFont(font);
    setTitleSize(0.67);
    
    cUsedTrkSCTProp->cd(5);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    sprintf(name,"trkana/Track_Parameters/hSCTRes_Ring_%d",level);
    hUsedSCTResVsRing = (TH2F*)file->Get(name);
    hUsedSCTResVsRing->SetLineColor(ColorForUsed+2);
    hUsedSCTResVsRing->ProfileX()->Draw();
    hUsedSCTResVsRing->ProfileX()->SetTitle("SCT Processed Residuals Vs Ring");
    hUsedSCTResVsRing->ProfileX()->GetYaxis()->SetTitle("average SCT residual mean (#mum)");
    hUsedSCTResVsRing->ProfileX()->GetXaxis()->SetTitle("Ring (#eta)");
    hUsedSCTResVsRing->ProfileX()->GetYaxis()->SetRangeUser(-60.0,60.0);
    setTitleFont(font);
    setTitleSize(0.67);
    
    cUsedTrkSCTProp->cd(6);
    if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
    sprintf(name,"trkana/SCTRes/hproSCTMeanResBaLay_%d",level);
    hprofUsedGlob = (TProfile*)file->Get(name);
    hprofUsedGlob->Draw();
    hprofUsedGlob->SetTitle("SCT Processed Res Vs Layer");
    hprofUsedGlob->GetYaxis()->SetTitle("average SCT residual mean (#mum)");
    hprofUsedGlob->GetXaxis()->SetTitle("Trk layer");
    hprofUsedGlob->GetYaxis()->SetRangeUser(-70.0,70.0);
    hprofUsedGlob->SetStats(kFALSE);

    sprintf(name,"trkana/SCTRes/hproSCTMeanResBaLaySide0_%d",level);
    hprofUsedSide0 = (TProfile*)file->Get(name);
    hprofUsedSide0->SetLineColor(kBlue);
    hprofUsedSide0->Draw("SAME");

    sprintf(name,"trkana/SCTRes/hproSCTMeanResBaLaySide1_%d",level);
    hprofUsedSide1 = (TProfile*)file->Get(name);
    hprofUsedSide1->SetLineColor(kRed);
    hprofUsedSide1->Draw("SAME");
    setTitleFont(font);
    setTitleSize(0.60);

    TLegend *leg0 = new TLegend(0.65,0.8,0.95,0.95);
    sprintf(name,"All sides");
    leg0->AddEntry(hprofUsedGlob,name,"F");
    sprintf(name,"Side 0");
    leg0->AddEntry(hprofUsedSide0,name,"F");
    sprintf(name,"Side 1");
    leg0->AddEntry(hprofUsedSide1,name,"F");
    leg0->SetBorderSize(1);
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw("P");

    cUsedTrkSCTProp->Update();
  }
}
