/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void SCTBarrelRes() {

  TH1F *hSCTBarrelResL0;
  TH1F *hSCTBarrelResL0Proc;
  TH1F *hSCTBarrelResL1;
  TH1F *hSCTBarrelResL1Proc;
  TH1F *hSCTBarrelResL2;
  TH1F *hSCTBarrelResL2Proc;
  TH1F *hSCTBarrelResL3;
  TH1F *hSCTBarrelResL3Proc;

  cout << " SCT Barrel Residuals per Layer..." << endl;
 
  bool ShowProcessed = true;
  bool ShowFit = showFits;

  if(SR1Ntuple) ShowProcessed = false;

  cSCTBarrelRes = new TCanvas("cSCTBarrelRes","SCT Barrel Residuals per Layer",CanvasSizeX4[0],CanvasSizeX4[1]);
  cSCTBarrelRes->Divide(2,2);
  
  cSCTBarrelRes->cd(1);
  hSCTBarrelResL0 = (TH1F*)file->Get("trkana/SCTRes/hSCTResBaLay0Wide_0");  
  hSCTBarrelResL0->SetTitle("SCT Barrel Residuals - Layer 0");
  hSCTBarrelResL0->GetXaxis()->SetTitle("SCT Barrel Residuals (mm)");
  hSCTBarrelResL0->GetYaxis()->SetTitle("Entries");
  hSCTBarrelResL0->SetLineColor(ColorForAll+2);
  hSCTBarrelResL0->SetFillColor(ColorForAll);
  hSCTBarrelResL0->SetLineWidth(1.5);
  hSCTBarrelResL0->SetFillStyle(3001);
  if (normalizePlots) hSCTBarrelResL0->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTBarrelResL0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitL0 = DrawFitFunc(hSCTBarrelResL0,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResBaLay0Wide_%d",level);
    hSCTBarrelResL0Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTBarrelResL0Proc->Scale(1./(float)iEvents);
    hSCTBarrelResL0Proc->SetLineColor(ColorForUsed+2);
    hSCTBarrelResL0Proc->SetFillColor(ColorForUsed);
    hSCTBarrelResL0Proc->SetLineWidth(1.5);
    hSCTBarrelResL0Proc->SetFillStyle(3001);
    hSCTBarrelResL0Proc->DrawCopy("SAME");
    FitValues fitL0_2 = DrawFitFunc(hSCTBarrelResL0Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTBarrelResL0,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTBarrelResL0Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL0.FitMean,1000*fitL0.FitSigma);
  leg1->AddEntry(hSCTBarrelResL0,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL0_2.FitMean,1000*fitL0_2.FitSigma);
    leg1->AddEntry(hSCTBarrelResL0Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();
  
  //
  // -----------------------------------------------------------------
  //

  cSCTBarrelRes->cd(2);
  hSCTBarrelResL1 = (TH1F*)file->Get("trkana/SCTRes/hSCTResBaLay1Wide_0");  
  hSCTBarrelResL1->SetTitle("SCT Barrel Residuals - Layer 1");
  hSCTBarrelResL1->GetXaxis()->SetTitle("SCT Barrel Residuals (mm)");
  hSCTBarrelResL1->GetYaxis()->SetTitle("Entries");
  hSCTBarrelResL1->SetLineColor(ColorForAll+2);
  hSCTBarrelResL1->SetFillColor(ColorForAll);
  hSCTBarrelResL1->SetLineWidth(1.5);
  hSCTBarrelResL1->SetFillStyle(3001);
  if (normalizePlots) hSCTBarrelResL1->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTBarrelResL1->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitL1 = DrawFitFunc(hSCTBarrelResL1,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResBaLay1Wide_%d",level);
    hSCTBarrelResL1Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTBarrelResL1Proc->Scale(1./(float)iEvents);
    hSCTBarrelResL1Proc->SetLineColor(ColorForUsed+2);
    hSCTBarrelResL1Proc->SetFillColor(ColorForUsed);
    hSCTBarrelResL1Proc->SetLineWidth(1.5);
    hSCTBarrelResL1Proc->SetFillStyle(3001);
    hSCTBarrelResL1Proc->DrawCopy("SAME");
    FitValues fitL1_2 = DrawFitFunc(hSCTBarrelResL1Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTBarrelResL1,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTBarrelResL1Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL1.FitMean,1000*fitL1.FitSigma);
  leg1->AddEntry(hSCTBarrelResL1,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL1_2.FitMean,1000*fitL1_2.FitSigma);
    leg1->AddEntry(hSCTBarrelResL1Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cSCTBarrelRes->cd(3);
  hSCTBarrelResL2 = (TH1F*)file->Get("trkana/SCTRes/hSCTResBaLay2Wide_0");  
  hSCTBarrelResL2->SetTitle("SCT Barrel Residuals - Layer 2");
  hSCTBarrelResL2->GetXaxis()->SetTitle("SCT Barrel Residuals (mm)");
  hSCTBarrelResL2->GetYaxis()->SetTitle("Entries");
  hSCTBarrelResL2->SetLineColor(ColorForAll+2);
  hSCTBarrelResL2->SetFillColor(ColorForAll);
  hSCTBarrelResL2->SetLineWidth(1.5);
  hSCTBarrelResL2->SetFillStyle(3001);
  if (normalizePlots) hSCTBarrelResL2->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTBarrelResL2->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitL2 = DrawFitFunc(hSCTBarrelResL2,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResBaLay2Wide_%d",level);
    hSCTBarrelResL2Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTBarrelResL2Proc->Scale(1./(float)iEvents);
    hSCTBarrelResL2Proc->SetLineColor(ColorForUsed+2);
    hSCTBarrelResL2Proc->SetFillColor(ColorForUsed);
    hSCTBarrelResL2Proc->SetLineWidth(1.5);
    hSCTBarrelResL2Proc->SetFillStyle(3001);
    hSCTBarrelResL2Proc->DrawCopy("SAME");
    FitValues fitL2_2 = DrawFitFunc(hSCTBarrelResL2Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTBarrelResL2,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTBarrelResL2Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL2.FitMean,1000*fitL2.FitSigma);
  leg1->AddEntry(hSCTBarrelResL2,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL2_2.FitMean,1000*fitL2_2.FitSigma);
    leg1->AddEntry(hSCTBarrelResL2Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cSCTBarrelRes->cd(4);
  hSCTBarrelResL3 = (TH1F*)file->Get("trkana/SCTRes/hSCTResBaLay3Wide_0");  
  hSCTBarrelResL3->SetTitle("SCT Barrel Residuals - Layer 3");
  hSCTBarrelResL3->GetXaxis()->SetTitle("SCT Barrel Residuals (mm)");
  hSCTBarrelResL3->GetYaxis()->SetTitle("Entries");
  hSCTBarrelResL3->SetLineColor(ColorForAll+2);
  hSCTBarrelResL3->SetFillColor(ColorForAll);
  hSCTBarrelResL3->SetLineWidth(1.5);
  hSCTBarrelResL3->SetFillStyle(3001);
  if (normalizePlots) hSCTBarrelResL3->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTBarrelResL3->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitL3 = DrawFitFunc(hSCTBarrelResL3,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResBaLay3Wide_%d",level);
    hSCTBarrelResL3Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTBarrelResL3Proc->Scale(1./(float)iEvents);
    hSCTBarrelResL3Proc->SetLineColor(ColorForUsed+2);
    hSCTBarrelResL3Proc->SetFillColor(ColorForUsed);
    hSCTBarrelResL3Proc->SetLineWidth(1.5);
    hSCTBarrelResL3Proc->SetFillStyle(3001);
    hSCTBarrelResL3Proc->DrawCopy("SAME");
    FitValues fitL3_2 = DrawFitFunc(hSCTBarrelResL3Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTBarrelResL3,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTBarrelResL3Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL3.FitMean,1000*fitL3.FitSigma);
  leg1->AddEntry(hSCTBarrelResL3,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitL3_2.FitMean,1000*fitL3_2.FitSigma);
    leg1->AddEntry(hSCTBarrelResL3Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  return;

}
