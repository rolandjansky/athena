/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void resErrors() {

  if (!PixelOnly && iSCTHits>0) {

    cout << " SCT Processed Barrel Residual Errors..." << endl;
  
    // *********************************
    //
    //             SCT
    //
    // *********************************
    TH1F *histSCTResErrLy0;
    TH1F *histSCTResErrLy1;
    TH1F *histSCTResErrLy2;
    TH1F *histSCTResErrLy3;
    TH1F *histPIXResErrLy0;
    TH1F *histPIXResErrLy1;
    TH1F *histPIXResErrLy2;
    TH1F *histPIXEtaResErrLy0;
    TH1F *histPIXEtaResErrLy1;
    TH1F *histPIXEtaResErrLy2;

    cSCTResErrors = new TCanvas("cSCTResErrors","SCT Processed Barrel Residual Errors",
				CanvasSizeX4[0],CanvasSizeX4[1]);
    cSCTResErrors->Divide(2,2);
  
    cSCTResErrors->cd(1);
    sprintf(name,"trkana/SCTRes/hSCTErrBaLay0_%d",level);
    histSCTResErrLy0 = (TH1F*)file->Get(name);
    histSCTResErrLy0->SetTitle("SCT Processed Barrel Residual Errors (Layer 0)");
    histSCTResErrLy0->GetXaxis()->SetTitle("SCT Processed Residual Errors (mm)");
    histSCTResErrLy0->GetYaxis()->SetTitle("Entries");
    histSCTResErrLy0->SetLineColor(ColorForAll+2);
    histSCTResErrLy0->SetFillColor(ColorForAll);
    histSCTResErrLy0->SetLineWidth(2);
    histSCTResErrLy0->SetFillStyle(3001);
    if (normalizePlots) histSCTResErrLy0->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    histSCTResErrLy0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
	
    //
    // -----------------------------------------------------------------
    //

    cSCTResErrors->cd(2);
    sprintf(name,"trkana/SCTRes/hSCTErrBaLay1_%d",level);
    histSCTResErrLy1 = (TH1F*)file->Get(name);
    histSCTResErrLy1->SetTitle("SCT Processed Barrel Residuals Errors (Layer 1)");
    histSCTResErrLy1->GetXaxis()->SetTitle("SCT Processed Residual Errors (mm)");
    histSCTResErrLy1->GetYaxis()->SetTitle("Entries");
    histSCTResErrLy1->SetLineColor(ColorForAll+2);
    histSCTResErrLy1->SetLineWidth(2);
    histSCTResErrLy1->SetFillColor(ColorForAll);
    histSCTResErrLy1->SetFillStyle(3001);
    if (normalizePlots) histSCTResErrLy1->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    histSCTResErrLy1->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);

    //
    // -----------------------------------------------------------------
    //

    cSCTResErrors->cd(3);
    sprintf(name,"trkana/SCTRes/hSCTErrBaLay2_%d",level);
    histSCTResErrLy2 = (TH1F*)file->Get(name);
    histSCTResErrLy2->SetTitle("SCT Processed Barrel Residual Errors (Layer 2)");
    histSCTResErrLy2->GetXaxis()->SetTitle("SCT Processed Residual Errors (mm)");
    histSCTResErrLy2->GetYaxis()->SetTitle("Entries");
    histSCTResErrLy2->SetLineColor(ColorForAll+2);
    histSCTResErrLy2->SetLineWidth(2);
    histSCTResErrLy2->SetFillColor(ColorForAll);
    histSCTResErrLy2->SetFillStyle(3001);
    if (normalizePlots) histSCTResErrLy2->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    histSCTResErrLy2->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
  
    //
    // -----------------------------------------------------------------
    //

    cSCTResErrors->cd(4);
    sprintf(name,"trkana/SCTRes/hSCTErrBaLay3_%d",level);
    histSCTResErrLy3 = (TH1F*)file->Get(name);
    histSCTResErrLy3->SetTitle("SCT Processed Barrel Residual Errors (Layer 3)");
    histSCTResErrLy3->GetXaxis()->SetTitle("SCT Processed Residual Errors (mm)");
    histSCTResErrLy3->GetYaxis()->SetTitle("Entries");
    histSCTResErrLy3->SetLineColor(ColorForAll+2);
    histSCTResErrLy3->SetLineWidth(2);
    histSCTResErrLy3->SetFillColor(ColorForAll);
    histSCTResErrLy3->SetFillStyle(3001);
    if (normalizePlots) histSCTResErrLy3->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    histSCTResErrLy3->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);

    cSCTResErrors->Update();
  
  }

  //
  // -----------------------------------------------------------------
  //

  if (!SCTOnly && iPixelHits>0) {  

    cout << " Pixel Processed Barrel Residual Errors..." << endl;
  
    // *********************************
    //
    //             Pixels
    //
    // *********************************
    cPixResErrors = new TCanvas("cPixResErrors","Pixel Processed Barrel Residual Errors",
				CanvasSizeX6[0],CanvasSizeX6[1]);
    cPixResErrors->Divide(3,2);
  
    cPixResErrors->cd(1);
    sprintf(name,"trkana/PIXRes/hPIXErrPhiBaLay0_%d",level);
    histPixResErrLy0 = (TH1F*)file->Get(name);
    histPixResErrLy0->SetTitle("Pixel Proc Barrel Res Errors (Layer 0)");
    histPixResErrLy0->GetXaxis()->SetTitle("Pixel Processed #phi Residual Errors (mm)");
    histPixResErrLy0->GetYaxis()->SetTitle("Entries");
    histPixResErrLy0->SetLineColor(ColorForAll+2);
    histPixResErrLy0->SetLineWidth(2);
    histPixResErrLy0->SetFillColor(ColorForAll);
    histPixResErrLy0->SetFillStyle(3001);
    if (normalizePlots) histPixResErrLy0->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    histPixResErrLy0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);

    //
    // -----------------------------------------------------------------
    //

    cPixResErrors->cd(2);
    sprintf(name,"trkana/PIXRes/hPIXErrPhiBaLay1_%d",level);
    histPixResErrLy1 = (TH1F*)file->Get(name);
    histPixResErrLy1->SetTitle("Pixel Proc Barrel Res Errors (Layer 1)");
    histPixResErrLy1->GetXaxis()->SetTitle("Pixel Processed #phi Residual Errors (mm)");
    histPixResErrLy1->GetYaxis()->SetTitle("Entries");
    histPixResErrLy1->SetLineColor(ColorForAll+2);
    histPixResErrLy1->SetLineWidth(2);
    histPixResErrLy1->SetFillColor(ColorForAll);
    histPixResErrLy1->SetFillStyle(3001);
    if (normalizePlots) histPixResErrLy1->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    histPixResErrLy1->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);

    //
    // -----------------------------------------------------------------
    //

    cPixResErrors->cd(3);
    sprintf(name,"trkana/PIXRes/hPIXErrPhiBaLay2_%d",level);
    histPixResErrLy2 = (TH1F*)file->Get(name);
    histPixResErrLy2->SetTitle("Pixel Proc Barrel Res Errors (Layer 2)");
    histPixResErrLy2->GetXaxis()->SetTitle("Pixel Processed #phi Residual Errors (mm)");
    histPixResErrLy2->GetYaxis()->SetTitle("Entries");
    histPixResErrLy2->SetLineColor(ColorForAll+2);
    histPixResErrLy2->SetLineWidth(2);
    histPixResErrLy2->SetFillColor(ColorForAll);
    histPixResErrLy2->SetFillStyle(3001);
    if (normalizePlots) histPixResErrLy2->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    histPixResErrLy2->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);

    //
    // -----------------------------------------------------------------
    //

    cPixResErrors->cd(4);
    sprintf(name,"trkana/PIXRes/hPIXErrEtaBaLay0_%d",level);
    histPixEtaResErrLy0 = (TH1F*)file->Get(name);
    histPixEtaResErrLy0->SetTitle("Pixel Proc Barrel #eta Res Errors (Layer 0)");
    histPixEtaResErrLy0->GetXaxis()->SetTitle("Pixel Processed #eta Residual Errors (mm)");
    histPixEtaResErrLy0->GetYaxis()->SetTitle("Entries");
    histPixEtaResErrLy0->SetLineColor(ColorForAll+2);
    histPixEtaResErrLy0->SetLineWidth(2);
    histPixEtaResErrLy0->SetFillColor(ColorForAll);
    histPixEtaResErrLy0->SetFillStyle(3001);
    if (normalizePlots) histPixEtaResErrLy0->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    histPixEtaResErrLy0->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);

    //
    // -----------------------------------------------------------------
    //

    cPixResErrors->cd(5);
    sprintf(name,"trkana/PIXRes/hPIXErrEtaBaLay1_%d",level);
    histPixEtaResErrLy1 = (TH1F*)file->Get(name);
    histPixEtaResErrLy1->SetTitle("Pixel Proc Barrel #eta Res Errors (Layer 1)");
    histPixEtaResErrLy1->GetXaxis()->SetTitle("Pixel Processed #eta Residual Errors (mm)");
    histPixEtaResErrLy1->GetYaxis()->SetTitle("Entries");
    histPixEtaResErrLy1->SetLineColor(ColorForAll+2);
    histPixEtaResErrLy1->SetLineWidth(2);
    histPixEtaResErrLy1->SetFillColor(ColorForAll);
    histPixEtaResErrLy1->SetFillStyle(3001);
    if (normalizePlots) histPixEtaResErrLy1->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    histPixEtaResErrLy1->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);

    //
    // -----------------------------------------------------------------
    //

    cPixResErrors->cd(6);
    sprintf(name,"trkana/PIXRes/hPIXErrEtaBaLay2_%d",level);
    histPixEtaResErrLy2 = (TH1F*)file->Get(name);
    histPixEtaResErrLy2->SetTitle("Pixel Proc Barrel #eta Res Errors (Layer 2)");
    histPixEtaResErrLy2->GetXaxis()->SetTitle("Pixel #eta Processed Residual Errors (mm)");
    histPixEtaResErrLy2->GetYaxis()->SetTitle("Entries");
    histPixEtaResErrLy2->SetLineColor(ColorForAll+2);
    histPixEtaResErrLy2->SetLineWidth(2);
    histPixEtaResErrLy2->SetFillColor(ColorForAll);
    histPixEtaResErrLy2->SetFillStyle(3001);
    if (normalizePlots) histPixEtaResErrLy2->Scale(1./(float)iEvents);
    if (!AtlasStyle) {
      gPad->SetGridy();
      gPad->SetGridx();
    }
    histPixEtaResErrLy2->DrawCopy();
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);

    cPixResErrors->Update();
  }
    
  return;
}
