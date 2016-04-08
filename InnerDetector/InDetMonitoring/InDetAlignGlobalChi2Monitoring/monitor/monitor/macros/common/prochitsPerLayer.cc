/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void prochitsPerLayer() {

  TH1F * hProcHitsPerLayerPixelEndcapA;
  TH1F * hProcHitsPerLayerPixelBarrel; 
  TH1F * hProcHitsPerLayerPixelEndcapC;
  TH1F * hProcHitsPerLayerSCTEndcapA;
  TH1F * hProcHitsPerLayerSCTBarrel; 
  TH1F * hProcHitsPerLayerSCTEndcapC;

  cout << " Processed Hits per layer..." << endl;
 
  cProcHitsPerLayer = new TCanvas("ProcHitsPerLayer","Processed Hits per layer",CanvasSizeX6[0],CanvasSizeX6[1]);
  cProcHitsPerLayer->Divide(3,2);
  
  cProcHitsPerLayer->cd(1);
  sprintf(name,"trkana/PIXHits/hPIXEndcap0Hits_%d",level);
  hProcHitsPerLayerPixelEndcapA = (TH1F*)file->Get(name);
  if (normalizePlots) hProcHitsPerLayerPixelEndcapA->Scale(1./(float)iEvents);
  if (hProcHitsPerLayerPixelEndcapA->GetMean()==0) 
    doSquare(hProcHitsPerLayerPixelEndcapA, "Pixel EndCapA");
  else {
    hProcHitsPerLayerPixelEndcapA->SetTitle("Processed Hits per Disk (Pixel EndCapA)");
    hProcHitsPerLayerPixelEndcapA->GetXaxis()->SetTitle("Pixel EndCapA Disk");
    hProcHitsPerLayerPixelEndcapA->GetYaxis()->SetTitle("Processed hits");
    hProcHitsPerLayerPixelEndcapA->SetLineColor(kRed);
    hProcHitsPerLayerPixelEndcapA->SetLineWidth(2);
    hProcHitsPerLayerPixelEndcapA->DrawCopy();
    
    if (!AtlasStyle) {
      gPad->SetGridy();
    gPad->SetGridx();
    }
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  cProcHitsPerLayer->cd(2);
  sprintf(name,"trkana/PIXHits/hPIXBarrelHits_%d",level);
  hProcHitsPerLayerPixelBarrel = (TH1F*)file->Get(name);
  if (normalizePlots) hProcHitsPerLayerPixelBarrel->Scale(1./(float)iEvents);
  if (hProcHitsPerLayerPixelBarrel->GetMean()==0)
    doSquare(hProcHitsPerLayerPixelBarrel, "Pixel Barrel");
  else {
    hProcHitsPerLayerPixelBarrel->SetTitle("Processed Hits per Layer (Pixel Barrel)");
    hProcHitsPerLayerPixelBarrel->GetXaxis()->SetTitle("Pixel Barrel Layer");
    hProcHitsPerLayerPixelBarrel->GetYaxis()->SetTitle("Processed hits");
    hProcHitsPerLayerPixelBarrel->SetLineColor(4);
    hProcHitsPerLayerPixelBarrel->SetLineWidth(2);
    hProcHitsPerLayerPixelBarrel->DrawCopy();
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }
  
  //
  // -----------------------------------------------------------------
  //

  cProcHitsPerLayer->cd(3);
  sprintf(name,"trkana/PIXHits/hPIXEndcap1Hits_%d",level);
  hProcHitsPerLayerPixelEndcapC = (TH1F*)file->Get(name);
  if (normalizePlots) hProcHitsPerLayerPixelEndcapC->Scale(1./(float)iEvents);
  if (hProcHitsPerLayerPixelEndcapC->GetMean()==0)
    doSquare(hProcHitsPerLayerPixelEndcapC, "Pixel EndCapC");
  else {
    hProcHitsPerLayerPixelEndcapC->SetTitle("Processed Hits per Disk (Pixel EndCapC)");
    hProcHitsPerLayerPixelEndcapC->GetXaxis()->SetTitle("Pixel EndCapC Disk");
    hProcHitsPerLayerPixelEndcapC->GetYaxis()->SetTitle("Processed hits");
    hProcHitsPerLayerPixelEndcapC->SetLineColor(kRed);
    hProcHitsPerLayerPixelEndcapC->SetLineWidth(2);
    hProcHitsPerLayerPixelEndcapC->DrawCopy();
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  cProcHitsPerLayer->cd(4);
  sprintf(name,"trkana/SCTHits/hSCTEndcap0Hits_%d",level);
  hProcHitsPerLayerSCTEndcapA = (TH1F*)file->Get(name);
  if (normalizePlots) hProcHitsPerLayerSCTEndcapA->Scale(1./(float)iEvents);
  if (hProcHitsPerLayerSCTEndcapA->GetMean()==0)
    doSquare(hProcHitsPerLayerSCTEndcapA, "SCT EndCapA");
  else {
    hProcHitsPerLayerSCTEndcapA->SetTitle("Processed Hits per Disk (SCT EndCapA)");
    hProcHitsPerLayerSCTEndcapA->GetXaxis()->SetTitle("EndCapA Disk");
    hProcHitsPerLayerSCTEndcapA->GetYaxis()->SetTitle("Processed hits");
    hProcHitsPerLayerSCTEndcapA->SetLineColor(kRed);
    hProcHitsPerLayerSCTEndcapA->SetLineWidth(2);
    hProcHitsPerLayerSCTEndcapA->DrawCopy();
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  cProcHitsPerLayer->cd(5);
  sprintf(name,"trkana/SCTHits/hSCTBarrelHits_%d",level);
  hProcHitsPerLayerSCTBarrel = (TH1F*)file->Get(name);
  if (normalizePlots) hProcHitsPerLayerSCTBarrel->Scale(1./(float)iEvents);
  if (hProcHitsPerLayerSCTBarrel->GetMean()==0)
    doSquare(hProcHitsPerLayerSCTBarrel, "SCT Barrel");
  else {
    hProcHitsPerLayerSCTBarrel->SetTitle("Processed Hits per Layer (SCT Barrel)");
    hProcHitsPerLayerSCTBarrel->GetXaxis()->SetTitle("SCT Barrel Layer");
    hProcHitsPerLayerSCTBarrel->GetYaxis()->SetTitle("Processed hits");
    hProcHitsPerLayerSCTBarrel->SetLineColor(4);
    hProcHitsPerLayerSCTBarrel->SetLineWidth(2);
    hProcHitsPerLayerSCTBarrel->DrawCopy();
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  cProcHitsPerLayer->cd(6);
  sprintf(name,"trkana/SCTHits/hSCTEndcap1Hits_%d",level);
  hProcHitsPerLayerSCTEndcapC = (TH1F*)file->Get(name);
  if (normalizePlots) hProcHitsPerLayerSCTEndcapC->Scale(1./(float)iEvents);
  if (hProcHitsPerLayerSCTEndcapC->GetMean()==0)
    doSquare(hProcHitsPerLayerSCTEndcapC, "SCT EndCapC");
  else {
    hProcHitsPerLayerSCTEndcapC->SetTitle("Processed Hits per Disk (SCT EndCapC)");
    hProcHitsPerLayerSCTEndcapC->GetXaxis()->SetTitle("SCT EndCapC Disk");
    hProcHitsPerLayerSCTEndcapC->GetYaxis()->SetTitle("Processed hits");
    hProcHitsPerLayerSCTEndcapC->SetLineColor(kRed);
    hProcHitsPerLayerSCTEndcapC->SetLineWidth(2);
    hProcHitsPerLayerSCTEndcapC->DrawCopy();
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }
  
  cProcHitsPerLayer->Update();

  return;

}
