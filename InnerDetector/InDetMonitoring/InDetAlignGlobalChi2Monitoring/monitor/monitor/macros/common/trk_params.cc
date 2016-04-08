/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void trk_params() {

  TH1F *hTrk_d0;
  TH1F *hTrk_z0;
  TH1F *hTrk_phi0;
  TH1F *hTrk_theta0;
  TH1F *hTrk_eta;
  TH1F *hTrk_qoverpT;
  TH1F *hTrk_pT;

  cout << " AlignTrk Parameters..." << endl;

  cParams = new TCanvas("AlignTrkParams","AlignTrk Parameters",
			CanvasSizeX6[0],CanvasSizeX6[1]);
  cParams->Divide(3,2);

  cParams->cd(1);
  if(iCosmics) hTrk_d0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoD0_0");
  else {
    if (iMisalignment==1) hTrk_d0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoD0Zoom_0");
    else hTrk_d0 =(TH1F*)file->Get("trkana/Track_Parameters/hRecoD0_0");
  }
  hTrk_d0->SetTitle("AlignTrk Parameter: d_{0}");
  hTrk_d0->GetXaxis()->SetTitle("d_{0} (mm)");
  hTrk_d0->GetYaxis()->SetTitle("Tracks");
  hTrk_d0->SetLineColor(ColorForAll+2);
  hTrk_d0->SetFillColor(ColorForAll);
  hTrk_d0->SetFillStyle(3001);
  if (normalizePlots) hTrk_d0->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrk_d0->DrawCopy();
  setTitleFont(font);
  ScalePlots(); 
  delete hTrk_d0;

  //
  // -----------------------------------------------------------------
  //

  cParams->cd(2);
  hTrk_z0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoZ0_0");
  hTrk_z0->SetTitle("AlignTrk Parameter: z_{0}");
  hTrk_z0->GetXaxis()->SetTitle("z_{0} (mm)");
  hTrk_z0->GetYaxis()->SetTitle("Tracks");
  hTrk_z0->SetLineColor(ColorForAll+2);
  hTrk_z0->SetFillColor(ColorForAll);
  hTrk_z0->SetFillStyle(3001);
  if (iCosmics) hTrk_z0->GetXaxis()->SetNdivisions(6);
  if (normalizePlots) hTrk_z0->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrk_z0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  delete hTrk_z0;
  
  //
  // -----------------------------------------------------------------
  //

  cParams->cd(3);
  hTrk_phi0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPhi0_0");
  hTrk_phi0->SetTitle("AlignTrk Parameter: #phi_{0}");
  hTrk_phi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
  hTrk_phi0->GetYaxis()->SetTitle("Tracks");
  hTrk_phi0->SetLineColor(ColorForAll+2);
  hTrk_phi0->SetFillColor(ColorForAll);
  hTrk_phi0->SetFillStyle(3001);
  if (normalizePlots) hTrk_phi0->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrk_phi0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  delete hTrk_phi0;

  //
  // -----------------------------------------------------------------
  //

  cParams->cd(4);
  hTrk_theta0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoTheta_0");
  hTrk_theta0->SetTitle("AlignTrk Parameter: #theta_{0}");
  hTrk_theta0->GetXaxis()->SetTitle("#theta_{0} (rad)");
  hTrk_theta0->GetYaxis()->SetTitle("Tracks");
  hTrk_theta0->SetLineColor(ColorForAll+2);
  hTrk_theta0->SetFillColor(ColorForAll);
  hTrk_theta0->SetFillStyle(3001);
  if (normalizePlots) hTrk_theta0->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrk_theta0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  delete hTrk_theta0;

  //
  // -----------------------------------------------------------------
  //

  cParams->cd(5);
  hTrk_eta = (TH1F*)file->Get("trkana/Track_Parameters/hRecoEta_0");
  hTrk_eta->SetTitle("AlignTrk Parameter: #eta");
  hTrk_eta->GetXaxis()->SetTitle("#eta = -ln (tan(#theta/2))");
  hTrk_eta->GetYaxis()->SetTitle("Tracks");
  hTrk_eta->SetLineColor(ColorForAll+2);
  hTrk_eta->SetFillColor(ColorForAll);
  hTrk_eta->SetFillStyle(3001);
  if (normalizePlots) hTrk_eta->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrk_eta->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  delete hTrk_eta;

  //
  // -----------------------------------------------------------------
  //

  cParams->cd(6);
  if (iBField>0) {
    hTrk_qoverpT = (TH1F*)file->Get("trkana/Track_Parameters/hRecoQoverPt_0");
    gPad->SetLogy();
    hTrk_qoverpT->SetTitle("AlignTrk Parameter: q/pT");
    hTrk_qoverpT->GetXaxis()->SetTitle("q/pT (GeV^{-1})");
    hTrk_qoverpT->GetYaxis()->SetTitle("Tracks");
    hTrk_qoverpT->SetLineColor(ColorForAll+2);
    hTrk_qoverpT->SetFillColor(ColorForAll);
    hTrk_qoverpT->SetFillStyle(3001);
    if (normalizePlots) hTrk_qoverpT->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrk_qoverpT->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTrk_qoverpT;
  }
  else {
    hTrk_pT = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPt_0");
    hTrk_pT->SetTitle("AlignTrk Parameter: q/pT");
    hTrk_pT->GetXaxis()->SetTitle("q/pT (GeV^{-1})");
    hTrk_pT->GetYaxis()->SetTitle("Tracks");
    hTrk_pT->SetLineColor(ColorForAll+2);
    hTrk_pT->SetFillColor(ColorForAll);
    hTrk_pT->SetFillStyle(3001);
    if (normalizePlots) hTrk_pT->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrk_pT->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    delete hTrk_pT;
  }

  cParams->Update();

}
