/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// do not rename the next method, RTT will stop working
void NewSiL1AlignRTT()
{
  gROOT->SetBatch(kTRUE);
  makePlot("Iter0/Collision/TotalMonitoring.root", "Iter0_plots");
  makePlot("Iter1/Collision/TotalMonitoring.root", "Iter1_plots");
}

void makePlot(Char_t filename[300], TString psname){

  //Track Collections to check
  TString FirstTrackCollection="ExtendedTracks_all"; // all tracks
  TString SecondTrackCollection="AlignTracks_all"; //tracks used to align 
  TString histname;
  Char_t nameh[300];

  file = new TFile(filename);
  TDirectory *IDAlignMonDir = file->GetDirectory("IDAlignMon");
  tmpName = "IDAlignMon/"+FirstTrackCollection;
  TDirectory *FirstTrkSelDir = file->GetDirectory(tmpName);
  tmpName = "IDAlignMon/"+SecondTrackCollection;
  TDirectory *SecondTrkSelDir = file->GetDirectory(tmpName);
  

  TH1F *hPixel_TrkPhiRes;
  TH1F *hPixel_TrkEtaRes;
  TH1F *hPixel_TrkPhiRes_2;
  TH1F *hPixel_TrkEtaRes_2;
  TH1F *hSCT_TrkRes;
  TH1F *hSCT_TrkRes_2;
  TH1F *h_events;
  TH1F *h_Aligntracks;
  TH1F *h_Tracks;


  TCanvas *cRes;
  cRes = new TCanvas("Silicon Residuals"," Silicon Residuals (New Code)",1000,800);
  cRes->Divide(2,2);
   
  // SUMMARY
  Int_t font=42;

  cRes->cd(1);
  TLine *TLineBorder;
  TLineBorder = new TLine(0.02,0.98,0.98,0.98);
  TLineBorder->SetLineColor(kRed);
  TLineBorder->Draw();
  TLineBorder = new TLine(0.02,0.02,0.98,0.02);
  TLineBorder->SetLineColor(kRed);
  TLineBorder->Draw();
  TLineBorder = new TLine(0.02,0.98,0.02,0.02);
  TLineBorder->SetLineColor(kRed);
  TLineBorder->Draw();
  TLineBorder = new TLine(0.98,0.98,0.98,0.02);
  TLineBorder->SetLineColor(kRed);
  TLineBorder->Draw();
 
  int iConsts = 0;
  
  TLatex *t = new TLatex();
  t->SetNDC();
  t->SetTextFont(font);
  t->SetTextSize(0.05);
  t->SetTextAlign(12);

  // Information
  t->SetTextSize(0.035);
  histname = "IDAlignMon/"+FirstTrackCollection+"/GenericTracks/Nhits_per_event";
  h_events = (TH1F*)file->Get(histname);
  float iEvents = h_events->GetEntries();	
  sprintf(nameh,"Events: %d",iEvents);
  t->DrawLatex(0.08,0.78,nameh);
  histname = "IDAlignMon/"+FirstTrackCollection+"/GenericTracks/Nhits_per_track";
  h_tracks = (TH1F*)file->Get(histname);
  float TotalTracks = h_tracks->GetEntries();
  sprintf(nameh,"ResolvedTrks_all: %d",TotalTracks);
  t->DrawLatex(0.08,0.74,nameh);
  histname = "IDAlignMon/"+SecondTrackCollection+"/GenericTracks/Nhits_per_track";
  h_Aligntracks = (TH1F*)file->Get(histname);
  float AlignTracks = h_Aligntracks->GetEntries();
  sprintf(nameh,"AlignTrks_all: %d",AlignTracks);
  t->DrawLatex(0.08,0.70,nameh);

  
  cRes->cd(2);

  histname = "IDAlignMon/"+FirstTrackCollection+"/Residuals/sct_b_biasedresidualx";
  hSCT_TrkRes = (TH1F*)file->Get(histname);	
  hSCT_TrkRes->SetTitle("Biased SCT  Residuals (Barrel)");
  hSCT_TrkRes->GetXaxis()->SetTitle("SCT Barrel Residuals (mm)");
  hSCT_TrkRes->GetYaxis()->SetTitle("Entries");
  hSCT_TrkRes->SetLineColor(kGray+2);
  hSCT_TrkRes->SetFillColor(kGray);
  hSCT_TrkRes->SetLineWidth(1);
  hSCT_TrkRes->SetFillStyle(3001);
  hSCT_TrkRes->Draw("histname");
  
  histname = "IDAlignMon/"+SecondTrackCollection+"/Residuals/sct_b_biasedresidualx";
  hSCT_TrkRes_2 = (TH1F*)file->Get(histname);	
  hSCT_TrkRes_2->SetLineColor(kRed+2);
  hSCT_TrkRes_2->SetFillColor(kRed);
  hSCT_TrkRes_2->SetLineWidth(1);
  hSCT_TrkRes_2->SetFillStyle(3001);
  hSCT_TrkRes_2->DrawCopy("same");

  cRes->cd(3);
  histname = "IDAlignMon/"+FirstTrackCollection+"/Residuals/pix_b_biasedresidualx";
  hPixel_TrkPhiRes = (TH1F*)file->Get(histname);	
  hPixel_TrkPhiRes->SetTitle("Biased Pixel #phi Residuals (Barrel)");
  hPixel_TrkPhiRes->GetXaxis()->SetTitle("Pixel Barrel #phi Residuals (mm)");
  hPixel_TrkPhiRes->GetYaxis()->SetTitle("Entries");
  hPixel_TrkPhiRes->SetLineColor(kGray+2);
  hPixel_TrkPhiRes->SetFillColor(kGray);
  hPixel_TrkPhiRes->SetLineWidth(1);
  hPixel_TrkPhiRes->SetFillStyle(3001);
  hPixel_TrkPhiRes->Draw("histname");
  
  histname = "IDAlignMon/"+SecondTrackCollection+"/Residuals/pix_b_biasedresidualx";
  hPixel_TrkPhiRes_2 = (TH1F*)file->Get(histname);	
  hPixel_TrkPhiRes_2->SetLineColor(kRed+2);
  hPixel_TrkPhiRes_2->SetFillColor(kRed);
  hPixel_TrkPhiRes_2->SetLineWidth(1);
  hPixel_TrkPhiRes_2->SetFillStyle(3001);
  hPixel_TrkPhiRes_2->DrawCopy("same");


  cRes->cd(4);
  histname = "IDAlignMon/"+FirstTrackCollection+"/Residuals/pix_b_biasedresidualy";
  hPixel_TrkEtaRes = (TH1F*)file->Get(histname);	
  hPixel_TrkEtaRes->SetTitle("Biased Pixel #eta Residuals (Barrel)");
  hPixel_TrkEtaRes->GetXaxis()->SetTitle("Pixel Barrel #eta Residuals (mm)");
  hPixel_TrkEtaRes->GetYaxis()->SetTitle("Entries");
  hPixel_TrkEtaRes->SetLineColor(kGray+2);
  hPixel_TrkEtaRes->SetLineWidth(1);
  hPixel_TrkEtaRes->SetFillColor(kGray);
  hPixel_TrkEtaRes->SetFillStyle(3001);
  hPixel_TrkEtaRes->Draw("histname");

  histname = "IDAlignMon/"+SecondTrackCollection+"/Residuals/pix_b_biasedresidualy";
  hPixel_TrkEtaRes_2 = (TH1F*)file->Get(histname);	
  hPixel_TrkEtaRes_2->SetLineColor(kRed+2);
  hPixel_TrkEtaRes_2->SetFillColor(kRed);
  hPixel_TrkEtaRes_2->SetLineWidth(1);
  hPixel_TrkEtaRes_2->SetFillStyle(3001);
  hPixel_TrkEtaRes_2->DrawCopy("same");
  
 
  cRes->Update();
  cRes->Print(psname+".eps");
  cRes->Print(psname+".png");
  
  delete cRes;
   
}
