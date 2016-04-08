/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void PIXEndcapARes() {

  TH1F *hPIXPhiEndcapAResD0;
  TH1F *hPIXPhiEndcapAResD0Proc;
  TH1F *hPIXPhiEndcapAResD1;
  TH1F *hPIXPhiEndcapAResD1Proc;
  TH1F *hPIXPhiEndcapAResD2;
  TH1F *hPIXPhiEndcapAResD2Proc;
  TH1F *hPIXPhiEndcapAResD0;
  TH1F *hPIXPhiEndcapAResD0Proc;
  TH1F *hPIXEtaEndcapAResD1;
  TH1F *hPIXEtaEndcapAResD1Proc;
  TH1F *hPIXEtaEndcapAResD2;
  TH1F *hPIXEtaEndcapAResD2Proc;
 
  cout << " Pixel EndcapA Residuals per Disc..." << endl;
 
  bool ShowProcessed = true;
  bool ShowFit = showFits;

  if(SR1Ntuple) ShowProcessed = false;

  cPIXEndcapARes = new TCanvas("cPIXEndcapARes","Pixel EndcapA Residuals per Disc",
			       CanvasSizeX6[0],CanvasSizeX6[1]);
  cPIXEndcapARes->Divide(3,2);
  
  cPIXEndcapARes->cd(1);
  hPIXPhiEndcapAResD0 = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiResEndcap0Disc0Wide_0");  
  hPIXPhiEndcapAResD0->SetTitle("Pixel Phi EndcapA Residuals - Disc 0");
  hPIXPhiEndcapAResD0->GetXaxis()->SetTitle("Pixel #phi EndcapA Residuals (mm)");
  hPIXPhiEndcapAResD0->GetYaxis()->SetTitle("Entries");
  hPIXPhiEndcapAResD0->SetLineColor(ColorForAll+2);
  hPIXPhiEndcapAResD0->SetFillColor(ColorForAll);
  hPIXPhiEndcapAResD0->SetLineWidth(1.5);
  hPIXPhiEndcapAResD0->SetFillStyle(3001);
  hPIXPhiEndcapAResD0->GetYaxis()->SetRangeUser(0,1.15*hPIXPhiEndcapAResD0->GetMaximum());
  if (normalizePlots) hPIXPhiEndcapAResD0->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hPIXPhiEndcapAResD0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD0 = DrawFitFunc(hPIXPhiEndcapAResD0,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXPhiResEndcap0Disc0Wide_%d",level);
    hPIXPhiEndcapAResD0Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXPhiEndcapAResD0Proc->Scale(1./(float)iEvents);
    hPIXPhiEndcapAResD0Proc->SetLineColor(ColorForUsed+2);
    hPIXPhiEndcapAResD0Proc->SetFillColor(ColorForUsed);
    hPIXPhiEndcapAResD0Proc->SetLineWidth(1.5);
    hPIXPhiEndcapAResD0Proc->SetFillStyle(3001);
    hPIXPhiEndcapAResD0Proc->DrawCopy("SAME");
    FitValues fitD0_2 = DrawFitFunc(hPIXPhiEndcapAResD0Proc,ColorForUsed-2,ShowFit);
    
    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXPhiEndcapAResD0,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXPhiEndcapAResD0Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0.FitMean,1000*fitD0.FitSigma);
  leg1->AddEntry(hPIXPhiEndcapAResD0,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0_2.FitMean,1000*fitD0_2.FitSigma);
    leg1->AddEntry(hPIXPhiEndcapAResD0Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXEndcapARes->cd(2);
  hPIXPhiEndcapAResD1 = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiResEndcap0Disc1Wide_0");  
  hPIXPhiEndcapAResD1->SetTitle("Pixel Phi EndcapA Residuals - Disk 1");
  hPIXPhiEndcapAResD1->GetXaxis()->SetTitle("Pixel #phi EndcapA Residuals (mm)");
  hPIXPhiEndcapAResD1->GetYaxis()->SetTitle("Entries");
  hPIXPhiEndcapAResD1->SetLineColor(ColorForAll+2);
  hPIXPhiEndcapAResD1->SetFillColor(ColorForAll);
  hPIXPhiEndcapAResD1->SetLineWidth(1.5);
  hPIXPhiEndcapAResD1->SetFillStyle(3001);
  hPIXPhiEndcapAResD1->GetYaxis()->SetRangeUser(0,1.15*hPIXPhiEndcapAResD1->GetMaximum());
  if (normalizePlots) hPIXPhiEndcapAResD1->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hPIXPhiEndcapAResD1->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD1 = DrawFitFunc(hPIXPhiEndcapAResD1,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXPhiResEndcap0Disc1Wide_%d",level);
    hPIXPhiEndcapAResD1Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXPhiEndcapAResD1Proc->Scale(1./(float)iEvents);
    hPIXPhiEndcapAResD1Proc->SetLineColor(ColorForUsed+2);
    hPIXPhiEndcapAResD1Proc->SetFillColor(ColorForUsed);
    hPIXPhiEndcapAResD1Proc->SetLineWidth(1.5);
    hPIXPhiEndcapAResD1Proc->SetFillStyle(3001);
    hPIXPhiEndcapAResD1Proc->DrawCopy("SAME");
    FitValues fitD1_2 = DrawFitFunc(hPIXPhiEndcapAResD1Proc,ColorForUsed-2,ShowFit);
    
    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXPhiEndcapAResD1,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXPhiEndcapAResD1Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1.FitMean,1000*fitD1.FitSigma);
  leg1->AddEntry(hPIXPhiEndcapAResD1,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1_2.FitMean,1000*fitD1_2.FitSigma);
    leg1->AddEntry(hPIXPhiEndcapAResD1Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXEndcapARes->cd(3);
  hPIXPhiEndcapAResD2 = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiResEndcap0Disc2Wide_0");  
  hPIXPhiEndcapAResD2->SetTitle("Pixel Phi EndcapA Residuals - Disc 2");
  hPIXPhiEndcapAResD2->GetXaxis()->SetTitle("Pixel #phi EndcapA Residuals (mm)");
  hPIXPhiEndcapAResD2->GetYaxis()->SetTitle("Entries");
  hPIXPhiEndcapAResD2->SetLineColor(ColorForAll+2);
  hPIXPhiEndcapAResD2->SetFillColor(ColorForAll);
  hPIXPhiEndcapAResD2->SetLineWidth(1.5);
  hPIXPhiEndcapAResD2->SetFillStyle(3001);
  hPIXPhiEndcapAResD2->GetYaxis()->SetRangeUser(0,1.15*hPIXPhiEndcapAResD2->GetMaximum());
  if (normalizePlots) hPIXPhiEndcapAResD2->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hPIXPhiEndcapAResD2->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD2 = DrawFitFunc(hPIXPhiEndcapAResD2,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXPhiResEndcap0Disc2Wide_%d",level);
    hPIXPhiEndcapAResD2Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXPhiEndcapAResD2Proc->Scale(1./(float)iEvents);
    hPIXPhiEndcapAResD2Proc->SetLineColor(ColorForUsed+2);
    hPIXPhiEndcapAResD2Proc->SetFillColor(ColorForUsed);
    hPIXPhiEndcapAResD2Proc->SetLineWidth(1.5);
    hPIXPhiEndcapAResD2Proc->SetFillStyle(3001);
    hPIXPhiEndcapAResD2Proc->DrawCopy("SAME");
    FitValues fitD2_2 = DrawFitFunc(hPIXPhiEndcapAResD2Proc,ColorForUsed-2,ShowFit);
        
    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXPhiEndcapAResD2,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXPhiEndcapAResD2Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2.FitMean,1000*fitD2.FitSigma);
  leg1->AddEntry(hPIXPhiEndcapAResD2,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2_2.FitMean,1000*fitD2_2.FitSigma);
    leg1->AddEntry(hPIXPhiEndcapAResD2Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXEndcapARes->cd(4);
  hPIXEtaEndcapAResD0 = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaResEndcap0Disc0Wide_0");  
  hPIXEtaEndcapAResD0->SetTitle("Pixel Eta EndcapA Residuals - Disc 0");
  hPIXEtaEndcapAResD0->GetXaxis()->SetTitle("Pixel #eta EndcapA Residuals (mm)");
  hPIXEtaEndcapAResD0->GetYaxis()->SetTitle("Entries");
  hPIXEtaEndcapAResD0->SetLineColor(ColorForAll+2);
  hPIXEtaEndcapAResD0->SetFillColor(ColorForAll);
  hPIXEtaEndcapAResD0->SetLineWidth(1.5);
  hPIXEtaEndcapAResD0->SetFillStyle(3001);
  hPIXEtaEndcapAResD0->GetYaxis()->SetRangeUser(0,1.15*hPIXEtaEndcapAResD0->GetMaximum());
  if (normalizePlots) hPIXEtaEndcapAResD3->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hPIXEtaEndcapAResD0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD0 = DrawFitFunc(hPIXEtaEndcapAResD0,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXEtaResEndcap0Disc0Wide_%d",level);
    hPIXEtaEndcapAResD0Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXEtaEndcapAResD0Proc->Scale(1./(float)iEvents);
    hPIXEtaEndcapAResD0Proc->SetLineColor(ColorForUsed+2);
    hPIXEtaEndcapAResD0Proc->SetFillColor(ColorForUsed);
    hPIXEtaEndcapAResD0Proc->SetLineWidth(1.5);
    hPIXEtaEndcapAResD0Proc->SetFillStyle(3001);
    hPIXEtaEndcapAResD0Proc->DrawCopy("SAME");
    FitValues fitD0_2 = DrawFitFunc(hPIXEtaEndcapAResD0Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXEtaEndcapAResD0,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXEtaEndcapAResD0Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }
  
  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0.FitMean,1000*fitD0.FitSigma);
  leg1->AddEntry(hPIXEtaEndcapAResD0,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0_2.FitMean,1000*fitD0_2.FitSigma);
    leg1->AddEntry(hPIXEtaEndcapAResD0Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXEndcapARes->cd(5);
  hPIXEtaEndcapAResD1 = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaResEndcap0Disc1Wide_0");  
  hPIXEtaEndcapAResD1->SetTitle("Pixel Eta EndcapA Residuals - Disc 1");
  hPIXEtaEndcapAResD1->GetXaxis()->SetTitle("Pixel #eta EndcapA Residuals (mm)");
  hPIXEtaEndcapAResD1->GetYaxis()->SetTitle("Entries");
  hPIXEtaEndcapAResD1->SetLineColor(ColorForAll+2);
  hPIXEtaEndcapAResD1->SetFillColor(ColorForAll);
  hPIXEtaEndcapAResD1->SetLineWidth(1.5);
  hPIXEtaEndcapAResD1->SetFillStyle(3001);
  hPIXEtaEndcapAResD1->GetYaxis()->SetRangeUser(0,1.15*hPIXEtaEndcapAResD1->GetMaximum());
  if (normalizePlots) hPIXEtaEndcapAResD1->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hPIXEtaEndcapAResD1->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD1 = DrawFitFunc(hPIXEtaEndcapAResD1,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXEtaResEndcap0Disc1Wide_%d",level);
    hPIXEtaEndcapAResD1Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXEtaEndcapAResD1Proc->Scale(1./(float)iEvents);
    hPIXEtaEndcapAResD1Proc->SetLineColor(ColorForUsed+2);
    hPIXEtaEndcapAResD1Proc->SetFillColor(ColorForUsed);
    hPIXEtaEndcapAResD1Proc->SetLineWidth(1.5);
    hPIXEtaEndcapAResD1Proc->SetFillStyle(3001);
    hPIXEtaEndcapAResD1Proc->DrawCopy("SAME");
    FitValues fitD1_2 = DrawFitFunc(hPIXEtaEndcapAResD1Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);    
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXEtaEndcapAResD1,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXEtaEndcapAResD1Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1.FitMean,1000*fitD1.FitSigma);
  leg1->AddEntry(hPIXEtaEndcapAResD1,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1_2.FitMean,1000*fitD1_2.FitSigma);
    leg1->AddEntry(hPIXEtaEndcapAResD1Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cPIXEndcapARes->cd(6);
  hPIXEtaEndcapAResD2 = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaResEndcap0Disc2Wide_0");  
  hPIXEtaEndcapAResD2->SetTitle("Pixel Eta EndcapA Residuals - Disc 2");
  hPIXEtaEndcapAResD2->GetXaxis()->SetTitle("Pixel #eta EndcapA Residuals (mm)");
  hPIXEtaEndcapAResD2->GetYaxis()->SetTitle("Entries");
  hPIXEtaEndcapAResD2->SetLineColor(ColorForAll+2);
  hPIXEtaEndcapAResD2->SetFillColor(ColorForAll);
  hPIXEtaEndcapAResD2->SetLineWidth(1.5);
  hPIXEtaEndcapAResD2->SetFillStyle(3001);
  hPIXEtaEndcapAResD2->GetYaxis()->SetRangeUser(0,1.15*hPIXEtaEndcapAResD2->GetMaximum());
  if (normalizePlots) hPIXEtaEndcapAResD2->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hPIXEtaEndcapAResD2->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.65);
  FitValues fitD2 = DrawFitFunc(hPIXEtaEndcapAResD2,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/PIXRes/hPIXEtaResEndcap0Disc2Wide_%d",level);
    hPIXEtaEndcapAResD2Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hPIXEtaEndcapAResD2Proc->Scale(1./(float)iEvents);
    hPIXEtaEndcapAResD2Proc->SetLineColor(ColorForUsed+2);
    hPIXEtaEndcapAResD2Proc->SetFillColor(ColorForUsed);
    hPIXEtaEndcapAResD2Proc->SetLineWidth(1.5);
    hPIXEtaEndcapAResD2Proc->SetFillStyle(3001);
    hPIXEtaEndcapAResD2Proc->DrawCopy("SAME");
    FitValues fitD2_2 = DrawFitFunc(hPIXEtaEndcapAResD2Proc,ColorForUsed-2,ShowFit);
    
    TLegend *leg0 = new TLegend(0.64,0.66,0.98,0.78);
    sprintf(name,"All tracks");
    leg0->AddEntry(hPIXEtaEndcapAResD2,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hPIXEtaEndcapAResD2Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.22,0.80,0.67,0.92);
  else TLegend *leg1 = new TLegend(0.65,0.56,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2.FitMean,1000*fitD2.FitSigma);
  leg1->AddEntry(hPIXEtaEndcapAResD2,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2_2.FitMean,1000*fitD2_2.FitSigma);
    leg1->AddEntry(hPIXEtaEndcapAResD2Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  return;

}
