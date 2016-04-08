/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void TrackMatching_ParametersVsZ() {

  if (!TrackMatching) return;

  TH2F *h2TrkMatching_d0VsZ;
  TH2F *h2ProcTrkMatching_d0VsZ;
  TH2F *h2TrkMatching_phi0VsZ;
  TH2F *h2ProcTrkMatching_phi0VsZ;
  TH2F *h2TrkMatching_theta0VsZ;
  TH2F *h2ProcTrkMatching_theta0VsZ;
  TH2F *h2TrkMatching_etaVsZ;
  TH2F *h2ProcTrkMatching_etaVsZ;
  TH2F *h2TrkMatching_qoverpTVsZ;
  TH2F *h2ProcTrkMatching_qoverpTVsZ;
 
  TProfile *HProf_d0VsZ;
  TProfile *HProf_Phi0VsZ;
  TProfile *HProf_Theta0VsZ;
  TProfile *HProf_EtaVsZ;
  TProfile *HProf_QoverPtVsZ;

  cout << " Track Parameters Matching Vs Z..." << endl;

  bool ShowFit = showFits;
  
  cCompTrackMatchingParamsVsZ = new TCanvas("CompTrkMatchingParamsVsZ","Track Matching (up-low) Parameter Comparation Vs Z",
					    CanvasSizeX6[0],CanvasSizeX6[1]);
  cCompTrackMatchingParamsVsZ->Divide(3,2);
  
  // -----------------------------------------------------------------
  // Track Matching delta d0 Vs z0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsZ->cd(1);
  h2TrkMatching_d0VsZ = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_D0_Vs_Z0"); 
  // sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_Z%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_Z2",level);    
  h2ProcTrkMatching_d0VsZ = (TH2F*)file->Get(name);
 
  h2TrkMatching_d0VsZ->SetTitle("#Deltad_{0} Vs z");
  h2TrkMatching_d0VsZ->GetXaxis()->SetTitle("Z_{0} (mm)");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_d0VsZ->Scale(1./(float)iEvents);
  h2TrkMatching_d0VsZ->SetLineColor(ColorForAll+2);
  h2TrkMatching_d0VsZ->DrawCopy();
  hProf_d0VsZ = h2TrkMatching_d0VsZ->ProfileX();
  hProf_d0VsZ->SetErrorOption(" ");
  hProf_d0VsZ->SetMarkerColor(ColorForAll+2);
  hProf_d0VsZ->GetYaxis()->SetTitle("#Delta d_{0}(mm)");
  // hProf_d0VsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range
  hProf_d0VsZ->SetMarkerStyle(21);
  hProf_d0VsZ->SetMarkerSize(0.4);
  hProf_d0VsZ->SetStats(kFALSE);
  hProf_d0VsZ->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_d0VsZ->Scale(1./(float)iEvents);
  h2ProcTrkMatching_d0VsZ->SetLineColor(ColorForUsed);
  hProf_d0VsZ = h2ProcTrkMatching_d0VsZ->ProfileX();
  // hProf_d0VsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range
  hProf_d0VsZ->SetErrorOption(" ");
  hProf_d0VsZ->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_d0VsZ,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_d0VsZ,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  

  // -----------------------------------------------------------------
  // Track Matching delta z0 Vs z0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsZ->cd(2);
  h2TrkMatching_z0VsZ = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Z0_Vs_Z0"); 
  // sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_Vs_Z%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Z0_Vs_Z2",level);    
  h2ProcTrkMatching_z0VsZ = (TH2F*)file->Get(name);
 
  h2TrkMatching_z0VsZ->SetTitle("#Delta z_{0} Vs z");
  h2TrkMatching_z0VsZ->GetXaxis()->SetTitle("Z_{0} (mm)");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_d0VsZ->Scale(1./(float)iEvents);
  h2TrkMatching_z0VsZ->SetLineColor(ColorForAll+2);
  h2TrkMatching_z0VsZ->DrawCopy();
  hProf_z0VsZ = h2TrkMatching_z0VsZ->ProfileX(); 
  // hProf_z0VsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range
  hProf_z0VsZ->SetErrorOption(" ");
  hProf_z0VsZ->SetMarkerColor(ColorForAll+2);
  hProf_z0VsZ->GetYaxis()->SetTitle("#Delta z_{0}(mm)");
  hProf_z0VsZ->SetMarkerStyle(21);
  hProf_z0VsZ->SetMarkerSize(0.4);
  hProf_z0VsZ->SetStats(kFALSE);
  hProf_z0VsZ->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_d0VsZ->Scale(1./(float)iEvents);
  h2ProcTrkMatching_z0VsZ->SetLineColor(ColorForUsed);
  hProf_z0VsZ = h2ProcTrkMatching_z0VsZ->ProfileX(); 
  // hProf_z0VsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range
  hProf_z0VsZ->SetErrorOption(" ");
  hProf_z0VsZ->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_z0VsZ,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_z0VsZ,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  

  // -----------------------------------------------------------------
  // Track Matching delta Phi0 Vs z0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsZ->cd(3);
  h2TrkMatching_Phi0VsZ = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Phi0_Vs_Z0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_Vs_Z%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_Vs_Z2",level);    
  h2ProcTrkMatching_Phi0VsZ = (TH2F*)file->Get(name);
  
  h2TrkMatching_Phi0VsZ->SetTitle("#Delta #phi_{0} Vs z");
  h2TrkMatching_Phi0VsZ->GetXaxis()->SetTitle("Z_{0} (mm)");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  // if (normalizePlots) h2TrkMatching_Phi0VsZ->Scale(1./(float)iEvents);
  h2TrkMatching_Phi0VsZ->SetLineColor(ColorForAll+2);
  h2TrkMatching_Phi0VsZ->DrawCopy();
  
  hProf_Phi0VsZ = h2TrkMatching_Phi0VsZ->ProfileX();
  //  hProf_Phi0VsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range
  hProf_Phi0VsZ->SetErrorOption(" ");
  hProf_Phi0VsZ->SetMarkerColor(ColorForAll+2);
  hProf_Phi0VsZ->GetYaxis()->SetTitle("#Delta #phi_{0} (rad)");
  hProf_Phi0VsZ->SetMarkerStyle(21);
  hProf_Phi0VsZ->SetMarkerSize(0.4);
  hProf_Phi0VsZ->SetStats(kFALSE);
  hProf_Phi0VsZ->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_Phi0VsZ->Scale(1./(float)iEvents);
  h2ProcTrkMatching_Phi0VsZ->SetLineColor(ColorForUsed);
  hProf_Phi0VsZ = h2ProcTrkMatching_Phi0VsZ->ProfileX();
  // hProf_Phi0VsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range 
  hProf_Phi0VsZ->SetErrorOption(" ");
  hProf_Phi0VsZ->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_Phi0VsZ,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_Phi0VsZ,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  
  // -----------------------------------------------------------------
  // Track Matching delta Theta0 Vs z0
  // -----------------------------------------------------------------
  
  cCompTrackMatchingParamsVsZ->cd(4);
  h2TrkMatching_Theta0VsZ = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Theta0_Vs_Z0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta0_Vs_Z%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta0_Vs_Z2",level);    
  h2ProcTrkMatching_Theta0VsZ = (TH2F*)file->Get(name);
  h2TrkMatching_Theta0VsZ->SetTitle("#Delta#theta_{0} Vs z");
  h2TrkMatching_Theta0VsZ->GetXaxis()->SetTitle("Z_{0} (mm)");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  // if (normalizePlots) h2TrkMatching_Theta0VsZ->Scale(1./(float)iEvents);
  h2TrkMatching_Theta0VsZ->SetLineColor(ColorForAll+2);
  h2TrkMatching_Theta0VsZ->DrawCopy();
  hProf_Theta0VsZ = h2TrkMatching_Theta0VsZ->ProfileX();
  // hProf_Theta0VsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range 
  hProf_Theta0VsZ->SetErrorOption(" ");
  hProf_Theta0VsZ->SetMarkerColor(ColorForAll+2);
  hProf_Theta0VsZ->GetYaxis()->SetTitle("#Delta #theta_{0} (rad)");
  hProf_Theta0VsZ->SetMarkerStyle(21);
  hProf_Theta0VsZ->SetMarkerSize(0.4);
  hProf_Theta0VsZ->SetStats(kFALSE);
  hProf_Theta0VsZ->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_Theta0VsZ->Scale(1./(float)iEvents);
  h2ProcTrkMatching_Theta0VsZ->SetLineColor(ColorForUsed);
  hProf_Theta0VsZ = h2ProcTrkMatching_Theta0VsZ->ProfileX();
  //hProf_Theta0VsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range  
  hProf_Theta0VsZ->SetErrorOption(" ");
  hProf_Theta0VsZ->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_Theta0VsZ,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_Theta0VsZ,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  // -----------------------------------------------------------------
  // Track Matching delta Eta Vs z0
  // -----------------------------------------------------------------

  cCompTrackMatchingParamsVsZ->cd(5);
  h2TrkMatching_EtaVsZ = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_Eta_Vs_Z0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_Vs_Z%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_Vs_Z2",level);    
  h2ProcTrkMatching_EtaVsZ = (TH2F*)file->Get(name);
  h2TrkMatching_EtaVsZ->SetTitle("#Delta#eta  Vs z");
  h2TrkMatching_EtaVsZ->GetXaxis()->SetTitle("Z_{0} (mm)");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //  if (normalizePlots) h2TrkMatching_EtaVsZ->Scale(1./(float)iEvents);
  h2TrkMatching_EtaVsZ->SetLineColor(ColorForAll+2);
  h2TrkMatching_EtaVsZ->DrawCopy();
  
  hProf_EtaVsZ = h2TrkMatching_EtaVsZ->ProfileX();
  // hProf_EtaVsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range 
  hProf_EtaVsZ->SetErrorOption(" ");
  hProf_EtaVsZ->SetMarkerColor(ColorForAll+2);
  hProf_EtaVsZ->GetYaxis()->SetTitle("#Delta #eta ");
  hProf_EtaVsZ->SetMarkerStyle(21);
  hProf_EtaVsZ->SetMarkerSize(0.4);
  hProf_EtaVsZ->SetStats(kFALSE);
  hProf_EtaVsZ->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  //if (normalizePlots) h2ProcTrkMatching_EtaVsZ->Scale(1./(float)iEvents);
  h2ProcTrkMatching_EtaVsZ->SetLineColor(ColorForUsed);
  hProf_EtaVsZ = h2ProcTrkMatching_EtaVsZ->ProfileX();
  //  hProf_EtaVsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range 
  hProf_EtaVsZ->SetErrorOption(" "); 
  hProf_EtaVsZ->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_EtaVsZ,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_EtaVsZ,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  
  // -----------------------------------------------------------------
  // Track Matching delta QoverPt Vs z0
  // -----------------------------------------------------------------
  
  cCompTrackMatchingParamsVsZ->cd(6);
  h2TrkMatching_QoverPtVsZ = (TH2F*)file->Get("trkana/TrkMatching/hTrkMatching_QoverPt_Vs_Z0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_QoverPt_Vs_Z%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_QoverPt_Vs_Z2",level);    
  h2ProcTrkMatching_QoverPtVsZ = (TH2F*)file->Get(name);
  h2TrkMatching_QoverPtVsZ->SetTitle("#Delta q/P_{T} Vs z");
  h2TrkMatching_QoverPtVsZ->GetXaxis()->SetTitle("Z_{0} (mm)");
  
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  //if (normalizePlots) h2TrkMatching_QoverPtVsZ->Scale(1./(float)iEvents);
  h2TrkMatching_QoverPtVsZ->SetLineColor(ColorForAll+2);
  h2TrkMatching_QoverPtVsZ->DrawCopy();
  
  hProf_QoverPtVsZ = h2TrkMatching_QoverPtVsZ->ProfileX(); 
  //  hProf_QoverPtVsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range 
  hProf_QoverPtVsZ->SetErrorOption(" ");
  hProf_QoverPtVsZ->SetMarkerColor(ColorForAll+2);
  hProf_QoverPtVsZ->GetYaxis()->SetTitle("#Delta q/p_{T} (GeV^{-1})");
  hProf_QoverPtVsZ->SetMarkerStyle(21);
  hProf_QoverPtVsZ->SetMarkerSize(0.4);
  hProf_QoverPtVsZ->SetStats(kFALSE);
  hProf_QoverPtVsZ->DrawCopy();
  setTitleSize(0.4);
  setTitleFont(font);

  // if (normalizePlots) h2ProcTrkMatching_QoverPtVsZ->Scale(1./(float)iEvents);
  h2ProcTrkMatching_QoverPtVsZ->SetLineColor(ColorForUsed);
  hProf_QoverPtVsZ = h2ProcTrkMatching_QoverPtVsZ->ProfileX();
  // hProf_QoverPtVsZ->GetXaxis()->SetRangeUser(-300,300);//Attilio Range  
  hProf_QoverPtVsZ->SetErrorOption(" ");
  hProf_QoverPtVsZ->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_QoverPtVsZ,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_QoverPtVsZ,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  



}


