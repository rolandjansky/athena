/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void hits() {

  TH1F *hTracks;
  TH1F *hTracks3;
  TH1F *hnPixelhits;
  TH1F *hnPixelhits2;
  TH1F *hnSCThits;
  TH1F *hnSCThits2;
  TH1F *hhits;
  TH1F *hhits2;
  TH1F *hHoles;
  TH1F *hHoles2;
  TH1F *hShared;
  TH1F *hShared2;

  cout << " Track and Hit information..." << endl;
 
  cHits = new TCanvas("TracksHits","Tracks and Hits information",CanvasSizeX6[0],CanvasSizeX6[1]);
  cHits->Divide(3,2);

  cHits->cd(1);
  hTracks = (TH1F*)file->Get("trkana/HitsTracks/hNTrk_0");
  hTracks->SetTitle("Reco Tracks");
  hTracks->GetXaxis()->SetTitle("Tracks per event");
  hTracks->GetYaxis()->SetTitle("Entries");
  if (normalizePlots) hTracks->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTracks->SetLineColor(ColorForAll+2);
  hTracks->SetLineWidth(1);
  hTracks->SetFillColor(ColorForAll);
  hTracks->SetFillStyle(3001);
  hTracks->SetStats(kFALSE);
  hTracks->SetTitle("Tracks per event");
  hTracks ->DrawCopy();
  ScalePlots();
  int ent1 = hTracks->GetEntries();
  float mean1 = hTracks->GetMean();

  sprintf(name,"trkana/HitsTracks/hNTrk_%d",level);
  hTracks3 = (TH1F*)file->Get(name);
  hTracks3->SetLineColor(ColorForUsed);
  hTracks3->SetLineWidth(1);
  hTracks3->SetFillColor(ColorForUsed-9);
  hTracks3->SetFillStyle(3001);
  if (normalizePlots) hTracks3->Scale(1./(float)iEvents);
  hTracks3 ->DrawCopy("same");
  int ent2 = hTracks3->GetEntries();
  float mean2 = hTracks3->GetMean();

  if (hTracks->GetMaximum() < hTracks3->GetMaximum()) {  
    hTracks->GetYaxis()->SetRangeUser(0,1.05*hTracks3->GetMaximum());
    hTracks->DrawCopy();
    hTracks3->DrawCopy("same");
  }
  setTitleFont(font);

  TLegend *leg0 = new TLegend(0.74,0.75,0.98,0.86);
  sprintf(name,"mean: %2.2f",mean1);
  leg0->AddEntry(hTracks,name,"L");
  sprintf(name,"mean:% 2.2f",mean2);
  leg0->AddEntry(hTracks3,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.47,0.89,0.98,1.0);
  sprintf(name,"All tracks (%2.1fk)",float(iTracks)/1000);
  leg0->AddEntry(hTracks,name,"L");
  sprintf(name,"Processed tracks (%2.1fk)",float(iUsedTracks)/1000);
  leg0->AddEntry(hTracks3,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
   
  //
  // -----------------------------------------------------------------
  //

  cHits->cd(2);
  hnPixelhits = (TH1F*)file->Get("trkana/PIXHits/hPIXhits_0");
  hnPixelhits->SetTitle("Pixel Hits on track");
  hnPixelhits->GetXaxis()->SetTitle("Pixel Hits on track");
  hnPixelhits->GetYaxis()->SetTitle("Entries");
  if (normalizePlots) hnPixelhits->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hnPixelhits->SetLineColor(ColorForAll+2);
  hnPixelhits->SetLineWidth(1);
  hnPixelhits->SetFillColor(ColorForAll);
  hnPixelhits->SetFillStyle(3001);
  int TracksWithoutPixelHits = hnPixelhits->GetBinContent(1);
  hnPixelhits->SetBinContent(1,0.0);
  hnPixelhits->SetStats(kFALSE);
  ForceStyle(hnPixelhits);
  hnPixelhits->DrawCopy();
  ScalePlots();
  ent1 = hnPixelhits->GetEntries();
  mean1 = hnPixelhits->GetMean();

  sprintf(name,"trkana/PIXHits/hPIXhits_%d",level);
  hnPixelhits2 = (TH1F*)file->Get(name);
  hnPixelhits2->SetLineColor(ColorForUsed);
  hnPixelhits2->SetLineWidth(1);
  hnPixelhits2->SetFillColor(ColorForUsed-9);
  hnPixelhits2->SetFillStyle(3001);
  int TracksWithoutPixelHits2 = hnPixelhits2->GetBinContent(1);
  hnPixelhits2->SetBinContent(1,0.0);
  if (normalizePlots) hnPixelhits2->Scale(1./(float)iEvents);
  hnPixelhits2 ->DrawCopy("same");
  ent2 = hnPixelhits2->GetEntries();
  mean2 = hnPixelhits2->GetMean();

  if (hnPixelhits->GetMaximum() < hnPixelhits2->GetMaximum()) {  
    hnPixelhits->GetYaxis()->SetRangeUser(0,1.05*hnPixelhits2->GetMaximum());
    hnPixelhits->DrawCopy();
    hnPixelhits2->DrawCopy("same");
  }
  setTitleFont(font);

  TLegend *leg0 = new TLegend(0.47,0.89,0.98,1.0);
  sprintf(name,"All tracks (%2.1fk)",float(iTracks)/1000);
  leg0->AddEntry(hnPixelhits,name,"L");
  sprintf(name,"Processed tracks (%2.1fk)",float(iUsedTracks)/1000);
  leg0->AddEntry(hnPixelhits2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.55,0.75,0.98,0.86);
  sprintf(name,"trks w/o hits: %d",TracksWithoutPixelHits);
  leg0->AddEntry(hnPixelhits,name,"L");
  sprintf(name,"trks w/o hits: %d",TracksWithoutPixelHits2);
  leg0->AddEntry(hnPixelhits2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.74,0.61,0.98,0.72);
  sprintf(name,"mean: %2.2f",mean1);
  leg0->AddEntry(hnPixelhits,name,"L");
  sprintf(name,"mean:% 2.2f",mean2);
  leg0->AddEntry(hnPixelhits2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //
  
  cHits->cd(3);
  hnSCThits = (TH1F*)file->Get("trkana/SCTHits/hSCThits_0");
  if (normalizePlots) hnSCThits->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hnSCThits->SetTitle("SCT Hits on track");
  hnSCThits->GetXaxis()->SetTitle("SCT Hits on track");
  hnSCThits->GetYaxis()->SetTitle("Entries");
  hnSCThits->SetLineColor(ColorForAll+2);
  hnSCThits->SetLineWidth(1);
  hnSCThits->SetFillColor(ColorForAll);
  hnSCThits->SetFillStyle(3001);
  int TracksWithoutSCTHits = hnSCThits->GetBinContent(1);
  hnSCThits->SetBinContent(1,0.0);
  hnSCThits->SetStats(kFALSE);
  ForceStyle(hnSCThits);
  hnSCThits->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  ent1 = hnSCThits->GetEntries();
  mean1 = hnSCThits->GetMean();

  sprintf(name,"trkana/SCTHits/hSCThits_%d",level);
  hnSCThits2 = (TH1F*)file->Get(name);
  hnSCThits2->SetLineColor(ColorForUsed);
  hnSCThits2->SetLineWidth(1);
  hnSCThits2->SetFillColor(ColorForUsed-9);
  hnSCThits2->SetFillStyle(3001);
  int TracksWithoutSCTHits2 = hnSCThits2->GetBinContent(1);
  hnSCThits2->SetBinContent(1,0.0);
  if (normalizePlots) hnSCThits2->Scale(1./(float)iEvents);
  hnSCThits2->DrawCopy("same");
  ent2 = hnSCThits2->GetEntries();
  mean2 = hnSCThits2->GetMean();
  
  if (hnSCThits->GetMaximum() < hnSCThits2->GetMaximum()) {  
    hnSCThits->GetYaxis()->SetRangeUser(0,1.05*hnSCThits2->GetMaximum());
    hnSCThits->DrawCopy();
    hnSCThits2->DrawCopy("same");
  }
  setTitleFont(font);

  TLegend *leg0 = new TLegend(0.47,0.89,0.98,1.0);
  sprintf(name,"All tracks (%2.1fk)",float(iTracks)/1000);
  leg0->AddEntry(hnSCThits,name,"L");
  sprintf(name,"Processed tracks (%2.1fk)",float(iUsedTracks)/1000);
  leg0->AddEntry(hnSCThits2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.60,0.75,0.98,0.86);
  sprintf(name,"trks w/o hits: %d",TracksWithoutSCTHits);
  leg0->AddEntry(hnSCThits,name,"L");
  sprintf(name,"trks w/o hits: %d",TracksWithoutSCTHits2);
  leg0->AddEntry(hnSCThits2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.74,0.61,0.98,0.72);
  sprintf(name,"mean: %2.2f",mean1);
  leg0->AddEntry(hnSCThits,name,"L");
  sprintf(name,"mean:% 2.2f",mean2);
  leg0->AddEntry(hnSCThits2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //

  cHits->cd(4);
  hnhits = (TH1F*)file->Get("trkana/HitsTracks/hhits_0");
  if (normalizePlots) hnhits->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }  
  hnhits->SetTitle("Hits on track");
  hnhits->GetXaxis()->SetTitle("Hits on track");
  hnhits->GetYaxis()->SetTitle("Entries");
  hnhits->SetLineColor(ColorForAll+2);
  hnhits->SetLineWidth(1);
  hnhits->SetFillColor(ColorForAll);
  hnhits->SetFillStyle(3001);
  hnhits->SetStats(kFALSE);
  hnhits->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleFont(font);
  ScalePlots();
  ent1 = hnhits->GetEntries();
  mean1 = hnhits->GetMean();
  
  sprintf(name,"trkana/HitsTracks/hhits_%d",level);
  hnhits2 = (TH1F*)file->Get(name);
  hnhits2->SetLineColor(ColorForUsed);
  hnhits2->SetLineWidth(1);
  hnhits2->SetFillColor(ColorForUsed-9);
  hnhits2->SetFillStyle(3001);
  if (normalizePlots) hnhits2->Scale(1./(float)iEvents);
  hnhits2->DrawCopy("same");
  ent2 = hnhits2->GetEntries();
  mean2 = hnhits2->GetMean();

  if (hnhits->GetMaximum() < hnhits2->GetMaximum()) {  
    hnhits->GetYaxis()->SetRangeUser(0,1.05*hnhits2->GetMaximum());
    hnhits->DrawCopy();
    hnhits2->DrawCopy("same");
  }

  TLegend *leg0 = new TLegend(0.74,0.75,0.98,0.86);
  sprintf(name,"mean: %2.2f",mean1);
  leg0->AddEntry(hnhits,name,"L");
  sprintf(name,"mean:% 2.2f",mean2);
  leg0->AddEntry(hnhits2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.47,0.89,0.98,1.0);
  sprintf(name,"All tracks (%2.1fk)",float(iTracks)/1000);
  leg0->AddEntry(hnhits,name,"L");
  sprintf(name,"Processed tracks (%2.1fk)",float(iUsedTracks)/1000);
  leg0->AddEntry(hnhits2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
  //
  // -----------------------------------------------------------------
  //

  cHits->cd(5);
  hHoles = (TH1F*)file->Get("trkana/HitsTracks/hHolesHits_0");
  if (normalizePlots) hHoles->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hHoles->SetTitle("Track holes");
  hHoles->GetXaxis()->SetTitle("Holes on track");
  hHoles->GetYaxis()->SetTitle("Entries");
  hHoles->SetStats(kFALSE);
  hHoles->SetLineColor(ColorForAll+2);
  hHoles->SetLineWidth(1);
  hHoles->SetFillColor(ColorForAll);
  hHoles->SetFillStyle(3001);
  hHoles->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleFont(font);
  ScalePlots();
  ent1 = hHoles->GetEntries();
  mean1 = hHoles->GetMean();

  sprintf(name,"trkana/HitsTracks/hHolesHits_%d",level);
  hHoles2 = (TH1F*)file->Get(name);
  hHoles2->SetLineColor(ColorForUsed);
  hHoles2->SetLineWidth(1);
  hHoles2->SetFillColor(ColorForUsed-9);
  hHoles2->SetFillStyle(3001);
  if (normalizePlots) hHoles2->Scale(1./(float)iEvents);
  hHoles2->DrawCopy("same");
  ent2 = hHoles2->GetEntries();
  mean2 = hHoles2->GetMean();

  if (hHoles->GetMaximum() < hHoles2->GetMaximum()) {  
    hHoles->GetYaxis()->SetRangeUser(0,1.05*hHoles2->GetMaximum());
    hHoles->DrawCopy();
    hHoles2->DrawCopy("same");
  }

  TLegend *leg0 = new TLegend(0.74,0.75,0.98,0.86);
  sprintf(name,"mean: %2.2f",mean1);
  leg0->AddEntry(hHoles,name,"L");
  sprintf(name,"mean:% 2.2f",mean2);
  leg0->AddEntry(hHoles2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.47,0.89,0.98,1.0);
  sprintf(name,"All tracks (%2.1fk)",float(iTracks)/1000);
  leg0->AddEntry(hHoles,name,"L");
  sprintf(name,"Processed tracks (%2.1fk)",float(iUsedTracks)/1000);
  leg0->AddEntry(hHoles2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  //
  // -----------------------------------------------------------------
  //

  cHits->cd(6);
  hShared = (TH1F*)file->Get("trkana/HitsTracks/hSharedHits_0");
  hShared->SetStats(kFALSE);
  hShared->SetLineColor(ColorForAll+2);
  hShared->SetLineWidth(1);
  hShared->SetFillColor(ColorForAll);
  hShared->SetFillStyle(3001);
  hShared->SetTitle("Shared Hits on Trk");
  hShared->GetXaxis()->SetTitle("Shared Hits on track");
  hShared->GetYaxis()->SetTitle("Entries");
  if (normalizePlots) hShared->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hShared->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  ent1 = hShared->GetEntries();
  mean1 = hShared->GetMean();
  
  sprintf(name,"trkana/HitsTracks/hSharedHits_%d",level);
  hShared2 = (TH1F*)file->Get(name);
  hShared2->SetLineColor(ColorForUsed);
  hShared2->SetLineWidth(1);
  hShared2->SetFillColor(ColorForUsed-9);
  hShared2->SetFillStyle(3001);
  if (normalizePlots) hShared2->Scale(1./(float)iEvents);
  hShared2->DrawCopy("same");
  ent2 = hShared2->GetEntries();
  mean2 = hShared2->GetMean();

  if (hShared->GetMaximum() < hShared2->GetMaximum()) {  
    hShared->GetYaxis()->SetRangeUser(0,1.05*hShared2->GetMaximum());
    hShared->DrawCopy();
    hShared2->DrawCopy("same");
  }

  TLegend *leg0 = new TLegend(0.74,0.75,0.98,0.86);
  sprintf(name,"mean: %2.2f",mean1);
  leg0->AddEntry(hShared,name,"L");
  sprintf(name,"mean:% 2.2f",mean2);
  leg0->AddEntry(hShared2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();

  TLegend *leg0 = new TLegend(0.47,0.89,0.98,1.0);
  sprintf(name,"All tracks (%2.1fk)",float(iTracks)/1000);
  leg0->AddEntry(hShared,name,"L");
  sprintf(name,"Processed tracks (%2.1fk)",float(iUsedTracks)/1000);
  leg0->AddEntry(hShared2,name,"L");
  leg0->SetTextSize(0.035);
  leg0->SetFillColor(kWhite);
  leg0->Draw();
  
}
