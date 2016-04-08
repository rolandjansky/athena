/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void SCTEndcapARes() {

  TH1F *hSCTEndcapAResD0;
  TH1F *hSCTEndcapAResD0Proc;
  TH1F *hSCTEndcapAResD1;
  TH1F *hSCTEndcapAResD1Proc;
  TH1F *hSCTEndcapAResD2;
  TH1F *hSCTEndcapAResD2Proc;
  TH1F *hSCTEndcapAResD3;
  TH1F *hSCTEndcapAResD3Proc;
  TH1F *hSCTEndcapAResD4;
  TH1F *hSCTEndcapAResD4Proc;
  TH1F *hSCTEndcapAResD5;
  TH1F *hSCTEndcapAResD5Proc;
  TH1F *hSCTEndcapAResD6;
  TH1F *hSCTEndcapAResD6Proc;
  TH1F *hSCTEndcapAResD7;
  TH1F *hSCTEndcapAResD7Proc;
  TH1F *hSCTEndcapAResD8;
  TH1F *hSCTEndcapAResD8Proc;
 
  cout << " SCT EndcapA Residuals per Disc..." << endl;
 
  bool ShowProcessed = true;
  bool ShowFit = showFits;

  if(SR1Ntuple) ShowProcessed = false;

  cSCTEndcapARes = new TCanvas("cSCTEndcapARes","SCT EndcapA Residuals per Disc",
			       CanvasSizeX9[0],CanvasSizeX9[1],CanvasSizeX9[2],CanvasSizeX9[3]);
  cSCTEndcapARes->Divide(3,3);
  
  cSCTEndcapARes->cd(1);
  hSCTEndcapAResD0 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap0Disc0Wide_0");  
  hSCTEndcapAResD0->SetTitle("SCT ECA Residuals - Disc 0");
  hSCTEndcapAResD0->GetXaxis()->SetTitle("SCT EndcapA Residuals (mm)");
  hSCTEndcapAResD0->GetYaxis()->SetTitle("Entries");
  hSCTEndcapAResD0->SetLineColor(ColorForAll+2);
  hSCTEndcapAResD0->SetFillColor(ColorForAll);
  hSCTEndcapAResD0->SetLineWidth(1.5);
  hSCTEndcapAResD0->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapAResD0->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapAResD0->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD0 = DrawFitFunc(hSCTEndcapAResD0,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap0Disc0Wide_%d",level);
    hSCTEndcapAResD0Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapAResD0Proc->Scale(1./(float)iEvents);
    hSCTEndcapAResD0Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapAResD0Proc->SetFillColor(ColorForUsed);
    hSCTEndcapAResD0Proc->SetLineWidth(1.5);
    hSCTEndcapAResD0Proc->SetFillStyle(3001);
    hSCTEndcapAResD0Proc->DrawCopy("SAME");
    FitValues fitD0_2 = DrawFitFunc(hSCTEndcapAResD0Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapAResD0,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapAResD0Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

 
  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0.FitMean,1000*fitD0.FitSigma);
  leg1->AddEntry(hSCTEndcapAResD0,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD0_2.FitMean,1000*fitD0_2.FitSigma);
    leg1->AddEntry(hSCTEndcapAResD0Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();
  

 
  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapARes->cd(2);
  hSCTEndcapAResD1 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap0Disc1Wide_0");  
  hSCTEndcapAResD1->SetTitle("SCT ECA Residuals - Disc 1");
  hSCTEndcapAResD1->GetXaxis()->SetTitle("SCT EndcapA Residuals (mm)");
  hSCTEndcapAResD1->GetYaxis()->SetTitle("Entries");
  hSCTEndcapAResD1->SetLineColor(ColorForAll+2);
  hSCTEndcapAResD1->SetFillColor(ColorForAll);
  hSCTEndcapAResD1->SetLineWidth(1.5);
  hSCTEndcapAResD1->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapAResD1->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapAResD1->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD1 = DrawFitFunc(hSCTEndcapAResD1,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap0Disc1Wide_%d",level);
    hSCTEndcapAResD1Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapAResD1Proc->Scale(1./(float)iEvents);
    hSCTEndcapAResD1Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapAResD1Proc->SetFillColor(ColorForUsed);
    hSCTEndcapAResD1Proc->SetLineWidth(1.5);
    hSCTEndcapAResD1Proc->SetFillStyle(3001);
    hSCTEndcapAResD1Proc->DrawCopy("SAME");
    FitValues fitD1_2 = DrawFitFunc(hSCTEndcapAResD1Proc,ColorForUsed-2,ShowFit);

    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapAResD1,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapAResD1Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1.FitMean,1000*fitD1.FitSigma);
  leg1->AddEntry(hSCTEndcapAResD1,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD1_2.FitMean,1000*fitD1_2.FitSigma);
    leg1->AddEntry(hSCTEndcapAResD1Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapARes->cd(3);
  hSCTEndcapAResD2 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap0Disc2Wide_0");  
  hSCTEndcapAResD2->SetTitle("SCT ECA Residuals - Disc 2");
  hSCTEndcapAResD2->GetXaxis()->SetTitle("SCT EndcapA Residuals (mm)");
  hSCTEndcapAResD2->GetYaxis()->SetTitle("Entries");
  hSCTEndcapAResD2->SetLineColor(ColorForAll+2);
  hSCTEndcapAResD2->SetFillColor(ColorForAll);
  hSCTEndcapAResD2->SetLineWidth(1.5);
  hSCTEndcapAResD2->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapAResD2->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapAResD2->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD2 = DrawFitFunc(hSCTEndcapAResD2,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap0Disc2Wide_%d",level);
    hSCTEndcapAResD2Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapAResD2Proc->Scale(1./(float)iEvents);
    hSCTEndcapAResD2Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapAResD2Proc->SetFillColor(ColorForUsed);
    hSCTEndcapAResD2Proc->SetLineWidth(1.5);
    hSCTEndcapAResD2Proc->SetFillStyle(3001);
    hSCTEndcapAResD2Proc->DrawCopy("SAME");
    FitValues fitD2_2 = DrawFitFunc(hSCTEndcapAResD2Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapAResD2,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapAResD2Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2.FitMean,1000*fitD2.FitSigma);
  leg1->AddEntry(hSCTEndcapAResD2,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD2_2.FitMean,1000*fitD2_2.FitSigma);
    leg1->AddEntry(hSCTEndcapAResD2Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapARes->cd(4);
  hSCTEndcapAResD3 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap0Disc3Wide_0");  
  hSCTEndcapAResD3->SetTitle("SCT ECA Residuals - Disc 3");
  hSCTEndcapAResD3->GetXaxis()->SetTitle("SCT EndcapA Residuals (mm)");
  hSCTEndcapAResD3->GetYaxis()->SetTitle("Entries");
  hSCTEndcapAResD3->SetLineColor(ColorForAll+2);
  hSCTEndcapAResD3->SetFillColor(ColorForAll);
  hSCTEndcapAResD3->SetLineWidth(1.5);
  hSCTEndcapAResD3->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapAResD3->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapAResD3->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD3 = DrawFitFunc(hSCTEndcapAResD3,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap0Disc3Wide_%d",level);
    hSCTEndcapAResD3Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapAResD3Proc->Scale(1./(float)iEvents);
    hSCTEndcapAResD3Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapAResD3Proc->SetFillColor(ColorForUsed);
    hSCTEndcapAResD3Proc->SetLineWidth(1.5);
    hSCTEndcapAResD3Proc->SetFillStyle(3001);
    hSCTEndcapAResD3Proc->DrawCopy("SAME");
    FitValues fitD3_2 = DrawFitFunc(hSCTEndcapAResD3Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapAResD3,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapAResD3Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD3.FitMean,1000*fitD3.FitSigma);
  leg1->AddEntry(hSCTEndcapAResD3,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD3_2.FitMean,1000*fitD3_2.FitSigma);
    leg1->AddEntry(hSCTEndcapAResD3Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();




  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapARes->cd(5);
  hSCTEndcapAResD4 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap0Disc4Wide_0");  
  hSCTEndcapAResD4->SetTitle("SCT ECA Residuals - Disc 4");
  hSCTEndcapAResD4->GetXaxis()->SetTitle("SCT EndcapA Residuals (mm)");
  hSCTEndcapAResD4->GetYaxis()->SetTitle("Entries");
  hSCTEndcapAResD4->SetLineColor(ColorForAll+2);
  hSCTEndcapAResD4->SetFillColor(ColorForAll);
  hSCTEndcapAResD4->SetLineWidth(1.5);
  hSCTEndcapAResD4->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapAResD4->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapAResD4->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD4 = DrawFitFunc(hSCTEndcapAResD4,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap0Disc4Wide_%d",level);
    hSCTEndcapAResD4Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapAResD4Proc->Scale(1./(float)iEvents);
    hSCTEndcapAResD4Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapAResD4Proc->SetFillColor(ColorForUsed);
    hSCTEndcapAResD4Proc->SetLineWidth(1.5);
    hSCTEndcapAResD4Proc->SetFillStyle(3001);
    hSCTEndcapAResD4Proc->DrawCopy("SAME");
    FitValues fitD4_2 = DrawFitFunc(hSCTEndcapAResD4Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapAResD4,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapAResD4Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD4.FitMean,1000*fitD4.FitSigma);
  leg1->AddEntry(hSCTEndcapAResD4,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD4_2.FitMean,1000*fitD4_2.FitSigma);
    leg1->AddEntry(hSCTEndcapAResD4Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();





  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapARes->cd(6);
  hSCTEndcapAResD5 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap0Disc5Wide_0");  
  hSCTEndcapAResD5->SetTitle("SCT ECA Residuals - Disc 5");
  hSCTEndcapAResD5->GetXaxis()->SetTitle("SCT EndcapA Residuals (mm)");
  hSCTEndcapAResD5->GetYaxis()->SetTitle("Entries");
  hSCTEndcapAResD5->SetLineColor(ColorForAll+2);
  hSCTEndcapAResD5->SetFillColor(ColorForAll);
  hSCTEndcapAResD5->SetLineWidth(1.5);
  hSCTEndcapAResD5->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapAResD5->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapAResD5->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD5 = DrawFitFunc(hSCTEndcapAResD5,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap0Disc5Wide_%d",level);
    hSCTEndcapAResD5Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapAResD5Proc->Scale(1./(float)iEvents);
    hSCTEndcapAResD5Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapAResD5Proc->SetFillColor(ColorForUsed);
    hSCTEndcapAResD5Proc->SetLineWidth(1.5);
    hSCTEndcapAResD5Proc->SetFillStyle(3001);
    hSCTEndcapAResD5Proc->DrawCopy("SAME");
    FitValues fitD5_2 = DrawFitFunc(hSCTEndcapAResD5Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapAResD5,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapAResD5Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD5.FitMean,1000*fitD5.FitSigma);
  leg1->AddEntry(hSCTEndcapAResD5,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD5_2.FitMean,1000*fitD5_2.FitSigma);
    leg1->AddEntry(hSCTEndcapAResD5Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();



  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapARes->cd(7);
  hSCTEndcapAResD6 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap0Disc6Wide_0");  
  hSCTEndcapAResD6->SetTitle("SCT ECA Residuals - Disc 6");
  hSCTEndcapAResD6->GetXaxis()->SetTitle("SCT EndcapA Residuals (mm)");
  hSCTEndcapAResD6->GetYaxis()->SetTitle("Entries");
  hSCTEndcapAResD6->SetLineColor(ColorForAll+2);
  hSCTEndcapAResD6->SetFillColor(ColorForAll);
  hSCTEndcapAResD6->SetLineWidth(1.5);
  hSCTEndcapAResD6->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapAResD6->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapAResD6->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD6 = DrawFitFunc(hSCTEndcapAResD6,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap0Disc6Wide_%d",level);
    hSCTEndcapAResD6Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapAResD6Proc->Scale(1./(float)iEvents);
    hSCTEndcapAResD6Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapAResD6Proc->SetFillColor(ColorForUsed);
    hSCTEndcapAResD6Proc->SetLineWidth(1.5);
    hSCTEndcapAResD6Proc->SetFillStyle(3001);
    hSCTEndcapAResD6Proc->DrawCopy("SAME");
    FitValues fitD6_2 = DrawFitFunc(hSCTEndcapAResD6Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapAResD6,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapAResD6Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD6.FitMean,1000*fitD6.FitSigma);
  leg1->AddEntry(hSCTEndcapAResD6,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD6_2.FitMean,1000*fitD6_2.FitSigma);
    leg1->AddEntry(hSCTEndcapAResD6Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();





  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapARes->cd(8);
  hSCTEndcapAResD7 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap0Disc7Wide_0");  
  hSCTEndcapAResD7->SetTitle("SCT ECA Residuals - Disc 7");
  hSCTEndcapAResD7->GetXaxis()->SetTitle("SCT EndcapA Residuals (mm)");
  hSCTEndcapAResD7->GetYaxis()->SetTitle("Entries");
  hSCTEndcapAResD7->SetLineColor(ColorForAll+2);
  hSCTEndcapAResD7->SetFillColor(ColorForAll);
  hSCTEndcapAResD7->SetLineWidth(1.5);
  hSCTEndcapAResD7->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapAResD7->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapAResD7->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD7 = DrawFitFunc(hSCTEndcapAResD7,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap0Disc7Wide_%d",level);
    hSCTEndcapAResD7Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapAResD7Proc->Scale(1./(float)iEvents);
    hSCTEndcapAResD7Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapAResD7Proc->SetFillColor(ColorForUsed);
    hSCTEndcapAResD7Proc->SetLineWidth(1.5);
    hSCTEndcapAResD7Proc->SetFillStyle(3001);
    hSCTEndcapAResD7Proc->DrawCopy("SAME");
    FitValues fitD7_2 = DrawFitFunc(hSCTEndcapAResD7Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapAResD7,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapAResD7Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD7.FitMean,1000*fitD7.FitSigma);
  leg1->AddEntry(hSCTEndcapAResD7,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD7_2.FitMean,1000*fitD7_2.FitSigma);
    leg1->AddEntry(hSCTEndcapAResD7Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();


  //
  // -----------------------------------------------------------------
  //

  cSCTEndcapARes->cd(9);
  hSCTEndcapAResD8 = (TH1F*)file->Get("trkana/SCTRes/hSCTResEndcap0Disc8Wide_0");  
  hSCTEndcapAResD8->SetTitle("SCT ECA Residuals - Disc 8");
  hSCTEndcapAResD8->GetXaxis()->SetTitle("SCT EndcapA Residuals (mm)");
  hSCTEndcapAResD8->GetYaxis()->SetTitle("Entries");
  hSCTEndcapAResD8->SetLineColor(ColorForAll+2);
  hSCTEndcapAResD8->SetFillColor(ColorForAll);
  hSCTEndcapAResD8->SetLineWidth(1.5);
  hSCTEndcapAResD8->SetFillStyle(3001);
  if (normalizePlots) hSCTEndcapAResD8->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCTEndcapAResD8->DrawCopy();
  setTitleFont(font);
  ScalePlots();
  setTitleSize(0.4);
  FitValues fitD8 = DrawFitFunc(hSCTEndcapAResD8,ColorForAll+2,ShowFit);

  if (ShowProcessed) {
    sprintf(name,"trkana/SCTRes/hSCTResEndcap0Disc8Wide_%d",level);
    hSCTEndcapAResD8Proc = (TH1F*)file->Get(name); 
    if(normalizePlots) hSCTEndcapAResD8Proc->Scale(1./(float)iEvents);
    hSCTEndcapAResD8Proc->SetLineColor(ColorForUsed+2);
    hSCTEndcapAResD8Proc->SetFillColor(ColorForUsed);
    hSCTEndcapAResD8Proc->SetLineWidth(1.5);
    hSCTEndcapAResD8Proc->SetFillStyle(3001);
    hSCTEndcapAResD8Proc->DrawCopy("SAME");
    FitValues fitD8_2 = DrawFitFunc(hSCTEndcapAResD8Proc,ColorForUsed-2,ShowFit);
      
    TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
    sprintf(name,"All tracks");
    leg0->AddEntry(hSCTEndcapAResD8,name,"L");
    sprintf(name,"Processed tracks");
    leg0->AddEntry(hSCTEndcapAResD8Proc,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }

  if (ShowProcessed) TLegend *leg1 = new TLegend(0.67,0.66,0.98,0.78);
  else TLegend *leg1 = new TLegend(0.67,0.70,0.98,0.78);
  sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD8.FitMean,1000*fitD8.FitSigma);
  leg1->AddEntry(hSCTEndcapAResD8,name,"L");
  if (ShowProcessed) {
    sprintf(name,"#mu: %2.0f #mum, #sigma: %2.0f #mum",1000*fitD8_2.FitMean,1000*fitD8_2.FitSigma);
    leg1->AddEntry(hSCTEndcapAResD8Proc,name,"L");
  }
  leg1->SetTextSize(0.035);
  leg1->SetFillColor(kWhite);
  leg1->Draw();

  return;

}
