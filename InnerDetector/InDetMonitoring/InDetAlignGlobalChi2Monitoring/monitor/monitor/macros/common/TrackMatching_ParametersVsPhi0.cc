/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void TrackMatching_ParametersVsPhi0() {

  if (!TrackMatching) return;

  TH2F *h2TrkMatching_d0VsPhi0;
  TH2F *h2ProcTrkMatching_d0VsPhi0;
  TH2F *h2TrkMatching_z0VsPhi0;
  TH2F *h2ProcTrkMatching_z0VsPhi0;
  TH2F *h2TrkMatching_phi0VsPhi0;
  TH2F *h2ProcTrkMatching_phi0VsPhi0;
  TH2F *h2TrkMatching_theta0VsPhi0;
  TH2F *h2ProcTrkMatching_theta0VsPhi0;
  TH2F *h2TrkMatching_etaVsPhi0;
  TH2F *h2ProcTrkMatching_etaVsPhi0;
  TH2F *h2TrkMatching_qoverpTVsPhi0;
  TH2F *h2ProcTrkMatching_qoverpTVsPhi0;
 
  TProfile *HProf_d0VsPhi0;
  TProfile *HProf_z0VsPhi0;
  TProfile *HProf_Phi0VsPhi0;
  TProfile *HProf_Theta0VsPhi0;
  TProfile *HProf_EtaVsPhi0;
  TProfile *HProf_QoverPtVsPhi0;

  cout << " Track Parameters Matching Vs Phi0..." << endl;

  bool ShowFit = showFits;
  
  cCompTrackMatchingParamsVsPhi0 = new TCanvas("CompTrkMatchingParamsVsPhi0","Track Matching (up-low) Parameter Comparation Vs Phi0",
					    CanvasSizeX6[0],CanvasSizeX6[1]);
  cCompTrackMatchingParamsVsPhi0->Divide(3,2);
  
  // -----------------------------------------------------------------
  // Track Matching delta d0 Vs Phi0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsPhi0->cd(1);
  h2TrkMatching_d0VsPhi0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_D0_Vs_Phi00"); 
  // sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_Phi02",level);    
  h2ProcTrkMatching_d0VsPhi0 = (TH2F*)file->Get(name);
 
  h2TrkMatching_d0VsPhi0->SetTitle("#Deltad_{0} Vs #Phi_{0}");
  h2TrkMatching_d0VsPhi0->GetXaxis()->SetTitle("#Phi_{0}(rad)");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_d0VsPhi0->Scale(1./(float)iEvents);
  h2TrkMatching_d0VsPhi0->SetLineColor(ColorForAll+2);
  h2TrkMatching_d0VsPhi0->DrawCopy();
  hProf_d0VsPhi0 = h2TrkMatching_d0VsPhi0->ProfileX(); 
  hProf_d0VsPhi0->SetErrorOption(" ");
  hProf_d0VsPhi0->SetMarkerColor(ColorForAll+2);
  hProf_d0VsPhi0->GetYaxis()->SetTitle("#Delta d_{0}(mm)");
  hProf_d0VsPhi0->SetMarkerStyle(21);
  hProf_d0VsPhi0->SetMarkerSize(0.4);
  // hProf_d0VsPhi0->SetStats(kFALSE);
  hProf_d0VsPhi0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_d0VsPhi0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_d0VsPhi0->SetLineColor(ColorForUsed);
  hProf_d0VsPhi0 = h2ProcTrkMatching_d0VsPhi0->ProfileX();
  hProf_d0VsPhi0->SetErrorOption(" "); 
  hProf_d0VsPhi0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_d0VsPhi0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_d0VsPhi0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
// -----------------------------------------------------------------
  // Track Matching delta z0 Vs Phi0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsPhi0->cd(2);
  h2TrkMatching_z0VsPhi0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Z0_Vs_Phi00"); 
  // sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_Phi02",level);    
  h2ProcTrkMatching_z0VsPhi0 = (TH2F*)file->Get(name);
 
  h2TrkMatching_z0VsPhi0->SetTitle("#Deltaz_{0} Vs #Phi_{0}");
  h2TrkMatching_z0VsPhi0->GetXaxis()->SetTitle("#Phi_{0} (rad)");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_d0VsPhi0->Scale(1./(float)iEvents);
  h2TrkMatching_z0VsPhi0->SetLineColor(ColorForAll+2);
  h2TrkMatching_z0VsPhi0->DrawCopy();
  hProf_z0VsPhi0 = h2TrkMatching_z0VsPhi0->ProfileX();
  hProf_z0VsPhi0->SetErrorOption(" "); 
  hProf_z0VsPhi0->SetMarkerColor(ColorForAll+2);
  hProf_z0VsPhi0->GetYaxis()->SetTitle("#Delta z_{0}(mm)");
  hProf_z0VsPhi0->SetMarkerStyle(21);
  hProf_z0VsPhi0->SetMarkerSize(0.4);
  // hProf_d0VsPhi0->SetStats(kFALSE);
  hProf_z0VsPhi0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_d0VsPhi0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_z0VsPhi0->SetLineColor(ColorForUsed);
  hProf_z0VsPhi0 = h2ProcTrkMatching_d0VsPhi0->ProfileX();
  hProf_z0VsPhi0->SetErrorOption(" ");  
  hProf_z0VsPhi0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_z0VsPhi0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_z0VsPhi0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  

  // -----------------------------------------------------------------
  // Track Matching delta Phi0 Vs Phi0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsPhi0->cd(3);
  h2TrkMatching_Phi0VsPhi0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Phi0_Vs_Phi00"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_Vs_Phi02",level);    
  h2ProcTrkMatching_Phi0VsPhi0 = (TH2F*)file->Get(name);
  
  h2TrkMatching_Phi0VsPhi0->SetTitle("#Delta#phi_{0} Vs #Phi_{0}");
  h2TrkMatching_Phi0VsPhi0->GetXaxis()->SetTitle("#Phi_{0} (rad)");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  // if (normalizePlots) h2TrkMatching_Phi0VsPhi0->Scale(1./(float)iEvents);
  h2TrkMatching_Phi0VsPhi0->SetLineColor(ColorForAll+2);
  h2TrkMatching_Phi0VsPhi0->DrawCopy();
  
  hProf_Phi0VsPhi0 = h2TrkMatching_Phi0VsPhi0->ProfileX(); 
  hProf_Phi0VsPhi0->SetErrorOption(" "); 
  hProf_Phi0VsPhi0->SetMarkerColor(ColorForAll+2);
  hProf_Phi0VsPhi0->GetYaxis()->SetTitle("#Delta #phi_{0} (rad)");
  hProf_Phi0VsPhi0->SetMarkerStyle(21);
  hProf_Phi0VsPhi0->SetMarkerSize(0.4);
  //  hProf_Phi0VsPhi0->SetStats(kFALSE);
  hProf_Phi0VsPhi0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_Phi0VsPhi0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_Phi0VsPhi0->SetLineColor(ColorForUsed);
  hProf_Phi0VsPhi0 = h2ProcTrkMatching_Phi0VsPhi0->ProfileX(); 
  hProf_Phi0VsPhi0->SetErrorOption(" "); 
  hProf_Phi0VsPhi0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_Phi0VsPhi0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_Phi0VsPhi0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  
  // -----------------------------------------------------------------
  // Track Matching delta Theta Vs Phi0
  // -----------------------------------------------------------------
  
  cCompTrackMatchingParamsVsPhi0->cd(4);
  h2TrkMatching_Theta0VsPhi0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Theta0_Vs_Phi00"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta0_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta0_Vs_Phi02",level);    
  h2ProcTrkMatching_Theta0VsPhi0 = (TH2F*)file->Get(name);
  h2TrkMatching_Theta0VsPhi0->SetTitle("#Delta#theta_{0} Vs #Phi_{0}");
  h2TrkMatching_Theta0VsPhi0->GetXaxis()->SetTitle("#Phi_{0} (rad)");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  // if (normalizePlots) h2TrkMatching_Theta0VsPhi0->Scale(1./(float)iEvents);
  h2TrkMatching_Theta0VsPhi0->SetLineColor(ColorForAll+2);
  h2TrkMatching_Theta0VsPhi0->DrawCopy();
  hProf_Theta0VsPhi0 = h2TrkMatching_Theta0VsPhi0->ProfileX(); 
  hProf_Theta0VsPhi0->GetYaxis()->SetRangeUser(-0.020,0.020);
  hProf_Theta0VsPhi0->SetErrorOption(" "); 
  hProf_Theta0VsPhi0->SetMarkerColor(ColorForAll+2);
  hProf_Theta0VsPhi0->GetYaxis()->SetTitle("#Delta #theta_{0} (rad)");
  hProf_Theta0VsPhi0->SetMarkerStyle(21);
  hProf_Theta0VsPhi0->SetMarkerSize(0.4);
  //  hProf_Theta0VsPhi0->SetStats(kFALSE);
  hProf_Theta0VsPhi0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_Theta0VsPhi0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_Theta0VsPhi0->SetLineColor(ColorForUsed);
  hProf_Theta0VsPhi0 = h2ProcTrkMatching_Theta0VsPhi0->ProfileX();
  hProf_Theta0VsPhi0->GetYaxis()->SetRangeUser(-0.020,0.020);
  hProf_Theta0VsPhi0->SetErrorOption(" ");  
  hProf_Theta0VsPhi0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_Theta0VsPhi0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_Theta0VsPhi0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  // -----------------------------------------------------------------
  // Track Matching delta  Eta Vs Phi0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsPhi0->cd(5);
  h2TrkMatching_EtaVsPhi0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Eta_Vs_Phi00"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_Vs_Phi02",level);    
  h2ProcTrkMatching_EtaVsPhi0 = (TH2F*)file->Get(name);
  h2TrkMatching_EtaVsPhi0->SetTitle("#Delta#eta Vs #Phi_{0}");
  h2TrkMatching_EtaVsPhi0->GetXaxis()->SetTitle("#Phi_{0} (mm)");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //  if (normalizePlots) h2TrkMatching_EtaVsPhi0->Scale(1./(float)iEvents);
  h2TrkMatching_EtaVsPhi0->SetLineColor(ColorForAll+2);
  h2TrkMatching_EtaVsPhi0->DrawCopy();
  
  hProf_EtaVsPhi0 = h2TrkMatching_EtaVsPhi0->ProfileX();
  hProf_Theta0VsPhi0->GetYaxis()->SetRangeUser(-0.020,0.020);
  hProf_EtaVsPhi0->SetErrorOption(" ");  
  hProf_EtaVsPhi0->SetMarkerColor(ColorForAll+2);
  hProf_EtaVsPhi0->GetYaxis()->SetTitle("#Delta #eta ");
  hProf_EtaVsPhi0->SetMarkerStyle(21);
  hProf_EtaVsPhi0->SetMarkerSize(0.4);
  //  hProf_EtaVsPhi0->SetStats(kFALSE);
  hProf_EtaVsPhi0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_EtaVsPhi0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_EtaVsPhi0->SetLineColor(ColorForUsed);
  hProf_EtaVsPhi0 = h2ProcTrkMatching_EtaVsPhi0->ProfileX();
  hProf_Theta0VsPhi0->GetYaxis()->SetRangeUser(-0.020,0.020);
  hProf_EtaVsPhi0->SetErrorOption(" ");   
  hProf_EtaVsPhi0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_EtaVsPhi0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_EtaVsPhi0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  
  // -----------------------------------------------------------------
  // Track Matching delta QoverPt Vs Phi0
  // -----------------------------------------------------------------
  
  cCompTrackMatchingParamsVsPhi0->cd(6);
  h2TrkMatching_QoverPtVsPhi0 = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_QoverPt_Vs_Phi00"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_QoverPt_Vs_D0%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_QoverPt_Vs_Phi02",level);    
  h2ProcTrkMatching_QoverPtVsPhi0 = (TH2F*)file->Get(name);
  h2TrkMatching_QoverPtVsPhi0->SetTitle("#Delta(q/p_{T}) Vs #Phi_{0}");
  h2TrkMatching_QoverPtVsPhi0->GetXaxis()->SetTitle("#Phi_{0} (rad)");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_QoverPtVsPhi0->Scale(1./(float)iEvents);
  h2TrkMatching_QoverPtVsPhi0->SetLineColor(ColorForAll+2);
  h2TrkMatching_QoverPtVsPhi0->DrawCopy();
  
  hProf_QoverPtVsPhi0 = h2TrkMatching_QoverPtVsPhi0->ProfileX(); 
  hProf_QoverPtVsPhi0->SetErrorOption(" ");  
  hProf_QoverPtVsPhi0->SetMarkerColor(ColorForAll+2);
  hProf_QoverPtVsPhi0->GetYaxis()->SetTitle("#Delta(q/p_{T}) (GeV^{-1})");
  hProf_QoverPtVsPhi0->SetMarkerStyle(21);
  hProf_QoverPtVsPhi0->SetMarkerSize(0.4);
  //  hProf_QoverPtVsPhi0->SetStats(kFALSE);
  hProf_QoverPtVsPhi0->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  // if (normalizePlots) h2ProcTrkMatching_QoverPtVsPhi0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_QoverPtVsPhi0->SetLineColor(ColorForUsed);
  hProf_QoverPtVsPhi0 = h2ProcTrkMatching_QoverPtVsPhi0->ProfileX();
  hProf_QoverPtVsPhi0->SetErrorOption(" ");   
  hProf_QoverPtVsPhi0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_QoverPtVsPhi0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_QoverPtVsPhi0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  



}


