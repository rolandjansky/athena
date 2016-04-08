/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void trkTrack_hits() {

  TH1F *hTrkTracks;
  TH1F *hTrknPixelhits;
  TH1F *hTrknSCThits;
  TH1F *hTrkhits;
  //TH1F *hTrknTRThits;
  TH1F *hTrkShared;
  TH1F *hTrkHoles;

  cout << " Trk::Track - Track and Hit information..." << endl;
 
  cTrackhits = new TCanvas("TrkTracksHits","Trk::Track - Track and Hits information",
			   CanvasSizeX6[0],
			   CanvasSizeX6[1]);
  cTrackhits->Divide(3,2);
  
  cTrackhits->cd(1);
  hTrkTracks = (TH1F*)file->Get("trkana/TrkTrack/hNTrkTrack_0");
  hTrkTracks->SetStats(kFALSE);
  hTrkTracks->GetXaxis()->SetTitle("Number of tracks");
  hTrkTracks->GetYaxis()->SetTitle("Events with tracks");
  if (normalizePlots) hTrkTracks->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrkTracks->SetBinContent(1,0.0);
  hTrkTracks->SetLineColor(ColorForAll+2);
  hTrkTracks->SetLineWidth(1);
  hTrkTracks->SetFillColor(ColorForAll);
  hTrkTracks->SetFillStyle(3001);
  hTrkTracks->SetStats(kFALSE);
  hTrkTracks->SetTitle("Trk::Track: Tracks per event");
  hTrkTracks ->DrawCopy();
  int Trk_ent1 = hTrkTracks->GetEntries();
  float Trk_mean1 = hTrkTracks->GetMean();
  
  float iNoTrkTracks = hTrkTracks->GetBinContent(1);
  
  setTitleFont(font);
  setTitleSize(0.46);
  ScalePlots();
  TLegend *leg0 = new TLegend(0.74,0.75,0.98,0.86);
  sprintf(name,"mean: %2.2f",Trk_mean1);
  leg0->AddEntry(hTrkTracks,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw(); 
  
  TLegend *leg0 = new TLegend(0.55,0.89,0.98,1.0);
  sprintf(name,"All Trk:Tracks (%2.1fk)",float(iTrkTracks)/1000);
  leg0->AddEntry(hTrkTracks,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  

  //
  //-------------------------------------------------
  //
 
  cTrackhits->cd(2);
  hTrknPixelhits = (TH1F*)file->Get("trkana/TrkTrack/hTrkPIXhits_0");
  hTrknPixelhits->SetTitle("Trk::Track Pixel Hits on track");
  hTrknPixelhits->GetXaxis()->SetTitle("Pixel Hits on track");
  hTrknPixelhits->GetYaxis()->SetTitle("Entries");
  if (normalizePlots) hTrknPixelhits->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrknPixelhits->SetLineColor(ColorForAll+2);
  hTrknPixelhits->SetLineWidth(1);
  hTrknPixelhits->SetFillColor(ColorForAll);
  hTrknPixelhits->SetFillStyle(3001);
  int TracksWithoutPixelHits = hTrknPixelhits->GetBinContent(1);
  hTrknPixelhits->SetBinContent(1,0.0);
  hTrknPixelhits->SetStats(kFALSE);
  ForceStyle(hTrknPixelhits);
  hTrknPixelhits->DrawCopy();
  ScalePlots();
  float ent1 = hTrknPixelhits->GetEntries();
  float mean1 = hTrknPixelhits->GetMean();
  setTitleFont(font);
  setTitleSize(0.53);
  ScalePlots();

  TLegend *leg0 = new TLegend(0.55,0.89,0.98,1.0);
  sprintf(name," tracks (%2.1fk)",float(iTrkTracks)/1000);
  leg0->AddEntry(hTrknPixelhits,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.55,0.75,0.98,0.86);
  sprintf(name," tracks w/o hits: %d",TracksWithoutPixelHits);
  leg0->AddEntry(hTrknPixelhits,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.74,0.61,0.98,0.72);
  sprintf(name,"mean: %2.2f",mean1);
  leg0->AddEntry(hTrknPixelhits,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();


  //
  // -----------------------------------------------------------------
  //
  
  cTrackhits->cd(3);
  hTrknSCThits = (TH1F*)file->Get("trkana/TrkTrack/hTrkSCThits_0");
  if (normalizePlots) hTrknSCThits->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrknSCThits->SetTitle("Trk::Tracks SCT Hits on track");
  hTrknSCThits->GetXaxis()->SetTitle("SCT Hits on track");
  hTrknSCThits->GetYaxis()->SetTitle("Entries");
  hTrknSCThits->SetLineColor(ColorForAll+2);
  hTrknSCThits->SetLineWidth(1);
  hTrknSCThits->SetFillColor(ColorForAll);
  hTrknSCThits->SetFillStyle(3001);
  int TracksWithoutSCTHits = hTrknSCThits->GetBinContent(1);
  hTrknSCThits->SetBinContent(1,0.0);
  hTrknSCThits->SetStats(kFALSE);
  ForceStyle(hTrknSCThits);
  hTrknSCThits->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  ent1 = hTrknSCThits->GetEntries();
  mean1 = hTrknSCThits->GetMean();
  setTitleFont(font);
  setTitleSize(0.53);
  ScalePlots();

  TLegend *leg0 = new TLegend(0.57,0.89,0.98,1.0);
  sprintf(name,"Trk::All tracks (%2.1fk)",float(iTrkTracks)/1000);
  leg0->AddEntry(hTrknSCThits,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.55,0.75,0.98,0.86);
  sprintf(name,"trks w/o hits: %d",TracksWithoutSCTHits);
  leg0->AddEntry(hTrknSCThits,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.74,0.61,0.98,0.72);
  sprintf(name,"mean: %2.2f",mean1);
  leg0->AddEntry(hTrknSCThits,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //

  cTrackhits->cd(4);
  hTrknhits = (TH1F*)file->Get("trkana/TrkTrack/hTrkHits_0");
  if (normalizePlots) hTrknhits->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrknhits->SetTitle("Trk::Track Hits on track");
  hTrknhits->GetXaxis()->SetTitle("Hits on track");
  hTrknhits->GetYaxis()->SetTitle("Entries");
  hTrknhits->SetLineColor(ColorForAll+2);
  hTrknhits->SetLineWidth(1);
  hTrknhits->SetFillColor(ColorForAll);
  hTrknhits->SetFillStyle(3001);
  hTrknhits->SetStats(kFALSE);
  hTrknhits->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleFont(font);
  ScalePlots();
  ent1 = hTrknhits->GetEntries();
  mean1 = hTrknhits->GetMean();
  
  TLegend *leg0 = new TLegend(0.74,0.75,0.98,0.86);
  sprintf(name,"mean: %2.2f",mean1);
  leg0->AddEntry(hTrknhits,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.55,0.89,0.98,1.0);
  sprintf(name,"All tracks (%2.1fk)",float(iTrkTracks)/1000);
  leg0->AddEntry(hTrknhits,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
  //
  // -----------------------------------------------------------------
  //

  cTrackhits->cd(5);

  hTrkHoles = (TH1F*)file->Get("trkana/TrkTrack/hTrkholes_0");
  if (normalizePlots) hTrkHoles->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrkHoles->SetTitle("Trk::Track Track holes");
  hTrkHoles->GetXaxis()->SetTitle("Holes on track");
  hTrkHoles->GetYaxis()->SetTitle("Entries");
  hTrkHoles->SetStats(kFALSE);
  hTrkHoles->SetLineColor(ColorForAll+2);
  hTrkHoles->SetLineWidth(1);
  hTrkHoles->SetFillColor(ColorForAll);
  hTrkHoles->SetFillStyle(3001);
  hTrkHoles->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleFont(font);
  ScalePlots();
  ent1 = hTrkHoles->GetEntries();
  mean1 = hTrkHoles->GetMean();


  TLegend *leg0 = new TLegend(0.74,0.75,0.98,0.86);
  sprintf(name,"mean: %2.2f",mean1);
  leg0->AddEntry(hTrkHoles,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.55,0.89,0.98,1.0);
  sprintf(name,"All tracks (%2.1fk)",float(iTrkTracks)/1000);
  leg0->AddEntry(hTrkHoles,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //

  cTrackhits->cd(6);
  hTrkShared = (TH1F*)file->Get("trkana/TrkTrack/hTrksharedHits_0");
  hTrkShared->SetStats(kFALSE);
  hTrkShared->SetLineColor(ColorForAll+2);
  hTrkShared->SetLineWidth(1);
  hTrkShared->SetFillColor(ColorForAll);
  hTrkShared->SetFillStyle(3001);
  hTrkShared->SetTitle("Trk::Track Shared Hits on Trk");
  hTrkShared->GetXaxis()->SetTitle("Shared Hits on track");
  hTrkShared->GetYaxis()->SetTitle("Entries");
  if (normalizePlots) hTrkShared->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrkShared->DrawCopy();
  setTitleFont(font);
  setTitleSize(0.53);
  ScalePlots();
  ent1 = hTrkShared->GetEntries();
  mean1 = hTrkShared->GetMean();
  
  TLegend *leg0 = new TLegend(0.74,0.75,0.98,0.86);
  sprintf(name,"mean: %2.2f",mean1);
  leg0->AddEntry(hTrkShared,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.55,0.89,0.98,1.0);
  sprintf(name,"All tracks (%2.1fk)",float(iTrkTracks)/1000);
  leg0->AddEntry(hTrkShared,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
}



