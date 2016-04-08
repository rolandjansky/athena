/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void SCTEndcapCRes() {

  TH1F *hSCTEndcapCResD0;
  TH1F *hSCTEndcapCResD0Proc;
  TH1F *hSCTEndcapCResD1;
  TH1F *hSCTEndcapCResD1Proc;
  TH1F *hSCTEndcapCResD2;
  TH1F *hSCTEndcapCResD2Proc;
  TH1F *hSCTEndcapCResD3;
  TH1F *hSCTEndcapCResD3Proc;
  TH1F *hSCTEndcapCResD4;
  TH1F *hSCTEndcapCResD4Proc;
  TH1F *hSCTEndcapCResD5;
  TH1F *hSCTEndcapCResD5Proc;
  TH1F *hSCTEndcapCResD6;
  TH1F *hSCTEndcapCResD6Proc;
  TH1F *hSCTEndcapCResD7;
  TH1F *hSCTEndcapCResD7Proc;
  TH1F *hSCTEndcapCResD8;
  TH1F *hSCTEndcapCResD8Proc;
 
  cout << " SCT EndcapC Residuals per Disc..." << endl;
 
  bool ShowProcessed = true;
  bool ShowFit = showFits;

  if(SR1Ntuple) ShowProcessed = false;

  cSCTEndcapCRes = new TCanvas("cSCTEndcapCRes","SCT EndcapC Residuals per Discr",
			       CanvasSizeX9[0],CanvasSizeX9[1],CanvasSizeX9[2],CanvasSizeX9[3]);
  cSCTEndcapCRes->Divide(3,3);
  
  cSCTEndcapCRes->cd(1);
  hSCTEndcapCResD0 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap1Disc0Wide_0");  
  hSCTEndcapCResD0->SetTitle("SCT ECC Residuals - Disc 0");
  hSCTEndcapCResD0->GetXaxis()->SetTitle("SCT EndcapC Residuals (mm)");
  hSCTEndcapCResD0->GetYaxis()->SetTitle("Entries");
  hSCTEndcapCResD0->SetLineColor(ColorForAll+2);
  hSCTEndcapCResD0->SetFillColor(ColorForAll);
  hSCTEndcapCResD0->SetLineWidth(1.5);
  hSCTEndcapCResD0->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapCResD0->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapCResD0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD0 = DrawFitFunc(hSCTEndcapCResD0,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap1Disc0Wide_%d",level);
    hSCTEndcapCResD0Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapCResD0Proc->Scale(1./(float)iEvents);
    hSCTEndcapCResD0Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapCResD0Proc->SetFillColor(ColorForUsed);
    hSCTEndcapCResD0Proc->SetLineWidth(1.5);
    hSCTEndcapCResD0Proc->SetFillStyle(3001);
    hSCTEndcapCResD0Proc->DrawCopy("SAME");
    FitValues fitD0_2 = DrawFitFunc(hSCTEndcapCResD0Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapCResD0,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapCResD0Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0.FitMean,1000*fitD0.FitSigma);
  leg1->AddEntry(hSCTEndcapCResD0,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0_2.FitMean,1000*fitD0_2.FitSigma);
    leg1->AddEntry(hSCTEndcapCResD0Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();
  

 
  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapCRes->cd(2);
  hSCTEndcapCResD1 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap1Disc1Wide_0");  
  hSCTEndcapCResD1->SetTitle("SCT ECC Residuals - Disc 1");
  hSCTEndcapCResD1->GetXaxis()->SetTitle("SCT EndcapC Residuals (mm)");
  hSCTEndcapCResD1->GetYaxis()->SetTitle("Entries");
  hSCTEndcapCResD1->SetLineColor(ColorForAll+2);
  hSCTEndcapCResD1->SetFillColor(ColorForAll);
  hSCTEndcapCResD1->SetLineWidth(1.5);
  hSCTEndcapCResD1->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapCResD1->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapCResD1->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD1 = DrawFitFunc(hSCTEndcapCResD1,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap1Disc1Wide_%d",level);
    hSCTEndcapCResD1Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapCResD1Proc->Scale(1./(float)iEvents);
    hSCTEndcapCResD1Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapCResD1Proc->SetFillColor(ColorForUsed);
    hSCTEndcapCResD1Proc->SetLineWidth(1.5);
    hSCTEndcapCResD1Proc->SetFillStyle(3001);
    hSCTEndcapCResD1Proc->DrawCopy("SAME");
    FitValues fitD1_2 = DrawFitFunc(hSCTEndcapCResD1Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapCResD1,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapCResD1Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1.FitMean,1000*fitD1.FitSigma);
  leg1->AddEntry(hSCTEndcapCResD1,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1_2.FitMean,1000*fitD1_2.FitSigma);
    leg1->AddEntry(hSCTEndcapCResD1Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapCRes->cd(3);
  hSCTEndcapCResD2 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap1Disc2Wide_0");  
  hSCTEndcapCResD2->SetTitle("SCT ECC Residuals - Disc 2");
  hSCTEndcapCResD2->GetXaxis()->SetTitle("SCT EndcapC Residuals (mm)");
  hSCTEndcapCResD2->GetYaxis()->SetTitle("Entries");
  hSCTEndcapCResD2->SetLineColor(ColorForAll+2);
  hSCTEndcapCResD2->SetFillColor(ColorForAll);
  hSCTEndcapCResD2->SetLineWidth(1.5);
  hSCTEndcapCResD2->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapCResD2->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapCResD2->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD2 = DrawFitFunc(hSCTEndcapCResD2,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap1Disc2Wide_%d",level);
    hSCTEndcapCResD2Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapCResD2Proc->Scale(1./(float)iEvents);
    hSCTEndcapCResD2Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapCResD2Proc->SetFillColor(ColorForUsed);
    hSCTEndcapCResD2Proc->SetLineWidth(1.5);
    hSCTEndcapCResD2Proc->SetFillStyle(3001);
    hSCTEndcapCResD2Proc->DrawCopy("SAME");
    FitValues fitD2_2 = DrawFitFunc(hSCTEndcapCResD2Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapCResD2,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapCResD2Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2.FitMean,1000*fitD2.FitSigma);
  leg1->AddEntry(hSCTEndcapCResD2,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2_2.FitMean,1000*fitD2_2.FitSigma);
    leg1->AddEntry(hSCTEndcapCResD2Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapCRes->cd(4);
  hSCTEndcapCResD3 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap1Disc3Wide_0");  
  hSCTEndcapCResD3->SetTitle("SCT ECC Residuals - Disc 3");
  hSCTEndcapCResD3->GetXaxis()->SetTitle("SCT EndcapC Residuals (mm)");
  hSCTEndcapCResD3->GetYaxis()->SetTitle("Entries");
  hSCTEndcapCResD3->SetLineColor(ColorForAll+2);
  hSCTEndcapCResD3->SetFillColor(ColorForAll);
  hSCTEndcapCResD3->SetLineWidth(1.5);
  hSCTEndcapCResD3->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapCResD3->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapCResD3->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD3 = DrawFitFunc(hSCTEndcapCResD3,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap1Disc3Wide_%d",level);
    hSCTEndcapCResD3Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapCResD3Proc->Scale(1./(float)iEvents);
    hSCTEndcapCResD3Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapCResD3Proc->SetFillColor(ColorForUsed);
    hSCTEndcapCResD3Proc->SetLineWidth(1.5);
    hSCTEndcapCResD3Proc->SetFillStyle(3001);
    hSCTEndcapCResD3Proc->DrawCopy("SAME");
    FitValues fitD3_2 = DrawFitFunc(hSCTEndcapCResD3Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapCResD3,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapCResD3Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD3.FitMean,1000*fitD3.FitSigma);
  leg1->AddEntry(hSCTEndcapCResD3,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD3_2.FitMean,1000*fitD3_2.FitSigma);
    leg1->AddEntry(hSCTEndcapCResD3Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();




  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapCRes->cd(5);
  hSCTEndcapCResD4 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap1Disc4Wide_0");  
  hSCTEndcapCResD4->SetTitle("SCT ECC Residuals - Disc 4");
  hSCTEndcapCResD4->GetXaxis()->SetTitle("SCT EndcapC Residuals (mm)");
  hSCTEndcapCResD4->GetYaxis()->SetTitle("Entries");
  hSCTEndcapCResD4->SetLineColor(ColorForAll+2);
  hSCTEndcapCResD4->SetFillColor(ColorForAll);
  hSCTEndcapCResD4->SetLineWidth(1.5);
  hSCTEndcapCResD4->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapCResD4->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapCResD4->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD4 = DrawFitFunc(hSCTEndcapCResD4,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap1Disc4Wide_%d",level);
    hSCTEndcapCResD4Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapCResD4Proc->Scale(1./(float)iEvents);
    hSCTEndcapCResD4Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapCResD4Proc->SetFillColor(ColorForUsed);
    hSCTEndcapCResD4Proc->SetLineWidth(1.5);
    hSCTEndcapCResD4Proc->SetFillStyle(3001);
    hSCTEndcapCResD4Proc->DrawCopy("SAME");
    FitValues fitD4_2 = DrawFitFunc(hSCTEndcapCResD4Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapCResD4,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapCResD4Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD4.FitMean,1000*fitD4.FitSigma);
  leg1->AddEntry(hSCTEndcapCResD4,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD4_2.FitMean,1000*fitD4_2.FitSigma);
    leg1->AddEntry(hSCTEndcapCResD4Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();





  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapCRes->cd(6);
  hSCTEndcapCResD5 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap1Disc5Wide_0");  
  hSCTEndcapCResD5->SetTitle("SCT ECC Residuals - Disc 5");
  hSCTEndcapCResD5->GetXaxis()->SetTitle("SCT EndcapC Residuals (mm)");
  hSCTEndcapCResD5->GetYaxis()->SetTitle("Entries");
  hSCTEndcapCResD5->SetLineColor(ColorForAll+2);
  hSCTEndcapCResD5->SetFillColor(ColorForAll);
  hSCTEndcapCResD5->SetLineWidth(1.5);
  hSCTEndcapCResD5->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapCResD5->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapCResD5->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD5 = DrawFitFunc(hSCTEndcapCResD5,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap1Disc5Wide_%d",level);
    hSCTEndcapCResD5Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapCResD5Proc->Scale(1./(float)iEvents);
    hSCTEndcapCResD5Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapCResD5Proc->SetFillColor(ColorForUsed);
    hSCTEndcapCResD5Proc->SetLineWidth(1.5);
    hSCTEndcapCResD5Proc->SetFillStyle(3001);
    hSCTEndcapCResD5Proc->DrawCopy("SAME");
    FitValues fitD5_2 = DrawFitFunc(hSCTEndcapCResD5Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapCResD5,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapCResD5Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD5.FitMean,1000*fitD5.FitSigma);
  leg1->AddEntry(hSCTEndcapCResD5,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD5_2.FitMean,1000*fitD5_2.FitSigma);
    leg1->AddEntry(hSCTEndcapCResD5Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();



  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapCRes->cd(7);
  hSCTEndcapCResD6 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap1Disc6Wide_0");  
  hSCTEndcapCResD6->SetTitle("SCT ECC Residuals - Disc 6");
  hSCTEndcapCResD6->GetXaxis()->SetTitle("SCT EndcapC Residuals (mm)");
  hSCTEndcapCResD6->GetYaxis()->SetTitle("Entries");
  hSCTEndcapCResD6->SetLineColor(ColorForAll+2);
  hSCTEndcapCResD6->SetFillColor(ColorForAll);
  hSCTEndcapCResD6->SetLineWidth(1.5);
  hSCTEndcapCResD6->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapCResD6->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapCResD6->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD6 = DrawFitFunc(hSCTEndcapCResD6,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap1Disc6Wide_%d",level);
    hSCTEndcapCResD6Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapCResD6Proc->Scale(1./(float)iEvents);
    hSCTEndcapCResD6Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapCResD6Proc->SetFillColor(ColorForUsed);
    hSCTEndcapCResD6Proc->SetLineWidth(1.5);
    hSCTEndcapCResD6Proc->SetFillStyle(3001);
    hSCTEndcapCResD6Proc->DrawCopy("SAME");
    FitValues fitD6_2 = DrawFitFunc(hSCTEndcapCResD6Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapCResD6,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapCResD6Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD6.FitMean,1000*fitD6.FitSigma);
  leg1->AddEntry(hSCTEndcapCResD6,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD6_2.FitMean,1000*fitD6_2.FitSigma);
    leg1->AddEntry(hSCTEndcapCResD6Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();





  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapCRes->cd(8);
  hSCTEndcapCResD7 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap1Disc7Wide_0");  
  hSCTEndcapCResD7->SetTitle("SCT ECC Residuals - Disc 7");
  hSCTEndcapCResD7->GetXaxis()->SetTitle("SCT EndcapC Residuals (mm)");
  hSCTEndcapCResD7->GetYaxis()->SetTitle("Entries");
  hSCTEndcapCResD7->SetLineColor(ColorForAll+2);
  hSCTEndcapCResD7->SetFillColor(ColorForAll);
  hSCTEndcapCResD7->SetLineWidth(1.5);
  hSCTEndcapCResD7->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapCResD7->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapCResD7->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD7 = DrawFitFunc(hSCTEndcapCResD7,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap1Disc7Wide_%d",level);
    hSCTEndcapCResD7Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapCResD7Proc->Scale(1./(float)iEvents);
    hSCTEndcapCResD7Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapCResD7Proc->SetFillColor(ColorForUsed);
    hSCTEndcapCResD7Proc->SetLineWidth(1.5);
    hSCTEndcapCResD7Proc->SetFillStyle(3001);
    hSCTEndcapCResD7Proc->DrawCopy("SAME");
    FitValues fitD7_2 = DrawFitFunc(hSCTEndcapCResD7Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapCResD7,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapCResD7Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD7.FitMean,1000*fitD7.FitSigma);
  leg1->AddEntry(hSCTEndcapCResD7,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD7_2.FitMean,1000*fitD7_2.FitSigma);
    leg1->AddEntry(hSCTEndcapCResD7Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();


  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapCRes->cd(9);
  hSCTEndcapCResD8 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap1Disc8Wide_0");  
  hSCTEndcapCResD8->SetTitle("SCT ECC Residuals - Disc 8");
  hSCTEndcapCResD8->GetXaxis()->SetTitle("SCT EndcapC Residuals (mm)");
  hSCTEndcapCResD8->GetYaxis()->SetTitle("Entries");
  hSCTEndcapCResD8->SetLineColor(ColorForAll+2);
  hSCTEndcapCResD8->SetFillColor(ColorForAll);
  hSCTEndcapCResD8->SetLineWidth(1.5);
  hSCTEndcapCResD8->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapCResD8->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapCResD8->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD8 = DrawFitFunc(hSCTEndcapCResD8,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap1Disc8Wide_%d",level);
    hSCTEndcapCResD8Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapCResD8Proc->Scale(1./(float)iEvents);
    hSCTEndcapCResD8Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapCResD8Proc->SetFillColor(ColorForUsed);
    hSCTEndcapCResD8Proc->SetLineWidth(1.5);
    hSCTEndcapCResD8Proc->SetFillStyle(3001);
    hSCTEndcapCResD8Proc->DrawCopy("SAME");
    FitValues fitD8_2 = DrawFitFunc(hSCTEndcapCResD8Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapCResD8,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapCResD8Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD8.FitMean,1000*fitD8.FitSigma);
  leg1->AddEntry(hSCTEndcapCResD8,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD8_2.FitMean,1000*fitD8_2.FitSigma);
    leg1->AddEntry(hSCTEndcapCResD8Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  return;

}
