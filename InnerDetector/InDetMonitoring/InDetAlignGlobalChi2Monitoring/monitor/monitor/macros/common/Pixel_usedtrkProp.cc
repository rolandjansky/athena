/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void Pixel_usedtrkProp(int level=1) {

  if (!SCTOnly && iPixelHits>0) {
    TH2F *hUSedPIXResVsPhi0;
    TH2F *hUsedPIXResVsZ0;
    TH2F *hUsedPIXResVsSector;
    TH2F *hUsedPIXResVsRing;
    TProfile *hprofUsedGlobPixel;
    cout << " Pixel - Used Track properties..." << endl;

    cUsedTrkPixelProp = new TCanvas("cUsedTrkPixelProp","Pixel - Used Track properties",
				    CanvasSizeX6[0],CanvasSizeX6[1]);
    cUsedTrkPixelProp->Divide(3,2);

    //   tree->Scan("Hit_DetType:Hit_BEc:TrkRes:Hit_phi0","TrkRes!=0");
    //   return;

    cUsedTrkPixelProp->cd(1);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    sprintf(name,"trkana/Track_Parameters/hPIXRes_RecoPhi0_%d",level);
    hUsedPIXResVsPhi0 = (TH2F*)file->Get(name);
    hUsedPIXResVsPhi0->SetLineColor(ColorForUsed+2);
    hUsedPIXResVsPhi0->ProfileX()->Draw();
    hUsedPIXResVsPhi0->ProfileX()->SetTitle("Pixel Processed Residuals Vs Trk #phi0");
    hUsedPIXResVsPhi0->ProfileX()->GetYaxis()->SetTitle("average Pixel residual mean (#mum)");
    hUsedPIXResVsPhi0->ProfileX()->GetXaxis()->SetTitle("Trk #phi0 (rad)");
    hUsedPIXResVsPhi0->ProfileX()->GetYaxis()->SetRangeUser(-20.0,20.0);
    setTitleFont(font);
    setTitleSize(0.67);
    
    cUsedTrkPixelProp->cd(2);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    sprintf(name,"trkana/Track_Parameters/hPIXRes_RecoZ0_%d",level);
    hUsedPIXResVsZ0 = (TH2F*)file->Get(name);
    hUsedPIXResVsZ0->SetLineColor(ColorForUsed+2);
    hUsedPIXResVsZ0->ProfileX()->Draw();
    hUsedPIXResVsZ0->ProfileX()->SetTitle("Pixel Processed Residuals Vs Trk z0");
    hUsedPIXResVsZ0->ProfileX()->GetYaxis()->SetTitle("average Pixel residual mean (#mum)");
    hUsedPIXResVsZ0->ProfileX()->GetXaxis()->SetTitle("Trk z0 (mm)");
    hUsedPIXResVsZ0->ProfileX()->GetYaxis()->SetRangeUser(-40.0,40.0);
    setTitleFont(font);
    setTitleSize(0.67);

    cUsedTrkPixelProp->cd(3);
    hUsedTrkPixel_delzr = (TH2F*)file->Get("trkana/PIXRes/hPIXDelzr_1");
    hUsedTrkPixel_delzr->SetTitle("Pixel Processed LocalY Res (delzr)");
    hUsedTrkPixel_delzr->GetXaxis()->SetTitle("Pixel Z residual (mm)");
    hUsedTrkPixel_delzr->SetLineColor(ColorForUsed+2);
    if (normalizePlots) hUsedTrkPixel_delzr->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hUsedTrkPixel_delzr->DrawCopy(); 
    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete hUsedTrkPixel_delzr;
    
    
    cUsedTrkPixelProp->cd(4);
    sprintf(name,"trkana/Track_Parameters/hPIXRes_Sector_%d",level);
    hUsedPIXResVsSector = (TH2F*)file->Get(name);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hUsedPIXResVsSector->SetLineColor(ColorForUsed+2);
    hUsedPIXResVsSector->ProfileX()->Draw();
    hUsedPIXResVsSector->ProfileX()->SetTitle("Pixel Processed Residuals Vs Sector");
    hUsedPIXResVsSector->ProfileX()->GetYaxis()->SetRangeUser(-20,20);
    hUsedPIXResVsSector->ProfileX()->GetYaxis()->SetTitle("average Pixel residual mean (#mum)");
    hUsedPIXResVsSector->ProfileX()->GetXaxis()->SetTitle("Sector (#phi)");
    setTitleFont(font);
    setTitleSize(0.67);
    
    cUsedTrkPixelProp->cd(5);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    
      
    sprintf(name,"trkana/Track_Parameters/hPIXRes_Ring_%d",level);
    hUsedPIXResVsRing = (TH2F*)file->Get(name);
    hUsedPIXResVsRing->SetLineColor(ColorForUsed+2);
    hUsedPIXResVsRing->ProfileX()->Draw();
    hUsedPIXResVsRing->ProfileX()->SetTitle("Pixel Processed Residuals Vs Ring");
    hUsedPIXResVsRing->ProfileX()->GetYaxis()->SetTitle("average Pixel residual mean (#mum)");
    hUsedPIXResVsRing->ProfileX()->GetXaxis()->SetTitle("Ring (#eta)");
    hUsedPIXResVsRing->ProfileX()->GetYaxis()->SetRangeUser(-40.0,40.0);
    setTitleFont(font);
    setTitleSize(0.67);

    cUsedTrkPixelProp->cd(6);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    sprintf(name,"trkana/PIXRes/hproPIXMeanResBaLay_%d",level);
    hprofUsedGlobPixel = (TProfile*)file->Get(name);
    hprofUsedGlobPixel->SetLineColor(ColorForUsed+2);
    hprofUsedGlobPixel->Draw();
    hprofUsedGlobPixel->SetTitle("Pixel Processed Residuals Vs Layer");
    hprofUsedGlobPixel->GetYaxis()->SetTitle("average Pixel residual mean (#mum)");
    hprofUsedGlobPixel->GetXaxis()->SetTitle("Trk layer");
    hprofUsedGlobPixel->GetYaxis()->SetRangeUser(-8.0,8.0);

    cUsedTrkPixelProp->Update();
    setTitleFont(font);
    setTitleSize(0.60);
  }
  else 
    cout << " Pixel - Used Track properties... No Pixels!!!" << endl;
}
