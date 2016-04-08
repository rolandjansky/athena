/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ResVsRecoPtPIXEndCapC() {

  if (iBField>0) {

    TH2F *hPIXPhiEndCapCResVsPtL0;
    TH2F *hPIXPhiEndCapCResVsPtL0Proc;
    TH2F *hPIXPhiEndCapCResVsPtL1;
    TH2F *hPIXPhiEndCapCResVsPtL1Proc;
    TH2F *hPIXPhiEndCapCResVsPtL2;
    TH2F *hPIXPhiEndCapCResVsPtL2Proc;
       
    TH2F *hPIXEtaEndCapCResVsPtL0;
    TH2F *hPIXEtaEndCapCResVsPtL0Proc;
    TH2F *hPIXEtaEndCapCResVsPtL1;
    TH2F *hPIXEtaEndCapCResVsPtL1Proc;
    TH2F *hPIXEtaEndCapCResVsPtL2;
    TH2F *hPIXEtaEndCapCResVsPtL2Proc;
   
    TProfile *HProfi;

    cout << " PIX EndCap C Residuals Vs reco pT per Disc..." << endl;
 
    bool ShowProcessed = true;
    if(SR1Ntuple) ShowProcessed = false;

    cPIXEndCapCResVspT = new TCanvas("cPIXEndCapCResVspT","PIX EndCap C  Residuals Vs reco pT per Disc",
				    CanvasSizeX6[0],CanvasSizeX6[1]);
    cPIXEndCapCResVspT->Divide(3,2);
  
    cPIXEndCapCResVspT->cd(1);
    hPIXPhiEndCapCResVsPtL0 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXPhiEC1Disc0_0");
    hPIXPhiEndCapCResVsPtL0->SetTitle("PIX #phi ECC Res Vs pT-D0");
    hPIXPhiEndCapCResVsPtL0->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXPhiEndCapCResVsPtL0->SetLineColor(ColorForAll+2);
    hPIXPhiEndCapCResVsPtL0->Draw();
    cPIXEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXPhiEndCapCResVsPtL0->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Phi EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXPhiEC1Disc0_%d",level);
      hPIXPhiEndCapCResVsPtL0Proc = (TH2F*)file->Get(name); 
      hPIXPhiEndCapCResVsPtL0Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXPhiEndCapCResVsPtL0Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXPhiEndCapCResVsPtL0);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXPhiEndCapCResVsPtL0,name,"L");
      sprintf(name,"Processed");
      leg0->AddEntry(hPIXPhiEndCapCResVsPtL0Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }


    //
    // -----------------------------------------------------------------
    //

    cPIXEndCapCResVspT->cd(2);
    hPIXPhiEndCapCResVsPtL1 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXPhiEC1Disc1_0");
    hPIXPhiEndCapCResVsPtL1->SetTitle("PIX #phi ECC Res Vs pT-D1");
    hPIXPhiEndCapCResVsPtL1->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXPhiEndCapCResVsPtL1->SetLineColor(ColorForAll+2);
    hPIXPhiEndCapCResVsPtL1->Draw();
    cPIXEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXPhiEndCapCResVsPtL1->ProfileX(); 
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
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXPhiEC1Disc1_%d",level);
      hPIXPhiEndCapCResVsPtL1Proc = (TH2F*)file->Get(name); 
      hPIXPhiEndCapCResVsPtL1Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXPhiEndCapCResVsPtL1Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXPhiEndCapCResVsPtL1);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXPhiEndCapCResVsPtL1,name,"L");
      sprintf(name,"Processed");
      leg0->AddEntry(hPIXPhiEndCapCResVsPtL1Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cPIXEndCapCResVspT->cd(3);
    hPIXPhiEndCapCResVsPtL2 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXPhiEC1Disc2_0");
    hPIXPhiEndCapCResVsPtL2->SetTitle("PIX #phi ECC Res Vs pT-D2");
    hPIXPhiEndCapCResVsPtL2->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXPhiEndCapCResVsPtL2->SetLineColor(ColorForAll+2);
    hPIXPhiEndCapCResVsPtL2->Draw();
    cPIXEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXPhiEndCapCResVsPtL2->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Phi EndCap Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXPhiEC1Disc2_%d",level);
      hPIXPhiEndCapCResVsPtL2Proc = (TH2F*)file->Get(name); 
      hPIXPhiEndCapCResVsPtL2Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXPhiEndCapCResVsPtL2Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXPhiEndCapCResVsPtL2);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXPhiEndCapCResVsPtL2,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXPhiEndCapCResVsPtL2Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cPIXEndCapCResVspT->cd(4);
    hPIXEtaEndCapCResVsPtL0 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXEtaEC1Disc0_0");
    hPIXEtaEndCapCResVsPtL0->SetTitle("PIX #eta ECC Res Vs pT-D0");
    hPIXEtaEndCapCResVsPtL0->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXEtaEndCapCResVsPtL0->SetLineColor(ColorForAll+2);
    hPIXEtaEndCapCResVsPtL0->Draw();
    cPIXEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXEtaEndCapCResVsPtL0->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Eta EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXEtaEC1Disc0_%d",level);
      hPIXEtaEndCapCResVsPtL0Proc = (TH2F*)file->Get(name); 
      hPIXEtaEndCapCResVsPtL0Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXEtaEndCapCResVsPtL0Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXEtaEndCapCResVsPtL0);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXEtaEndCapCResVsPtL0,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXEtaEndCapCResVsPtL0Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }


  cPIXEndCapCResVspT->cd(5);
    hPIXEtaEndCapCResVsPtL1 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXEtaEC1Disc1_0");
    hPIXEtaEndCapCResVsPtL1->SetTitle("PIX #eta ECC Res Vs pT-D1");
    hPIXEtaEndCapCResVsPtL1->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXEtaEndCapCResVsPtL1->SetLineColor(ColorForAll+2);
    hPIXEtaEndCapCResVsPtL1->Draw();
    cPIXEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXEtaEndCapCResVsPtL1->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Eta EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXEtaEC1Disc1_%d",level);
      hPIXEtaEndCapCResVsPtL1Proc = (TH2F*)file->Get(name); 
      hPIXEtaEndCapCResVsPtL1Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXEtaEndCapCResVsPtL1Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXEtaEndCapCResVsPtL1);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXEtaEndCapCResVsPtL1,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXEtaEndCapCResVsPtL1Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    cPIXEndCapCResVspT->cd(6);
    hPIXEtaEndCapCResVsPtL2 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXEtaEC1Disc2_0");
    hPIXEtaEndCapCResVsPtL2->SetTitle("PIX #eta ECC Res Vs pT-D2");
    hPIXEtaEndCapCResVsPtL2->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXEtaEndCapCResVsPtL2->SetLineColor(ColorForAll+2);
    hPIXEtaEndCapCResVsPtL2->Draw();
    cPIXEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXEtaEndCapCResVsPtL2->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Eta EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXEtaEC1Disc2_%d",level);
      hPIXEtaEndCapCResVsPtL2Proc = (TH2F*)file->Get(name); 
      hPIXEtaEndCapCResVsPtL2Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXEtaEndCapCResVsPtL2Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXEtaEndCapCResVsPtL2);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXEtaEndCapCResVsPtL2,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXEtaEndCapCResVsPtL2Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }
    

  }

  return;

}
