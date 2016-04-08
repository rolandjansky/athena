/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ProcResPerLayer(int level=1) {

  TProfile *hprofSCTBarrelResXLy;
  TProfile *hprofSCTBarrelResXLySide0;
  TProfile *hprofSCTBarrelResXLySide1;
  TProfile *hprofSCTEndCapAResXLy;
  TProfile *hprofSCTEndCapAResXLySide0;
  TProfile *hprofSCTEndCapAResXLySide1;
  
  cout << " Processed Residuals per Layer..." << endl;

  cProcResPerLayer = new TCanvas("AllProcessedResidualsPerLayer","All Processed Residuals per Layer",
				 CanvasSizeX6[0],CanvasSizeX6[1]);
  cProcResPerLayer->Divide(3,2);

  cProcResPerLayer->cd(1);
  sprintf(name,"trkana/PIXRes/hproPIXMeanResEndcap0_%d",level);
  TProfile *hprofPixelEndCapAResXLy = (TProfile*)file->Get(name);
  if (hprofPixelEndCapAResXLy->GetMean(2)==0)
    doSquare("Pixel EndCapA");
  else {
    gPad->SetGridy();
    hprofPixelEndCapAResXLy->SetTitle("Pixel EndCapA Processed Residuals");
    hprofPixelEndCapAResXLy->GetXaxis()->SetTitle("Disk");
    hprofPixelEndCapAResXLy->GetYaxis()->SetTitle("Pixel EndCapA Processed Residual means (#mum)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hprofPixelEndCapAResXLy->DrawCopy();
    setTitleFont(font);
    drawHorizontalLine(hprofPixelEndCapAResXLy);
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  cProcResPerLayer->cd(2);
  sprintf(name,"trkana/PIXRes/hproPIXMeanResBaLay_%d",level);
  TProfile *hprofPixelBarrelResXLy = (TProfile*)file->Get(name);
  if (hprofPixelBarrelResXLy->GetMean(2)==0)
    doSquare("Pixel Barrel");
  else {
    gPad->SetGridy();
    hprofPixelBarrelResXLy->SetTitle("Pixel Barrel Processed Residuals");
    hprofPixelBarrelResXLy->GetXaxis()->SetTitle("Layer");
    hprofPixelBarrelResXLy->GetYaxis()->SetTitle("Pixel Barrel Processed Residual means (#mum)");
    hprofPixelBarrelResXLy->DrawCopy();
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    setTitleFont(font);
    drawHorizontalLine(hprofPixelBarrelResXLy);
    setTitleSize(0.67);
  }

  //
  // -----------------------------------------------------------------
  //

  cProcResPerLayer->cd(3);
  sprintf(name,"trkana/PIXRes/hproPIXMeanResEndcap1_%d",level);
  TProfile *hprofPixelEndCapCResXLy = (TProfile*)file->Get(name);
  if (hprofPixelEndCapCResXLy->GetMean(2)==0)
    doSquare("Pixel EndCapC");
  else {
    gPad->SetGridy();
    hprofPixelEndCapCResXLy->SetTitle("Pixel EndCapC Processed Residuals");
    hprofPixelEndCapCResXLy->GetXaxis()->SetTitle("Disk");
    hprofPixelEndCapCResXLy->GetYaxis()->SetTitle("Pixel EndCapC Processed Residual means (#mum)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hprofPixelEndCapCResXLy->DrawCopy();
    setTitleFont(font);
    drawHorizontalLine(hprofPixelEndCapCResXLy);
    setTitleSize(0.67);
  }

  // ---------------
  //
  //       SCT
  //
  // ---------------

  //
  // -----------------------------------------------------------------
  //

  cProcResPerLayer->cd(4);
  sprintf(name,"trkana/SCTRes/hproSCTMeanResEndcap0_%d",level);
  hprofSCTEndCapAResXLy = (TProfile*)file->Get(name);
  if (hprofSCTEndCapAResXLy->GetMean(2)==0)
    doSquare("SCT EndCapA");
  else {
    gPad->SetGridy();
    hprofSCTEndCapAResXLy->SetTitle("SCT EndCapA ProcRes");
    hprofSCTEndCapAResXLy->GetXaxis()->SetTitle("Disk");
    hprofSCTEndCapAResXLy->GetYaxis()->SetTitle("SCT EndCapA Processed Residual means (#mum)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hprofSCTEndCapAResXLy->DrawCopy();
    setTitleFont(font);
    drawHorizontalLine(hprofSCTEndCapAResXLy);
    setTitleSize(0.46);

    // Side 0
    sprintf(name,"trkana/SCTRes/hproSCTMeanResEndcap0Side0_%d",level);
    hprofSCTEndCapAResXLySide0 = (TProfile*)file->Get(name);
    hprofSCTEndCapAResXLySide0->SetLineColor(kBlue);
    hprofSCTEndCapAResXLySide0->DrawCopy("SAME");

    // Side 1
    sprintf(name,"trkana/SCTRes/hproSCTMeanResEndcap0Side1_%d",level);
    hprofSCTEndCapAResXLySide1 = (TProfile*)file->Get(name);
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

  cProcResPerLayer->cd(5);
  sprintf(name,"trkana/SCTRes/hproSCTMeanResBaLay_%d",level);
  hprofSCTBarrelResXLy = (TProfile*)file->Get(name);
  if(hprofSCTBarrelResXLy->GetMean(2)==0)
    doSquare("SCT Barrel");
  else {
    gPad->SetGridy();
    hprofSCTBarrelResXLy->SetTitle("SCT Barrel ProcRes");
    hprofSCTBarrelResXLy->GetXaxis()->SetTitle("Layer");
    hprofSCTBarrelResXLy->GetYaxis()->SetTitle("SCT Barrel Processed Res means (#mum)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hprofSCTBarrelResXLy->DrawCopy();
    setTitleFont(font);
    drawHorizontalLine(hprofSCTBarrelResXLy);
    setTitleSize(0.46);
    
    // Side 0
    sprintf(name,"trkana/SCTRes/hproSCTMeanResBaLaySide0_%d",level);
    hprofSCTBarrelResXLySide0 = (TProfile*)file->Get(name);
    hprofSCTBarrelResXLySide0->SetLineColor(kBlue);
    hprofSCTBarrelResXLySide0->DrawCopy("SAME");

    // Side 1
    sprintf(name,"trkana/SCTRes/hproSCTMeanResBaLaySide1_%d",level);
    hprofSCTBarrelResXLySide1 = (TProfile*)file->Get(name);
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


  //
  // -----------------------------------------------------------------
  //

  cProcResPerLayer->cd(6);
  sprintf(name,"trkana/SCTRes/hproSCTMeanResEndcap1_%d",level);
  hprofSCTEndCapCResXLy = (TProfile*)file->Get(name);
  if (hprofSCTEndCapCResXLy->GetMean(2)==0)
    doSquare("SCT EndCapC");
  else {
    gPad->SetGridy();
    hprofSCTEndCapCResXLy->SetTitle("SCT EndCapC ProcRes");
    hprofSCTEndCapCResXLy->GetXaxis()->SetTitle("Disk");
    hprofSCTEndCapCResXLy->GetYaxis()->SetTitle("SCT EndCapC Processed Residual means (#mum)");
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    hprofSCTEndCapCResXLy->DrawCopy();
    setTitleFont(font);
    drawHorizontalLine(hprofSCTEndCapCResXLy);
    setTitleSize(0.46);

    // Side 0
    sprintf(name,"trkana/SCTRes/hproSCTMeanResEndcap1Side0_%d",level);
    hprofSCTEndCapCResXLySide0 = (TProfile*)file->Get(name);
    hprofSCTEndCapCResXLySide0->SetLineColor(kBlue);
    hprofSCTEndCapCResXLySide0->DrawCopy("SAME");

    // Side 1
    sprintf(name,"trkana/SCTRes/hproSCTMeanResEndcap1Side1_%d",level);
    hprofSCTEndCapCResXLySide1 = (TProfile*)file->Get(name);
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

}
