/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void trkTrack_recotruthdiff() {

  if (doTrkTrack && doTruth) {

    TH1F *hDiffTrkTruthTrk_d0;
    TH1F *hDiffTrkTruthTrk_z0;
    TH1F *hDiffTrkTruthTrk_phi0;
    TH1F *hDiffTrkTruthTrk_theta;
    TH1F *hDiffTrkTruthTrk_eta;
    TH1F *hDiffTrkTruthTrk_mom;

    cout << " Trk::Track Diff Parameter wrt Truth..." << endl;

    if(doTruth) {
    
      cTrkTruthDiffParams = new TCanvas("TrkTrack_DiffParams","Trk::Track Diff Parameter wrt Truth",
					CanvasSizeX6[0],CanvasSizeX6[1]);
      cTrkTruthDiffParams->Divide(3,2);
    
      cTrkTruthDiffParams->cd(1);
      hDiffTrkTruthTrk_d0 = (TH1F*)file->Get("trkana/TrkTrack/hTRecoD0_TruzD0_0");
      hDiffTrkTruthTrk_d0->SetTitle("Trk::Track: Reco-Truth (#Deltad0)");
      hDiffTrkTruthTrk_d0->GetXaxis()->SetTitle("d_{0} rec - d_{0} truth (mm)");
      hDiffTrkTruthTrk_d0->GetYaxis()->SetTitle("Tracks");
      hDiffTrkTruthTrk_d0->SetLineColor(ColorForAll+2);
      hDiffTrkTruthTrk_d0->SetFillColor(ColorForAll);
      hDiffTrkTruthTrk_d0->SetFillStyle(3001);
      if(normalizePlots) hDiffTrkTruthTrk_d0->Scale(1./(float)iEvents);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      };
      hDiffTrkTruthTrk_d0->DrawCopy();
      setTitleFont(font);
      ScalePlots();
      drawVerticalLine(hDiffTrkTruthTrk_d0);
      delete hDiffTrkTruthTrk_d0;

      //
      // -----------------------------------------------------------------
      //

      cTrkTruthDiffParams->cd(2);
      hDiffTrkTruthTrk_z0 = (TH1F*)file->Get("trkana/TrkTrack/hTRecoZ0_TruzZ0_0");
      hDiffTrkTruthTrk_z0->SetTitle("Trk::Track: Reco-Truth (#Deltaz_{0})");
      hDiffTrkTruthTrk_z0->GetXaxis()->SetTitle("z_{0} rec - z_{0} truth (mm)");
      hDiffTrkTruthTrk_z0->GetYaxis()->SetTitle("Tracks");
      hDiffTrkTruthTrk_z0->SetLineColor(ColorForAll+2);
      hDiffTrkTruthTrk_z0->SetFillColor(ColorForAll);
      hDiffTrkTruthTrk_z0->SetFillStyle(3001);
      if(normalizePlots) hDiffTrkTruthTrk_z0->Scale(1./(float)iEvents);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      };
      hDiffTrkTruthTrk_z0->DrawCopy();
      setTitleFont(font);
      ScalePlots();
      drawVerticalLine(hDiffTrkTruthTrk_z0);
      delete hDiffTrkTruthTrk_z0;

      //
      // -----------------------------------------------------------------
      //

      cTrkTruthDiffParams->cd(3);
      hDiffTrkTruthTrk_phi0 = (TH1F*)file->Get("trkana/TrkTrack/hTRecoPhi0_TruzPhi0_0");
      hDiffTrkTruthTrk_phi0->SetTitle("Trk::Track: Reco-Truth (#Delta#phi_{0})");
      hDiffTrkTruthTrk_phi0->GetXaxis()->SetTitle("#phi_{0} rec - #phi_{0} truth (rad)");
      hDiffTrkTruthTrk_phi0->GetYaxis()->SetTitle("Tracks");
      hDiffTrkTruthTrk_phi0->SetLineColor(ColorForAll+2);
      hDiffTrkTruthTrk_phi0->SetFillColor(ColorForAll);
      hDiffTrkTruthTrk_phi0->SetFillStyle(3001);
      if(normalizePlots) hDiffTrkTruthTrk_phi0->Scale(1./(float)iEvents);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      };
      hDiffTrkTruthTrk_phi0->DrawCopy();
      setTitleFont(font);
      ScalePlots();
      drawVerticalLine(hDiffTrkTruthTrk_phi0);
      delete hDiffTrkTruthTrk_phi0;

      //
      // -----------------------------------------------------------------
      //

      cTrkTruthDiffParams->cd(4);
      hDiffTrkTruthTrk_theta = (TH1F*)file->Get("trkana/TrkTrack/hTRecoTheta_TruzTheta_0");
      hDiffTrkTruthTrk_theta->SetTitle("Trk::Track: Reco-Truth (#Delta#theta)");
      hDiffTrkTruthTrk_theta->GetXaxis()->SetTitle("#theta rec - #theta truth (rad)");
      hDiffTrkTruthTrk_theta->GetYaxis()->SetTitle("Tracks");
      hDiffTrkTruthTrk_theta->SetLineColor(ColorForAll+2);
      hDiffTrkTruthTrk_theta->SetFillColor(ColorForAll);
      hDiffTrkTruthTrk_theta->SetFillStyle(3001);
      if(normalizePlots) hDiffTrkTruthTrk_theta->Scale(1./(float)iEvents);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      };
      hDiffTrkTruthTrk_theta->DrawCopy();
      setTitleFont(font);
      ScalePlots();
      drawVerticalLine(hDiffTrkTruthTrk_theta);
      delete hDiffTrkTruthTrk_theta;

      //
      // -----------------------------------------------------------------
      //

      cTrkTruthDiffParams->cd(5);
      hDiffTrkTruthTrk_eta = (TH1F*)file->Get("trkana/TrkTrack/hTRecoEta_TruzEta_0");
      hDiffTrkTruthTrk_eta->SetTitle("Trk::Track: Reco-Truth (#Delta#eta)");
      hDiffTrkTruthTrk_eta->GetXaxis()->SetTitle("#eta rec - #eta truth");
      hDiffTrkTruthTrk_eta->GetYaxis()->SetTitle("Tracks");
      hDiffTrkTruthTrk_eta->SetLineColor(ColorForAll+2);
      hDiffTrkTruthTrk_eta->SetFillColor(ColorForAll);
      hDiffTrkTruthTrk_eta->SetFillStyle(3001);
      hDiffTrkTruthTrk_eta->GetXaxis()->SetNdivisions(8);
      if(normalizePlots) hDiffTrkTruthTrk_eta->Scale(1./(float)iEvents);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      };
      hDiffTrkTruthTrk_eta->DrawCopy();
      setTitleFont(font);
      ScalePlots();
      drawVerticalLine(hDiffTrkTruthTrk_eta);
      delete hDiffTrkTruthTrk_eta;

      //
      // -----------------------------------------------------------------
      //

      cTrkTruthDiffParams->cd(6);
      hDiffTrkTruthTrk_mom = (TH1F*)file->Get("trkana/TrkTrack/hTRecoPt_TruzPt_0");
      hDiffTrkTruthTrk_mom->SetTitle("Trk::Track: Reco-Truth (#DeltapT)");
      hDiffTrkTruthTrk_mom->GetXaxis()->SetTitle("pT rec - pT truth (Gev/c)");
      hDiffTrkTruthTrk_mom->GetYaxis()->SetTitle("Tracks");
      hDiffTrkTruthTrk_mom->SetLineColor(ColorForAll+2);
      hDiffTrkTruthTrk_mom->SetFillColor(ColorForAll);
      hDiffTrkTruthTrk_mom->SetFillStyle(3001);
      if(normalizePlots) hDiffTrkTruthTrk_mom->Scale(1./(float)iEvents);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      };
      hDiffTrkTruthTrk_mom->DrawCopy();
      setTitleFont(font);
      ScalePlots();
      drawVerticalLine(hDiffTrkTruthTrk_mom);
      delete hDiffTrkTruthTrk_mom;

    }
  }

}
