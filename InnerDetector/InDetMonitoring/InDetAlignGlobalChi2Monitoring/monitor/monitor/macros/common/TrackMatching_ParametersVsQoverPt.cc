/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void TrackMatching_ParametersVsQoverPt() {

  if (!TrackMatching) return;

  TH2F *h2TrkMatching_d0QoverPt;
  TH2F *h2ProcTrkMatching_d0QoverPt;
  TH2F *h2TrkMatching_z0QoverPt;
  TH2F *h2ProcTrkMatching_z0QoverPt;
  TH2F *h2TrkMatching_phi0QoverPt;
  TH2F *h2ProcTrkMatching_phi0QoverPt;
  TH2F *h2TrkMatching_theta0QoverPt;
  TH2F *h2ProcTrkMatching_theta0QoverPt;
  TH2F *h2TrkMatching_etaQoverPt;
  TH2F *h2ProcTrkMatching_etaQoverPt;
  TH2F *h2TrkMatching_qoverpTQoverPt;
  TH2F *h2ProcTrkMatching_qoverpTQoverPt;
 
  TProfile *HProf_d0QoverPt;
  TProfile *HProf_z0QoverPt;
  TProfile *HProf_Phi0QoverPt;
  TProfile *HProf_Theta0QoverPt;
  TProfile *HProf_EtaQoverPt;
  TProfile *HProf_QoverPtQoverPt;

  cout << " Track Parameters Matching Vs QoverPt..." << endl;

  bool ShowFit = showFits;
  
  cCompTrackMatchingParamsVsQoverPt = new TCanvas("CompTrkMatchingParamsQoverPt","Track Matching (up-low) Parameter Comparation Vs QoverPt",
					    CanvasSizeX6[0],CanvasSizeX6[1]);
  cCompTrackMatchingParamsVsQoverPt->Divide(3,2);
  
  // -----------------------------------------------------------------
  // Track Matching delta d0 Vs QoverPt
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsQoverPt->cd(1);
  h2TrkMatching_d0QoverPt = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_D0_Vs_QoverPt0"); 
  // sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_QoverPt%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_QoverPt2",level);    
  h2ProcTrkMatching_d0QoverPt = (TH2F*)file->Get(name);
 
  h2TrkMatching_d0QoverPt->SetTitle("#Deltad_{0} Vs q/p_{T}");
  h2TrkMatching_d0QoverPt->GetXaxis()->SetTitle("q/p_{T} (GeV^{-1})");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_d0QoverPt->Scale(1./(float)iEvents);
  h2TrkMatching_d0QoverPt->SetLineColor(ColorForAll+2);
  h2TrkMatching_d0QoverPt->DrawCopy();  
  hProf_d0QoverPt = h2TrkMatching_d0QoverPt->ProfileX(); 
  hProf_d0QoverPt->SetErrorOption(" ");    
  hProf_d0QoverPt->SetMarkerColor(ColorForAll+2);
  hProf_d0QoverPt->GetYaxis()->SetTitle("#Delta d_{0}(mm)");
  hProf_d0QoverPt->SetMarkerStyle(21);
  hProf_d0QoverPt->SetMarkerSize(0.4);
  hProf_d0QoverPt->SetStats(kFALSE);
  hProf_d0QoverPt->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_d0QoverPt->Scale(1./(float)iEvents);
  h2ProcTrkMatching_d0QoverPt->SetLineColor(ColorForUsed);
  hProf_d0QoverPt = h2ProcTrkMatching_d0QoverPt->ProfileX();
  hProf_d0QoverPt->SetErrorOption(" ");     
  hProf_d0QoverPt->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_d0QoverPt,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_d0QoverPt,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
// -----------------------------------------------------------------
  // Track Matching delta z0 Vs QoverPt
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsQoverPt->cd(2);
  h2TrkMatching_z0QoverPt = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Z0_Vs_QoverPt0"); 
  // sprintf(name,"trkana/TrkMatching/hTrkMatching_QoverPt_Vs_QoverPt%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Z0_Vs_QoverPt2",level);    
  h2ProcTrkMatching_z0QoverPt = (TH2F*)file->Get(name);
 
  h2TrkMatching_z0QoverPt->SetTitle("#Deltaz_{0} Vs q/p_{T}");
  h2TrkMatching_z0QoverPt->GetXaxis()->SetTitle("q/p_{T} (GeV^{-1})");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_d0QoverPt->Scale(1./(float)iEvents);
  h2TrkMatching_z0QoverPt->SetLineColor(ColorForAll+2);
  h2TrkMatching_z0QoverPt->DrawCopy();
  hProf_z0QoverPt = h2TrkMatching_z0QoverPt->ProfileX();
  hProf_z0QoverPt->SetErrorOption(" ");     
  hProf_z0QoverPt->SetMarkerColor(ColorForAll+2);
  hProf_z0QoverPt->GetYaxis()->SetTitle("#Delta d_{0}(mm)");
  hProf_z0QoverPt->SetMarkerStyle(21);
  hProf_z0QoverPt->SetMarkerSize(0.4);
  hProf_z0QoverPt->SetStats(kFALSE);
  hProf_z0QoverPt->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_d0QoverPt->Scale(1./(float)iEvents);
  h2ProcTrkMatching_z0QoverPt->SetLineColor(ColorForUsed);
  hProf_z0QoverPt = h2ProcTrkMatching_d0QoverPt->ProfileX(); 
  hProf_z0QoverPt->SetErrorOption(" ");    
  hProf_z0QoverPt->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_z0QoverPt,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_z0QoverPt,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  

  // -----------------------------------------------------------------
  // Track Matching delta Phi0 Vs QoverPt
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsQoverPt->cd(3);
  h2TrkMatching_Phi0QoverPt = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Phi0_Vs_QoverPt0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_Vs_QoverPt%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_Vs_QoverPt2",level);    
  h2ProcTrkMatching_Phi0QoverPt = (TH2F*)file->Get(name);
  
  h2TrkMatching_Phi0QoverPt->SetTitle("#Delta #phi_{0} Vs q/p_{T}");
  h2TrkMatching_Phi0QoverPt->GetXaxis()->SetTitle("q/p_{T} (GeV^{-1})");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  // if (normalizePlots) h2TrkMatching_Phi0QoverPt->Scale(1./(float)iEvents);
  h2TrkMatching_Phi0QoverPt->SetLineColor(ColorForAll+2);
  h2TrkMatching_Phi0QoverPt->DrawCopy();
  
  hProf_Phi0QoverPt = h2TrkMatching_Phi0QoverPt->ProfileX(); 
  hProf_Phi0QoverPt->SetErrorOption(" ");    
  hProf_Phi0QoverPt->SetMarkerColor(ColorForAll+2);
  hProf_Phi0QoverPt->GetYaxis()->SetTitle("#Delta #phi_{0} (rad)");
  hProf_Phi0QoverPt->SetMarkerStyle(21);
  hProf_Phi0QoverPt->SetMarkerSize(0.4);
  hProf_Phi0QoverPt->SetStats(kFALSE);
  hProf_Phi0QoverPt->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_Phi0QoverPt->Scale(1./(float)iEvents);
  h2ProcTrkMatching_Phi0QoverPt->SetLineColor(ColorForUsed);
  hProf_Phi0QoverPt = h2ProcTrkMatching_Phi0QoverPt->ProfileX(); 
  hProf_Phi0QoverPt->SetErrorOption(" ");    
  hProf_Phi0QoverPt->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_Phi0QoverPt,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_Phi0QoverPt,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  
  // -----------------------------------------------------------------
  // Track Matching delta Theta Vs QoverPt
  // -----------------------------------------------------------------
  
  cCompTrackMatchingParamsVsQoverPt->cd(4);
  h2TrkMatching_Theta0QoverPt = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Theta0_Vs_QoverPt0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta0_Vs_QoverPt%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta0_Vs_QoverPt2",level);    
  h2ProcTrkMatching_Theta0QoverPt = (TH2F*)file->Get(name);
  h2TrkMatching_Theta0QoverPt->SetTitle("#Delta#theta_{0} Vs q/p_{T}");
  h2TrkMatching_Theta0QoverPt->GetXaxis()->SetTitle("q/p_{T} (GeV^{-1})");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  // if (normalizePlots) h2TrkMatching_Theta0QoverPt->Scale(1./(float)iEvents);
  h2TrkMatching_Theta0QoverPt->SetLineColor(ColorForAll+2);
  h2TrkMatching_Theta0QoverPt->DrawCopy();
  hProf_Theta0QoverPt = h2TrkMatching_Theta0QoverPt->ProfileX();
  hProf_Theta0QoverPt->SetErrorOption(" ");     
  hProf_Theta0QoverPt->SetMarkerColor(ColorForAll+2);
  hProf_Theta0QoverPt->GetYaxis()->SetTitle("#Delta #theta_{0} (rad)");
  hProf_Theta0QoverPt->SetMarkerStyle(21);
  hProf_Theta0QoverPt->SetMarkerSize(0.4);
  hProf_Theta0QoverPt->SetStats(kFALSE);
  hProf_Theta0QoverPt->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_Theta0QoverPt->Scale(1./(float)iEvents);
  h2ProcTrkMatching_Theta0QoverPt->SetLineColor(ColorForUsed);
  hProf_Theta0QoverPt = h2ProcTrkMatching_Theta0QoverPt->ProfileX(); 
  hProf_Theta0QoverPt->SetErrorOption(" ");     
  hProf_Theta0QoverPt->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_Theta0QoverPt,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_Theta0QoverPt,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  // -----------------------------------------------------------------
  // Track Matching delta  Eta Vs QoverPt
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsQoverPt->cd(5);
  h2TrkMatching_EtaQoverPt = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Eta_Vs_QoverPt0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_Vs_QoverPt%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_Vs_QoverPt2",level);    
  h2ProcTrkMatching_EtaQoverPt = (TH2F*)file->Get(name);
  h2TrkMatching_EtaQoverPt->SetTitle("#Delta#eta Vs q/p_{T}");
  h2TrkMatching_EtaQoverPt->GetXaxis()->SetTitle("q/p_{T} (GeV^{-1})");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //  if (normalizePlots) h2TrkMatching_EtaQoverPt->Scale(1./(float)iEvents);
  h2TrkMatching_EtaQoverPt->SetLineColor(ColorForAll+2);
  h2TrkMatching_EtaQoverPt->DrawCopy();
  
  hProf_EtaQoverPt = h2TrkMatching_EtaQoverPt->ProfileX();
  hProf_EtaQoverPt->SetErrorOption(" ");      
  hProf_EtaQoverPt->SetMarkerColor(ColorForAll+2);
  hProf_EtaQoverPt->GetYaxis()->SetTitle("#Delta #eta ");
  hProf_EtaQoverPt->SetMarkerStyle(21);
  hProf_EtaQoverPt->SetMarkerSize(0.4);
  hProf_EtaQoverPt->SetStats(kFALSE);
  hProf_EtaQoverPt->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_EtaQoverPt->Scale(1./(float)iEvents);
  h2ProcTrkMatching_EtaQoverPt->SetLineColor(ColorForUsed);
  hProf_EtaQoverPt = h2ProcTrkMatching_EtaQoverPt->ProfileX(); 
  hProf_EtaQoverPt->SetErrorOption(" ");      
  hProf_EtaQoverPt->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_EtaQoverPt,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_EtaQoverPt,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  
  // -----------------------------------------------------------------
  // Track Matching delta QoverPt Vs QoverPt
  // -----------------------------------------------------------------
  
  cCompTrackMatchingParamsVsQoverPt->cd(6);
  h2TrkMatching_QoverPtQoverPt = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_QoverPt_Vs_QoverPt0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_QoverPt_Vs_QoverPt%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_QoverPt_Vs_QoverPt2",level);    
  h2ProcTrkMatching_QoverPtQoverPt = (TH2F*)file->Get(name);
  h2TrkMatching_QoverPtQoverPt->SetTitle("#Delta q/p_{T} Vs q/p_{T}");
  h2TrkMatching_QoverPtQoverPt->GetXaxis()->SetTitle("q/p_{T} (GeV^{-1})");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_QoverPtQoverPt->Scale(1./(float)iEvents);
  h2TrkMatching_QoverPtQoverPt->SetLineColor(ColorForAll+2);
  h2TrkMatching_QoverPtQoverPt->DrawCopy();
  
  hProf_QoverPtQoverPt = h2TrkMatching_QoverPtQoverPt->ProfileX(); 
  hProf_QoverPtQoverPt->SetErrorOption(" ");      
  hProf_QoverPtQoverPt->SetMarkerColor(ColorForAll+2);
  hProf_QoverPtQoverPt->GetYaxis()->SetTitle("#Delta q/p_{T} (GeV_{-1})");
  hProf_QoverPtQoverPt->SetMarkerStyle(21);
  hProf_QoverPtQoverPt->SetMarkerSize(0.4);
  hProf_QoverPtQoverPt->SetStats(kFALSE);
  hProf_QoverPtQoverPt->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  // if (normalizePlots) h2ProcTrkMatching_QoverPtQoverPt->Scale(1./(float)iEvents);
  h2ProcTrkMatching_QoverPtQoverPt->SetLineColor(ColorForUsed);
  hProf_QoverPtQoverPt = h2ProcTrkMatching_QoverPtQoverPt->ProfileX();
  hProf_QoverPtQoverPt->SetErrorOption(" ");      
  hProf_QoverPtQoverPt->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_QoverPtQoverPt,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_QoverPtQoverPt,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  



}


