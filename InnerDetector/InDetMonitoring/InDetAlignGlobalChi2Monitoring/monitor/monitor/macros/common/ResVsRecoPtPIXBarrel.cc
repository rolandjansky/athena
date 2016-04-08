/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ResVsRecoPtPIXBarrel() {

  if (iBField>0) {

    TH2F *hPIXPhiBarrelResVsPtL0;
    TH2F *hPIXPhiBarrelResVsPtL0Proc;
    TH2F *hPIXPhiBarrelResVsPtL1;
    TH2F *hPIXPhiBarrelResVsPtL1Proc;
    TH2F *hPIXPhiBarrelResVsPtL2;
    TH2F *hPIXPhiBarrelResVsPtL2Proc;
       
    TH2F *hPIXEtaBarrelResVsPtL0;
    TH2F *hPIXEtaBarrelResVsPtL0Proc;
    TH2F *hPIXEtaBarrelResVsPtL1;
    TH2F *hPIXEtaBarrelResVsPtL1Proc;
    TH2F *hPIXEtaBarrelResVsPtL2;
    TH2F *hPIXEtaBarrelResVsPtL2Proc;
   
    TProfile *HProfi;

    cout << " PIX Barrel Residuals Vs reco pT per Layer..." << endl;
 
    bool ShowProcessed = true;
    if(SR1Ntuple) ShowProcessed = false;

    cPIXBarrelResVspT = new TCanvas("cPIXBarrelResVspT","PIX Barrel Residuals Vs reco pT per Layer",
				    CanvasSizeX6[0],CanvasSizeX6[1]);
    cPIXBarrelResVspT->Divide(3,2);
  
    cPIXBarrelResVspT->cd(1);
    hPIXPhiBarrelResVsPtL0 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXPhiBaLay0_0");
    hPIXPhiBarrelResVsPtL0->SetTitle("PIX #phi Ba Res Vs pT -L0");
    hPIXPhiBarrelResVsPtL0->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXPhiBarrelResVsPtL0->SetLineColor(ColorForAll+2);
    hPIXPhiBarrelResVsPtL0->Draw();
    cPIXBarrelResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXPhiBarrelResVsPtL0->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Phi Barrel Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXPhiBaLay0_%d",level);
      hPIXPhiBarrelResVsPtL0Proc = (TH2F*)file->Get(name); 
      hPIXPhiBarrelResVsPtL0Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXPhiBarrelResVsPtL0Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXPhiBarrelResVsPtL0);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXPhiBarrelResVsPtL0,name,"L");
      sprintf(name,"Processed");
      leg0->AddEntry(hPIXPhiBarrelResVsPtL0Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }


    //
    // -----------------------------------------------------------------
    //

    cPIXBarrelResVspT->cd(2);
    hPIXPhiBarrelResVsPtL1 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXPhiBaLay1_0");
    hPIXPhiBarrelResVsPtL1->SetTitle("PIX #phi Ba Res Vs pT -L1");
    hPIXPhiBarrelResVsPtL1->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXPhiBarrelResVsPtL1->SetLineColor(ColorForAll+2);
    hPIXPhiBarrelResVsPtL1->Draw();
    cPIXBarrelResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXPhiBarrelResVsPtL1->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Phi Barrel Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXPhiBaLay1_%d",level);
      hPIXPhiBarrelResVsPtL1Proc = (TH2F*)file->Get(name); 
      hPIXPhiBarrelResVsPtL1Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXPhiBarrelResVsPtL1Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXPhiBarrelResVsPtL1);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXPhiBarrelResVsPtL1,name,"L");
      sprintf(name,"Processed");
      leg0->AddEntry(hPIXPhiBarrelResVsPtL1Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cPIXBarrelResVspT->cd(3);
    hPIXPhiBarrelResVsPtL2 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXPhiBaLay2_0");
    hPIXPhiBarrelResVsPtL2->SetTitle("PIX #phi Ba Res Vs pT -L2");
    hPIXPhiBarrelResVsPtL2->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXPhiBarrelResVsPtL2->SetLineColor(ColorForAll+2);
    hPIXPhiBarrelResVsPtL2->Draw();
    cPIXBarrelResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXPhiBarrelResVsPtL2->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT Barrel Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXPhiBaLay2_%d",level);
      hPIXPhiBarrelResVsPtL2Proc = (TH2F*)file->Get(name); 
      hPIXPhiBarrelResVsPtL2Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXPhiBarrelResVsPtL2Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXPhiBarrelResVsPtL2);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXPhiBarrelResVsPtL2,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXPhiBarrelResVsPtL2Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cPIXBarrelResVspT->cd(4);
    hPIXEtaBarrelResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXEtaBaLay0_0");
    hPIXEtaBarrelResVsPtL3->SetTitle("PIX #eta Ba Res Vs pT -L0");
    hPIXEtaBarrelResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXEtaBarrelResVsPtL3->SetLineColor(ColorForAll+2);
    hPIXEtaBarrelResVsPtL3->Draw();
    cPIXBarrelResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXEtaBarrelResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Eta Barrel Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXEtaBaLay0_%d",level);
      hPIXEtaBarrelResVsPtL3Proc = (TH2F*)file->Get(name); 
      hPIXEtaBarrelResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXEtaBarrelResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXEtaBarrelResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXEtaBarrelResVsPtL3,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXEtaBarrelResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }


  cPIXBarrelResVspT->cd(5);
    hPIXEtaBarrelResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXEtaBaLay1_0");
    hPIXEtaBarrelResVsPtL3->SetTitle("PIX #eta Ba Res Vs pT -L1");
    hPIXEtaBarrelResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXEtaBarrelResVsPtL3->SetLineColor(ColorForAll+2);
    hPIXEtaBarrelResVsPtL3->Draw();
    cPIXBarrelResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXEtaBarrelResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Eta Barrel Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXEtaBaLay1_%d",level);
      hPIXEtaBarrelResVsPtL3Proc = (TH2F*)file->Get(name); 
      hPIXEtaBarrelResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXEtaBarrelResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXEtaBarrelResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXEtaBarrelResVsPtL3,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXEtaBarrelResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    cPIXBarrelResVspT->cd(6);
    hPIXEtaBarrelResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXEtaBaLay2_0");
    hPIXEtaBarrelResVsPtL3->SetTitle("PIX #eta Ba Res Vs pT -L2");
    hPIXEtaBarrelResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXEtaBarrelResVsPtL3->SetLineColor(ColorForAll+2);
    hPIXEtaBarrelResVsPtL3->Draw();
    cPIXBarrelResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXEtaBarrelResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Eta Barrel Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXEtaBaLay2_%d",level);
      hPIXEtaBarrelResVsPtL3Proc = (TH2F*)file->Get(name); 
      hPIXEtaBarrelResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXEtaBarrelResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXEtaBarrelResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXEtaBarrelResVsPtL3,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXEtaBarrelResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }
    

  }

  return;

}
