/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void CompTrkParams() {

  TH1F *h2Trk_d0;
  TH1F *h2ProcTrk_d0;
  TH1F *h2Trk_z0;
  TH1F *h2ProcTrk_z0;
  TH1F *h2Trk_phi0;
  TH1F *h2ProcTrk_phi0;
  TH1F *h2Trk_theta0;
  TH1F *h2ProcTrk_theta0;
  TH1F *h2Trk_eta;
  TH1F *h2ProcTrk_eta;
  TH1F *h2Trk_qoverpT;
  TH1F *h2ProcTrk_qoverpT;
  TH1F *h2Trk_pT;
  TH1F *h2ProcTrk_pT;
  
  cout << " AlignTrk Parameter Comparation..." << endl;

  cCompParams = new TCanvas("CompTrkParams","AlignTrk Parameter Comparation",
			    CanvasSizeX6[0],CanvasSizeX6[1]);
  cCompParams->Divide(3,2);
  
  cCompParams->cd(1);
  if (iMisalignment==1) {
    h2Trk_d0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoD0Zoom_0"); 
    sprintf(name,"trkana/Track_Parameters/hRecoD0Zoom_%d",level);  
    h2ProcTrk_d0 = (TH1F*)file->Get(name);
  }
  else {
    h2Trk_d0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoD0_0"); 
    sprintf(name,"trkana/Track_Parameters/hRecoD0_%d",level);    
    h2ProcTrk_d0 = (TH1F*)file->Get(name);
  }
  h2Trk_d0->SetTitle("AlignTrk comparation");
  h2Trk_d0->GetXaxis()->SetTitle("d_{0} (mm)");
  h2Trk_d0->GetYaxis()->SetTitle("Tracks");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  if (normalizePlots) h2Trk_d0->Scale(1./(float)iEvents);
  doSpace(h2Trk_d0);
  h2Trk_d0->SetLineColor(ColorForAll+2);
  h2Trk_d0->SetLineWidth(1);
  h2Trk_d0->SetFillColor(ColorForAll);
  h2Trk_d0->SetFillStyle(3001);
  h2Trk_d0->DrawCopy();
  
  h2ProcTrk_d0->SetLineColor(ColorForUsed);
  h2ProcTrk_d0->SetLineWidth(1);
  h2ProcTrk_d0->SetFillColor(ColorForUsed-9);
  h2ProcTrk_d0->SetFillStyle(3001);
  if (normalizePlots) h2ProcTrk_d0->Scale(1./(float)iEvents);
  h2ProcTrk_d0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2Trk_d0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrk_d0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //

  cCompParams->cd(2);
  h2Trk_z0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoZ0_0");   
  h2Trk_z0->SetTitle("AlignTrk comparation");
  h2Trk_z0->GetXaxis()->SetTitle("z_{0} (mm)");
  h2Trk_z0->GetYaxis()->SetTitle("Tracks");
  if (normalizePlots) h2Trk_z0->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  doSpace(h2Trk_z0);
  h2Trk_z0->SetLineColor(ColorForAll+2);
  h2Trk_z0->SetLineWidth(1);
  h2Trk_z0->SetFillColor(ColorForAll);
  h2Trk_z0->SetFillStyle(3001);
  h2Trk_z0->DrawCopy();

  sprintf(name,"trkana/Track_Parameters/hRecoZ0_%d",level);  
  h2ProcTrk_z0 = (TH1F*)file->Get(name);
  h2ProcTrk_z0->SetLineColor(ColorForUsed);
  h2ProcTrk_z0->SetLineWidth(1);
  h2ProcTrk_z0->SetFillColor(ColorForUsed-9);
  h2ProcTrk_z0->SetFillStyle(3001);
  if (normalizePlots) h2ProcTrk_z0->Scale(1./(float)iEvents);
  h2ProcTrk_z0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2Trk_z0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrk_z0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //

  cCompParams->cd(3);
  h2Trk_phi0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPhi0_0");   
  h2Trk_phi0->SetTitle("AlignTrk comparation");
  h2Trk_phi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
  h2Trk_phi0->GetYaxis()->SetTitle("Tracks");
  if (normalizePlots) h2Trk_phi0->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  doSpace(h2Trk_phi0);
  h2Trk_phi0->SetLineColor(ColorForAll+2);
  h2Trk_phi0->SetLineWidth(1);
  h2Trk_phi0->SetFillColor(ColorForAll);
  h2Trk_phi0->SetFillStyle(3001);
  h2Trk_phi0->DrawCopy();

    sprintf(name,"trkana/Track_Parameters/hRecoPhi0_%d",level);  
  h2ProcTrk_phi0 = (TH1F*)file->Get(name);
  h2ProcTrk_phi0->SetLineColor(ColorForUsed);
  h2ProcTrk_phi0->SetLineWidth(1);
  h2ProcTrk_phi0->SetFillColor(ColorForUsed-9);
  h2ProcTrk_phi0->SetFillStyle(3001);
  if (normalizePlots) h2ProcTrk_phi0->Scale(1./(float)iEvents);
  h2ProcTrk_phi0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
 
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93); 
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2Trk_phi0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrk_phi0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //

  cCompParams->cd(4);
  h2Trk_theta0 = (TH1F*)file->Get("trkana/Track_Parameters/hRecoTheta_0");   
  h2Trk_theta0->SetTitle("AlignTrk comparation");
  h2Trk_theta0->GetXaxis()->SetTitle("#theta_{0} (rad)");
  h2Trk_theta0->GetYaxis()->SetTitle("Tracks");
  if (normalizePlots) h2Trk_theta0->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  doSpace(h2Trk_theta0);
  h2Trk_theta0->SetLineColor(ColorForAll+2);
  h2Trk_theta0->SetLineWidth(1);
  h2Trk_theta0->SetFillColor(ColorForAll);
  h2Trk_theta0->SetFillStyle(3001);
  h2Trk_theta0->DrawCopy();

    sprintf(name,"trkana/Track_Parameters/hRecoTheta_%d",level);  
  h2ProcTrk_theta0 = (TH1F*)file->Get(name);
  h2ProcTrk_theta0->SetLineColor(ColorForUsed);
  h2ProcTrk_theta0->SetLineWidth(1);
  h2ProcTrk_theta0->SetFillColor(ColorForUsed-9);
  h2ProcTrk_theta0->SetFillStyle(3001);
  if (normalizePlots) h2ProcTrk_theta0->Scale(1./(float)iEvents);
  h2ProcTrk_theta0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2Trk_theta0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrk_theta0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //

  cCompParams->cd(5);
  h2Trk_eta = (TH1F*)file->Get("trkana/Track_Parameters/hRecoEta_0");
  h2Trk_eta->SetTitle("AlignTrk comparation");
  h2Trk_eta->GetXaxis()->SetTitle("#eta = -ln (tan(#theta/2))");
  h2Trk_eta->GetYaxis()->SetTitle("Tracks");
  if (normalizePlots) h2Trk_eta->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  doSpace(h2Trk_eta);
  h2Trk_eta->SetLineColor(ColorForAll+2);
  h2Trk_eta->SetLineWidth(1);
  h2Trk_eta->SetFillColor(ColorForAll);
  h2Trk_eta->SetFillStyle(3001);
  h2Trk_eta->DrawCopy();

    sprintf(name,"trkana/Track_Parameters/hRecoEta_%d",level);    
  h2ProcTrk_eta = (TH1F*)file->Get(name);
  h2ProcTrk_eta->SetLineColor(ColorForUsed);
  h2ProcTrk_eta->SetLineWidth(1);
  h2ProcTrk_eta->SetFillColor(ColorForUsed-9);
  h2ProcTrk_eta->SetFillStyle(3001);
  if (normalizePlots) h2ProcTrk_eta->Scale(1./(float)iEvents);
  h2ProcTrk_eta->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2Trk_eta,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrk_eta,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //

  cCompParams->cd(6);
  if (iBField>0) {
    h2Trk_qoverpT = (TH1F*)file->Get("trkana/Track_Parameters/hRecoQoverPt_0");
    gPad->SetLogy();
    h2Trk_qoverpT->SetTitle("AlignTrk comparation");
    h2Trk_qoverpT->GetXaxis()->SetTitle("q/pT (GeV^{-1})");
    h2Trk_qoverpT->GetYaxis()->SetTitle("Tracks");
    if (normalizePlots) h2Trk_qoverpT->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h2Trk_qoverpT->SetLineColor(ColorForAll+2);
    h2Trk_qoverpT->SetLineWidth(1);
    h2Trk_qoverpT->SetFillColor(ColorForAll);
    h2Trk_qoverpT->SetFillStyle(3001);
    h2Trk_qoverpT->DrawCopy();

    sprintf(name,"trkana/Track_Parameters/hRecoQoverPt_%d",level);    
    h2ProcTrk_qoverpT = (TH1F*)file->Get(name);
    h2ProcTrk_qoverpT->SetLineColor(ColorForUsed);
    h2ProcTrk_qoverpT->SetLineWidth(1);
    h2ProcTrk_qoverpT->SetFillColor(ColorForUsed-9);
    h2ProcTrk_qoverpT->SetFillStyle(3001);
    if (normalizePlots) h2ProcTrk_qoverpT->Scale(1./(float)iEvents);
    h2ProcTrk_qoverpT->DrawCopy("SAME");
    setTitleFont(font);
    ScalePlots();
  }
  else {
    h2Trk_pT = (TH1F*)file->Get("trkana/Track_Parameters/hRecoPt_0");
    h2Trk_pT->SetTitle("AlignTrk comparation");
    h2Trk_pT->GetXaxis()->SetTitle("q/pT (GeV^{-1})");
    h2Trk_pT->GetYaxis()->SetTitle("Tracks");
    if (normalizePlots) h2Trk_pT->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h2Trk_pT->SetLineColor(ColorForAll+2);
    h2Trk_pT->SetLineWidth(1);
    h2Trk_pT->SetFillColor(ColorForAll);
    h2Trk_pT->SetFillStyle(3001);
    h2Trk_pT->DrawCopy();

    sprintf(name,"trkana/Track_Parameters/hRecoPt_%d",level);    
    h2ProcTrk_pT = (TH1F*)file->Get(name);
    h2ProcTrk_pT->SetLineColor(ColorForUsed);
    h2ProcTrk_pT->SetLineWidth(1);
    h2ProcTrk_pT->SetFillColor(ColorForUsed-9);
    h2ProcTrk_pT->SetFillStyle(3001);
    if (normalizePlots) h2ProcTrk_pT->Scale(1./(float)iEvents);
    h2ProcTrk_pT->DrawCopy("SAME");
    setTitleFont(font);
    ScalePlots();
  }
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  if (iBField>0) leg0->AddEntry(h2Trk_qoverpT,name,"L");
  else leg0->AddEntry(h2Trk_pT,name,"L");
  sprintf(name,"Processed tracks");
  if (iBField>0) leg0->AddEntry(h2ProcTrk_qoverpT,name,"L");
  else leg0->AddEntry(h2ProcTrk_pT,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  cCompParams->Update();

}
