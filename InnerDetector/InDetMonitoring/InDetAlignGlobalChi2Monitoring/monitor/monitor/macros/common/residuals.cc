/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream.h>

void residuals() {

  if (iCosmics && iSetup==5)
    gROOT->ProcessLine(".x macros/SR1/residuals.cc");
  else {

    cout << " Summary table and Residuals..." << endl;

    TH1F *hSCT_TrkRes;
    TH1F *hPixel_TrkRes;
    TH1F *hPixel_EtaTrkRes;

    cInfoRes = new TCanvas("GlobalResiduals","Global Residuals",CanvasSizeX4[0],CanvasSizeX4[1]);
    cInfoRes->Divide(2,2);

    cInfoRes->cd(2);
    if(!PixelOnly) {
      hSCT_TrkRes = (TH1F*)file->Get("trkana/SCTRes/hSCTRes_0");
      hSCT_TrkRes->SetTitle("SCT Residuals (Barrel+EndCaps)");
      hSCT_TrkRes->GetXaxis()->SetTitle("SCT Residuals (mm)");
      hSCT_TrkRes->GetYaxis()->SetTitle("Entries");
      hSCT_TrkRes->SetLineColor(ColorForAll+2);
      hSCT_TrkRes->SetLineWidth(1);
      hSCT_TrkRes->SetFillColor(ColorForAll);
      hSCT_TrkRes->SetFillStyle(3001);
      if (normalizePlots) hSCT_TrkRes->Scale(1./(float)iEvents);
      if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
      hSCT_TrkRes->DrawCopy();
      setTitleFont(font);
      ScalePlots();    
    }
    else {
      TLine *TLineBorder;
      TLineBorder = new TLine(0.02,0.98,0.98,0.98);
      TLineBorder->Draw();
      TLineBorder = new TLine(0.02,0.02,0.98,0.02);
      TLineBorder->Draw();
      TLineBorder = new TLine(0.02,0.98,0.02,0.02);
      TLineBorder->Draw();
      TLineBorder = new TLine(0.98,0.98,0.98,0.02);
      TLineBorder->Draw();

      TLatex *t1 = new TLatex();
      t1->SetNDC();
      t1->SetTextFont(42);
      t1->SetTextSize(0.05);
      t1->SetTextAlign(12);
      sprintf(name,"Pixels Only");
      t1->DrawLatex(0.40,0.50,name);
    }

    //
    // -----------------------------------------------------------------
    //

    cInfoRes->cd(3);
    if(!SCTOnly) {
      hPixel_TrkRes = (TH1F*)file->Get("trkana/PIXRes/hPIXPhiRes_0");
      hPixel_TrkRes->SetTitle("Pixel phi Residuals (Barrel+EndCaps)");
      hPixel_TrkRes->GetXaxis()->SetTitle("Pixel #phi Residuals (mm)");
      hPixel_TrkRes->GetYaxis()->SetTitle("Entries");
      hPixel_TrkRes->SetLineColor(ColorForAll+2);
      hPixel_TrkRes->SetLineWidth(1);
      hPixel_TrkRes->SetFillColor(ColorForAll);
      hPixel_TrkRes->SetFillStyle(3001);
      if (normalizePlots) hPixel_TrkRes->Scale(1./(float)iEvents);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      hPixel_TrkRes->DrawCopy();
      setTitleFont(font);
      ScalePlots();
    }
    else {
      TLine *TLineBorder;
      TLineBorder = new TLine(0.02,0.98,0.98,0.98);
      TLineBorder->Draw();
      TLineBorder = new TLine(0.02,0.02,0.98,0.02);
      TLineBorder->Draw();
      TLineBorder = new TLine(0.02,0.98,0.02,0.02);
      TLineBorder->Draw();
      TLineBorder = new TLine(0.98,0.98,0.98,0.02);
      TLineBorder->Draw();

      TLatex *t1 = new TLatex();
      t1->SetNDC();
      t1->SetTextFont(42);
      t1->SetTextSize(0.05);
      t1->SetTextAlign(12);
      sprintf(name,"SCT Barrel Only");
      t1->DrawLatex(0.40,0.50,name);
    }
  
    //
    // -----------------------------------------------------------------
    //

    cInfoRes->cd(4);
    if(!SCTOnly) {
      hPixel_EtaTrkRes = (TH1F*)file->Get("trkana/PIXRes/hPIXEtaRes_0");
      hPixel_EtaTrkRes->SetTitle("Pixel eta Residuals (Barrel+EndCaps)");
      hPixel_EtaTrkRes->GetXaxis()->SetTitle("Pixel #eta Residuals (mm)");
      hPixel_EtaTrkRes->GetYaxis()->SetTitle("Entries");
      hPixel_EtaTrkRes->SetLineColor(ColorForAll+2);
      hPixel_EtaTrkRes->SetLineWidth(1);
      hPixel_EtaTrkRes->SetFillColor(ColorForAll);
      hPixel_EtaTrkRes->SetFillStyle(3001);
      if (normalizePlots) hPixel_EtaTrkRes->Scale(1./(float)iEvents);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      hPixel_EtaTrkRes->DrawCopy();
      setTitleFont(font);
      ScalePlots();
    }
    else {
      TLine *TLineBorder;
      TLineBorder = new TLine(0.02,0.98,0.98,0.98);
      TLineBorder->Draw();
      TLineBorder = new TLine(0.02,0.02,0.98,0.02);
      TLineBorder->Draw();
      TLineBorder = new TLine(0.02,0.98,0.02,0.02);
      TLineBorder->Draw();
      TLineBorder = new TLine(0.98,0.98,0.98,0.02);
      TLineBorder->Draw();

      TLatex *t1 = new TLatex();
      t1->SetNDC();
      t1->SetTextFont(42);
      t1->SetTextSize(0.05);
      t1->SetTextAlign(12);
      sprintf(name,"SCT Barrel Only");
      t1->DrawLatex(0.40,0.50,name);
    }
  
    // SUMMARY
    cInfoRes->cd(1);
    TLine *TLineBorder;
    TLineBorder = new TLine(0.02,0.98,0.98,0.98);
    TLineBorder->SetLineColor(kRed);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.02,0.02,0.98,0.02);
    TLineBorder->SetLineColor(kRed);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.02,0.98,0.02,0.02);
    TLineBorder->SetLineColor(kRed);
    TLineBorder->Draw();
    TLineBorder = new TLine(0.98,0.98,0.98,0.02);
    TLineBorder->SetLineColor(kRed);
    TLineBorder->Draw();

    int iConsts = 0;
  
    TLatex *t = new TLatex();
    t->SetNDC();
    t->SetTextFont(font);
    t->SetTextSize(0.05);
    t->SetTextAlign(12);
    sprintf(name,"%s",aSetup);
    t->DrawLatex(0.08,0.90,name);
    t->SetTextSize(0.035);

    sprintf(name,"%s",aSim);
    if (iCosmics) t->DrawLatex(0.8,0.83,name);
    else t->DrawLatex(0.8,0.90,name);

    TLine *line1;
    line1 = new TLine(0.06,0.87,0.95,0.87);
    line1->Draw();

    // Information
    t->SetTextSize(0.035);
    sprintf(name,"Iteration: %d",IterNumber);
    t->DrawLatex(0.08,0.82,name);
    sprintf(name,"Events: %d",iEvents);
    t->DrawLatex(0.08,0.78,name);
    sprintf(name,"Total AlignTrks: %d",iTracks);
    t->DrawLatex(0.08,0.74,name);
    sprintf(name," - Total hits: %d [ Pixel hits: %d, SCT hits: %d ]",iHits,iPixelHits,iSCTHits);
    t->DrawLatex(0.08,0.70,name);
    sprintf(name,"Processed AlignTrks: %d",iUsedTracks);
    t->DrawLatex(0.08,0.66,name);
    sprintf(name," - Processed hits: %d",iUsedHits);
    t->DrawLatex(0.08,0.62,name);

    if (doTrkTrack) {
      sprintf(name,"Total Trk::Tracks: %d",iTrkTracks);
      t->DrawLatex(0.08,0.58,name);
      if (isTRT) sprintf(name,"-- Silicon+TRT Tracks --");
      else sprintf(name," -- Silicon Only Tracks --");
      t->DrawLatex(0.08,0.54,name);
    }

    if (iAlignProcessLevel!=999) {
      sprintf(name,"AlignProcessLevel: %d",iAlignProcessLevel);
      t->DrawLatex(0.08,0.45,name);
    }
    if (iTrkCons!=999) {
      sprintf(name,"Track Constraint = %d",iTrkCons);
      t->DrawLatex(0.08,0.41,name);
    }

    if (iBeamSpot!=999) {
      if (iBeamSpot) sprintf(name,"BeamSpot retrieved from the DB");
      else  sprintf(name,"BeamSpot retrieved for the jobOptions");
      t->DrawLatex(0.08,0.37,name);
    }

    //   sprintf(name,"Alignment Parameters: %d",iAlignPars);
    //   t->DrawLatex(0.08,0.32,name);
    if (iAlignOpt==1) sprintf(name,"Using Vtx fit!");
    else sprintf(name,"No Vtx fit used!");
    t->DrawLatex(0.08,0.27,name);

    if (iMinimumHitsPerTrack!=999) {
      sprintf(name,"Internal Track Cut:");
      t->DrawLatex(0.08,0.23,name);
      sprintf(name," - Minimum required hits: %d",iMinimumHitsPerTrack);
      t->DrawLatex(0.08,0.19,name);
      sprintf(name," - Minimum required Pixel hits: %d",iMinimumPixelHitsPerTrack);
      t->DrawLatex(0.08,0.16,name);
      sprintf(name," - Minimum required SCT hits: %d",iMinimumSCTHitsPerTrack);
      t->DrawLatex(0.08,0.13,name);
    }

    if (iBField!=999) {
      sprintf(name,"B Field: %2.2f Tesla",iBField);
      t->DrawLatex(0.08,0.07,name);
    }

    if (iNtupleVersion!=999.0) {
      TLatex *t2 = new TLatex();
      t2->SetNDC();
      t2->SetTextFont(font);
      t2->SetTextSize(0.05);
      t2->SetTextAlign(12);
      t2->SetTextSize(0.035);
      sprintf(name,"Ntuple version: %2.1f",iNtupleVersion);
      t2->DrawLatex(0.63,0.09,name);
    }

    TLatex *t2 = new TLatex();
    t2->SetNDC();
    t2->SetTextFont(font);
    t2->SetTextSize(0.05);
    t2->SetTextAlign(12);
    t2->SetTextSize(0.035);
    sprintf(name,"Number of CPUs used: %d",NumberOfCPUs);
    t2->DrawLatex(0.63,0.13,name);

    TLine *TLineBorder2;
    TLineBorder2 = new TLine(0.61,0.17,0.95,0.17);
    TLineBorder2->SetLineColor(kGray+1);
    TLineBorder2->Draw();
    TLineBorder2 = new TLine(0.61,0.05,0.95,0.05);
    TLineBorder2->SetLineColor(kGray+1);
    TLineBorder2->Draw();
    TLineBorder2 = new TLine(0.61,0.05,0.61,0.17);
    TLineBorder2->SetLineColor(kGray+1);
    TLineBorder2->Draw();
    TLineBorder2 = new TLine(0.95,0.05,0.95,0.17);
    TLineBorder2->SetLineColor(kGray+1);
    TLineBorder2->Draw();

    cInfoRes->Update();
    
  }
}
