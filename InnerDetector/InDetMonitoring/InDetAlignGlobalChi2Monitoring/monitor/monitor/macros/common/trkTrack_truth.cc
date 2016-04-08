/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void trkTrack_truth() {

  if (doTrkTrack && doTruth) {

    Bool_t doLabels = true;

    TH1F *hTrkTrackTruth_d0;
    TH1F *hTrkTrackTruth_z0;
    TH1F *hTrkTrackTruth_phi0;
    TH1F *hTrkTrackTruth_theta0;
    TH1F *hTrkTrackTruth_eta;
    TH1F *hTrkTrackTruth_qoverpT;

    cout << " Trk::Track Truth Parameters..." << endl;

    cTruthTrackParams = new TCanvas("TrkTrackTruthParams","Trk::Track Truth Parameters",
				    CanvasSizeX6[0],CanvasSizeX6[1]);
    cTruthTrackParams->Divide(3,2);

    cTruthTrackParams->cd(1);
    if(iCosmics) hTrkTrackTruth_d0 = (TH1F*)file->Get("trkana/TrkTrack/hTTruzD0Zoom_0");
    else {
      if (iMisalignment==1) hTrkTrackTruth_d0 = (TH1F*)file->Get("trkana/TrkTrack/hTTruzD0Zoom_0");
      else hTrkTrackTruth_d0 =(TH1F*)file->Get("trkana/TrkTrack/hTTruzD0Zoom_0");
    }
    hTrkTrackTruth_d0->SetTitle("Trk::Track Truth Parameter: d0");
    hTrkTrackTruth_d0->GetXaxis()->SetTitle("d_{0} (mm)");
    hTrkTrackTruth_d0->GetYaxis()->SetTitle("Tracks");
    hTrkTrackTruth_d0->SetLineColor(ColorForAll+2);
    hTrkTrackTruth_d0->SetFillColor(ColorForAll);
    hTrkTrackTruth_d0->SetFillStyle(3001);
    if (iCosmics) hTrkTrackTruth_d0->GetXaxis()->SetNdivisions(6);
    if (!doLabels) hTrkTrackTruth_d0->SetStats(kFALSE);
    else hTrkTrackTruth_d0->GetYaxis()->SetRangeUser(0,1.15*hTrkTrackTruth_d0->GetMaximum());
    if (normalizePlots) hTrkTrackTruth_d0->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrackTruth_d0->DrawCopy();
    setTitleFont(font);
    setTitleSize(0.67);
    ScalePlots();
    delete hTrkTrackTruth_d0;

    //
    // -----------------------------------------------------------------
    //

    cTruthTrackParams->cd(2);
    hTrkTrackTruth_z0 = (TH1F*)file->Get("trkana/TrkTrack/hTTruzZ0_0");
    hTrkTrackTruth_z0->SetTitle("Trk::Track Truth Parameter: z0");
    hTrkTrackTruth_z0->GetXaxis()->SetTitle("z_{0} (mm)");
    hTrkTrackTruth_z0->GetYaxis()->SetTitle("Tracks");
    hTrkTrackTruth_z0->SetLineColor(ColorForAll+2);
    hTrkTrackTruth_z0->SetFillColor(ColorForAll);
    hTrkTrackTruth_z0->SetFillStyle(3001);
    if (iCosmics) hTrkTrackTruth_z0->GetXaxis()->SetNdivisions(6);
    if (!doLabels) hTrkTrackTruth_z0->SetStats(kFALSE);
    else hTrkTrackTruth_z0->GetYaxis()->SetRangeUser(0,1.15*hTrkTrackTruth_z0->GetMaximum());
    if (normalizePlots) hTrkTrackTruth_z0->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrackTruth_z0->DrawCopy();
    setTitleFont(font);
    setTitleSize(0.67);
    ScalePlots();
    delete hTrkTrackTruth_z0;
  
    //
    // -----------------------------------------------------------------
    //

    cTruthTrackParams->cd(3);
    hTrkTrackTruth_phi0 = (TH1F*)file->Get("trkana/TrkTrack/hTTruzPhi0_0");
    hTrkTrackTruth_phi0->SetTitle("Trk::Track Truth Parameter: phi0");
    hTrkTrackTruth_phi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
    hTrkTrackTruth_phi0->GetYaxis()->SetTitle("Tracks");
    hTrkTrackTruth_phi0->SetLineColor(ColorForAll+2);
    hTrkTrackTruth_phi0->SetFillColor(ColorForAll);
    hTrkTrackTruth_phi0->SetFillStyle(3001);
    if (!doLabels) hTrkTrackTruth_phi0->SetStats(kFALSE);
    else hTrkTrackTruth_phi0->GetYaxis()->SetRangeUser(0,1.15*hTrkTrackTruth_phi0->GetMaximum());
    if (normalizePlots) hTrkTrackTruth_phi0->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrackTruth_phi0->DrawCopy();
    setTitleFont(font);
    setTitleSize(0.67);
    ScalePlots();
    delete hTrkTrackTruth_phi0;

    //
    // -----------------------------------------------------------------
    //

    cTruthTrackParams->cd(4);
    hTrkTrackTruth_theta0 = (TH1F*)file->Get("trkana/TrkTrack/hTTruzTheta_0");
    hTrkTrackTruth_theta0->SetTitle("Trk::Track Truth Parameter: theta0");
    hTrkTrackTruth_theta0->GetXaxis()->SetTitle("#theta_{0} (rad)");
    hTrkTrackTruth_theta0->GetYaxis()->SetTitle("Tracks");
    hTrkTrackTruth_theta0->SetLineColor(ColorForAll+2);
    hTrkTrackTruth_theta0->SetFillColor(ColorForAll);
    hTrkTrackTruth_theta0->SetFillStyle(3001);
    if (!doLabels) hTrkTrackTruth_theta0->SetStats(kFALSE);
    else hTrkTrackTruth_theta0->GetYaxis()->SetRangeUser(0,1.15*hTrkTrackTruth_theta0->GetMaximum());
    if (normalizePlots) hTrkTrackTruth_theta0->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrackTruth_theta0->DrawCopy();
    setTitleFont(font);
    setTitleSize(0.67);
    ScalePlots();
    delete hTrkTrackTruth_theta0;

    //
    // -----------------------------------------------------------------
    //

    cTruthTrackParams->cd(5);
    hTrkTrackTruth_eta = (TH1F*)file->Get("trkana/TrkTrack/hTTruzEta_0");
    hTrkTrackTruth_eta->SetTitle("Trk::Track Truth Parameter: eta");
    hTrkTrackTruth_eta->GetXaxis()->SetTitle("#eta = -ln (tan(#theta/2))");
    hTrkTrackTruth_eta->GetYaxis()->SetTitle("Tracks");
    hTrkTrackTruth_eta->SetLineColor(ColorForAll+2);
    hTrkTrackTruth_eta->SetFillColor(ColorForAll);
    hTrkTrackTruth_eta->SetFillStyle(3001);
    if (!doLabels) hTrkTrackTruth_eta->SetStats(kFALSE);
    else hTrkTrackTruth_eta->GetYaxis()->SetRangeUser(0,1.15*hTrkTrackTruth_eta->GetMaximum());
    if (normalizePlots) hTrkTrackTruth_eta->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrackTruth_eta->DrawCopy();
    setTitleFont(font);
    setTitleSize(0.67);
    ScalePlots();
    delete hTrkTrackTruth_eta;

    //
    // -----------------------------------------------------------------
    //

    cTruthTrackParams->cd(6);
    hTrkTrackTruth_pt = (TH1F*)file->Get("trkana/TrkTrack/hTTruzPt_0");
    // if (iBField>0) gPad->SetLogy();
    hTrkTrackTruth_pt->SetTitle("Trk::Track Parameter: pT");
    hTrkTrackTruth_pt->GetXaxis()->SetTitle("pT (GeV/c)");
    hTrkTrackTruth_pt->GetYaxis()->SetTitle("Tracks");
    hTrkTrackTruth_pt->SetLineColor(ColorForAll+2);
    hTrkTrackTruth_pt->SetFillColor(ColorForAll);
    hTrkTrackTruth_pt->SetFillStyle(3001);
    hTrkTrackTruth_pt->GetXaxis()->SetNdivisions(8);
    if (!doLabels) hTrkTrackTruth_pt->SetStats(kFALSE);
    else hTrkTrackTruth_pt->GetYaxis()->SetRangeUser(0,1.15*hTrkTrackTruth_pt->GetMaximum());
    if (normalizePlots) hTrkTrackTruth_pt->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrackTruth_pt->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTrkTrackTruth_pt;

    cTruthTrackParams->Update();

  }

}
