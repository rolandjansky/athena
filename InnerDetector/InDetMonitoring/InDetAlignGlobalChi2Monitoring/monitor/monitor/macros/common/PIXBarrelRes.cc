/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void PIXBarrelRes() {

  //Histograns Phi Residuals
  TH1F *hPIXPhiBarrelResL0;
  TH1F *hPIXPhiBarrelResL0Proc;
  TH1F *hPIXPhiBarrelResL1;
  TH1F *hPIXPhiBarrelResL1Proc;
  TH1F *hPIXPhiBarrelResL2;
  TH1F *hPIXPhiBarrelResL2Proc;

  //Histograms Eta Residuals
  TH1F *hPIXEtaBarrelResL0;
  TH1F *hPIXEtaBarrelResL0Proc;
  TH1F *hPIXEtaBarrelResL1;
  TH1F *hPIXEtaBarrelResL1Proc;
  TH1F *hPIXEtaBarrelResL2;
  TH1F *hPIXEtaBarrelResL2Proc;

  cout << " PIX Barrel Residuals per Layer..." << endl;
 
  bool ShowProcessed = true;
  bool ShowFit = showFits;

  if(SR1Ntuple) ShowProcessed = false;

  cPIXBarrelRes = new TCanvas("cPIXBarrelRes","PIX Barrel Residuals per Layer",
			      CanvasSizeX6[0],CanvasSizeX6[1]);
  cPIXBarrelRes->Divide(3,2);
  
  cPIXBarrelRes->cd(1);
  hPIXPhiBarrelResL0 = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiResBaLay0Wide_0");  
  hPIXPhiBarrelResL0->SetTitle("Pixel Phi Barrel Residuals - Layer 0");
  hPIXPhiBarrelResL0->GetXaxis()->SetTitle("Pixel #phi Barrel Residuals (mm)");
  hPIXPhiBarrelResL0->GetYaxis()->SetTitle("Entries");
  hPIXPhiBarrelResL0->SetLineColor(ColorForAll+2);
  hPIXPhiBarrelResL0->SetFillColor(ColorForAll);
  hPIXPhiBarrelResL0->SetLineWidth(1.5);
  hPIXPhiBarrelResL0->SetFillStyle(3001);
  hPIXPhiBarrelResL0->GetYaxis()->SetRangeUser(0,1.15*hPIXPhiBarrelResL0->GetMaximum());
  if (normalizePlots) hPIXPhiBarrelResL0->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hPIXPhiBarrelResL0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitL0 = DrawFitFunc(hPIXPhiBarrelResL0,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXPhiResBaLay0Wide_%d",level);
    hPIXPhiBarrelResL0Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXPhiBarrelResL0Proc->Scale(1./(float)iEvents);
    hPIXPhiBarrelResL0Proc->SetLineColor(ColorForUsed+2);
    hPIXPhiBarrelResL0Proc->SetFillColor(ColorForUsed);
    hPIXPhiBarrelResL0Proc->SetLineWidth(1.5);
    hPIXPhiBarrelResL0Proc->SetFillStyle(3001);
    hPIXPhiBarrelResL0Proc->DrawCopy("SAME");
    FitValues fitL0_2 = DrawFitFunc(hPIXPhiBarrelResL0Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXPhiBarrelResL0,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXPhiBarrelResL0Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL0.FitMean,1000*fitL0.FitSigma);
  leg1->AddEntry(hPIXPhiBarrelResL0,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL0_2.FitMean,1000*fitL0_2.FitSigma);
    leg1->AddEntry(hPIXPhiBarrelResL0Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXBarrelRes->cd(2);
  hPIXPhiBarrelResL1 = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiResBaLay1Wide_0");  
  hPIXPhiBarrelResL1->SetTitle("Pixel Phi Barrel Residuals - Layer 1");
  hPIXPhiBarrelResL1->GetXaxis()->SetTitle("Pixel #phi Barrel Residuals (mm)");
  hPIXPhiBarrelResL1->GetYaxis()->SetTitle("Entries");
  hPIXPhiBarrelResL1->SetLineColor(ColorForAll+2);
  hPIXPhiBarrelResL1->SetFillColor(ColorForAll);
  hPIXPhiBarrelResL1->SetLineWidth(1.5);
  hPIXPhiBarrelResL1->SetFillStyle(3001);
  hPIXPhiBarrelResL1->GetYaxis()->SetRangeUser(0,1.15*hPIXPhiBarrelResL1->GetMaximum());
  if (normalizePlots) hPIXPhiBarrelResL1->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hPIXPhiBarrelResL1->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitL1 = DrawFitFunc(hPIXPhiBarrelResL1,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXPhiResBaLay1Wide_%d",level);
    hPIXPhiBarrelResL1Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXPhiBarrelResL1Proc->Scale(1./(float)iEvents);
    hPIXPhiBarrelResL1Proc->SetLineColor(ColorForUsed+2);
    hPIXPhiBarrelResL1Proc->SetFillColor(ColorForUsed);
    hPIXPhiBarrelResL1Proc->SetLineWidth(1.5);
    hPIXPhiBarrelResL1Proc->SetFillStyle(3001);
    hPIXPhiBarrelResL1Proc->DrawCopy("SAME");
    FitValues fitL1_2 = DrawFitFunc(hPIXPhiBarrelResL1Proc,ColorForUsed-2,ShowFit);
   
    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXPhiBarrelResL1,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXPhiBarrelResL1Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL1.FitMean,1000*fitL1.FitSigma);
  leg1->AddEntry(hPIXPhiBarrelResL1,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL1_2.FitMean,1000*fitL1_2.FitSigma);
    leg1->AddEntry(hPIXPhiBarrelResL1Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXBarrelRes->cd(3);
  hPIXPhiBarrelResL2 = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiResBaLay2Wide_0");  
  hPIXPhiBarrelResL2->SetTitle("Pixel #phi Barrel Residuals - Layer 2");
  hPIXPhiBarrelResL2->GetXaxis()->SetTitle("Pixel Phi Barrel Residuals (mm)");
  hPIXPhiBarrelResL2->GetYaxis()->SetTitle("Entries");
  hPIXPhiBarrelResL2->SetLineColor(ColorForAll+2);
  hPIXPhiBarrelResL2->SetFillColor(ColorForAll);
  hPIXPhiBarrelResL2->SetLineWidth(1.5);
  hPIXPhiBarrelResL2->SetFillStyle(3001);
  hPIXPhiBarrelResL2->GetYaxis()->SetRangeUser(0,1.15*hPIXPhiBarrelResL2->GetMaximum());
  if (normalizePlots) hPIXPhiBarrelResL2->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hPIXPhiBarrelResL2->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitL2 = DrawFitFunc(hPIXPhiBarrelResL2,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXPhiResBaLay2Wide_%d",level);
    hPIXPhiBarrelResL2Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXPhiBarrelResL2Proc->Scale(1./(float)iEvents);
    hPIXPhiBarrelResL2Proc->SetLineColor(ColorForUsed+2);
    hPIXPhiBarrelResL2Proc->SetFillColor(ColorForUsed);
    hPIXPhiBarrelResL2Proc->SetLineWidth(1.5);
    hPIXPhiBarrelResL2Proc->SetFillStyle(3001);
    hPIXPhiBarrelResL2Proc->DrawCopy("SAME");
    FitValues fitL2_2 = DrawFitFunc(hPIXPhiBarrelResL2Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXPhiBarrelResL2,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXPhiBarrelResL2Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }
  
  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL2.FitMean,1000*fitL2.FitSigma);
  leg1->AddEntry(hPIXPhiBarrelResL2,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL2_2.FitMean,1000*fitL2_2.FitSigma);
    leg1->AddEntry(hPIXPhiBarrelResL2Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //
  cPIXBarrelRes->cd(4);
  hPIXEtaBarrelResL0 = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaResBaLay0Wide_0");  
  hPIXEtaBarrelResL0->SetTitle("Pixel Eta Barrel Residuals - Layer 0");
  hPIXEtaBarrelResL0->GetXaxis()->SetTitle("Pixel #eta Barrel Residuals (mm)");
  hPIXEtaBarrelResL0->GetYaxis()->SetTitle("Entries");
  hPIXEtaBarrelResL0->SetLineColor(ColorForAll+2);
  hPIXEtaBarrelResL0->SetFillColor(ColorForAll);
  hPIXEtaBarrelResL0->SetLineWidth(1.5);
  hPIXEtaBarrelResL0->SetFillStyle(3001);
  hPIXEtaBarrelResL0->GetYaxis()->SetRangeUser(0,1.15*hPIXEtaBarrelResL0->GetMaximum());
  if (normalizePlots) hPIXEtaBarrelResL0->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hPIXEtaBarrelResL0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitL0 = DrawFitFunc(hPIXEtaBarrelResL0,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXEtaResBaLay0Wide_%d",level);
    hPIXEtaBarrelResL0Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXEtaBarrelResL0Proc->Scale(1./(float)iEvents);
    hPIXEtaBarrelResL0Proc->SetLineColor(ColorForUsed+2);
    hPIXEtaBarrelResL0Proc->SetFillColor(ColorForUsed);
    hPIXEtaBarrelResL0Proc->SetLineWidth(1.5);
    hPIXEtaBarrelResL0Proc->SetFillStyle(3001);
    hPIXEtaBarrelResL0Proc->DrawCopy("SAME");
    FitValues fitL0_2 = DrawFitFunc(hPIXEtaBarrelResL0Proc,ColorForUsed-2,ShowFit);
    
    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXEtaBarrelResL0,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXEtaBarrelResL0Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL0.FitMean,1000*fitL0.FitSigma);
  leg1->AddEntry(hPIXEtaBarrelResL0,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL0_2.FitMean,1000*fitL0_2.FitSigma);
    leg1->AddEntry(hPIXEtaBarrelResL0Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();
  
  //
  // -----------------------------------------------------------------
  //

  cPIXBarrelRes->cd(5);
  hPIXEtaBarrelResL1 = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaResBaLay1Wide_0");  
  hPIXEtaBarrelResL1->SetTitle("Pixel Eta Barrel Residuals - Layer 1");
  hPIXEtaBarrelResL1->GetXaxis()->SetTitle("Pixel #eta Barrel Residuals (mm)");
  hPIXEtaBarrelResL1->GetYaxis()->SetTitle("Entries");
  hPIXEtaBarrelResL1->SetLineColor(ColorForAll+2);
  hPIXEtaBarrelResL1->SetFillColor(ColorForAll);
  hPIXEtaBarrelResL1->SetLineWidth(1.5);
  hPIXEtaBarrelResL1->SetFillStyle(3001);
  hPIXEtaBarrelResL1->GetYaxis()->SetRangeUser(0,1.15*hPIXEtaBarrelResL1->GetMaximum());
  if (normalizePlots) hPIXEtaBarrelResL1->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hPIXEtaBarrelResL1->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitL1 = DrawFitFunc(hPIXEtaBarrelResL1,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXEtaResBaLay1Wide_%d",level);
    hPIXEtaBarrelResL1Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXEtaBarrelResL1Proc->Scale(1./(float)iEvents);
    hPIXEtaBarrelResL1Proc->SetLineColor(ColorForUsed+2);
    hPIXEtaBarrelResL1Proc->SetFillColor(ColorForUsed);
    hPIXEtaBarrelResL1Proc->SetLineWidth(1.5);
    hPIXEtaBarrelResL1Proc->SetFillStyle(3001);
    hPIXEtaBarrelResL1Proc->DrawCopy("SAME");
    FitValues fitL1_2 = DrawFitFunc(hPIXEtaBarrelResL1Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXEtaBarrelResL1,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXEtaBarrelResL1Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL1.FitMean,1000*fitL1.FitSigma);
  leg1->AddEntry(hPIXEtaBarrelResL1,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL1_2.FitMean,1000*fitL1_2.FitSigma);
    leg1->AddEntry(hPIXEtaBarrelResL1Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXBarrelRes->cd(6);
  hPIXEtaBarrelResL2 = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaResBaLay2Wide_0");  
  hPIXEtaBarrelResL2->SetTitle("Pixel Eta Barrel Residuals - Layer 2");
  hPIXEtaBarrelResL2->GetXaxis()->SetTitle("Pixel #eta Barrel Residuals (mm)");
  hPIXEtaBarrelResL2->GetYaxis()->SetTitle("Entries");
  hPIXEtaBarrelResL2->SetLineColor(ColorForAll+2);
  hPIXEtaBarrelResL2->SetFillColor(ColorForAll);
  hPIXEtaBarrelResL2->SetLineWidth(1.5);
  hPIXEtaBarrelResL2->SetFillStyle(3001);
  hPIXEtaBarrelResL2->GetYaxis()->SetRangeUser(0,1.15*hPIXEtaBarrelResL2->GetMaximum());
  if (normalizePlots) hPIXEtaBarrelResL2->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hPIXEtaBarrelResL2->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitL2 = DrawFitFunc(hPIXEtaBarrelResL2,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXEtaResBaLay2Wide_%d",level);
    hPIXEtaBarrelResL2Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXEtaBarrelResL2Proc->Scale(1./(float)iEvents);
    hPIXEtaBarrelResL2Proc->SetLineColor(ColorForUsed+2);
    hPIXEtaBarrelResL2Proc->SetFillColor(ColorForUsed);
    hPIXEtaBarrelResL2Proc->SetLineWidth(1.5);
    hPIXEtaBarrelResL2Proc->SetFillStyle(3001);
    hPIXEtaBarrelResL2Proc->DrawCopy("SAME");
    FitValues fitL2_2 = DrawFitFunc(hPIXEtaBarrelResL2Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXEtaBarrelResL2,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXEtaBarrelResL2Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL2.FitMean,1000*fitL2.FitSigma);
  leg1->AddEntry(hPIXEtaBarrelResL2,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL2_2.FitMean,1000*fitL2_2.FitSigma);
    leg1->AddEntry(hPIXEtaBarrelResL2Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //


  return;

}
