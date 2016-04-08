/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void AllResiduals() {
  TH1F *hPixel_TrkPhiRes;
  TH1F *hPixel_TrkEtaRes;
  TH1F *h1BarrelSCT_TrkRes;
  TH1F *hPixel_EndCapTrkPhiRes;
  TH1F *hPixel_EndCapTrkEtaRes;
  TH1F *h1EndCapSCT_TrkRes;

  cout << " Residuals..." << endl;

  cAllRes = new TCanvas("AllResiduals","All Residuals",CanvasSizeX6[0],CanvasSizeX6[1]);
  cAllRes->Divide(3,2);
  
  cAllRes->cd(1);
  hPixel_TrkPhiRes = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiResBa_0");  
  hPixel_TrkPhiRes->SetTitle("Pixel #phi Residuals (Barrel)");
  hPixel_TrkPhiRes->GetXaxis()->SetTitle("Pixel Barrel #phi Residuals (mm)");
  hPixel_TrkPhiRes->GetYaxis()->SetTitle("Entries");
  hPixel_TrkPhiRes->SetLineColor(ColorForAll+2);
  hPixel_TrkPhiRes->SetFillColor(ColorForAll);
  hPixel_TrkPhiRes->SetLineWidth(1.5);
  hPixel_TrkPhiRes->SetFillStyle(3001);
  if (normalizePlots) hPixel_TrkPhiRes->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  if (hPixel_TrkPhiRes->GetMean()!=0.0) {
    hPixel_TrkPhiRes->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }
  else {
    hPixel_TrkPhiRes->Delete();
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(42);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"Pixel Barrel");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);

    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }

  //
  // -----------------------------------------------------------------
  //

  cAllRes->cd(2);
  hPixel_TrkEtaRes = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaResBa_0");
  hPixel_TrkEtaRes->SetTitle("Pixel #eta Residuals (Barrel)");
  hPixel_TrkEtaRes->GetXaxis()->SetTitle("Pixel Barrel #eta Residuals (mm)");
  hPixel_TrkEtaRes->GetYaxis()->SetTitle("Entries");
  hPixel_TrkEtaRes->SetLineColor(ColorForAll+2);
  hPixel_TrkEtaRes->SetLineWidth(1.5);
  hPixel_TrkEtaRes->SetFillColor(ColorForAll);
  hPixel_TrkEtaRes->SetFillStyle(3001);
  if (normalizePlots) hPixel_TrkEtaRes->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  if (hPixel_TrkEtaRes->GetMean()!=0.0) {
    hPixel_TrkEtaRes->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }
  else {
    hPixel_TrkEtaRes->Delete();
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(42);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"Pixel Barrel");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);

    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }

  //
  // -----------------------------------------------------------------
  //

  cAllRes->cd(3);
  h1BarrelSCT_TrkRes = (TH1F*)file->Get("trkana/SCTRes/hSCTResBaWide_0");
  h1BarrelSCT_TrkRes->SetTitle("SCT Residuals (Barrel)");
  h1BarrelSCT_TrkRes->GetXaxis()->SetTitle("SCT Barrel Residuals (mm)");
  h1BarrelSCT_TrkRes->GetYaxis()->SetTitle("Entries");
  h1BarrelSCT_TrkRes->SetLineColor(ColorForAll+2);
  h1BarrelSCT_TrkRes->SetFillColor(ColorForAll);
  h1BarrelSCT_TrkRes->SetLineWidth(1.5);
  h1BarrelSCT_TrkRes->SetFillStyle(3001);
  if (normalizePlots) h1BarrelSCT_TrkRes->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  if (h1BarrelSCT_TrkRes->GetMean()!=0.0) {
    h1BarrelSCT_TrkRes->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }
  else {
    h1BarrelSCT_TrkRes->Delete();
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(42);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"SCT Barrel");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);
    
    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }

  //
  // -----------------------------------------------------------------
  //

  cAllRes->cd(4);
  hPixel_EndCapTrkPhiRes = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiResEndcap_0");  
  hPixel_EndCapTrkPhiRes->SetTitle("Pixel #phi Residuals (EndCaps)");
  hPixel_EndCapTrkPhiRes->GetXaxis()->SetTitle("Pixel EndCaps #phi Residuals (mm)");
  hPixel_EndCapTrkPhiRes->GetYaxis()->SetTitle("Entries");
  hPixel_EndCapTrkPhiRes->SetLineColor(ColorForAll+2);
  hPixel_EndCapTrkPhiRes->SetFillColor(ColorForAll);
  hPixel_EndCapTrkPhiRes->SetLineWidth(1.5);
  hPixel_EndCapTrkPhiRes->SetFillStyle(3001);
  if (normalizePlots) hPixel_EndCapTrkPhiRes->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  if (hPixel_EndCapTrkPhiRes->GetMean()!=0.0) {
    hPixel_EndCapTrkPhiRes->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }
  else {
    hPixel_EndCapTrkPhiRes->Delete();
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(42);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"Pixel EndCaps");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);

    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }

  //
  // -----------------------------------------------------------------
  //

  cAllRes->cd(5);
  hPixel_EndCapTrkEtaRes = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaResEndcap_0");  
  hPixel_EndCapTrkEtaRes->SetTitle("Pixel #eta Residuals (EndCaps)");
  hPixel_EndCapTrkEtaRes->GetXaxis()->SetTitle("Pixel EndCaps #eta Residuals (mm)");
  hPixel_EndCapTrkEtaRes->GetYaxis()->SetTitle("Entries");
  hPixel_EndCapTrkEtaRes->SetLineColor(ColorForAll+2);
  hPixel_EndCapTrkEtaRes->SetFillColor(ColorForAll);
  hPixel_EndCapTrkEtaRes->SetLineWidth(1.5);
  hPixel_EndCapTrkEtaRes->SetFillStyle(3001);
  if (normalizePlots) hPixel_EndCapTrkEtaRes->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  if (hPixel_EndCapTrkEtaRes->GetMean()!=0.0) {
    hPixel_EndCapTrkEtaRes->DrawCopy();  
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }
  else {
    hPixel_EndCapTrkEtaRes->Delete();
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(42);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"Pixel EndCaps");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);

    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }

  //
  // -----------------------------------------------------------------
  //

  cAllRes->cd(6);
  h1EndCapSCT_TrkRes = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap_0");
  h1EndCapSCT_TrkRes->SetTitle("SCT Residuals (EndCaps)");
  h1EndCapSCT_TrkRes->GetXaxis()->SetTitle("SCT EndCaps Residuals (mm)");
  h1EndCapSCT_TrkRes->GetYaxis()->SetTitle("Entries");
  h1EndCapSCT_TrkRes->SetLineColor(ColorForAll+2);
  h1EndCapSCT_TrkRes->SetFillColor(ColorForAll);
  h1EndCapSCT_TrkRes->SetLineWidth(1.5);
  h1EndCapSCT_TrkRes->SetFillStyle(3001);
  if (normalizePlots) h1EndCapSCT_TrkRes->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  if (h1EndCapSCT_TrkRes->GetMean()!=0.0) {
    h1EndCapSCT_TrkRes->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  }
  else {
    h1EndCapSCT_TrkRes->Delete();
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(42);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"SCT EndCaps");
    t->DrawLatex(0.3,0.5,name);
    sprintf(name,"no processed");
    t->DrawLatex(0.33,0.44,name);

    TLine *TLineBorder;
    TLineBorder = new TLine(0.05,0.90,0.92,0.90);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.10,0.92,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.05,0.90,0.05,0.10);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.92,0.90,0.92,0.10);
    TLineBorder->Draw();
  }
  
  return;

}
