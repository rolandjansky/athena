/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void TrackMatching_ParametersVsEta() {

  if (!TrackMatching) return;

  TH2F *h2TrkMatching_d0VsEta;
  TH2F *h2ProcTrkMatching_d0VsEta;
  TH2F *h2TrkMatching_z0VsEta;
  TH2F *h2ProcTrkMatching_z0VsEta;
  TH2F *h2TrkMatching_phi0VsEta;
  TH2F *h2ProcTrkMatching_phi0VsEta;
  TH2F *h2TrkMatching_theta0VsEta;
  TH2F *h2ProcTrkMatching_theta0VsEta;
  TH2F *h2TrkMatching_etaVsEta;
  TH2F *h2ProcTrkMatching_etaVsEta;
  TH2F *h2TrkMatching_qoverpTVsEta;
  TH2F *h2ProcTrkMatching_qoverpTVsEta;
 
  TProfile *HProf_d0VsEta;
  TProfile *HProf_z0VsEta;
  TProfile *HProf_Phi0VsEta;
  TProfile *HProf_Theta0VsEta;
  TProfile *HProf_EtaVsEta;
  TProfile *HProf_QoverPtVsEta;

  cout << " Track Parameters Matching Vs Eta..." << endl;

  bool ShowFit = showFits;
  
  cCompTrackMatchingParamsVsEta = new TCanvas("CompTrkMatchingParamsVsEta","Track Matching (up-low) Parameter Comparation Vs D0",
					    CanvasSizeX6[0],CanvasSizeX6[1]);
  cCompTrackMatchingParamsVsEta->Divide(3,2);
  
  // -----------------------------------------------------------------
  // Track Matching delta d0 Vs D0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsEta->cd(1);
  h2TrkMatching_d0VsEta = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_D0_Vs_Eta0"); 
  // sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_Eta%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_Eta2",level);    
  h2ProcTrkMatching_d0VsEta = (TH2F*)file->Get(name);
 
  h2TrkMatching_d0VsEta->SetTitle("#Deltad_{0} Vs #eta");
  h2TrkMatching_d0VsEta->GetXaxis()->SetTitle("#eta");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_d0VsEta->Scale(1./(float)iEvents);
  h2TrkMatching_d0VsEta->SetLineColor(ColorForAll+2);
  h2TrkMatching_d0VsEta->DrawCopy();
  hProf_d0VsEta = h2TrkMatching_d0VsEta->ProfileX(); 
  hProf_d0VsEta->SetErrorOption(" ");  
  hProf_d0VsEta->SetMarkerColor(ColorForAll+2);
  hProf_d0VsEta->GetYaxis()->SetTitle("#Delta d_{0} (mm) ");
  hProf_d0VsEta->SetMarkerStyle(21);
  hProf_d0VsEta->SetMarkerSize(0.4);
  hProf_d0VsEta->SetStats(kFALSE);
  hProf_d0VsEta->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_d0VsEta->Scale(1./(float)iEvents);
  h2ProcTrkMatching_d0VsEta->SetLineColor(ColorForUsed);
  hProf_d0VsEta = h2ProcTrkMatching_d0VsEta->ProfileX(); 
  hProf_d0VsEta->SetErrorOption(" ");  
  hProf_d0VsEta->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_d0VsEta,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_d0VsEta,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
// -----------------------------------------------------------------
  // Track Matching delta z0 Vs D0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsEta->cd(2);
  h2TrkMatching_z0VsEta = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Z0_Vs_Eta0"); 
  // sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_Eta%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Z0_Vs_Eta2",level);    
  h2ProcTrkMatching_z0VsEta = (TH2F*)file->Get(name);
 
  h2TrkMatching_z0VsEta->SetTitle("#Deltaz_{0} Vs #eta");
  h2TrkMatching_z0VsEta->GetXaxis()->SetTitle("#eta  ");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_d0VsEta->Scale(1./(float)iEvents);
  h2TrkMatching_z0VsEta->SetLineColor(ColorForAll+2);
  h2TrkMatching_z0VsEta->DrawCopy();
  hProf_z0VsEta = h2TrkMatching_z0VsEta->ProfileX();
  hProf_z0VsEta->SetErrorOption(" ");   
  hProf_z0VsEta->SetMarkerColor(ColorForAll+2);
  hProf_z0VsEta->GetYaxis()->SetTitle("#Delta z_{0}(mm) ");
  hProf_z0VsEta->SetMarkerStyle(21);
  hProf_z0VsEta->SetMarkerSize(0.4);
  hProf_d0VsEta->SetStats(kFALSE);
  hProf_z0VsEta->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_d0VsEta->Scale(1./(float)iEvents);
  h2ProcTrkMatching_z0VsEta->SetLineColor(ColorForUsed);
  hProf_z0VsEta = h2ProcTrkMatching_d0VsEta->ProfileX(); 
  hProf_z0VsEta->SetErrorOption(" ");   
  hProf_z0VsEta->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_z0VsEta,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_z0VsEta,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  

  // -----------------------------------------------------------------
  // Track Matching delta Phi0 Vs D0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsEta->cd(3);
  h2TrkMatching_Phi0VsEta = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Phi0_Vs_Eta0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_Vs_Eta%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_Vs_Eta2",level);    
  h2ProcTrkMatching_Phi0VsEta = (TH2F*)file->Get(name);
  
  h2TrkMatching_Phi0VsEta->SetTitle("#Delta#phi_{0} Vs #eta");
  h2TrkMatching_Phi0VsEta->GetXaxis()->SetTitle("#eta  ");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  // if (normalizePlots) h2TrkMatching_Phi0VsEta->Scale(1./(float)iEvents);
  h2TrkMatching_Phi0VsEta->SetLineColor(ColorForAll+2);
  h2TrkMatching_Phi0VsEta->DrawCopy();
  
  hProf_Phi0VsEta = h2TrkMatching_Phi0VsEta->ProfileX();
  hProf_Phi0VsEta->SetErrorOption(" ");    
  hProf_Phi0VsEta->SetMarkerColor(ColorForAll+2);
  hProf_Phi0VsEta->GetYaxis()->SetTitle("#Delta #phi_{0} (rad)");
  hProf_Phi0VsEta->SetMarkerStyle(21);
  hProf_Phi0VsEta->SetMarkerSize(0.4);
  hProf_Phi0VsEta->SetStats(kFALSE);
  hProf_Phi0VsEta->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_Phi0VsEta->Scale(1./(float)iEvents);
  h2ProcTrkMatching_Phi0VsEta->SetLineColor(ColorForUsed);
  hProf_Phi0VsEta = h2ProcTrkMatching_Phi0VsEta->ProfileX();
  hProf_Phi0VsEta->SetErrorOption(" ");     
  hProf_Phi0VsEta->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_Phi0VsEta,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_Phi0VsEta,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  
  // -----------------------------------------------------------------
  // Track Matching delta Theta Vs D0
  // -----------------------------------------------------------------
  
  cCompTrackMatchingParamsVsEta->cd(4);
  h2TrkMatching_Theta0VsEta = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Theta0_Vs_Eta0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta0_Vs_Eta%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta0_Vs_Eta2",level);    
  h2ProcTrkMatching_Theta0VsEta = (TH2F*)file->Get(name);
  h2TrkMatching_Theta0VsEta->SetTitle("#Delta#theta_{0} Vs #eta");
  h2TrkMatching_Theta0VsEta->GetXaxis()->SetTitle("#eta  ");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  // if (normalizePlots) h2TrkMatching_Theta0VsEta->Scale(1./(float)iEvents);
  h2TrkMatching_Theta0VsEta->SetLineColor(ColorForAll+2);
  h2TrkMatching_Theta0VsEta->DrawCopy();
  hProf_Theta0VsEta = h2TrkMatching_Theta0VsEta->ProfileX(); 
  hProf_Theta0VsEta->SetErrorOption(" ");    
  hProf_Theta0VsEta->SetMarkerColor(ColorForAll+2);
  hProf_Theta0VsEta->GetYaxis()->SetTitle("#Delta #theta_{0} (rad)");
  hProf_Theta0VsEta->SetMarkerStyle(21);
  hProf_Theta0VsEta->SetMarkerSize(0.4);
  hProf_Theta0VsEta->SetStats(kFALSE);
  hProf_Theta0VsEta->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_Theta0VsEta->Scale(1./(float)iEvents);
  h2ProcTrkMatching_Theta0VsEta->SetLineColor(ColorForUsed);
  hProf_Theta0VsEta = h2ProcTrkMatching_Theta0VsEta->ProfileX();
  hProf_Theta0VsEta->SetErrorOption(" ");     
  hProf_Theta0VsEta->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_Theta0VsEta,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_Theta0VsEta,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  // -----------------------------------------------------------------
  // Track Matching delta  Eta Vs D0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsEta->cd(5);
  h2TrkMatching_EtaVsEta = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Eta_Vs_Eta0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_Vs_Eta%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_Vs_Eta2",level);    
  h2ProcTrkMatching_EtaVsEta = (TH2F*)file->Get(name);
  h2TrkMatching_EtaVsEta->SetTitle("#Delta#eta Vs #eta");
  h2TrkMatching_EtaVsEta->GetXaxis()->SetTitle("#eta  ");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //  if (normalizePlots) h2TrkMatching_EtaVsEta->Scale(1./(float)iEvents);
  h2TrkMatching_EtaVsEta->SetLineColor(ColorForAll+2);
  h2TrkMatching_EtaVsEta->DrawCopy();
  
  hProf_EtaVsEta = h2TrkMatching_EtaVsEta->ProfileX(); 
  hProf_EtaVsEta->SetErrorOption(" ");    
  hProf_EtaVsEta->SetMarkerColor(ColorForAll+2);
  hProf_EtaVsEta->GetYaxis()->SetTitle("#Delta #eta ");
  hProf_EtaVsEta->SetMarkerStyle(21);
  hProf_EtaVsEta->SetMarkerSize(0.4);
  hProf_EtaVsEta->SetStats(kFALSE);
  hProf_EtaVsEta->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_EtaVsEta->Scale(1./(float)iEvents);
  h2ProcTrkMatching_EtaVsEta->SetLineColor(ColorForUsed);
  hProf_EtaVsEta = h2ProcTrkMatching_EtaVsEta->ProfileX(); 
  hProf_EtaVsEta->SetErrorOption(" ");    
  hProf_EtaVsEta->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_EtaVsEta,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_EtaVsEta,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  
  // -----------------------------------------------------------------
  // Track Matching delta QoverPt Vs D0
  // -----------------------------------------------------------------
  
  cCompTrackMatchingParamsVsEta->cd(6);
  h2TrkMatching_QoverPtVsEta = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_QoverPt_Vs_Eta0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_QoverPt_Vs_Eta%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_QoverPt_Vs_Eta2",level);    
  h2ProcTrkMatching_QoverPtVsEta = (TH2F*)file->Get(name);
  h2TrkMatching_QoverPtVsEta->SetTitle("#Deltaq/p_{T} Vs #eta");
  h2TrkMatching_QoverPtVsEta->GetXaxis()->SetTitle("#eta  ");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_QoverPtVsEta->Scale(1./(float)iEvents);
  h2TrkMatching_QoverPtVsEta->SetLineColor(ColorForAll+2);
  h2TrkMatching_QoverPtVsEta->DrawCopy();
  
  hProf_QoverPtVsEta = h2TrkMatching_QoverPtVsEta->ProfileX();
  hProf_QoverPtVsEta->SetErrorOption(" ");     
  hProf_QoverPtVsEta->SetMarkerColor(ColorForAll+2);
  hProf_QoverPtVsEta->GetYaxis()->SetTitle("#Delta q/p_{T} (GeV^{-1})");
  hProf_QoverPtVsEta->SetMarkerStyle(21);
  hProf_QoverPtVsEta->SetMarkerSize(0.4);
  hProf_QoverPtVsEta->SetStats(kFALSE);
  hProf_QoverPtVsEta->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  // if (normalizePlots) h2ProcTrkMatching_QoverPtVsEta->Scale(1./(float)iEvents);
  h2ProcTrkMatching_QoverPtVsEta->SetLineColor(ColorForUsed);
  hProf_QoverPtVsEta = h2ProcTrkMatching_QoverPtVsEta->ProfileX(); 
  hProf_QoverPtVsEta->SetErrorOption(" ");    
  hProf_QoverPtVsEta->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_QoverPtVsEta,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_QoverPtVsEta,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  



}


