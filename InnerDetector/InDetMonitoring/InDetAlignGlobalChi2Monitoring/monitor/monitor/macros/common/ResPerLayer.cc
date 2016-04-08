/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream.h>

void ResPerLayer() {
  TProfile *hprofSCTBarrelResXLy;
  TProfile *hprofSCTBarrelResXLySide0;
  TProfile *hprofSCTBarrelResXLySide1;
  TProfile *hprofSCTEndCapAResXLy;
  TProfile *hprofSCTEndCapAResXLySide0;
  TProfile *hprofSCTEndCapAResXLySide1;
  
  cout << " Residuals per Layer..." << endl;

  cResPerLayer = new TCanvas("AllResidualsPerLayer","All Residuals per Layer",
			     CanvasSizeX6[0],CanvasSizeX6[1]);
  cResPerLayer->Divide(3,2);

  Float_t jMax = 0.0;
  Bool_t doECA = true;
  Bool_t doECC = true;

  cResPerLayer->cd(1);
  TProfile *hprofPixelEndCapAResXLy = (TProfile*)file->Get("trkana/PIXRes/hproPIXMeanResEndcap0_0");
  if (hprofPixelEndCapAResXLy->GetMean(2)==0) {
    doECA = false;
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(62);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"Pixel EndCapA");
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

  cResPerLayer->cd(3);
  TProfile *hprofPixelEndCapCResXLy = (TProfile*)file->Get("trkana/PIXRes/hproPIXMeanResEndcap1_0");
  if (hprofPixelEndCapCResXLy->GetMean(2)==0)
    {
      doECC = false;
      TLatex *t = new TLatex();
      t->SetNDC();
      t->SetTextFont(62);
      t->SetTextColor(kRed);
      t->SetTextSize(0.06);
      t->SetTextAngle(25);
      sprintf(name,"Pixel EndCapC");
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

  if (doECA){
    cResPerLayer->cd(1);
    hprofPixelEndCapAResXLy->SetTitle("Pixel EndCapA Residuals per Disk");
    hprofPixelEndCapAResXLy->GetXaxis()->SetTitle("Disk");
    hprofPixelEndCapAResXLy->GetYaxis()->SetTitle("Pixel EndCapA Residual means (#mum)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hprofPixelEndCapAResXLy->DrawCopy();
    cResPerLayer->Update();
    setTitleFont(font);
    drawHorizontalLine(hprofPixelEndCapAResXLy);
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  if (doECC){
    cResPerLayer->cd(3);
    gPad->SetGridy();
    hprofPixelEndCapCResXLy->SetTitle("Pixel EndCapC Residuals per Disk");
    hprofPixelEndCapCResXLy->GetXaxis()->SetTitle("Disk");
    hprofPixelEndCapCResXLy->GetYaxis()->SetTitle("Pixel EndCapC Residual means (#mum)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hprofPixelEndCapCResXLy->DrawCopy();
    setTitleFont(font);
    drawHorizontalLine(hprofPixelEndCapCResXLy);
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //
  
  cResPerLayer->cd(2);
  TProfile *hprofPixelBarrelResXLy = (TProfile*)file->Get("trkana/PIXRes/hproPIXMeanResBaLay_0");
  if (hprofPixelBarrelResXLy->GetMean(2)!=0) {
       
    gPad->SetGridy();
    hprofPixelBarrelResXLy->SetTitle("Pixel Barrel Residuals per Layer");
    hprofPixelBarrelResXLy->GetXaxis()->SetTitle("Layer");
    hprofPixelBarrelResXLy->GetYaxis()->SetTitle("Pixel Barrel Residual means (#mum)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hprofPixelBarrelResXLy->DrawCopy();
    setTitleFont(font);
    drawHorizontalLine(hprofPixelBarrelResXLy);
    setTitleSize(0.67);
  }
  else {
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(62);
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
  
  // ---------------
  //
  //       SCT
  //
  // ---------------
  
  jMax = 0.0;
  doECA = true;
  doECC = true;
 
  //
  // -----------------------------------------------------------------
  //

  cResPerLayer->cd(4);
  hprofSCTEndCapAResXLy = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResEndcap0_0");
  if (hprofSCTEndCapAResXLy->GetMean(2)==0) {
    doECA = false;
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(62);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"SCT EndCapA");
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

  cResPerLayer->cd(6);
  hprofSCTEndCapCResXLy = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResEndcap1_0");
  if (hprofSCTEndCapCResXLy->GetMean(2)==0) {
    doECC = false;
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(62);
    t->SetTextColor(kRed);
    t->SetTextSize(0.06);
    t->SetTextAngle(25);
    sprintf(name,"SCT EndCapC");
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

  if (doECA){
    cResPerLayer->cd(4);
    gPad->SetGridy();
    hprofSCTEndCapAResXLy->SetTitle("SCT EndCapA Residuals");
    hprofSCTEndCapAResXLy->GetXaxis()->SetTitle("Disk");
    hprofSCTEndCapAResXLy->GetYaxis()->SetTitle("SCT EndCapA Residual means (#mum)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hprofSCTEndCapAResXLy->DrawCopy();
    setTitleFont(font);
    drawHorizontalLine(hprofSCTEndCapAResXLy);
    setTitleSize(0.46);

    // Side 0
    hprofSCTEndCapAResXLySide0 = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResEndcap0Side0_0");
    hprofSCTEndCapAResXLySide0->SetLineColor(kBlue);
    hprofSCTEndCapAResXLySide0->DrawCopy("SAME");

    // Side 1
    hprofSCTEndCapAResXLySide1 = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResEndcap0Side1_0");
    hprofSCTEndCapAResXLySide1->SetLineColor(kRed);
    hprofSCTEndCapAResXLySide1->DrawCopy("SAME");

    TLegend *leg0 = new TLegend(0.47,0.89,0.67,1.0);
    sprintf(name,"All");
    leg0->AddEntry(hprofSCTEndCapAResXLy,name,"L");
    sprintf(name,"Side 0");
    leg0->AddEntry(hprofSCTEndCapAResXLySide0,name,"L");
    sprintf(name,"Side 1");
    leg0->AddEntry(hprofSCTEndCapAResXLySide1,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
  }
 
  //
  // -----------------------------------------------------------------
  //

  if (doECC){
    cResPerLayer->cd(6);
    gPad->SetGridy();
    hprofSCTEndCapCResXLy->SetTitle("SCT EndCapC Residuals");
    hprofSCTEndCapCResXLy->GetXaxis()->SetTitle("Disk");
    hprofSCTEndCapCResXLy->GetYaxis()->SetTitle("SCT EndCapC Residual means (#mum)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hprofSCTEndCapCResXLy->DrawCopy();
    setTitleFont(font);
    drawHorizontalLine(hprofSCTEndCapCResXLy);
    setTitleSize(0.46);

    // Side 0
    hprofSCTEndCapCResXLySide0 = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResEndcap1Side0_0");
    hprofSCTEndCapCResXLySide0->SetLineColor(kBlue);
    hprofSCTEndCapCResXLySide0->DrawCopy("SAME");

    // Side 1
    hprofSCTEndCapCResXLySide1 = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResEndcap1Side1_0");
    hprofSCTEndCapCResXLySide1->SetLineColor(kRed);
    hprofSCTEndCapCResXLySide1->DrawCopy("SAME");
    
    TLegend *leg0 = new TLegend(0.47,0.89,0.67,1.0);
    sprintf(name,"All");
    leg0->AddEntry(hprofSCTEndCapCResXLy,name,"L");
    sprintf(name,"Side 0");
    leg0->AddEntry(hprofSCTEndCapCResXLySide0,name,"L");
    sprintf(name,"Side 1");
    leg0->AddEntry(hprofSCTEndCapCResXLySide1,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();

  }

  //
  // -----------------------------------------------------------------
  //
  
  cResPerLayer->cd(5);
  hprofSCTBarrelResXLy = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResBaLay_0");
  if(hprofSCTBarrelResXLy->GetMean(2)!=0){
    gPad->SetGridy();
    hprofSCTBarrelResXLy->SetTitle("SCT Barrel Residuals");
    hprofSCTBarrelResXLy->GetXaxis()->SetTitle("Layer");
    hprofSCTBarrelResXLy->GetYaxis()->SetTitle("SCT Barrel Residual means (#mum)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hprofSCTBarrelResXLy->DrawCopy();
    setTitleFont(font);
    drawHorizontalLine(hprofSCTBarrelResXLy);
    setTitleSize(0.46);

    // Side 0
    hprofSCTBarrelResXLySide0 = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResBaLaySide0_0");
    hprofSCTBarrelResXLySide0->SetLineColor(kBlue);
    hprofSCTBarrelResXLySide0->DrawCopy("SAME");
    
    // Side 1
    hprofSCTBarrelResXLySide1 = (TProfile*)file->Get("trkana/SCTRes/hproSCTMeanResBaLaySide1_0");
    hprofSCTBarrelResXLySide1->SetLineColor(kRed);
    hprofSCTBarrelResXLySide1->DrawCopy("SAME");

    TLegend *leg0 = new TLegend(0.47,0.89,0.67,1.0);
    sprintf(name,"All");
    leg0->AddEntry(hprofSCTBarrelResXLy,name,"L");
    sprintf(name,"Side 0");
    leg0->AddEntry(hprofSCTBarrelResXLySide0,name,"L");
    sprintf(name,"Side 1");
    leg0->AddEntry(hprofSCTBarrelResXLySide1,name,"L");
    leg0->SetTextSize(0.035);
    leg0->SetFillColor(kWhite);
    leg0->Draw();
    
  }
  else {
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(62);
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
}
