/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ResVsRecoPtPIXEndCapA() {

  if (iBField>0) {

    TH2F *hPIXPhiEndCapAResVsPtL0;
    TH2F *hPIXPhiEndCapAResVsPtL0Proc;
    TH2F *hPIXPhiEndCapAResVsPtL1;
    TH2F *hPIXPhiEndCapAResVsPtL1Proc;
    TH2F *hPIXPhiEndCapAResVsPtL2;
    TH2F *hPIXPhiEndCapAResVsPtL2Proc;
       
    TH2F *hPIXEtaEndCapAResVsPtL0;
    TH2F *hPIXEtaEndCapAResVsPtL0Proc;
    TH2F *hPIXEtaEndCapAResVsPtL1;
    TH2F *hPIXEtaEndCapAResVsPtL1Proc;
    TH2F *hPIXEtaEndCapAResVsPtL2;
    TH2F *hPIXEtaEndCapAResVsPtL2Proc;
   
    TProfile *HProfi;

    cout << " PIX EndCap A Residuals Vs reco pT per Disc..." << endl;
 
    bool ShowProcessed = true;
    if(SR1Ntuple) ShowProcessed = false;

    cPIXEndCapAResVspT = new TCanvas("cPIXEndCapAResVspT","PIX EndCap A  Residuals Vs reco pT per Disc",
				    CanvasSizeX6[0],CanvasSizeX6[1]);
    cPIXEndCapAResVspT->Divide(3,2);
  
    cPIXEndCapAResVspT->cd(1);
    hPIXPhiEndCapAResVsPtL0 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXPhiEC0Disc0_0");
    hPIXPhiEndCapAResVsPtL0->SetTitle("PIX #phi ECA Res Vs pT-D0");
    hPIXPhiEndCapAResVsPtL0->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXPhiEndCapAResVsPtL0->SetLineColor(ColorForAll+2);
    hPIXPhiEndCapAResVsPtL0->Draw();
    cPIXEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXPhiEndCapAResVsPtL0->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Phi EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXPhiEC0Disc0_%d",level);
      hPIXPhiEndCapAResVsPtL0Proc = (TH2F*)file->Get(name); 
      hPIXPhiEndCapAResVsPtL0Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXPhiEndCapAResVsPtL0Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXPhiEndCapAResVsPtL0);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXPhiEndCapAResVsPtL0,name,"L");
      sprintf(name,"Processed");
      leg0->AddEntry(hPIXPhiEndCapAResVsPtL0Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }


    //
    // -----------------------------------------------------------------
    //

    cPIXEndCapAResVspT->cd(2);
    hPIXPhiEndCapAResVsPtL1 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXPhiEC0Disc1_0");
    hPIXPhiEndCapAResVsPtL1->SetTitle("PIX #phi ECA Res Vs pT-D1");
    hPIXPhiEndCapAResVsPtL1->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXPhiEndCapAResVsPtL1->SetLineColor(ColorForAll+2);
    hPIXPhiEndCapAResVsPtL1->Draw();
    cPIXEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXPhiEndCapAResVsPtL1->ProfileX(); 
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
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXPhiEC0Disc1_%d",level);
      hPIXPhiEndCapAResVsPtL1Proc = (TH2F*)file->Get(name); 
      hPIXPhiEndCapAResVsPtL1Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXPhiEndCapAResVsPtL1Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXPhiEndCapAResVsPtL1);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXPhiEndCapAResVsPtL1,name,"L");
      sprintf(name,"Processed");
      leg0->AddEntry(hPIXPhiEndCapAResVsPtL1Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cPIXEndCapAResVspT->cd(3);
    hPIXPhiEndCapAResVsPtL2 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXPhiEC0Disc2_0");
    hPIXPhiEndCapAResVsPtL2->SetTitle("PIX #phi ECA Res Vs pT-D2");
    hPIXPhiEndCapAResVsPtL2->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXPhiEndCapAResVsPtL2->SetLineColor(ColorForAll+2);
    hPIXPhiEndCapAResVsPtL2->Draw();
    cPIXEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXPhiEndCapAResVsPtL2->ProfileX(); 
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
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXPhiEC0Disc2_%d",level);
      hPIXPhiEndCapAResVsPtL2Proc = (TH2F*)file->Get(name); 
      hPIXPhiEndCapAResVsPtL2Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXPhiEndCapAResVsPtL2Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXPhiEndCapAResVsPtL2);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXPhiEndCapAResVsPtL2,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXPhiEndCapAResVsPtL2Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cPIXEndCapAResVspT->cd(4);
    hPIXEtaEndCapAResVsPtL0 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXEtaEC0Disc0_0");
    hPIXEtaEndCapAResVsPtL0->SetTitle("PIX #eta ECA Res Vs pT-D0");
    hPIXEtaEndCapAResVsPtL0->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXEtaEndCapAResVsPtL0->SetLineColor(ColorForAll+2);
    hPIXEtaEndCapAResVsPtL0->Draw();
    cPIXEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXEtaEndCapAResVsPtL0->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Eta EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXEtaEC0Disc0_%d",level);
      hPIXEtaEndCapAResVsPtL0Proc = (TH2F*)file->Get(name); 
      hPIXEtaEndCapAResVsPtL0Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXEtaEndCapAResVsPtL0Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXEtaEndCapAResVsPtL0);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXEtaEndCapAResVsPtL0,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXEtaEndCapAResVsPtL0Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }


  cPIXEndCapAResVspT->cd(5);
    hPIXEtaEndCapAResVsPtL1 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXEtaEC0Disc1_0");
    hPIXEtaEndCapAResVsPtL1->SetTitle("PIX #eta ECA Res Vs pT-D1");
    hPIXEtaEndCapAResVsPtL1->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXEtaEndCapAResVsPtL1->SetLineColor(ColorForAll+2);
    hPIXEtaEndCapAResVsPtL1->Draw();
    cPIXEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXEtaEndCapAResVsPtL1->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Eta EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXEtaEC0Disc1_%d",level);
      hPIXEtaEndCapAResVsPtL1Proc = (TH2F*)file->Get(name); 
      hPIXEtaEndCapAResVsPtL1Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXEtaEndCapAResVsPtL1Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXEtaEndCapAResVsPtL1);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXEtaEndCapAResVsPtL1,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXEtaEndCapAResVsPtL1Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    cPIXEndCapAResVspT->cd(6);
    hPIXEtaEndCapAResVsPtL2 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_PIXEtaEC0Disc2_0");
    hPIXEtaEndCapAResVsPtL2->SetTitle("PIX #eta ECA Res Vs pT-D2");
    hPIXEtaEndCapAResVsPtL2->GetXaxis()->SetTitle("pT (GeV/c)");
    hPIXEtaEndCapAResVsPtL2->SetLineColor(ColorForAll+2);
    hPIXEtaEndCapAResVsPtL2->Draw();
    cPIXEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hPIXEtaEndCapAResVsPtL2->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("PIX Eta EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(62);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_PIXEtaEC0Disc2_%d",level);
      hPIXEtaEndCapAResVsPtL2Proc = (TH2F*)file->Get(name); 
      hPIXEtaEndCapAResVsPtL2Proc->SetLineColor(ColorForUsed+2);
      HProfi = hPIXEtaEndCapAResVsPtL2Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hPIXEtaEndCapAResVsPtL2);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hPIXEtaEndCapAResVsPtL2,name,"L");
      sprintf(name,"Processed ");
      leg0->AddEntry(hPIXEtaEndCapAResVsPtL2Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }
    

  }

  return;

}
