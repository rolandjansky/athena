/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void trkTrack_params() {

  if (doTrkTrack) {

    TH1F *hTrkTrack_d0;
    TH1F *hTrkTrack_z0;
    TH1F *hTrkTrack_phi0;
    TH1F *hTrkTrack_theta0;
    TH1F *hTrkTrack_eta;
    TH1F *hTrkTrack_qoverpT;
    TH1F *hTrkTrack_pT;

    cout << " Trk::Track Parameters..." << endl;

    cTrackParams = new TCanvas("TrkTrackParams","Trk::Track Parameters",
			       CanvasSizeX6[0],CanvasSizeX6[1]);
    cTrackParams->Divide(3,2);

    cTrackParams->cd(1);
    if(iCosmics) hTrkTrack_d0 = (TH1F*)file->Get("trkana/TrkTrack/hTRecoD0_0");
    else {
	  if (iMisalignment==1) hTrkTrack_d0 = (TH1F*)file->Get("trkana/TrkTrack/hTRecoD0Zoom_0");
	  else hTrkTrack_d0 =(TH1F*)file->Get("trkana/TrkTrack/hTRecoD0_0");
    }
    hTrkTrack_d0->SetTitle("Trk::Track Parameter: d_{0}");
    hTrkTrack_d0->GetXaxis()->SetTitle("d_{0} (mm)");
    hTrkTrack_d0->GetYaxis()->SetTitle("Tracks");
    hTrkTrack_d0->SetLineColor(ColorForAll+2);
    hTrkTrack_d0->SetFillColor(ColorForAll);
    hTrkTrack_d0->SetFillStyle(3001);
    if (normalizePlots) hTrkTrack_d0->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrack_d0->DrawCopy();
    setTitleFont(font);
    ScalePlots(); 
    delete hTrkTrack_d0;

    //
    // -----------------------------------------------------------------
    //

    cTrackParams->cd(2);
    hTrkTrack_z0 = (TH1F*)file->Get("trkana/TrkTrack/hTRecoZ0_0");
    hTrkTrack_z0->SetTitle("Trk::Track Parameter: z_{0}");
    hTrkTrack_z0->GetXaxis()->SetTitle("z_{0} (mm)");
    hTrkTrack_z0->GetYaxis()->SetTitle("Tracks");
    hTrkTrack_z0->SetLineColor(ColorForAll+2);
    hTrkTrack_z0->SetFillColor(ColorForAll);
    hTrkTrack_z0->SetFillStyle(3001);
    if (normalizePlots) hTrkTrack_z0->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrack_z0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTrkTrack_z0;
  
    //
    // -----------------------------------------------------------------
    //

    cTrackParams->cd(3);
    hTrkTrack_phi0 = (TH1F*)file->Get("trkana/TrkTrack/hTRecoPhi0_0");
    hTrkTrack_phi0->SetTitle("Trk::Track Parameter: #phi_{0}");
    hTrkTrack_phi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
    hTrkTrack_phi0->GetYaxis()->SetTitle("Tracks");
    hTrkTrack_phi0->SetLineColor(ColorForAll+2);
    hTrkTrack_phi0->SetFillColor(ColorForAll);
    hTrkTrack_phi0->SetFillStyle(3001);
    if (normalizePlots) hTrkTrack_phi0->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrack_phi0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTrkTrack_phi0;

    //
    // -----------------------------------------------------------------
    //

    cTrackParams->cd(4);
    hTrkTrack_theta0 = (TH1F*)file->Get("trkana/TrkTrack/hTRecoTheta_0");
    hTrkTrack_theta0->SetTitle("Trk::Track Parameter: #theta_{0}");
    hTrkTrack_theta0->GetXaxis()->SetTitle("#theta_{0} (rad)");
    hTrkTrack_theta0->GetYaxis()->SetTitle("Tracks");
    hTrkTrack_theta0->SetLineColor(ColorForAll+2);
    hTrkTrack_theta0->SetFillColor(ColorForAll);
    hTrkTrack_theta0->SetFillStyle(3001);
    if (normalizePlots) hTrkTrack_theta0->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrack_theta0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTrkTrack_theta0;

    //
    // -----------------------------------------------------------------
    //

    cTrackParams->cd(5);
    hTrkTrack_eta = (TH1F*)file->Get("trkana/TrkTrack/hTRecoEta_0");
    hTrkTrack_eta->SetTitle("Trk::Track Parameter: #eta");
    hTrkTrack_eta->GetXaxis()->SetTitle("#eta = -ln (tan(#theta/2))");
    hTrkTrack_eta->GetYaxis()->SetTitle("Tracks");
    hTrkTrack_eta->SetLineColor(ColorForAll+2);
    hTrkTrack_eta->SetFillColor(ColorForAll);
    hTrkTrack_eta->SetFillStyle(3001);
    if (normalizePlots) hTrkTrack_eta->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrack_eta->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTrkTrack_eta;

    //
    // -----------------------------------------------------------------
    //

    cTrackParams->cd(6);
    if (iBField>0) {
      hTrkTrack_qoverpt = (TH1F*)file->Get("trkana/TrkTrack/hTRecoQoverPt_0");
      gPad->SetLogy();
      hTrkTrack_qoverpt->SetTitle("Trk::Track Parameter: q/pt");
      hTrkTrack_qoverpt->GetXaxis()->SetTitle("q/pT (GeV^{-1})");
    }
    else {
      hTrkTrack_qoverpt = (TH1F*)file->Get("trkana/TrkTrack/hTRecoQoverP_0");
      hTrkTrack_qoverpt->SetTitle("Trk::Track Parameter: q/p");
      hTrkTrack_qoverpt->GetXaxis()->SetTitle("q/p (GeV^{-1})");   
    }
    hTrkTrack_qoverpt->GetYaxis()->SetTitle("Tracks");
    hTrkTrack_qoverpt->SetLineColor(ColorForAll+2);
    hTrkTrack_qoverpt->SetFillColor(ColorForAll);
    hTrkTrack_qoverpt->SetFillStyle(3001);
    if (normalizePlots) hTrkTrack_qoverpt->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkTrack_qoverpt->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTrkTrack_qoverpt;
  }

  return;
}
