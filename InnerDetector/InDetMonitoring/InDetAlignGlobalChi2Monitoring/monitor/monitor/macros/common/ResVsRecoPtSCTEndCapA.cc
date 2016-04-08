/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ResVsRecoPtSCTEndCapA() {

  if (iBField>0) {

    TH2F *hSCTEndCapAResVsPtL0;
    TH2F *hSCTEndCapAResVsPtL0Proc;
    TH2F *hSCTEndCapAResVsPtL1;
    TH2F *hSCTEndCapAResVsPtL1Proc;
    TH2F *hSCTEndCapAResVsPtL2;
    TH2F *hSCTEndCapAResVsPtL2Proc;
    TH2F *hSCTEndCapAResVsPtL3;
    TH2F *hSCTEndCapAResVsPtL3Proc;
    TH2F *hSCTEndCapAResVsPtL4;
    TH2F *hSCTEndCapAResVsPtL4Proc;
    TH2F *hSCTEndCapAResVsPtL5;
    TH2F *hSCTEndCapAResVsPtL5Proc;
    TH2F *hSCTEndCapAResVsPtL6;
    TH2F *hSCTEndCapAResVsPtL6Proc;
    TH2F *hSCTEndCapAResVsPtL7;
    TH2F *hSCTEndCapAResVsPtL7Proc;
    TH2F *hSCTEndCapAResVsPtL8;
    TH2F *hSCTEndCapAResVsPtL8Proc;
    
    TProfile *HProfi;
    
    cout << " SCT EndCapA Residuals Vs reco pT per Layer..." << endl;
    
    bool ShowProcessed = true;
    if(SR1Ntuple) ShowProcessed = false;

    cSCTEndCapAResVspT = new TCanvas("cSCTEndCapAResVspT","SCT EndCapA Residuals Vs reco pT per Layer",
				    CanvasSizeX4[0],CanvasSizeX4[1]);
    cSCTEndCapAResVspT->Divide(3,3);
  
    cSCTEndCapAResVspT->cd(1);
    hSCTEndCapAResVsPtL0 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC0Disc0_0");
    hSCTEndCapAResVsPtL0->SetTitle("SCT ECA Res Vs pT-Disc 0");
    hSCTEndCapAResVsPtL0->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapAResVsPtL0->SetLineColor(ColorForAll+2);
    hSCTEndCapAResVsPtL0->Draw();
    cSCTEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapAResVsPtL0->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC0Disc0_%d",level);
      hSCTEndCapAResVsPtL0Proc = (TH2F*)file->Get(name); 
      hSCTEndCapAResVsPtL0Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapAResVsPtL0Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapAResVsPtL0);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL0,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL0Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cSCTEndCapAResVspT->cd(2);
    hSCTEndCapAResVsPtL1 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC0Disc1_0");
    hSCTEndCapAResVsPtL1->SetTitle("SCT ECA Res Vs pT-Disc 1");
    hSCTEndCapAResVsPtL1->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapAResVsPtL1->SetLineColor(ColorForAll+2);
    hSCTEndCapAResVsPtL1->Draw();
    cSCTEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapAResVsPtL1->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC0Disc1_%d",level);
      hSCTEndCapAResVsPtL1Proc = (TH2F*)file->Get(name); 
      hSCTEndCapAResVsPtL1Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapAResVsPtL1Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapAResVsPtL1);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL1,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL1Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cSCTEndCapAResVspT->cd(3);
    hSCTEndCapAResVsPtL2 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC0Disc2_0");
    hSCTEndCapAResVsPtL2->SetTitle("SCT ECA Res Vs pT-Disc 2");
    hSCTEndCapAResVsPtL2->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapAResVsPtL2->SetLineColor(ColorForAll+2);
    hSCTEndCapAResVsPtL2->Draw();
    cSCTEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapAResVsPtL2->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC0Disc2_%d",level);
      hSCTEndCapAResVsPtL2Proc = (TH2F*)file->Get(name); 
      hSCTEndCapAResVsPtL2Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapAResVsPtL2Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapAResVsPtL2);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL2,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL2Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cSCTEndCapAResVspT->cd(4);
    hSCTEndCapAResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC0Disc3_0");
    hSCTEndCapAResVsPtL3->SetTitle("SCT ECA Res Vs pT-Disc 3");
    hSCTEndCapAResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapAResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapAResVsPtL3->Draw();
    cSCTEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapAResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC0Disc3_%d",level);
      hSCTEndCapAResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapAResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapAResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapAResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    //--------------------------------------------------------------
    //

    cSCTEndCapAResVspT->cd(5);
    hSCTEndCapAResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC0Disc4_0");
    hSCTEndCapAResVsPtL3->SetTitle("SCT ECA Res Vs pT-Disc 4");
    hSCTEndCapAResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapAResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapAResVsPtL3->Draw();
    cSCTEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapAResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC0Disc4_%d",level);
      hSCTEndCapAResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapAResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapAResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapAResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    //--------------------------------------------------------------
    //

    cSCTEndCapAResVspT->cd(6);
    hSCTEndCapAResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC0Disc5_0");
    hSCTEndCapAResVsPtL3->SetTitle("SCT ECA Res Vs pT-Disc 5");
    hSCTEndCapAResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapAResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapAResVsPtL3->Draw();
    cSCTEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapAResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC0Disc5_%d",level);
      hSCTEndCapAResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapAResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapAResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapAResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

   //
    //--------------------------------------------------------------
    //

    cSCTEndCapAResVspT->cd(7);
    hSCTEndCapAResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC0Disc6_0");
    hSCTEndCapAResVsPtL3->SetTitle("SCT ECA Res Vs pT-Disc 6");
    hSCTEndCapAResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapAResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapAResVsPtL3->Draw();
    cSCTEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapAResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC0Disc6_%d",level);
      hSCTEndCapAResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapAResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapAResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapAResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

   //
    //--------------------------------------------------------------
    //

    cSCTEndCapAResVspT->cd(8);
    hSCTEndCapAResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC0Disc7_0");
    hSCTEndCapAResVsPtL3->SetTitle("SCT ECA Res Vs pT-Disc 7");
    hSCTEndCapAResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapAResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapAResVsPtL3->Draw();
    cSCTEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapAResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC0Disc7_%d",level);
      hSCTEndCapAResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapAResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapAResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapAResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }
   //
    //--------------------------------------------------------------
    //

    cSCTEndCapAResVspT->cd(9);
    hSCTEndCapAResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC0Disc8_0");
    hSCTEndCapAResVsPtL3->SetTitle("SCT ECA Res Vs pT - Disc 8");
    hSCTEndCapAResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapAResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapAResVsPtL3->Draw();
    cSCTEndCapAResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapAResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapA Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC0Disc8_%d",level);
      hSCTEndCapAResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapAResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapAResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapAResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapAResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }
    
  }
  
  return;

}
