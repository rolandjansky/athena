/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void TrackMatching_ParametersVsD0() {

  if (!TrackMatching) return;

  TH2F *h2TrkMatching_d0VsD0;
  TH2F *h2ProcTrkMatching_d0VsD0;
  TH2F *h2TrkMatching_z0VsD0;
  TH2F *h2ProcTrkMatching_z0VsD0;
  TH2F *h2TrkMatching_phi0VsD0;
  TH2F *h2ProcTrkMatching_phi0VsD0;
  TH2F *h2TrkMatching_theta0VsD0;
  TH2F *h2ProcTrkMatching_theta0VsD0;
  TH2F *h2TrkMatching_etaVsD0;
  TH2F *h2ProcTrkMatching_etaVsD0;
  TH2F *h2TrkMatching_qoverpTVsD0;
  TH2F *h2ProcTrkMatching_qoverpTVsD0;
 
  TProfile *HProf_d0VsD0;
  TProfile *HProf_z0VsD0;
  TProfile *HProf_Phi0VsD0;
  TProfile *HProf_Theta0VsD0;
  TProfile *HProf_EtaVsD0;
  TProfile *HProf_QoverPtVsD0;

  cout << " Track Parameters Matching Vs d0..." << endl;

  bool ShowFit = showFits;
  
  cCompTrackMatchingParamsVsD0 = new TCanvas("CompTrkMatchingParamsVsd0","Track Matching (up-low) Parameter Comparation Vs D0",
					    CanvasSizeX6[0],CanvasSizeX6[1]);
  cCompTrackMatchingParamsVsD0->Divide(3,2);
  
  // -----------------------------------------------------------------
  // Track Matching delta d0 Vs D0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsD0->cd(1);
  h2TrkMatching_d0VsD0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_D0_Vs_D00"); 
  // sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_D02",level);    
  h2ProcTrkMatching_d0VsD0 = (TH2F*)file->Get(name);
 
  h2TrkMatching_d0VsD0->SetTitle("#Deltad_{0} Vs d_{0}");
  h2TrkMatching_d0VsD0->GetXaxis()->SetTitle("d_{0} (mm)");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_d0VsD0->Scale(1./(float)iEvents);
  h2TrkMatching_d0VsD0->SetLineColor(ColorForAll+2);
  h2TrkMatching_d0VsD0->DrawCopy();
  hProf_d0VsD0 = h2TrkMatching_d0VsD0->ProfileX(); 
  hProf_d0VsD0->SetErrorOption(" "); //s
  hProf_d0VsD0->SetMarkerColor(ColorForAll+2);
  hProf_d0VsD0->GetYaxis()->SetTitle("#Delta d_{0}(mm)");
  hProf_d0VsD0->SetMarkerStyle(21);
  hProf_d0VsD0->SetMarkerSize(0.4);
  hProf_d0VsD0->SetStats(kFALSE);
  hProf_d0VsD0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_d0VsD0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_d0VsD0->SetLineColor(ColorForUsed);
  hProf_d0VsD0 = h2ProcTrkMatching_d0VsD0->ProfileX(); 
  hProf_d0VsD0->SetErrorOption(" ");
  hProf_d0VsD0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_d0VsD0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_d0VsD0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
// -----------------------------------------------------------------
  // Track Matching delta z0 Vs D0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsD0->cd(2);
  h2TrkMatching_z0VsD0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Z0_Vs_D00"); 
  // sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Z0_Vs_D02",level);    
  h2ProcTrkMatching_z0VsD0 = (TH2F*)file->Get(name);
 
  h2TrkMatching_z0VsD0->SetTitle("#Deltaz_{0} Vs d_{0}");
  h2TrkMatching_z0VsD0->GetXaxis()->SetTitle("d_{0} (mm)");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_d0VsD0->Scale(1./(float)iEvents);
  h2TrkMatching_z0VsD0->SetLineColor(ColorForAll+2);
  h2TrkMatching_z0VsD0->DrawCopy();
  hProf_z0VsD0 = h2TrkMatching_z0VsD0->ProfileX(); 
  hProf_z0VsD0->SetErrorOption(" ");
  hProf_z0VsD0->SetMarkerColor(ColorForAll+2);
  hProf_z0VsD0->GetYaxis()->SetTitle("#Delta z_{0}(mm)");
  hProf_z0VsD0->SetMarkerStyle(21);
  hProf_z0VsD0->SetMarkerSize(0.4);
  hProf_z0VsD0->SetStats(kFALSE);
  hProf_z0VsD0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_d0VsD0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_z0VsD0->SetLineColor(ColorForUsed);
  hProf_z0VsD0 = h2ProcTrkMatching_z0VsD0->ProfileX(); 
  hProf_z0VsD0->SetErrorOption(" ");
  hProf_z0VsD0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_z0VsD0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_z0VsD0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  

  // -----------------------------------------------------------------
  // Track Matching delta Phi0 Vs D0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsD0->cd(3);
  h2TrkMatching_Phi0VsD0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Phi0_Vs_D00"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_Vs_D02",level);    
  h2ProcTrkMatching_Phi0VsD0 = (TH2F*)file->Get(name);
  
  h2TrkMatching_Phi0VsD0->SetTitle("#Delta#phi_{0} Vs d_{0}");
  h2TrkMatching_Phi0VsD0->GetXaxis()->SetTitle("d_{0} (mm)");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  // if (normalizePlots) h2TrkMatching_Phi0VsD0->Scale(1./(float)iEvents);
  h2TrkMatching_Phi0VsD0->SetLineColor(ColorForAll+2);
  h2TrkMatching_Phi0VsD0->DrawCopy();
  
  hProf_Phi0VsD0 = h2TrkMatching_Phi0VsD0->ProfileX();
  hProf_Phi0VsD0->SetErrorOption(" ");
  hProf_Phi0VsD0->SetMarkerColor(ColorForAll+2);
  hProf_Phi0VsD0->GetYaxis()->SetTitle("#Delta #phi_{0} (rad)");
  hProf_Phi0VsD0->SetMarkerStyle(21);
  hProf_Phi0VsD0->SetMarkerSize(0.4);
  hProf_Phi0VsD0->SetStats(kFALSE);
  hProf_Phi0VsD0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_Phi0VsD0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_Phi0VsD0->SetLineColor(ColorForUsed);
  hProf_Phi0VsD0 = h2ProcTrkMatching_Phi0VsD0->ProfileX();
  hProf_Phi0VsD0->SetErrorOption(" "); 
  hProf_Phi0VsD0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_Phi0VsD0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_Phi0VsD0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  
  // -----------------------------------------------------------------
  // Track Matching delta Theta Vs D0
  // -----------------------------------------------------------------
  
  cCompTrackMatchingParamsVsD0->cd(4);
  h2TrkMatching_Theta0VsD0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Theta0_Vs_D00"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta0_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta0_Vs_D02",level);    
  h2ProcTrkMatching_Theta0VsD0 = (TH2F*)file->Get(name);
  h2TrkMatching_Theta0VsD0->SetTitle("#Delta#theta_{0} Vs d_{0}");
  h2TrkMatching_Theta0VsD0->GetXaxis()->SetTitle("d_{0} (mm)");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  // if (normalizePlots) h2TrkMatching_Theta0VsD0->Scale(1./(float)iEvents);
  h2TrkMatching_Theta0VsD0->SetLineColor(ColorForAll+2);
  h2TrkMatching_Theta0VsD0->DrawCopy();
  hProf_Theta0VsD0 = h2TrkMatching_Theta0VsD0->ProfileX(); 
  hProf_Theta0VsD0->SetErrorOption(" ");
  hProf_Theta0VsD0->SetMarkerColor(ColorForAll+2);
  hProf_Theta0VsD0->GetYaxis()->SetTitle("#Delta #theta_{0} (rad)");
  hProf_Theta0VsD0->SetMarkerStyle(21);
  hProf_Theta0VsD0->SetMarkerSize(0.4);
  hProf_Theta0VsD0->SetStats(kFALSE);
  hProf_Theta0VsD0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_Theta0VsD0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_Theta0VsD0->SetLineColor(ColorForUsed);
  hProf_Theta0VsD0 = h2ProcTrkMatching_Theta0VsD0->ProfileX();
  hProf_Theta0VsD0->SetErrorOption(" "); 
  hProf_Theta0VsD0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_Theta0VsD0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_Theta0VsD0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  // -----------------------------------------------------------------
  // Track Matching delta  Eta Vs D0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsD0->cd(5);
  h2TrkMatching_EtaVsD0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Eta_Vs_D00"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_Vs_D02",level);    
  h2ProcTrkMatching_EtaVsD0 = (TH2F*)file->Get(name);
  h2TrkMatching_EtaVsD0->SetTitle("#Delta#eta Vs d_{0}");
  h2TrkMatching_EtaVsD0->GetXaxis()->SetTitle("d_{0} (mm)");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //  if (normalizePlots) h2TrkMatching_EtaVsD0->Scale(1./(float)iEvents);
  h2TrkMatching_EtaVsD0->SetLineColor(ColorForAll+2);
  h2TrkMatching_EtaVsD0->DrawCopy();
  
  hProf_EtaVsD0 = h2TrkMatching_EtaVsD0->ProfileX(); 
  hProf_EtaVsD0->SetErrorOption(" ");
  hProf_EtaVsD0->SetMarkerColor(ColorForAll+2);
  hProf_EtaVsD0->GetYaxis()->SetTitle("#Delta #eta ");
  hProf_EtaVsD0->SetMarkerStyle(21);
  hProf_EtaVsD0->SetMarkerSize(0.4);
  hProf_EtaVsD0->SetStats(kFALSE);
  hProf_EtaVsD0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_EtaVsD0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_EtaVsD0->SetLineColor(ColorForUsed);
  hProf_EtaVsD0 = h2ProcTrkMatching_EtaVsD0->ProfileX(); 
  hProf_EtaVsD0->SetErrorOption(" ");
  hProf_EtaVsD0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_EtaVsD0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_EtaVsD0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  
  // -----------------------------------------------------------------
  // Track Matching delta QoverPt Vs D0
  // -----------------------------------------------------------------
  
  cCompTrackMatchingParamsVsD0->cd(6);
  h2TrkMatching_QoverPtVsD0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_QoverPt_Vs_D00"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_QoverPt_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_QoverPt_Vs_D02",level);    
  h2ProcTrkMatching_QoverPtVsD0 = (TH2F*)file->Get(name);
  h2TrkMatching_QoverPtVsD0->SetTitle("#Delta(q/p_{T}) Vs d_{0}");
  h2TrkMatching_QoverPtVsD0->GetXaxis()->SetTitle("d_{0} (mm)");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_QoverPtVsD0->Scale(1./(float)iEvents);
  h2TrkMatching_QoverPtVsD0->SetLineColor(ColorForAll+2);
  h2TrkMatching_QoverPtVsD0->DrawCopy();
  
  hProf_QoverPtVsD0 = h2TrkMatching_QoverPtVsD0->ProfileX();
  hProf_QoverPtVsD0->SetErrorOption(" "); 
  hProf_QoverPtVsD0->SetMarkerColor(ColorForAll+2);
  hProf_QoverPtVsD0->GetYaxis()->SetTitle("#Delta q/p_{T} (GeV^{-1})");
  hProf_QoverPtVsD0->SetMarkerStyle(21);
  hProf_QoverPtVsD0->SetMarkerSize(0.4);
  hProf_QoverPtVsD0->SetStats(kFALSE);
  hProf_QoverPtVsD0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  // if (normalizePlots) h2ProcTrkMatching_QoverPtVsD0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_QoverPtVsD0->SetLineColor(ColorForUsed);
  hProf_QoverPtVsD0 = h2ProcTrkMatching_QoverPtVsD0->ProfileX(); 
  hProf_QoverPtVsD0->SetErrorOption(" "); 
  hProf_QoverPtVsD0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_QoverPtVsD0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_QoverPtVsD0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  



}


