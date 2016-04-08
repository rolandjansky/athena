/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void TrkParCorrProfNoTruth() {

  TH2F *hTrkd0phi0;
  TH2F *hProcTrkd0phi0;
  TH2F *hTrkz0eta;
  TH2F *hProcTrkz0eta;

  cout << " Quick Track Parameter Correlations (d0 and z0 Vs phi0)..." << endl;

  cParCorr = new TCanvas("cParCorr","Track Parameter Correlations 1",
			 CanvasSizeX4[0],CanvasSizeX4[1]);
  cParCorr->Divide(2,2);
  
  cParCorr->cd(1);
  hTrkd0phi0 = (TH2F*)file->Get("trkana/Track_Parameters/hRecoD0_RecoPhi0_0");
  gPad->SetGridx();
  gPad->SetGridy();
  hTrkd0phi0->SetTitle("d_{0} Vs #phi_{0} (All tracks)");
  hTrkd0phi0->GetYaxis()->SetTitle("d_{0} (mm)");
  hTrkd0phi0->GetXaxis()->SetTitle("phi0 (rad)");
  hTrkd0phi0->Draw("Cont4Z");
  cParCorr->Update();
  if (!AtlasStyle) {
    TPaveStats *st = (TPaveStats*)hTrkd0phi0->FindObject("stats"); 
    st->SetX1NDC(0.66); //new x start position
    st->SetX2NDC(0.88); //new x start position
    st->SetY1NDC(0.82); //new y end position
    st->SetOptStat(1110);
    st->Draw();
  }
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hTrkd0phi0->Scale(1./(float)iEvents);

  cParCorr->cd(2);
  sprintf(name,"trkana/Track_Parameters/hRecoD0_RecoPhi0_%d",level);
  hProcTrkd0phi0 = (TH2F*)file->Get(name);
  gPad->SetGridx();
  gPad->SetGridy();
  hProcTrkd0phi0->SetTitle("d_{0} Vs #phi_{0} (Processed tracks)");
  hProcTrkd0phi0->GetYaxis()->SetTitle("d_{0} (mm)");
  hProcTrkd0phi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
  hProcTrkd0phi0->Draw("Cont4Z");
  cParCorr->Update();
  if (!AtlasStyle) {
    TPaveStats *st2 = (TPaveStats*)hProcTrkd0phi0->FindObject("stats"); 
    st2->SetX1NDC(0.66); //new x start position
    st2->SetX2NDC(0.88); //new x start position
    st2->SetY1NDC(0.82); //new y end position
    st2->SetOptStat(1110);
    st2->Draw();
  }
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hProcTrkd0phi0->Scale(1./(float)iEvents);
  
  cParCorr->cd(3);
  hTrkz0eta = (TH2F*)file->Get("trkana/Track_Parameters/hRecoZ0_RecoEta_0");
  gPad->SetGridx();
  gPad->SetGridy();
  hTrkz0eta->SetTitle("z_{0} Vs #eta (All tracks)");
  hTrkz0eta->GetYaxis()->SetTitle("z_{0} (mm)");
  hTrkz0eta->GetXaxis()->SetTitle("#eta");
  if (iCosmics) hTrkz0eta->GetYaxis()->SetRangeUser(-1000,1000);
  hTrkz0eta->Draw("Cont4Z");
  cParCorr->Update();
  if (!AtlasStyle) {
    TPaveStats *st = (TPaveStats*)hTrkz0eta->FindObject("stats"); 
    st->SetX1NDC(0.66); //new x start position
    st->SetX2NDC(0.88); //new x start position
    st->SetY1NDC(0.82); //new y end position
    st->SetOptStat(1110);
    st->Draw();
  }
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hTrkz0eta->Scale(1./(float)iEvents);
  
  cParCorr->cd(4);
  sprintf(name,"trkana/Track_Parameters/hRecoZ0_RecoEta_%d",level);
  hProcTrkz0eta = (TH2F*)file->Get(name);
  gPad->SetGridx();
  gPad->SetGridy();
  hProcTrkz0eta->SetTitle("z_{0} Vs #eta (Processed tracks)");
  hProcTrkz0eta->GetYaxis()->SetTitle("z_{0} (mm)");
  hProcTrkz0eta->GetXaxis()->SetTitle("#eta");
  if (iCosmics) hProcTrkz0eta->GetYaxis()->SetRangeUser(-1000,1000);
  hProcTrkz0eta->Draw("Cont4Z");
  hProcTrkz0eta->Draw("Cont4Z");
  cParCorr->Update();
  if (!AtlasStyle) {
    TPaveStats *st2 = (TPaveStats*)hProcTrkz0eta->FindObject("stats"); 
    st2->SetX1NDC(0.66); //new x start position
    st2->SetX2NDC(0.88); //new x start position
    st2->SetY1NDC(0.82); //new y end position
    st2->SetOptStat(1110);
    st2->Draw();
  }
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hProcTrkz0eta->Scale(1./(float)iEvents);
}
