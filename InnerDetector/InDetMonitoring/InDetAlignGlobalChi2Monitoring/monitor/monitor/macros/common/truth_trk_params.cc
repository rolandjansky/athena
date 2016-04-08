/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void truth_trk_params() {

  if (doTruth && !iCosmics) {

    TH1F *hTruthTrk_d0;
    TH1F *hTruthTrk_z0;
    TH1F *hTruthTrk_phi0;
    TH1F *hTruthTrk_theta0;
    TH1F *hTruthTrk_eta;
    TH1F *hTruthTrk_qoverpT;

    cout << " AlignTrk truth Parameters..." << endl;

    cTruthParams = new TCanvas("TruthTrkParams","AlignTrk truth Parameters",
			       CanvasSizeX6[0],CanvasSizeX6[1]);
    cTruthParams->Divide(3,2);
    
    cTruthParams->cd(1);
    hTruthTrk_d0 = (TH1F*)file->Get("trkana/Track_Parameters/hTruzD0Zoom_0");
    hTruthTrk_d0->SetTitle("AlignTrk truth Parameter: d_{0}");
    hTruthTrk_d0->GetXaxis()->SetTitle("Truth d_{0} (mm)");
    hTruthTrk_d0->GetYaxis()->SetTitle("Tracks");
    hTruthTrk_d0->SetLineColor(ColorForAll+2);
    hTruthTrk_d0->SetFillColor(ColorForAll);
    hTruthTrk_d0->SetFillStyle(3001);
    if (iCosmics) hTruthTrk_d0->GetXaxis()->SetNdivisions(6);
    if (normalizePlots) hTruthTrk_d0->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    forceFont(hTruthTrk_d0);
    hTruthTrk_d0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTruthTrk_d0;

    //
    // -----------------------------------------------------------------
    //

    cTruthParams->cd(2);
    hTruthTrk_z0 = (TH1F*)file->Get("trkana/Track_Parameters/hTruzZ0_0");
    hTruthTrk_z0->SetTitle("AlignTrk truth Parameter: z_{0}");
    hTruthTrk_z0->GetXaxis()->SetTitle("Truth z_{0} (mm)");
    hTruthTrk_z0->GetYaxis()->SetTitle("Tracks");
    hTruthTrk_z0->SetLineColor(ColorForAll+2);
    hTruthTrk_z0->SetFillColor(ColorForAll);
    hTruthTrk_z0->SetFillStyle(3001);
    if (iCosmics) hTruthTrk_z0->GetXaxis()->SetNdivisions(6);
    if (normalizePlots) hTruthTrk_z0->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hTruthTrk_z0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTruthTrk_z0;
    
    //
    // -----------------------------------------------------------------
    //

    cTruthParams->cd(3);
    hTruthTrk_phi0 = (TH1F*)file->Get("trkana/Track_Parameters/hTruzPhi0_0");
    hTruthTrk_phi0->Draw();
    hTruthTrk_phi0->SetTitle("AlignTrk truth Parameter: #phi_{0}");
    hTruthTrk_phi0->GetXaxis()->SetTitle("Truth #phi_{0} (rad)");
    hTruthTrk_phi0->GetYaxis()->SetTitle("Tracks");
    hTruthTrk_phi0->SetLineColor(ColorForAll+2);
    hTruthTrk_phi0->SetFillColor(ColorForAll);
    hTruthTrk_phi0->SetFillStyle(3001);
    if (normalizePlots) hTruthTrk_phi0->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hTruthTrk_phi0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTruthTrk_phi0;
    
    //
    // -----------------------------------------------------------------
    //

    cTruthParams->cd(4);
    hTruthTrk_theta0 = (TH1F*)file->Get("trkana/Track_Parameters/hTruzTheta_0");
    hTruthTrk_theta0->SetTitle("AlignTrk truth Parameter: #theta_{0}");
    hTruthTrk_theta0->GetXaxis()->SetTitle("Truth #theta_{0} (rad)");
    hTruthTrk_theta0->GetYaxis()->SetTitle("Tracks");
    hTruthTrk_theta0->SetLineColor(ColorForAll+2);
    hTruthTrk_theta0->SetFillColor(ColorForAll);
    hTruthTrk_theta0->SetFillStyle(3001);
    if (normalizePlots) hTruthTrk_theta0->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hTruthTrk_theta0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTruthTrk_theta0;
    
    //
    // -----------------------------------------------------------------
    //

    cTruthParams->cd(5);
      
    hTruthTrk_eta = (TH1F*)file->Get("trkana/Track_Parameters/hTruzEta_0");
    hTruthTrk_eta->SetTitle("AlignTrk truth Parameter: #eta");
    hTruthTrk_eta->GetXaxis()->SetTitle("Truth #eta = -ln (tan(#theta/2))");
    hTruthTrk_eta->GetYaxis()->SetTitle("Tracks");
    hTruthTrk_eta->SetLineColor(ColorForAll+2);
    hTruthTrk_eta->SetFillColor(ColorForAll);
    hTruthTrk_eta->SetFillStyle(3001);
    if (normalizePlots) hTruthTrk_eta->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hTruthTrk_eta->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTruthTrk_eta;
    
    //
    // -----------------------------------------------------------------
    //

    cTruthParams->cd(6);
    hTruthTrk_qoverpT = (TH1F*)file->Get("trkana/Track_Parameters/hTruzQoverPt_0");
    gPad->SetLogy();
    hTruthTrk_qoverpT->SetTitle("AlignTrk truth Parameter: q/pT");
    hTruthTrk_qoverpT->GetXaxis()->SetTitle("Truth q/pT (GeV^{-1})");
    hTruthTrk_qoverpT->GetYaxis()->SetTitle("Tracks");
    hTruthTrk_qoverpT->SetLineColor(ColorForAll+2);
    hTruthTrk_qoverpT->SetFillColor(ColorForAll);
    hTruthTrk_qoverpT->SetFillStyle(3001);
    if (normalizePlots) hTruthTrk_qoverpT->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hTruthTrk_qoverpT->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTruthTrk_qoverpT;
    
    cTruthParams->Update();

    doTruth = true;
  }
}
