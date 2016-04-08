/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void PIXEndcapCRes() {

  TH1F *hPIXPhiEndcapCResD0;
  TH1F *hPIXPhiEndcapCResD0Proc;
  TH1F *hPIXPhiEndcapCResD1;
  TH1F *hPIXPhiEndcapCResD1Proc;
  TH1F *hPIXPhiEndcapCResD2;
  TH1F *hPIXPhiEndcapCResD2Proc;
  TH1F *hPIXPhiEndcapCResD0;
  TH1F *hPIXPhiEndcapCResD0Proc;
  TH1F *hPIXEtaEndcapCResD1;
  TH1F *hPIXEtaEndcapCResD1Proc;
  TH1F *hPIXEtaEndcapCResD2;
  TH1F *hPIXEtaEndcapCResD2Proc;
 
  cout << " Pixel EndcapC Residuals per Disc..." << endl;
 
  bool ShowProcessed = true;
  bool ShowFit = showFits;

  if(SR1Ntuple) ShowProcessed = false;

  cPIXEndcapCRes = new TCanvas("cPIXEndcapCRes","Pixel EndcapC Residuals per Disc",
			       CanvasSizeX6[0],CanvasSizeX6[1]);
  cPIXEndcapCRes->Divide(3,2);
  
  cPIXEndcapCRes->cd(1);
  hPIXPhiEndcapCResD0 = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiResEndcap1Disc0Wide_0");  
  hPIXPhiEndcapCResD0->SetTitle("Pixel Phi EndcapC Residuals - Disc 0");
  hPIXPhiEndcapCResD0->GetXaxis()->SetTitle("Pixel #phi EndcapC Residuals (mm)");
  hPIXPhiEndcapCResD0->GetYaxis()->SetTitle("Entries");
  hPIXPhiEndcapCResD0->SetLineColor(ColorForAll+2);
  hPIXPhiEndcapCResD0->SetFillColor(ColorForAll);
  hPIXPhiEndcapCResD0->SetLineWidth(1.5);
  hPIXPhiEndcapCResD0->SetFillStyle(3001);
  hPIXPhiEndcapCResD0->GetYaxis()->SetRangeUser(0,1.15*hPIXPhiEndcapCResD0->GetMaximum());
  if (normalizePlots) hPIXPhiEndcapCResD0->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hPIXPhiEndcapCResD0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD0 = DrawFitFunc(hPIXPhiEndcapCResD0,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXPhiResEndcap1Disc0Wide_%d",level);
    hPIXPhiEndcapCResD0Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXPhiEndcapCResD0Proc->Scale(1./(float)iEvents);
    hPIXPhiEndcapCResD0Proc->SetLineColor(ColorForUsed+2);
    hPIXPhiEndcapCResD0Proc->SetFillColor(ColorForUsed);
    hPIXPhiEndcapCResD0Proc->SetLineWidth(1.5);
    hPIXPhiEndcapCResD0Proc->SetFillStyle(3001);
    hPIXPhiEndcapCResD0Proc->DrawCopy("SAME");
    FitValues fitD0_2 = DrawFitFunc(hPIXPhiEndcapCResD0Proc,ColorForUsed-2,ShowFit);
    
    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXPhiEndcapCResD0,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXPhiEndcapCResD0Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0.FitMean,1000*fitD0.FitSigma);
  leg1->AddEntry(hPIXPhiEndcapCResD0,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0_2.FitMean,1000*fitD0_2.FitSigma);
    leg1->AddEntry(hPIXPhiEndcapCResD0Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();
  
  //
  // -----------------------------------------------------------------
  //

  cPIXEndcapCRes->cd(2);
  hPIXPhiEndcapCResD1 = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiResEndcap1Disc1Wide_0");  
  hPIXPhiEndcapCResD1->SetTitle("Pixel Phi EndcapC Residuals - Disc 1");
  hPIXPhiEndcapCResD1->GetXaxis()->SetTitle("Pixel #phi EndcapC Residuals (mm)");
  hPIXPhiEndcapCResD1->GetYaxis()->SetTitle("Entries");
  hPIXPhiEndcapCResD1->SetLineColor(ColorForAll+2);
  hPIXPhiEndcapCResD1->SetFillColor(ColorForAll);
  hPIXPhiEndcapCResD1->SetLineWidth(1.5);
  hPIXPhiEndcapCResD1->SetFillStyle(3001);
  hPIXPhiEndcapCResD1->GetYaxis()->SetRangeUser(0,1.15*hPIXPhiEndcapCResD1->GetMaximum());
  if (normalizePlots) hPIXPhiEndcapCResD1->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hPIXPhiEndcapCResD1->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD1 = DrawFitFunc(hPIXPhiEndcapCResD1,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXPhiResEndcap1Disc1Wide_%d",level);
    hPIXPhiEndcapCResD1Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXPhiEndcapCResD1Proc->Scale(1./(float)iEvents);
    hPIXPhiEndcapCResD1Proc->SetLineColor(ColorForUsed+2);
    hPIXPhiEndcapCResD1Proc->SetFillColor(ColorForUsed);
    hPIXPhiEndcapCResD1Proc->SetLineWidth(1.5);
    hPIXPhiEndcapCResD1Proc->SetFillStyle(3001);
    hPIXPhiEndcapCResD1Proc->DrawCopy("SAME");
    FitValues fitD1_2 = DrawFitFunc(hPIXPhiEndcapCResD1Proc,ColorForUsed-2,ShowFit);
    
    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXPhiEndcapCResD1,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXPhiEndcapCResD1Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1.FitMean,1000*fitD1.FitSigma);
  leg1->AddEntry(hPIXPhiEndcapCResD1,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1_2.FitMean,1000*fitD1_2.FitSigma);
    leg1->AddEntry(hPIXPhiEndcapCResD1Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXEndcapCRes->cd(3);
  hPIXPhiEndcapCResD2 = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiResEndcap1Disc2Wide_0");  
  hPIXPhiEndcapCResD2->SetTitle("Pixel Phi EndcapC Residuals - Disc 2");
  hPIXPhiEndcapCResD2->GetXaxis()->SetTitle("Pixel #phi EndcapC Residuals (mm)");
  hPIXPhiEndcapCResD2->GetYaxis()->SetTitle("Entries");
  hPIXPhiEndcapCResD2->SetLineColor(ColorForAll+2);
  hPIXPhiEndcapCResD2->SetFillColor(ColorForAll);
  hPIXPhiEndcapCResD2->SetLineWidth(1.5);
  hPIXPhiEndcapCResD2->SetFillStyle(3001);
  hPIXPhiEndcapCResD2->GetYaxis()->SetRangeUser(0,1.15*hPIXPhiEndcapCResD2->GetMaximum());
  if (normalizePlots) hPIXPhiEndcapCResD2->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hPIXPhiEndcapCResD2->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD2 = DrawFitFunc(hPIXPhiEndcapCResD2,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXPhiResEndcap1Disc2Wide_%d",level);
    hPIXPhiEndcapCResD2Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXPhiEndcapCResD2Proc->Scale(1./(float)iEvents);
    hPIXPhiEndcapCResD2Proc->SetLineColor(ColorForUsed+2);
    hPIXPhiEndcapCResD2Proc->SetFillColor(ColorForUsed);
    hPIXPhiEndcapCResD2Proc->SetLineWidth(1.5);
    hPIXPhiEndcapCResD2Proc->SetFillStyle(3001);
    hPIXPhiEndcapCResD2Proc->DrawCopy("SAME");
    FitValues fitD2_2 = DrawFitFunc(hPIXPhiEndcapCResD2Proc,ColorForUsed-2,ShowFit);
        
    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXPhiEndcapCResD2,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXPhiEndcapCResD2Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2.FitMean,1000*fitD2.FitSigma);
  leg1->AddEntry(hPIXPhiEndcapCResD2,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2_2.FitMean,1000*fitD2_2.FitSigma);
    leg1->AddEntry(hPIXPhiEndcapCResD2Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXEndcapCRes->cd(4);
  hPIXEtaEndcapCResD0 = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaResEndcap1Disc0Wide_0");  
  hPIXEtaEndcapCResD0->SetTitle("Pixel Eta EndcapC Residuals - Disc 0");
  hPIXEtaEndcapCResD0->GetXaxis()->SetTitle("Pixel #eta EndcapC Residuals (mm)");
  hPIXEtaEndcapCResD0->GetYaxis()->SetTitle("Entries");
  hPIXEtaEndcapCResD0->SetLineColor(ColorForAll+2);
  hPIXEtaEndcapCResD0->SetFillColor(ColorForAll);
  hPIXEtaEndcapCResD0->SetLineWidth(1.5);
  hPIXEtaEndcapCResD0->SetFillStyle(3001);
  hPIXEtaEndcapCResD0->GetYaxis()->SetRangeUser(0,1.15*hPIXEtaEndcapCResD0->GetMaximum());
  if (normalizePlots) hPIXEtaEndcapCResD3->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hPIXEtaEndcapCResD0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD0 = DrawFitFunc(hPIXEtaEndcapCResD0,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXEtaResEndcap1Disc0Wide_%d",level);
    hPIXEtaEndcapCResD0Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXEtaEndcapCResD0Proc->Scale(1./(float)iEvents);
    hPIXEtaEndcapCResD0Proc->SetLineColor(ColorForUsed+2);
    hPIXEtaEndcapCResD0Proc->SetFillColor(ColorForUsed);
    hPIXEtaEndcapCResD0Proc->SetLineWidth(1.5);
    hPIXEtaEndcapCResD0Proc->SetFillStyle(3001);
    hPIXEtaEndcapCResD0Proc->DrawCopy("SAME");
    FitValues fitD0_2 = DrawFitFunc(hPIXEtaEndcapCResD0Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXEtaEndcapCResD0,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXEtaEndcapCResD0Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }
  
  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0.FitMean,1000*fitD0.FitSigma);
  leg1->AddEntry(hPIXEtaEndcapCResD0,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0_2.FitMean,1000*fitD0_2.FitSigma);
    leg1->AddEntry(hPIXEtaEndcapCResD0Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXEndcapCRes->cd(5);
  hPIXEtaEndcapCResD1 = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaResEndcap1Disc1Wide_0");  
  hPIXEtaEndcapCResD1->SetTitle("Pixel Eta EndcapC Residuals - Disc 1");
  hPIXEtaEndcapCResD1->GetXaxis()->SetTitle("Pixel #eta EndcapC Residuals (mm)");
  hPIXEtaEndcapCResD1->GetYaxis()->SetTitle("Entries");
  hPIXEtaEndcapCResD1->SetLineColor(ColorForAll+2);
  hPIXEtaEndcapCResD1->SetFillColor(ColorForAll);
  hPIXEtaEndcapCResD1->SetLineWidth(1.5);
  hPIXEtaEndcapCResD1->SetFillStyle(3001);
  hPIXEtaEndcapCResD1->GetYaxis()->SetRangeUser(0,1.15*hPIXEtaEndcapCResD1->GetMaximum());
  if (normalizePlots) hPIXEtaEndcapCResD1->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hPIXEtaEndcapCResD1->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD1 = DrawFitFunc(hPIXEtaEndcapCResD1,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXEtaResEndcap1Disc1Wide_%d",level);
    hPIXEtaEndcapCResD1Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXEtaEndcapCResD1Proc->Scale(1./(float)iEvents);
    hPIXEtaEndcapCResD1Proc->SetLineColor(ColorForUsed+2);
    hPIXEtaEndcapCResD1Proc->SetFillColor(ColorForUsed);
    hPIXEtaEndcapCResD1Proc->SetLineWidth(1.5);
    hPIXEtaEndcapCResD1Proc->SetFillStyle(3001);
    hPIXEtaEndcapCResD1Proc->DrawCopy("SAME");
    FitValues fitD1_2 = DrawFitFunc(hPIXEtaEndcapCResD1Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);    
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXEtaEndcapCResD1,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXEtaEndcapCResD1Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1.FitMean,1000*fitD1.FitSigma);
  leg1->AddEntry(hPIXEtaEndcapCResD1,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1_2.FitMean,1000*fitD1_2.FitSigma);
    leg1->AddEntry(hPIXEtaEndcapCResD1Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXEndcapCRes->cd(6);
  hPIXEtaEndcapCResD2 = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaResEndcap1Disc2Wide_0");  
  hPIXEtaEndcapCResD2->SetTitle("Pixel Eta EndcapC Residuals - Disc 2");
  hPIXEtaEndcapCResD2->GetXaxis()->SetTitle("Pixel #eta EndcapC Residuals (mm)");
  hPIXEtaEndcapCResD2->GetYaxis()->SetTitle("Entries");
  hPIXEtaEndcapCResD2->SetLineColor(ColorForAll+2);
  hPIXEtaEndcapCResD2->SetFillColor(ColorForAll);
  hPIXEtaEndcapCResD2->SetLineWidth(1.5);
  hPIXEtaEndcapCResD2->SetFillStyle(3001);
  hPIXEtaEndcapCResD2->GetYaxis()->SetRangeUser(0,1.15*hPIXEtaEndcapCResD2->GetMaximum());
  if (normalizePlots) hPIXEtaEndcapCResD2->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hPIXEtaEndcapCResD2->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD2 = DrawFitFunc(hPIXEtaEndcapCResD2,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXEtaResEndcap1Disc2Wide_%d",level);
    hPIXEtaEndcapCResD2Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXEtaEndcapCResD2Proc->Scale(1./(float)iEvents);
    hPIXEtaEndcapCResD2Proc->SetLineColor(ColorForUsed+2);
    hPIXEtaEndcapCResD2Proc->SetFillColor(ColorForUsed);
    hPIXEtaEndcapCResD2Proc->SetLineWidth(1.5);
    hPIXEtaEndcapCResD2Proc->SetFillStyle(3001);
    hPIXEtaEndcapCResD2Proc->DrawCopy("SAME");
    FitValues fitD2_2 = DrawFitFunc(hPIXEtaEndcapCResD2Proc,ColorForUsed-2,ShowFit);
    
    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXEtaEndcapCResD2,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXEtaEndcapCResD2Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2.FitMean,1000*fitD2.FitSigma);
  leg1->AddEntry(hPIXEtaEndcapCResD2,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2_2.FitMean,1000*fitD2_2.FitSigma);
    leg1->AddEntry(hPIXEtaEndcapCResD2Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  return;
}
