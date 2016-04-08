/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void trkdifftruth() {

  if (doTruth) {

    TH1F *hDiffTruthTrk_d0;
    TH1F *hDiffTruthTrk_z0;
    TH1F *hDiffTruthTrk_phi0;
    TH1F *hDiffTruthTrk_theta;
    TH1F *hDiffTruthTrk_eta;
    TH1F *hDiffTruthTrk_mom;

    cout << " AlignTrk Diff Parameter wrt Truth..." << endl;

    cTruthDiffParams = new TCanvas("TrkDiffParams","Track Diff Parameter wrt Truth",
				   CanvasSizeX6[0],CanvasSizeX6[1]);
    cTruthDiffParams->Divide(3,2);
    
    cTruthDiffParams->cd(1);
    hDiffTruthTrk_d0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoD0_TruzD0_0");
    hDiffTruthTrk_d0->SetTitle("AlignTrk: Reco-Truth (#Deltad0)");
    hDiffTruthTrk_d0->GetXaxis()->SetTitle("d_{0} rec - d_{0} truth (mm)");
    hDiffTruthTrk_d0->GetYaxis()->SetTitle("Tracks");
    hDiffTruthTrk_d0->SetLineColor(ColorForAll+2);
    hDiffTruthTrk_d0->SetFillColor(ColorForAll);
    hDiffTruthTrk_d0->SetFillStyle(3001);
    if(normalizePlots) hDiffTruthTrk_d0->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    };
    hDiffTruthTrk_d0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    drawVerticalLine(hDiffTruthTrk_d0);
    delete hDiffTruthTrk_d0;

    //
    // -----------------------------------------------------------------
    //

    cTruthDiffParams->cd(2);
    hDiffTruthTrk_z0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoZ0_TruzZ0_0");
    hDiffTruthTrk_z0->SetTitle("AlignTrk: Reco-Truth (#Deltaz_{0})");
    hDiffTruthTrk_z0->GetXaxis()->SetTitle("z_{0} rec - z_{0} truth (mm)");
    hDiffTruthTrk_z0->GetYaxis()->SetTitle("Tracks");
    hDiffTruthTrk_z0->SetLineColor(ColorForAll+2);
    hDiffTruthTrk_z0->SetFillColor(ColorForAll);
    hDiffTruthTrk_z0->SetFillStyle(3001);
    if(normalizePlots) hDiffTruthTrk_z0->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    };
    hDiffTruthTrk_z0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    drawVerticalLine(hDiffTruthTrk_z0);
    delete hDiffTruthTrk_z0;

    //
    // -----------------------------------------------------------------
    //

    cTruthDiffParams->cd(3);
    hDiffTruthTrk_phi0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPhi0_TruzPhi0_0");
    hDiffTruthTrk_phi0->SetTitle("AlignTrk: Reco-Truth (#Delta#phi_{0})");
    hDiffTruthTrk_phi0->GetXaxis()->SetTitle("#phi_{0} rec - #phi_{0} truth (rad)");
    hDiffTruthTrk_phi0->GetYaxis()->SetTitle("Tracks");
    hDiffTruthTrk_phi0->SetLineColor(ColorForAll+2);
    hDiffTruthTrk_phi0->SetFillColor(ColorForAll);
    hDiffTruthTrk_phi0->SetFillStyle(3001);
    if(normalizePlots) hDiffTruthTrk_phi0->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    };
    hDiffTruthTrk_phi0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    drawVerticalLine(hDiffTruthTrk_phi0);
    delete hDiffTruthTrk_phi0;

    //
    // -----------------------------------------------------------------
    //

    cTruthDiffParams->cd(4);
    hDiffTruthTrk_theta = (TH1F*)file->Get("trkana/Track_Parameters/hRecoTheta_TruzTheta_0");
    hDiffTruthTrk_theta->SetTitle("AlignTrk: Reco-Truth (#Delta#theta)");
    hDiffTruthTrk_theta->GetXaxis()->SetTitle("#theta rec - #theta truth (rad)");
    hDiffTruthTrk_theta->GetYaxis()->SetTitle("Tracks");
    hDiffTruthTrk_theta->SetLineColor(ColorForAll+2);
    hDiffTruthTrk_theta->SetFillColor(ColorForAll);
    hDiffTruthTrk_theta->SetFillStyle(3001);
    if(normalizePlots) hDiffTruthTrk_theta->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    };
    hDiffTruthTrk_theta->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    drawVerticalLine(hDiffTruthTrk_theta);
    delete hDiffTruthTrk_theta;

    //
    // -----------------------------------------------------------------
    //

    cTruthDiffParams->cd(5);
    hDiffTruthTrk_eta = (TH1F*)file->Get("trkana/Track_Parameters/hRecoEta_TruzEta_0");
    hDiffTruthTrk_eta->SetTitle("AlignTrk: Reco-Truth (#Delta#eta)");
    hDiffTruthTrk_eta->GetXaxis()->SetTitle("#eta rec - #eta truth");
    hDiffTruthTrk_eta->GetYaxis()->SetTitle("Tracks");
    hDiffTruthTrk_eta->SetLineColor(ColorForAll+2);
    hDiffTruthTrk_eta->SetFillColor(ColorForAll);
    hDiffTruthTrk_eta->SetFillStyle(3001);
    hDiffTruthTrk_eta->GetXaxis()->SetNdivisions(8);
    if(normalizePlots) hDiffTruthTrk_eta->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    };
    hDiffTruthTrk_eta->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    drawVerticalLine(hDiffTruthTrk_eta);
    delete hDiffTruthTrk_eta;

    //
    // -----------------------------------------------------------------
    //

    cTruthDiffParams->cd(6);
    hDiffTruthTrk_mom = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPt_TruzPt_0");
    hDiffTruthTrk_mom->SetTitle("AlignTrk: Reco-Truth (#DeltapT)");
    hDiffTruthTrk_mom->GetXaxis()->SetTitle("pT rec - pT truth (Gev/c)");
    hDiffTruthTrk_mom->GetYaxis()->SetTitle("Tracks");
    hDiffTruthTrk_mom->SetLineColor(ColorForAll+2);
    hDiffTruthTrk_mom->SetFillColor(ColorForAll);
    hDiffTruthTrk_mom->SetFillStyle(3001);
    if(normalizePlots) hDiffTruthTrk_mom->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    };
    hDiffTruthTrk_mom->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    drawVerticalLine(hDiffTruthTrk_mom);
    delete hDiffTruthTrk_mom;

    
  }

}
