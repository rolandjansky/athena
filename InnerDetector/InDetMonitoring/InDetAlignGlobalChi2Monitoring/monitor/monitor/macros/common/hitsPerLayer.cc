/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void hitsPerLayer() {
  TH1F * hHitsPerLayerPixelEndcapA;
  TH1F * hHitsPerLayerPixelBarrel; 
  TH1F * hHitsPerLayerPixelEndcapC;
  TH1F * hHitsPerLayerSCTEndcapA;
  TH1F * hHitsPerLayerSCTBarrel; 
  TH1F * hHitsPerLayerSCTEndcapC;
  
  cout << " Hits per layer..." << endl;
 
  cHitsPerLayer = new TCanvas("HitsPerLayer","Hits per layer",CanvasSizeX6[0],CanvasSizeX6[1]);
  cHitsPerLayer->Divide(3,2);

  cHitsPerLayer->cd(1);
  hHitsPerLayerPixelEndcapA = (TH1F*)file->Get("trkana/PIXHits/hPIXEndcap0Hits_0");
  if (normalizePlots) hHitsPerLayerPixelEndcapA->Scale(1./(float)iEvents);
  hHitsPerLayerPixelEndcapA->Draw();
  hHitsPerLayerPixelEndcapA->SetTitle("Hits per Disk (Pixel EndCapA)");
  hHitsPerLayerPixelEndcapA->GetXaxis()->SetTitle("Pixel EndCapA Disk");
  hHitsPerLayerPixelEndcapA->GetYaxis()->SetTitle("Hits");
  hHitsPerLayerPixelEndcapA->SetLineColor(kRed);
  hHitsPerLayerPixelEndcapA->SetLineWidth(2);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  setTitleFont(font);
  ScalePlots();

  //
  // -----------------------------------------------------------------
  //

  cHitsPerLayer->cd(2);
  hHitsPerLayerPixelBarrel = (TH1F*)file->Get("trkana/PIXHits/hPIXBarrelHits_0");
  if (normalizePlots) hHitsPerLayerPixelBarrel->Scale(1./(float)iEvents);
  hHitsPerLayerPixelBarrel->Draw();
  hHitsPerLayerPixelBarrel->SetTitle("Hits per Layer (Pixel Barrel)");
  hHitsPerLayerPixelBarrel->GetXaxis()->SetTitle("Pixel Barrel Layer");
  hHitsPerLayerPixelBarrel->GetYaxis()->SetTitle("Hits");
  hHitsPerLayerPixelBarrel->SetLineColor(4);
  hHitsPerLayerPixelBarrel->SetLineWidth(2);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  setTitleFont(font);
  ScalePlots();

  //
  // -----------------------------------------------------------------
  //

  cHitsPerLayer->cd(3);
  hHitsPerLayerPixelEndcapC = (TH1F*)file->Get("trkana/PIXHits/hPIXEndcap1Hits_0");
  if (normalizePlots) hHitsPerLayerPixelEndcapC->Scale(1./(float)iEvents);
  hHitsPerLayerPixelEndcapC->Draw();
  hHitsPerLayerPixelEndcapC->SetTitle("Hits per Disk (Pixel EndCapC)");
  hHitsPerLayerPixelEndcapC->GetXaxis()->SetTitle("Pixel EndCapC Disk");
  hHitsPerLayerPixelEndcapC->GetYaxis()->SetTitle("Hits");
  hHitsPerLayerPixelEndcapC->SetLineColor(kRed);
  hHitsPerLayerPixelEndcapC->SetLineWidth(2);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }  
  setTitleFont(font);
  ScalePlots();
  
  //
  // -----------------------------------------------------------------
  //

  cHitsPerLayer->cd(4);
  hHitsPerLayerSCTEndcapA = (TH1F*)file->Get("trkana/SCTHits/hSCTEndcap0Hits_0");
  if (normalizePlots) hHitsPerLayerSCTEndcapA->Scale(1./(float)iEvents);
  hHitsPerLayerSCTEndcapA->Draw();
  hHitsPerLayerSCTEndcapA->SetTitle("Hits per Disk (SCT EndCapA)");
  hHitsPerLayerSCTEndcapA->GetXaxis()->SetTitle("EndCapA Disk");
  hHitsPerLayerSCTEndcapA->GetYaxis()->SetTitle("Hits");
  hHitsPerLayerSCTEndcapA->SetLineColor(kRed);
  hHitsPerLayerSCTEndcapA->SetLineWidth(2);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  setTitleFont(font);
  ScalePlots();

  //
  // -----------------------------------------------------------------
  //

  cHitsPerLayer->cd(5);
  hHitsPerLayerSCTBarrel = (TH1F*)file->Get("trkana/SCTHits/hSCTBarrelHits_0");
  if (normalizePlots) hHitsPerLayerSCTBarrel->Scale(1./(float)iEvents);
  hHitsPerLayerSCTBarrel->Draw();
  hHitsPerLayerSCTBarrel->SetTitle("Hits per Layer (SCT Barrel)");
  hHitsPerLayerSCTBarrel->GetXaxis()->SetTitle("SCT Barrel Layer");
  hHitsPerLayerSCTBarrel->GetYaxis()->SetTitle("Hits");
  hHitsPerLayerSCTBarrel->SetLineColor(4);
  hHitsPerLayerSCTBarrel->SetLineWidth(2);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  setTitleFont(font);
  ScalePlots();

  //
  // -----------------------------------------------------------------
  //

  cHitsPerLayer->cd(6);
  hHitsPerLayerSCTEndcapC = (TH1F*)file->Get("trkana/SCTHits/hSCTEndcap1Hits_0");
  if (normalizePlots) hHitsPerLayerSCTEndcapC->Scale(1./(float)iEvents);
  hHitsPerLayerSCTEndcapC->Draw();
  hHitsPerLayerSCTEndcapC->SetTitle("Hits per Disk (SCT EndCapC)");
  hHitsPerLayerSCTEndcapC->GetXaxis()->SetTitle("SCT EndCapC Disk");
  hHitsPerLayerSCTEndcapC->GetYaxis()->SetTitle("Hits");
  hHitsPerLayerSCTEndcapC->SetLineColor(kRed);
  hHitsPerLayerSCTEndcapC->SetLineWidth(2);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  setTitleFont(font);
  ScalePlots();

}
