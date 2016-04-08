/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void AllProcessedResiduals() {

  TH1F *hPixel_ProcTrkPhiRes;
  TH1F *hPixel_ProcTrkEtaRes;
  TH1F *hSCT_ProcTrkRes;
  TH1F *hPixel_EndCapProcTrkPhiRes;
  TH1F *hPixel_EndCapProcTrkEtaRes;
  TH1F *hEndcapSCT_ProcTrkRes;

  cout << " Processed Residuals..." << endl;

  cProcAllRes = new TCanvas("AllProcessedResiduals","All Processed Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
  cProcAllRes->Divide(3,2);
  
  cProcAllRes->cd(1);
  sprintf(name,"trkana/PIXRes/hPIXPhiResBaWide_%d",level);
  hPixel_ProcTrkPhiRes = (TH1F*)file->Get(name);
  if (hPixel_ProcTrkPhiRes->GetMean()==0)
    doSquare(hPixel_ProcTrkPhiRes, "Pixel Barrel");
  else {
    hPixel_ProcTrkPhiRes->SetTitle("Pixel #phi Processed Res (Barrel)");
    hPixel_ProcTrkPhiRes->GetXaxis()->SetTitle("Pixel Barrel #phi Residuals (mm)");
    hPixel_ProcTrkPhiRes->GetYaxis()->SetTitle("Entries");
    hPixel_ProcTrkPhiRes->SetLineColor(ColorForUsed+2);
    hPixel_ProcTrkPhiRes->SetFillColor(ColorForUsed);
    hPixel_ProcTrkPhiRes->SetLineWidth(1.5);
    hPixel_ProcTrkPhiRes->SetFillStyle(3001);
    if (normalizePlots)  hPixel_ProcTrkPhiRes->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hPixel_ProcTrkPhiRes->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  cProcAllRes->cd(2); 
  sprintf(name,"trkana/PIXRes/hPIXEtaResBa_%d",level);
  hPixel_ProcTrkEtaRes = (TH1F*)file->Get(name);
  if (hPixel_ProcTrkEtaRes->GetMean()==0)
    doSquare(hPixel_ProcTrkEtaRes, "Pixel Barrel");
  else {
    hPixel_ProcTrkEtaRes->SetTitle("Pixel #eta Processed Res (Barrel)");
    hPixel_ProcTrkEtaRes->GetXaxis()->SetTitle("Pixel Barrel #eta Residuals (mm)");
    hPixel_ProcTrkEtaRes->GetYaxis()->SetTitle("Entries");
    hPixel_ProcTrkEtaRes->SetLineColor(ColorForUsed+2);
    hPixel_ProcTrkEtaRes->SetFillColor(ColorForUsed);
    hPixel_ProcTrkEtaRes->SetLineWidth(1.5);
    hPixel_ProcTrkEtaRes ->SetFillStyle(3001);
    if (normalizePlots) hPixel_ProcTrkEtaRes->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hPixel_ProcTrkEtaRes->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  cProcAllRes->cd(3);
  sprintf(name,"trkana/SCTRes/hSCTResBaWide_%d",level);
  hSCT_ProcTrkRes = (TH1F*)file->Get(name);
  if (hSCT_ProcTrkRes->GetMean()==0)
    doSquare(hSCT_ProcTrkRes, "SCT Barrel");
  else {
    hSCT_ProcTrkRes->SetTitle("SCT Processed Res (Barrel)");
    hSCT_ProcTrkRes->GetXaxis()->SetTitle("SCT Barrel Residuals (mm)");
    hSCT_ProcTrkRes->GetYaxis()->SetTitle("Entries");
    hSCT_ProcTrkRes->SetLineColor(ColorForUsed+2);
    hSCT_ProcTrkRes->SetFillColor(ColorForUsed);
    hSCT_ProcTrkRes->SetLineWidth(1.5);
    hSCT_ProcTrkRes->SetFillStyle(3001);
    if (normalizePlots) hSCT_ProcTrkRes->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hSCT_ProcTrkRes->DrawCopy();  
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  cProcAllRes->cd(4);
  sprintf(name,"trkana/PIXRes/hPIXPhiResEndcap_%d",level);
  hPixel_EndCapProcTrkPhiRes = (TH1F*)file->Get(name);
  if (hPixel_EndCapProcTrkPhiRes->GetMean()==0)
    doSquare(hPixel_EndCapProcTrkPhiRes, "Pixel EndCap");
  else {
    hPixel_EndCapProcTrkPhiRes->SetTitle("Pixel #phi Processed Res (EndCaps)");
    hPixel_EndCapProcTrkPhiRes->GetXaxis()->SetTitle("Pixel EndCap #phi Residuals (mm)");
    hPixel_EndCapProcTrkPhiRes->GetYaxis()->SetTitle("Entries");
    hPixel_EndCapProcTrkPhiRes->SetLineColor(ColorForUsed+2);
    hPixel_EndCapProcTrkPhiRes->SetFillColor(ColorForUsed);
    hPixel_EndCapProcTrkPhiRes->SetLineWidth(1.5);
    hPixel_EndCapProcTrkPhiRes->SetFillStyle(3001);
    if (normalizePlots) hPixel_EndCapProcTrkPhiRes->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); } 
    hPixel_EndCapProcTrkPhiRes->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  cProcAllRes->cd(5);
  sprintf(name,"trkana/PIXRes/hPIXEtaResEndcap_%d",level);
  hPixel_EndCapProcTrkEtaRes=(TH1F*)file->Get(name);
  if (hPixel_EndCapProcTrkEtaRes->GetMean()==0)
    doSquare(hPixel_EndCapProcTrkEtaRes, "Pixel EndCap");
  else {
    hPixel_EndCapProcTrkEtaRes->SetTitle("Pixel #eta Processed Res (EndCaps)");
    hPixel_EndCapProcTrkEtaRes->GetXaxis()->SetTitle("Pixel EndCap #eta Residuals (mm)");
    hPixel_EndCapProcTrkEtaRes->GetYaxis()->SetTitle("Entries");
    hPixel_EndCapProcTrkEtaRes->SetLineColor(ColorForUsed+2);
    hPixel_EndCapProcTrkEtaRes->SetFillColor(ColorForUsed);
    hPixel_EndCapProcTrkEtaRes->SetLineWidth(1.5);
    hPixel_EndCapProcTrkEtaRes->SetFillStyle(3001);
    if (normalizePlots) hPixel_EndCapProcTrkEtaRes->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hPixel_EndCapProcTrkEtaRes->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  cProcAllRes->cd(6);
  sprintf(name,"trkana/SCTRes/hSCTResEndcap_%d",level);
  hEndCapSCT_ProcTrkRes = (TH1F*)file->Get(name);
  if (hEndCapSCT_ProcTrkRes->GetMean()==0)
    doSquare(hEndCapSCT_ProcTrkRes, "SCT EndCap");
  else {
    hEndCapSCT_ProcTrkRes->SetTitle("SCT Processed Res (EndCaps)");
    hEndCapSCT_ProcTrkRes->GetXaxis()->SetTitle("SCT EndCap Residuals (mm)");
    hEndCapSCT_ProcTrkRes->GetYaxis()->SetTitle("Entries");
    hEndCapSCT_ProcTrkRes->SetLineColor(ColorForUsed+2);
    hEndCapSCT_ProcTrkRes->SetFillColor(ColorForUsed);
    hEndCapSCT_ProcTrkRes->SetLineWidth(1.5);
    hEndCapSCT_ProcTrkRes->SetFillStyle(3001);
    if (normalizePlots) hEndCapSCT_ProcTrkRes->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hEndCapSCT_ProcTrkRes->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }
  
  cProcAllRes->Update();
    
}
