/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void hitCorrelations() {
  TH2F *hPixelVSSCT;
  TH2F *hPixelVSSCT2;
  TH2F *hPixelEta;
  TH2F *hPixelEta2;
  TH2F *hSCTEta;
  TH2F *hSCTEta2;

  cout << " More Hit information..." << endl;

  cHitCorr = new TCanvas("TracksHits2","More Hits information",CanvasSizeX6[0],CanvasSizeX6[1]);
  cHitCorr->Divide(3,2);

  cHitCorr->cd(1);
  hPixelVSSCT = (TH2F*)file->Get("trkana/HitsTracks/hPIXvSCTHits_0");
  hPixelVSSCT->Draw("COLZ");
  hPixelVSSCT->SetTitle("Hits on track");
  hPixelVSSCT->GetXaxis()->SetTitle("SCT Hits on track");
  hPixelVSSCT->GetYaxis()->SetTitle("Pixel Hits on track");
  if (!AtlasStyle) {
    gPad->SetGridx();
    gPad->SetGridy();
  }
  gPad->SetLogz();
  cHitCorr->Update();
  moveTPaveStats(hPixelVSSCT);
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hPixelVSSCT->Scale(1./(float)iEvents);

  //
  // -----------------------------------------------------------------
  //

  cHitCorr->cd(4);
  sprintf(name,"trkana/HitsTracks/hPIXvSCTHits_%d",level);
  hPixelVSSCT2 = (TH2F*)file->Get(name);
  hPixelVSSCT2 ->Draw("COLZ");
  hPixelVSSCT2->SetTitle("Processed Hits on track");
  hPixelVSSCT2->GetXaxis()->SetTitle("Processed SCT Hits on track");
  hPixelVSSCT2->GetYaxis()->SetTitle("Processed Pixel Hits on track");
  if (!AtlasStyle) {
    gPad->SetGridx();
    gPad->SetGridy();
  }
  gPad->SetLogz();
  cHitCorr->Update();
  moveTPaveStats(hPixelVSSCT2);
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hPixelVSSCT2->Scale(1./(float)iEvents);

  //
  // -----------------------------------------------------------------
  //

  cHitCorr->cd(2);
  hPixelEta = (TH2F*)file->Get("trkana/PIXHits/hPIXHitsvEta_0");
  hPixelEta->Draw("COLZ");
  hPixelEta->SetTitle("Pixel Hits VS eta");
  hPixelEta->GetXaxis()->SetTitle("#eta");
  hPixelEta->GetYaxis()->SetTitle("Pixel Hits on track");
  if (!AtlasStyle) {
    gPad->SetGridx();
    gPad->SetGridy();
  }
  gPad->SetLogz();
  cHitCorr->Update();
  moveTPaveStats(hPixelEta);
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hPixelEta->Scale(1./(float)iEvents);

  //
  // -----------------------------------------------------------------
  //

  cHitCorr->cd(5);
  sprintf(name,"trkana/PIXHits/hPIXHitsvEta_%d",level);
  hPixelEta2 = (TH2F*)file->Get(name);
  hPixelEta2->Draw("COLZ");
  hPixelEta2->SetTitle("Processed Pixel Hits VS eta");
  hPixelEta2->GetXaxis()->SetTitle("#eta");
  hPixelEta2->GetYaxis()->SetTitle("Processed Pixel Hits on track");
  if (!AtlasStyle) {
    gPad->SetGridx();
    gPad->SetGridy();
  }
  gPad->SetLogz();
  cHitCorr->Update();
  moveTPaveStats(hPixelEta2);
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hPixelEta2->Scale(1./(float)iEvents);

  cHitCorr->cd(3);
  hSCTEta = (TH2F*)file->Get("trkana/SCTHits/hSCTHitsvEta_0");
  hSCTEta->Draw("COLZ");
  hSCTEta->SetTitle("SCT Hits VS eta");
  hSCTEta->GetXaxis()->SetTitle("#eta");
  hSCTEta->GetYaxis()->SetTitle("Processed SCT Hits on track");
  if (!AtlasStyle) {
    gPad->SetGridx();
    gPad->SetGridy();
  }
  gPad->SetLogz();
  cHitCorr->Update();
  moveTPaveStats(hSCTEta);
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hSCTEta->Scale(1./(float)iEvents);

  //
  // -----------------------------------------------------------------
  //

  cHitCorr->cd(6);
  sprintf(name,"trkana/SCTHits/hSCTHitsvEta_%d",level);
  hSCTEta2 = (TH2F*)file->Get(name);
  hSCTEta2->Draw("COLZ");
  hSCTEta2->SetTitle("Processed SCT Hits VS eta");
  hSCTEta2->GetXaxis()->SetTitle("#eta");
  hSCTEta2->GetYaxis()->SetTitle("Processed SCT Hits on track");
  if (!AtlasStyle) {
    gPad->SetGridx();
    gPad->SetGridy();
  }
  gPad->SetLogz();
  cHitCorr->Update();
  moveTPaveStats(hSCTEta2);
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hSCTEta2->Scale(1./(float)iEvents);

}
