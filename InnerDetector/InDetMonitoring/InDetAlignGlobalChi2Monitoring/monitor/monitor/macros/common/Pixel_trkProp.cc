/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void Pixel_trkProp() {

  if (!SCTOnly && iPixelHits>0) {

    TH2F *hPIXResVsPhi0;
    TH2F *hPIXResVsZ0;
    TH2F *hPIXResVsSector;
    TH2F *hPIXResVsRing;
    TProfile * hprofGlobPixel;

    cout << " Pixel - Track properties..." << endl;

    cTrkPixelProp = new TCanvas("TrkPixelProp","Pixel - Track properties",
				CanvasSizeX6[0],CanvasSizeX6[1]);
    cTrkPixelProp->Divide(3,2);

    //   tree->Scan("Trk_DetType:Trk_BEc:TrkRes:Trk_phi0","TrkRes!=0");
    //   return;

    cTrkPixelProp->cd(1);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hPIXResVsPhi0 = (TH2F*)file->Get("trkana/Track_Parameters/hPIXRes_RecoPhi0_0");
    hPIXResVsPhi0->SetLineColor(ColorForAll+2);
    hPIXResVsPhi0->ProfileX()->Draw();
    hPIXResVsPhi0->ProfileX()->SetTitle("Pixel Residuals Vs Trk #phi_{0}");
    hPIXResVsPhi0->ProfileX()->GetYaxis()->SetTitle("average Pixel residual mean (#mum)");
    hPIXResVsPhi0->ProfileX()->GetXaxis()->SetTitle("Trk #phi_{0} (rad)");
    hPIXResVsPhi0->ProfileX()->GetYaxis()->SetRangeUser(-20.0,20.0);
   
    setTitleFont(font);
    setTitleSize(0.67);
    
    cTrkPixelProp->cd(2);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hPIXResVsZ0 = (TH2F*)file->Get("trkana/Track_Parameters/hPIXRes_RecoZ0_0");
    hPIXResVsZ0->SetLineColor(ColorForAll+2);
    hPIXResVsZ0->ProfileX()->Draw();
    hPIXResVsZ0->ProfileX()->SetTitle("Pixel Residuals Vs Trk z_{0}");
    hPIXResVsZ0->ProfileX()->GetYaxis()->SetTitle("average Pixel residual mean (#mum)");
    hPIXResVsZ0->ProfileX()->GetXaxis()->SetTitle("Trk z_{0} (mm)");
    hPIXResVsZ0->ProfileX()->GetYaxis()->SetRangeUser(-40.0,40.0);
    setTitleFont(font);
    setTitleSize(0.67);
    
    cTrkPixelProp->cd(3);
    hTrkPixel_delzr = (TH2F*)file->Get("trkana/PIXRes/hPIXDelzr_0");
    hTrkPixel_delzr->SetTitle("Pixel LocalY residual (delzr)");
    hTrkPixel_delzr->GetXaxis()->SetTitle("Pixel Z residual (mm)");
    hTrkPixel_delzr->SetLineColor(ColorForAll+2);
    if (normalizePlots) hTrkPixel_delzr->Scale(1./(float)iEvents);
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
    hTrkPixel_delzr->DrawCopy(); 

    setTitleFont(font);
    ScalePlots();
    setTitleSize(0.67);
    delete hTrkPixel_delzr;
    
    
    cTrkPixelProp->cd(4);
    hPIXResVsSector = (TH2F*)file->Get("trkana/Track_Parameters/hPIXRes_Sector_0");
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hPIXResVsSector->SetLineColor(ColorForAll+2);
    hPIXResVsSector->ProfileX()->Draw();
    hPIXResVsSector->ProfileX()->SetTitle("Pixel Residuals Vs Sector");
    hPIXResVsSector->ProfileX()->GetYaxis()->SetRangeUser(-20,20);
    hPIXResVsSector->ProfileX()->GetYaxis()->SetTitle("average Pixel residual mean (#mum)");
    hPIXResVsSector->ProfileX()->GetXaxis()->SetTitle("Sector (#phi)");
    setTitleFont(font);
    setTitleSize(0.67);
    
    cTrkPixelProp->cd(5);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    
    hPIXResVsRing = (TH2F*)file->Get("trkana/Track_Parameters/hPIXRes_Ring_0");
    hPIXResVsRing->SetLineColor(ColorForAll+2);
    hPIXResVsRing->ProfileX()->Draw();
    hPIXResVsRing->ProfileX()->SetTitle("Pixel Residuals Vs Ring");
    hPIXResVsRing->ProfileX()->GetYaxis()->SetTitle("average Pixel residual mean (#mum)");
    hPIXResVsRing->ProfileX()->GetXaxis()->SetTitle("Ring (#eta)");
    hPIXResVsRing->ProfileX()->GetYaxis()->SetRangeUser(-40.0,40.0);
    setTitleFont(font);
    setTitleSize(0.67);
    
    cTrkPixelProp->cd(6);
    if (!AtlasStyle) {
      gPad->SetGridx();
      gPad->SetGridy();
    }
    hprofGlobPixel = (TProfile*)file->Get("trkana/PIXRes/hproPIXMeanResBaLay_0");
    hprofGlobPixel->SetLineColor(ColorForAll+2);
    hprofGlobPixel->Draw();
    hprofGlobPixel->SetTitle("Pixel Residuals Vs Layer");
    hprofGlobPixel->GetYaxis()->SetTitle("average Pixel residual mean (#mum)");
    hprofGlobPixel->GetXaxis()->SetTitle("Trk layer");
    hprofGlobPixel->GetYaxis()->SetRangeUser(-8.0,8.0);

    //  hprofGlobPixel->SetStats(kFALSE);
    setTitleFont(font);
    setTitleSize(0.60);
    cTrkPixelProp->Update();
    
  }
  else 
    cout << " Pixel - Track properties... No Pixels!!!" << endl;
}
