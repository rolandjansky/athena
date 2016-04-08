/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void Used_trk_params() {

  TH1F *hProcTrk_d0;
  TH1F *hProcTrk_z0;
  TH1F *hProcTrk_phi0;
  TH1F *hProcTrk_theta0;
  TH1F *hProcTrk_eta;
  TH1F *hProcTrk_qoverpT;
  TH1F *hProcTrk_pT;

  cout << " Processed Track Parameters..." << endl;

  cUsedParams = new TCanvas("ProcTrkParams","Processed AlignTrk Parameters",
			    CanvasSizeX6[0],CanvasSizeX6[1]);
  cUsedParams->Divide(3,2);

  cUsedParams->cd(1);
  if(iCosmics) {
    sprintf(name,"trkana/Track_Parameters/hRecoD0_%d",level);
    hProcTrk_d0 =(TH1F*)file->Get(name);
  }
  else {
    if (iMisalignment==1) {
      sprintf(name,"trkana/Track_Parameters/hRecoD0Zoom_%d",level);
      hProcTrk_d0 = (TH1F*)file->Get(name);
    }
    else {
      sprintf(name,"trkana/Track_Parameters/hRecoD0_%d",level);
      hProcTrk_d0 =(TH1F*)file->Get(name);
    }
  }
  hProcTrk_d0->SetTitle("Processed AlignTrk Parameter: d_{0}");
  hProcTrk_d0->GetXaxis()->SetTitle("d_{0} (mm)");
  hProcTrk_d0->GetYaxis()->SetTitle("Tracks");
  hProcTrk_d0->SetLineColor(ColorForUsed+2);
  hProcTrk_d0->SetFillColor(ColorForUsed);
  hProcTrk_d0->SetFillStyle(3001);
  if (normalizePlots) hProcTrk_d0->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hProcTrk_d0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.67);
  delete hProcTrk_d0;

  //
  // -----------------------------------------------------------------
  //

  cUsedParams->cd(2);
  sprintf(name,"trkana/Track_Parameters/hRecoZ0_%d",level);
  hProcTrk_z0 = (TH1F*)file->Get(name);
  hProcTrk_z0->SetTitle("Processed AlignTrk Parameter: z_{0}");
  hProcTrk_z0->GetXaxis()->SetTitle("z_{0} (mm)");
  hProcTrk_z0->GetYaxis()->SetTitle("Tracks");
  hProcTrk_z0->SetLineColor(ColorForUsed+2);
  hProcTrk_z0->SetFillColor(ColorForUsed);
  hProcTrk_z0->SetFillStyle(3001);
  if (iCosmics) hProcTrk_z0->GetXaxis()->SetNdivisions(6);
  if (normalizePlots) hProcTrk_z0->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hProcTrk_z0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.67);
  delete hProcTrk_z0;
  
  //
  // -----------------------------------------------------------------
  //

  cUsedParams->cd(3);
  sprintf(name,"trkana/Track_Parameters/hRecoPhi0_%d",level);
  hProcTrk_phi0 = (TH1F*)file->Get(name);
  hProcTrk_phi0->SetTitle("Processed AlignTrk Parameter: #phi_{0}");
  hProcTrk_phi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
  hProcTrk_phi0->GetYaxis()->SetTitle("Tracks");
  hProcTrk_phi0->SetLineColor(ColorForUsed+2);
  hProcTrk_phi0->SetFillColor(ColorForUsed);
  hProcTrk_phi0->SetFillStyle(3001);
  if (normalizePlots) hProcTrk_phi0->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hProcTrk_phi0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.67);
  delete hProcTrk_phi0;

  //
  // -----------------------------------------------------------------
  //

  cUsedParams->cd(4);
  sprintf(name,"trkana/Track_Parameters/hRecoTheta_%d",level);
  hProcTrk_theta0 = (TH1F*)file->Get(name);
  hProcTrk_theta0->SetTitle("Processed AlignTrk Parameter: #theta_{0}");
  hProcTrk_theta0->GetXaxis()->SetTitle("#theta_{0} (rad)");
  hProcTrk_theta0->GetYaxis()->SetTitle("Tracks");
  hProcTrk_theta0->SetLineColor(ColorForUsed+2);
  hProcTrk_theta0->SetFillColor(ColorForUsed);
  hProcTrk_theta0->SetFillStyle(3001);
  if (normalizePlots) hProcTrk_theta0->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hProcTrk_theta0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.67);
  delete hProcTrk_theta0;

  //
  // -----------------------------------------------------------------
  //

  cUsedParams->cd(5);
  sprintf(name,"trkana/Track_Parameters/hRecoEta_%d",level);
  hProcTrk_eta = (TH1F*)file->Get(name);
  hProcTrk_eta->SetTitle("Processed AlignTrk Parameter: #eta");
  hProcTrk_eta->GetXaxis()->SetTitle("#eta = -ln (tan(#theta/2))");
  hProcTrk_eta->GetYaxis()->SetTitle("Tracks");
  hProcTrk_eta->SetLineColor(ColorForUsed+2);
  hProcTrk_eta->SetFillColor(ColorForUsed);
  hProcTrk_eta->SetFillStyle(3001);
  if (normalizePlots) hProcTrk_eta->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hProcTrk_eta->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.67);
  delete hProcTrk_eta;

  //
  // -----------------------------------------------------------------
  //

  cUsedParams->cd(6);
  if (iBField>0) {
    sprintf(name,"trkana/Track_Parameters/hRecoQoverPt_%d",level);
    hProcTrk_qoverpT = (TH1F*)file->Get(name);
    gPad->SetLogy();
    hProcTrk_qoverpT->SetTitle("Processed AlignTrk Parameter: q/pT");
    hProcTrk_qoverpT->GetXaxis()->SetTitle("q/pT (GeV^{-1})");
    hProcTrk_qoverpT->GetYaxis()->SetTitle("Tracks");
    hProcTrk_qoverpT->SetLineColor(ColorForUsed+2);
    hProcTrk_qoverpT->SetFillColor(ColorForUsed);
    hProcTrk_qoverpT->SetFillStyle(3001);
    if (normalizePlots)  hProcTrk_qoverpT->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hProcTrk_qoverpT->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete hProcTrk_qoverpT;
  }
  else {
    sprintf(name,"trkana/Track_Parameters/hRecoPt_%d",level);
    hProcTrk_pT = (TH1F*)file->Get(name);
    hProcTrk_pT->SetTitle("Processed AlignTrk Parameter: q/pT");
    hProcTrk_pT->GetXaxis()->SetTitle("q/pT (GeV^{-1})");
    hProcTrk_pT->GetYaxis()->SetTitle("Tracks");
    hProcTrk_pT->SetLineColor(ColorForUsed+2);
    hProcTrk_pT->SetFillColor(ColorForUsed);
    hProcTrk_pT->SetFillStyle(3001);
    if (normalizePlots)  hProcTrk_pT->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hProcTrk_pT->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete hProcTrk_pT;
  }
  cUsedParams->Update();

}
