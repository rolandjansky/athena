/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void TrackMatching_Parameters() {

  if (!TrackMatching) return;

  TH1F *h2TrkMatching_d0;
  TH1F *h2ProcTrkMatching_d0;
  TH1F *h2TrkMatching_z0;
  TH1F *h2ProcTrkMatching_z0;
  TH1F *h2TrkMatching_phi0;
  TH1F *h2ProcTrkMatching_phi0;
  TH1F *h2TrkMatching_theta0;
  TH1F *h2ProcTrkMatching_theta0;
  TH1F *h2TrkMatching_eta;
  TH1F *h2ProcTrkMatching_eta;
  TH1F *h2TrkMatching_qoverpT;
  TH1F *h2ProcTrkMatching_qoverpT;
  TH1F *h2TrkMatching_pT;
  TH1F *h2ProcTrkMatching_pT;
  
  cout << " Track Parameters Matching Comparation..." << endl;

  bool ShowFit = showFits;
  
  cCompTrackMatchingParams = new TCanvas("CompTrkMatchingParams","Track Matching (up-low) Parameter Comparation",
			    CanvasSizeX6[0],CanvasSizeX6[1]);
  cCompTrackMatchingParams->Divide(3,2);
  
  // -----------------------------------------------------------------
  // Track Matching delta d0
  // -----------------------------------------------------------------

  cCompTrackMatchingParams->cd(1);
  h2TrkMatching_d0 = (TH1F*)file->Get("trkana/TrkMatching/hTrkMatching_D0_0"); 
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_D0_2");    
  h2ProcTrkMatching_d0 = (TH1F*)file->Get(name);
  
  h2TrkMatching_d0->SetTitle("Trk matching (up - low) comparation");
  h2TrkMatching_d0->GetXaxis()->SetTitle("#Deltad_{0} (mm)");
  h2TrkMatching_d0->GetYaxis()->SetTitle("Tracks");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  if (normalizePlots) h2TrkMatching_d0->Scale(1./(float)iEvents);
  doSpace(h2TrkMatching_d0);
  h2TrkMatching_d0->SetLineColor(ColorForAll+2);
  h2TrkMatching_d0->SetLineWidth(1);
  h2TrkMatching_d0->SetFillColor(ColorForAll);
  h2TrkMatching_d0->SetFillStyle(3001);
  h2TrkMatching_d0->DrawCopy();
  FitValues fitdd0_0 = DrawFitFunc(h2TrkMatching_d0,ColorForAll+2,ShowFit);
  
  h2ProcTrkMatching_d0->SetLineColor(ColorForUsed);
  h2ProcTrkMatching_d0->SetLineWidth(1);
  h2ProcTrkMatching_d0->SetFillColor(ColorForUsed-9);
  h2ProcTrkMatching_d0->SetFillStyle(3001);
  if (normalizePlots) h2ProcTrkMatching_d0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_d0->DrawCopy("SAME");
  FitValues fitdd0_2 = DrawFitFunc(h2ProcTrkMatching_d0,ColorForUsed+2,ShowFit);
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_d0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_d0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitdd0_0.FitMean,1000*fitdd0_0.FitSigma);
  leg1->AddEntry(h2TrkMatching_d0,name,"L");
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitdd0_2.FitMean,1000*fitdd0_2.FitSigma);
  leg1->AddEntry(h2ProcTrkMatching_d0,name,"L");
  
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();
  
  // -----------------------------------------------------------------
  // Track Matching delta z0
  // -----------------------------------------------------------------
  
  cCompTrackMatchingParams->cd(2);
  h2TrkMatching_z0 = (TH1F*)file->Get("trkana/TrkMatching/hTrkMatching_Z0_0");     
  h2TrkMatching_z0->SetTitle("Trk matching (up - low) comparation");
  h2TrkMatching_z0->GetXaxis()->SetTitle("#Deltaz_{0} (mm)");
  h2TrkMatching_z0->GetYaxis()->SetTitle("Tracks");
  if (normalizePlots) h2TrkMatching_z0->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  doSpace(h2TrkMatching_z0);
  h2TrkMatching_z0->SetLineColor(ColorForAll+2);
  h2TrkMatching_z0->SetLineWidth(1);
  h2TrkMatching_z0->SetFillColor(ColorForAll);
  h2TrkMatching_z0->SetFillStyle(3001);
  h2TrkMatching_z0->DrawCopy();
  FitValues fitdz0_0 = DrawFitFunc(h2TrkMatching_z0,ColorForAll+2,ShowFit);
  
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Z0_%d",level);  
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Z0_2");  
  h2ProcTrkMatching_z0 = (TH1F*)file->Get(name);
  h2ProcTrkMatching_z0->SetLineColor(ColorForUsed);
  h2ProcTrkMatching_z0->SetLineWidth(1);
  h2ProcTrkMatching_z0->SetFillColor(ColorForUsed-9);
  h2ProcTrkMatching_z0->SetFillStyle(3001);
  if (normalizePlots) h2ProcTrkMatching_z0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_z0->DrawCopy("SAME");
  FitValues fitdz0_2 = DrawFitFunc(h2ProcTrkMatching_z0,ColorForUsed+2,ShowFit);
  setTitleFont(font);
  ScalePlots();
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_z0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_z0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitdz0_0.FitMean,1000*fitdz0_0.FitSigma);
  leg1->AddEntry(h2TrkMatching_z0,name,"L");
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitdz0_2.FitMean,1000*fitdz0_2.FitSigma);
  leg1->AddEntry(h2ProcTrkMatching_z0,name,"L");
  
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();
  
  // -----------------------------------------------------------------
  //  Track Matching delta phi0
  // -----------------------------------------------------------------

  cCompTrackMatchingParams->cd(3);
  h2TrkMatching_phi0 = (TH1F*)file->Get("trkana/TrkMatching/hTrkMatching_Phi0_0");   
  h2TrkMatching_phi0->SetTitle("Trk matching (up - low) comparation");
  h2TrkMatching_phi0->GetXaxis()->SetTitle("#Delta#phi_{0} (mrad)");
  h2TrkMatching_phi0->GetYaxis()->SetTitle("Tracks");
  if (normalizePlots) h2TrkMatching_phi0->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  doSpace(h2TrkMatching_phi0);
  h2TrkMatching_phi0->SetLineColor(ColorForAll+2);
  h2TrkMatching_phi0->SetLineWidth(1);
  h2TrkMatching_phi0->SetFillColor(ColorForAll);
  h2TrkMatching_phi0->SetFillStyle(3001);
  h2TrkMatching_phi0->DrawCopy();
  FitValues fitdphi0_0 = DrawFitFunc(h2TrkMatching_phi0,ColorForAll+2,ShowFit);
  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_%d",level);  
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Phi0_2");  
  h2ProcTrkMatching_phi0 = (TH1F*)file->Get(name);
  h2ProcTrkMatching_phi0->SetLineColor(ColorForUsed);
  h2ProcTrkMatching_phi0->SetLineWidth(1);
  h2ProcTrkMatching_phi0->SetFillColor(ColorForUsed-9);
  h2ProcTrkMatching_phi0->SetFillStyle(3001);
  if (normalizePlots) h2ProcTrkMatching_phi0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_phi0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  FitValues fitdphi0_2 = DrawFitFunc(h2ProcTrkMatching_phi0,ColorForUsed+2,ShowFit);
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93); 
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_phi0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_phi0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
  TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  sprintf(name,"#mu: %2.1f mrad, #sigma: %2.1f mrad",fitdphi0_0.FitMean,fitdphi0_0.FitSigma);
  leg1->AddEntry(h2TrkMatching_phi0,name,"L");
  sprintf(name,"#mu: %2.1f mrad, #sigma: %2.1f mrad",fitdphi0_2.FitMean,fitdphi0_2.FitSigma);
  leg1->AddEntry(h2ProcTrkMatching_phi0,name,"L");
  
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();
  
  
  //// -----------------------------------------------------------------
  // Track Matching delta theta
  // -----------------------------------------------------------------

  cCompTrackMatchingParams->cd(4);
  h2TrkMatching_theta0 = (TH1F*)file->Get("trkana/TrkMatching/hTrkMatching_Theta_0");   
  h2TrkMatching_theta0->SetTitle("Trk matching (up - low) comparation");
  h2TrkMatching_theta0->GetXaxis()->SetTitle("#Delta#theta_{0} (rad)");
  h2TrkMatching_theta0->GetYaxis()->SetTitle("Tracks");
  if (normalizePlots) h2TrkMatching_theta0->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  doSpace(h2TrkMatching_theta0);
  h2TrkMatching_theta0->SetLineColor(ColorForAll+2);
  h2TrkMatching_theta0->SetLineWidth(1);
  h2TrkMatching_theta0->SetFillColor(ColorForAll);
  h2TrkMatching_theta0->SetFillStyle(3001);
  h2TrkMatching_theta0->DrawCopy();
  FitValues fitdtheta0_0 = DrawFitFunc(h2TrkMatching_theta0,ColorForAll+2,ShowFit);

  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta_%d",level);  
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Theta_2");  
  h2ProcTrkMatching_theta0 = (TH1F*)file->Get(name);
  h2ProcTrkMatching_theta0->SetLineColor(ColorForUsed);
  h2ProcTrkMatching_theta0->SetLineWidth(1);
  h2ProcTrkMatching_theta0->SetFillColor(ColorForUsed-9);
  h2ProcTrkMatching_theta0->SetFillStyle(3001);
  if (normalizePlots) h2ProcTrkMatching_theta0->Scale(1./(float)iEvents);
  h2ProcTrkMatching_theta0->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  FitValues fitdtheta0_2 = DrawFitFunc(h2ProcTrkMatching_theta0,ColorForUsed+2,ShowFit);

  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_theta0,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_theta0,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
  // cout << "Fit Thta mean" << fitdtheta0_0.FitMean <<endl;
  //cout << "Fit Theta Sigma" << fitdtheta0_0.FitSigma <<endl;

  TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  sprintf(name,"#mu: %2.0f mrad, #sigma: %2.0f mrad",1000*fitdtheta0_0.FitMean,1000*fitdtheta0_0.FitSigma);
  leg1->AddEntry(h2TrkMatching_theta0,name,"L");
  sprintf(name,"#mu: %2.0f mrad, #sigma: %2.0f mrad",1000*fitdtheta0_2.FitMean,1000*fitdtheta0_2.FitSigma);
  leg1->AddEntry(h2ProcTrkMatching_theta0,name,"L");
  
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();




  // -----------------------------------------------------------------
  // Track Matching delta eta
  // -----------------------------------------------------------------

  cCompTrackMatchingParams->cd(5);
  h2TrkMatching_eta = (TH1F*)file->Get("trkana/TrkMatching/hTrkMatching_Eta_0");
  h2TrkMatching_eta->SetTitle("Trk matching (up - low) comparation");
  h2TrkMatching_eta->GetXaxis()->SetTitle("#Delta#eta = -ln (tan(#theta/2))");
  h2TrkMatching_eta->GetYaxis()->SetTitle("Tracks");
  if (normalizePlots) h2TrkMatching_eta->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  doSpace(h2TrkMatching_eta);
  h2TrkMatching_eta->SetLineColor(ColorForAll+2);
  h2TrkMatching_eta->SetLineWidth(1);
  h2TrkMatching_eta->SetFillColor(ColorForAll);
  h2TrkMatching_eta->SetFillStyle(3001);
  h2TrkMatching_eta->DrawCopy();
  FitValues fitdeta_0 = DrawFitFunc(h2TrkMatching_eta,ColorForAll+2,ShowFit);

  //sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_%d",level);    
  sprintf(name,"trkana/TrkMatching/hTrkMatching_Eta_2");    
  h2ProcTrkMatching_eta = (TH1F*)file->Get(name);
  h2ProcTrkMatching_eta->SetLineColor(ColorForUsed);
  h2ProcTrkMatching_eta->SetLineWidth(1);
  h2ProcTrkMatching_eta->SetFillColor(ColorForUsed-9);
  h2ProcTrkMatching_eta->SetFillStyle(3001);
  if (normalizePlots) h2ProcTrkMatching_eta->Scale(1./(float)iEvents);
  h2ProcTrkMatching_eta->DrawCopy("SAME");
  setTitleFont(font);
  ScalePlots();
  FitValues fitdeta_2 = DrawFitFunc(h2ProcTrkMatching_eta,ColorForUsed+2,ShowFit);
  
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else             TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  leg0->AddEntry(h2TrkMatching_eta,name,"L");
  sprintf(name,"Processed tracks");
  leg0->AddEntry(h2ProcTrkMatching_eta,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  // cout << "Fit Eta mean" <<fitdeta_0.FitMean <<endl;
  //cout << "Fit Eta Sigma" <<fitdeta_0.FitSigma <<endl;

  TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  sprintf(name,"#mu: %2.0e, #sigma: %2.0e",fitdeta_0.FitMean,fitdeta_0.FitSigma);
  leg1->AddEntry(h2TrkMatching_eta,name,"L");
  sprintf(name,"#mu: %2.0e, #sigma: %2.0e",fitdeta_2.FitMean,fitdeta_2.FitSigma);
  leg1->AddEntry(h2ProcTrkMatching_eta,name,"L");
  



  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  // TList *FunList;
  // FunList = h2TrkMatching_eta->GetListOfFunctions();
  // TF1 *TFun1[i] = (TF1*) FunList->First();
   
  // -----------------------------------------------------------------
  //  Track Matching delta qoverpt / pt
  // -----------------------------------------------------------------

    cCompTrackMatchingParams->cd(6);
  if (iBField>0) {
    h2TrkMatching_qoverpT = (TH1F*)file->Get("trkana/TrkMatching/hTrkMatching_Qoverpt_0");
    // gPad->SetLogy();
    h2TrkMatching_qoverpT->SetTitle("Trk matching (up - low) comparation");
    h2TrkMatching_qoverpT->GetXaxis()->SetTitle("#Delta q/pT (GeV^{-1})");
    h2TrkMatching_qoverpT->GetYaxis()->SetTitle("Tracks");
    if (normalizePlots) h2TrkMatching_qoverpT->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h2TrkMatching_qoverpT->SetLineColor(ColorForAll+2);
    h2TrkMatching_qoverpT->SetLineWidth(1);
    h2TrkMatching_qoverpT->SetFillColor(ColorForAll);
    h2TrkMatching_qoverpT->SetFillStyle(3001);
    h2TrkMatching_qoverpT->DrawCopy();
    FitValues fitdqovetpT_0 = DrawFitFunc(h2TrkMatching_qoverpT,ColorForAll+2,ShowFit);
    
    //sprintf(name,"trkana/TrkMatching/hTrkMatching_Qoverpt_%d",level);    
    sprintf(name,"trkana/TrkMatching/hTrkMatching_Qoverpt_2");    
    h2ProcTrkMatching_qoverpT = (TH1F*)file->Get(name);
    h2ProcTrkMatching_qoverpT->SetLineColor(ColorForUsed);
    h2ProcTrkMatching_qoverpT->SetLineWidth(1);
    h2ProcTrkMatching_qoverpT->SetFillColor(ColorForUsed-9);
    h2ProcTrkMatching_qoverpT->SetFillStyle(3001);
    if (normalizePlots) h2ProcTrkMatching_qoverpT->Scale(1./(float)iEvents);
    h2ProcTrkMatching_qoverpT->DrawCopy("SAME");
    setTitleFont(font);
    ScalePlots();
    FitValues fitdqovetpT_2 = DrawFitFunc(h2ProcTrkMatching_qoverpT,ColorForUsed+2,ShowFit);
    
  }
  else {
    h2TrkMatching_pT = (TH1F*)file->Get("trkana/TrkMatching/hTrkMatching_Pt_0");
    h2TrkMatching_pT->SetTitle("Trk matching (up - low) comparation");
    h2TrkMatching_pT->GetXaxis()->SetTitle("#Delta q pT (GeV^{-1})");
    h2TrkMatching_pT->GetYaxis()->SetTitle("Tracks");
    if (normalizePlots) h2TrkMatching_pT->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    h2TrkMatching_pT->SetLineColor(ColorForAll+2);
    h2TrkMatching_pT->SetLineWidth(1);
    h2TrkMatching_pT->SetFillColor(ColorForAll);
    h2TrkMatching_pT->SetFillStyle(3001);
    h2TrkMatching_pT->DrawCopy();
    FitValues fitdpT_0 = DrawFitFunc(h2TrkMatching_pT,ColorForAll+2,ShowFit);
    

    //sprintf(name,"trkana/TrkMatching/hTrkMatching_Pt_%d",level);    
    sprintf(name,"trkana/TrkMatching/hTrkMatching_Pt_2");    
    h2ProcTrkMatching_pT = (TH1F*)file->Get(name);
    h2ProcTrkMatching_pT->SetLineColor(ColorForUsed);
    h2ProcTrkMatching_pT->SetLineWidth(1);
    h2ProcTrkMatching_pT->SetFillColor(ColorForUsed-9);
    h2ProcTrkMatching_pT->SetFillStyle(3001);
    if (normalizePlots) h2ProcTrkMatching_pT->Scale(1./(float)iEvents);
    h2ProcTrkMatching_pT->DrawCopy("SAME");
    setTitleFont(font);
    ScalePlots();
    FitValues fitdpT_2 = DrawFitFunc(h2ProcTrkMatching_pT,ColorForUsed+2,ShowFit);
  }
  if (!AtlasStyle) TLegend *leg0 = new TLegend(0.3,0.83,0.67,0.93);
  else TLegend *leg0 = new TLegend(0.54,0.86,0.98,0.98);
  sprintf(name,"All tracks");
  if (iBField>0) leg0->AddEntry(h2TrkMatching_qoverpT,name,"L");
  else leg0->AddEntry(h2TrkMatching_pT,name,"L");
  sprintf(name,"Processed tracks");
  if (iBField>0) leg0->AddEntry(h2ProcTrkMatching_qoverpT,name,"L");
  else leg0->AddEntry(h2ProcTrkMatching_pT,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  if (iBField>0) {
    TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
    sprintf(name,"#mu: %2.0e, #sigma: %2.0e",fitdqovetpT_0.FitMean,fitdqovetpT_0.FitSigma);
    leg1->AddEntry(h2TrkMatching_qoverpT,name,"L");
    sprintf(name,"#mu: %2.0e, #sigma: %2.0e",fitdqovetpT_2.FitMean,fitdqovetpT_2.FitSigma);
    leg1->AddEntry(h2ProcTrkMatching_qoverpT,name,"L");
    
    leg1->SetTextSize(0.035);
    leg1->SetFillColor(kWhite);
    leg1->Draw();
  }
  else
    {
      TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
      sprintf(name,"#mu: %2.0f, #sigma: %2.0f",fitdpT_0.FitMean,fitdpT_0.FitSigma);
      leg1->AddEntry(h2TrkMatching_pT,name,"L");
      sprintf(name,"#mu: %2.0f, #sigma: %2.0f",fitdpT_2.FitMean,fitdpT_2.FitSigma);
      leg1->AddEntry(h2ProcTrkMatching_pT,name,"L");
      
      leg1->SetTextSize(0.035);
      leg1->SetFillColor(kWhite);
      leg1->Draw();
    }
  cCompTrackMatchingParams->Update();

}
