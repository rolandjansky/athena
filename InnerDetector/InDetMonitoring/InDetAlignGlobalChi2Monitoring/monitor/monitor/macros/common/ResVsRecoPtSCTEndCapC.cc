/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ResVsRecoPtSCTEndCapC(int level=1) {

  if (iBField>0) {

    TH2F *hSCTEndCapCResVsPtL0;
    TH2F *hSCTEndCapCResVsPtL0Proc;
    TH2F *hSCTEndCapCResVsPtL1;
    TH2F *hSCTEndCapCResVsPtL1Proc;
    TH2F *hSCTEndCapCResVsPtL2;
    TH2F *hSCTEndCapCResVsPtL2Proc;
    TH2F *hSCTEndCapCResVsPtL3;
    TH2F *hSCTEndCapCResVsPtL3Proc;
    TH2F *hSCTEndCapCResVsPtL4;
    TH2F *hSCTEndCapCResVsPtL4Proc;
    TH2F *hSCTEndCapCResVsPtL5;
    TH2F *hSCTEndCapCResVsPtL5Proc;
    TH2F *hSCTEndCapCResVsPtL6;
    TH2F *hSCTEndCapCResVsPtL6Proc;
    TH2F *hSCTEndCapCResVsPtL7;
    TH2F *hSCTEndCapCResVsPtL7Proc;
    TH2F *hSCTEndCapCResVsPtL8;
    TH2F *hSCTEndCapCResVsPtL8Proc;
    
    TProfile *HProfi;
    
    cout << " SCT EndCapC Residuals Vs reco pT per Layer..." << endl;
    
    bool ShowProcessed = true;
    if(SR1Ntuple) ShowProcessed = false;

    cSCTEndCapCResVspT = new TCanvas("cSCTEndCapCResVspT","SCT EndCapC Residuals Vs reco pT per Layer",
				    CanvasSizeX4[0],CanvasSizeX4[1]);
    cSCTEndCapCResVspT->Divide(3,3);
  
    cSCTEndCapCResVspT->cd(1);
    hSCTEndCapCResVsPtL0 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC1Disc0_0");
    hSCTEndCapCResVsPtL0->SetTitle("SCT ECC Res Vs pT-Disc 0");
    hSCTEndCapCResVsPtL0->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapCResVsPtL0->SetLineColor(ColorForAll+2);
    hSCTEndCapCResVsPtL0->Draw();
    cSCTEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapCResVsPtL0->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC1Disc0_%d",level);
      hSCTEndCapCResVsPtL0Proc = (TH2F*)file->Get(name); 
      hSCTEndCapCResVsPtL0Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapCResVsPtL0Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapCResVsPtL0);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL0,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL0Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cSCTEndCapCResVspT->cd(2);
    hSCTEndCapCResVsPtL1 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC1Disc1_0");
    hSCTEndCapCResVsPtL1->SetTitle("SCT ECC Res Vs pT-Disc 1");
    hSCTEndCapCResVsPtL1->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapCResVsPtL1->SetLineColor(ColorForAll+2);
    hSCTEndCapCResVsPtL1->Draw();
    cSCTEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapCResVsPtL1->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC1Disc1_%d",level);
      hSCTEndCapCResVsPtL1Proc = (TH2F*)file->Get(name); 
      hSCTEndCapCResVsPtL1Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapCResVsPtL1Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapCResVsPtL1);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL1,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL1Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cSCTEndCapCResVspT->cd(3);
    hSCTEndCapCResVsPtL2 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC1Disc2_0");
    hSCTEndCapCResVsPtL2->SetTitle("SCT ECC Res Vs pT-Disc 2");
    hSCTEndCapCResVsPtL2->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapCResVsPtL2->SetLineColor(ColorForAll+2);
    hSCTEndCapCResVsPtL2->Draw();
    cSCTEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapCResVsPtL2->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC1Disc2_%d",level);
      hSCTEndCapCResVsPtL2Proc = (TH2F*)file->Get(name); 
      hSCTEndCapCResVsPtL2Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapCResVsPtL2Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapCResVsPtL2);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL2,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL2Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    // -----------------------------------------------------------------
    //

    cSCTEndCapCResVspT->cd(4);
    hSCTEndCapCResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC1Disc3_0");
    hSCTEndCapCResVsPtL3->SetTitle("SCT ECC Res Vs pT-Disc 3");
    hSCTEndCapCResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapCResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapCResVsPtL3->Draw();
    cSCTEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapCResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC1Disc3_%d",level);
      hSCTEndCapCResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapCResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapCResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapCResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    //--------------------------------------------------------------
    //

    cSCTEndCapCResVspT->cd(5);
    hSCTEndCapCResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC1Disc4_0");
    hSCTEndCapCResVsPtL3->SetTitle("SCT ECC Res Vs pT-Disc 4");
    hSCTEndCapCResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapCResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapCResVsPtL3->Draw();
    cSCTEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapCResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC1Disc4_%d",level);
      hSCTEndCapCResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapCResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapCResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapCResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

    //
    //--------------------------------------------------------------
    //

    cSCTEndCapCResVspT->cd(6);
    hSCTEndCapCResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC1Disc5_0");
    hSCTEndCapCResVsPtL3->SetTitle("SCT ECC Res Vs pT-Disc 5");
    hSCTEndCapCResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapCResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapCResVsPtL3->Draw();
    cSCTEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapCResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC1Disc5_%d",level);
      hSCTEndCapCResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapCResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapCResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapCResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

   //
    //--------------------------------------------------------------
    //

    cSCTEndCapCResVspT->cd(7);
    hSCTEndCapCResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC1Disc6_0");
    hSCTEndCapCResVsPtL3->SetTitle("SCT ECC Res Vs pT-Disc 6");
    hSCTEndCapCResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapCResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapCResVsPtL3->Draw();
    cSCTEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapCResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC1Disc6_%d",level);
      hSCTEndCapCResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapCResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapCResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapCResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }

   //
    //--------------------------------------------------------------
    //

    cSCTEndCapCResVspT->cd(8);
    hSCTEndCapCResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC1Disc7_0");
    hSCTEndCapCResVsPtL3->SetTitle("SCT ECC Res Vs pT-Disc 7");
    hSCTEndCapCResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapCResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapCResVsPtL3->Draw();
    cSCTEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapCResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC1Disc7_%d",level);
      hSCTEndCapCResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapCResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapCResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapCResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }
   //
    //--------------------------------------------------------------
    //

    cSCTEndCapCResVspT->cd(9);
    hSCTEndCapCResVsPtL3 = (TH2F*)file->Get("trkana/Track_Parameters/hResRecoPt_SCTEC1Disc8_0");
    hSCTEndCapCResVsPtL3->SetTitle("SCT ECC Res Vs pT - Disc 8");
    hSCTEndCapCResVsPtL3->GetXaxis()->SetTitle("pT (GeV/c)");
    hSCTEndCapCResVsPtL3->SetLineColor(ColorForAll+2);
    hSCTEndCapCResVsPtL3->Draw();
    cSCTEndCapCResVspT->Update();
    if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }

    HProfi = hSCTEndCapCResVsPtL3->ProfileX(); 
    HProfi->SetLineColor(ColorForAll+2);
    HProfi->SetMarkerColor(ColorForAll+2);
    HProfi->GetYaxis()->SetTitle("SCT EndCapC Residuals (mm)");
    HProfi->SetMarkerStyle(21);
    HProfi->SetMarkerSize(0.4);
    HProfi->SetStats(kFALSE);
    HProfi->DrawCopy();
    setTitleSize(0.4);
    setTitleFont(font);

    if (ShowProcessed) {
      sprintf(name,"trkana/Track_Parameters/hResRecoPt_SCTEC1Disc8_%d",level);
      hSCTEndCapCResVsPtL3Proc = (TH2F*)file->Get(name); 
      hSCTEndCapCResVsPtL3Proc->SetLineColor(ColorForUsed+2);
      HProfi = hSCTEndCapCResVsPtL3Proc->ProfileX(); 
      HProfi->SetLineColor(ColorForUsed+1);
      HProfi->SetMarkerColor(ColorForUsed+1);
      HProfi->SetMarkerStyle(21);
      HProfi->SetMarkerSize(0.4);
      HProfi->DrawCopy("SAME");
      redrawTPaveStats(hSCTEndCapCResVsPtL3);

      TLegend *leg0 = new TLegend(0.41,0.89,0.67,1.0);
      sprintf(name,"All tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3,name,"L");
      sprintf(name,"Processed tracks");
      leg0->AddEntry(hSCTEndCapCResVsPtL3Proc,name,"L");
      leg0->SetTextSize(0.035);
      leg0->SetFillColor(kWhite);
      leg0->Draw();
    }
    
  }
  
  return;

}
