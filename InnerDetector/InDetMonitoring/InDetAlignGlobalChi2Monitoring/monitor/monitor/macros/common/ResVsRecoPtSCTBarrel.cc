/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ResVsRecoPtSCTBarrel() {

  if (iBField>0) {

    TH2F *hSCTBarrelResVsPtL0;
    TH2F *hSCTBarrelResVsPtL0Proc;
    TH2F *hSCTBarrelResVsPtL1;
    TH2F *hSCTBarrelResVsPtL1Proc;
    TH2F *hSCTBarrelResVsPtL2;
    TH2F *hSCTBarrelResVsPtL2Proc;
    TH2F *hSCTBarrelResVsPtL3;
    TH2F *hSCTBarrelResVsPtL3Proc;

    TProfile *HProfi;

    cout << " SCT Barrel Residuals Vs reco pT per Layer..." << endl;
 
    bool ShowProcessed = true;
    if(SR1Ntuple) ShowProcessed = false;

    cSCTBarrelResVspT = new TCanvas("cSCTBarrelResVspT","SCT Barrel Residuals Vs reco pT per Layer",
				    CanvasSizeX4[0],CanvasSizeX4[1]);
    cSCTBarrelResVspT->Divide(2,2);
  
    cSCTBarrelResVspT->cd(1);
    hSCTBarrelResVsPtL0 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTBaLay0_0");
    hSCTBarrelResVsPtL0->SetTitle("SCT Barrel Res Vs pT - Layer 0");
    hSCTBarrelResVsPtL0->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTBarrelResVsPtL0->SetLineColor(ColorForAll+2);
    hSCTBarrelResVsPtL0->Draw();
    cSCTBarrelResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTBarrelResVsPtL0->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT Barrel Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTBaLay0_%d",level);
      hSCTBarrelResVsPtL0Proc = (TH2F*)file->Get(name); 
      hSCTBarrelResVsPtL0Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTBarrelResVsPtL0Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTBarrelResVsPtL0);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTBarrelResVsPtL0,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTBarrelResVsPtL0Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cSCTBarrelResVspT->cd(2);
    hSCTBarrelResVsPtL1 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTBaLay1_0");
    hSCTBarrelResVsPtL1->SetTitle("SCT Barrel Res Vs pT - Layer 1");
    hSCTBarrelResVsPtL1->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTBarrelResVsPtL1->SetLineColor(ColorForAll+2);
    hSCTBarrelResVsPtL1->Draw();
    cSCTBarrelResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTBarrelResVsPtL1->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT Barrel Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTBaLay1_%d",level);
      hSCTBarrelResVsPtL1Proc = (TH2F*)file->Get(name); 
      hSCTBarrelResVsPtL1Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTBarrelResVsPtL1Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTBarrelResVsPtL1);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTBarrelResVsPtL1,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTBarrelResVsPtL1Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cSCTBarrelResVspT->cd(3);
    hSCTBarrelResVsPtL2 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTBaLay2_0");
    hSCTBarrelResVsPtL2->SetTitle("SCT Barrel Res Vs pT - Layer 2");
    hSCTBarrelResVsPtL2->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTBarrelResVsPtL2->SetLineColor(ColorForAll+2);
    hSCTBarrelResVsPtL2->Draw();
    cSCTBarrelResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTBarrelResVsPtL2->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT Barrel Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTBaLay2_%d",level);
      hSCTBarrelResVsPtL2Proc = (TH2F*)file->Get(name); 
      hSCTBarrelResVsPtL2Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTBarrelResVsPtL2Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTBarrelResVsPtL2);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTBarrelResVsPtL2,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTBarrelResVsPtL2Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cSCTBarrelResVspT->cd(4);
    hSCTBarrelResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTBaLay3_0");
    hSCTBarrelResVsPtL3->SetTitle("SCT Barrel Res Vs pT - Layer 3");
    hSCTBarrelResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTBarrelResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTBarrelResVsPtL3->Draw();
    cSCTBarrelResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTBarrelResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT Barrel Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTBaLay3_%d",level);
      hSCTBarrelResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTBarrelResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTBarrelResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTBarrelResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTBarrelResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTBarrelResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

  }

  return;

}
