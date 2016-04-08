/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream.h>

void residuals() {

  TH1F *hSCT_TrkRes;
  TH1F *hSCT_TrkResSide0;
  TH1F *hSCT_TrkResSide1;

  cout << " Summary table and Residuals..." << endl;

  cInfoRes = new TCanvas("GlobalResiduals","Global Residuals",900,700);
  cInfoRes->Divide(2,2);

  cInfoRes->cd(2);
  hSCT_TrkRes = (TH1F*)file->Get("trkana/SCTRes/hSCTResBaWide_0");
  hSCT_TrkRes->SetTitle("SCT Residuals (Barrel)");
  hSCT_TrkRes->GetXaxis()->SetTitle("SCT Residuals (mm)");
  hSCT_TrkRes->SetLineColor(ColorForAll+2);
  hSCT_TrkRes->SetLineWidth(1);
  hSCT_TrkRes->SetFillColor(ColorForAll);
  hSCT_TrkRes->SetFillStyle(3001);
  if (normalizePlots) hSCT_TrkRes->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCT_TrkRes->DrawCopy();
  setTitleFont(font);
  ScalePlots();    
      
  //
  // -----------------------------------------------------------------
  //

  cInfoRes->cd(3);
  hSCT_TrkResSide0 = (TH1F*)file->Get("trkana/SCTRes/hSCTResBaSide0_0");
  hSCT_TrkResSide0->SetTitle("SCT Residuals (Barrel - Side 0)");
  hSCT_TrkResSide0->GetXaxis()->SetTitle("SCT Residuals (mm)");
  hSCT_TrkResSide0->SetLineColor(ColorForAll+2);
  hSCT_TrkResSide0->SetLineWidth(1);
  hSCT_TrkResSide0->SetFillColor(ColorForAll);
  hSCT_TrkResSide0->SetFillStyle(3001);
  if (normalizePlots) hSCT_TrkResSide0->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCT_TrkResSide0->DrawCopy();
  setTitleFont(font);
  ScalePlots(); 

  //
  // -----------------------------------------------------------------
  //
  
  cInfoRes->cd(4);
  hSCT_TrkResSide1= (TH1F*)file->Get("trkana/SCTRes/hSCTResBaSide1_0");
  hSCT_TrkResSide1->SetTitle("SCT Residuals (Barrel - Side 1)");
  hSCT_TrkResSide1->GetXaxis()->SetTitle("SCT Residuals (mm)");
  hSCT_TrkResSide1->SetLineColor(ColorForAll+2);
  hSCT_TrkResSide1->SetLineWidth(1);
  hSCT_TrkResSide1->SetFillColor(ColorForAll);
  hSCT_TrkResSide1->SetFillStyle(3001);
  if (normalizePlots) hSCT_TrkResSide1->Scale(1./(float)iEvents);
  if (!AtlasStyle) {
    gPad->SetGridy();
    gPad->SetGridx();
  }
  hSCT_TrkResSide1->DrawCopy();
  setTitleFont(font);
  ScalePlots(); 
 
  //
  // -----------------------------------------------------------------
  //

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
  sprintf(name," - Total hits: %d",iHits);
  t->DrawLatex(0.08,0.70,name);
  sprintf(name,"Processed AlignTrks: %d",iUsedTracks);
  t->DrawLatex(0.08,0.66,name);
  sprintf(name," - Processed hits: %d",iUsedHits);
  t->DrawLatex(0.08,0.62,name);

  if (doTrkTrack) {
    sprintf(name,"Total Trk::Tracks: %d",iTrkTracks);
    t->DrawLatex(0.08,0.58,name);
  }

  // sprintf(name,"Requested modules %d (%d DoF)",ihitmap,6*ihitmap);
  //t->DrawLatex(0.08,0.66,name);
  // sprintf(name,"- Modules with hits: %d",total);
  //t->DrawLatex(0.08,0.62,name);
  //   sprintf(name,"- HitCut per Module: %d",iNhitsCut);
  //   t->DrawLatex(0.08,0.58,name);
  //sprintf(name,"- Modules with more than %d hits: %d",iNhitsCut,iDoF/iAlignPars);
  //t->DrawLatex(0.08,0.54,name);
  //sprintf(name,"- Mods excluded from alignment: %d",ihitmap-iConsts);
  //t->DrawLatex(0.08,0.50,name);
  //   sprintf(name,"Selected modules %d (%d DoF)",iDoF/iAlignPars,iDoF);
  //   t->DrawLatex(0.08,0.46,name);
  //   sprintf(name,"ModCut: %d",iModCut);
  //   t->DrawLatex(0.08,0.41,name);

  if (iAlignProcessLevel!=999) {
    sprintf(name,"AlignProcessLevel: %d",iAlignProcessLevel);
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
    t2->DrawLatex(0.65,0.09,name);
  }

  TLatex *t2 = new TLatex();
  t2->SetNDC();
  t2->SetTextFont(font);
  t2->SetTextSize(0.05);
  t2->SetTextAlign(12);
  t2->SetTextSize(0.035);
  sprintf(name,"Number of CPUs used: %d",NumberOfCPUs);
  t2->DrawLatex(0.65,0.13,name);

  TLine *TLineBorder2;
  TLineBorder2 = new TLine(0.63,0.17,0.95,0.17);
  TLineBorder2->SetLineColor(kGray+1);
  TLineBorder2->Draw();
  TLineBorder2 = new TLine(0.63,0.05,0.95,0.05);
  TLineBorder2->SetLineColor(kGray+1);
  TLineBorder2->Draw();
  TLineBorder2 = new TLine(0.63,0.05,0.63,0.17);
  TLineBorder2->SetLineColor(kGray+1);
  TLineBorder2->Draw();
  TLineBorder2 = new TLine(0.95,0.05,0.95,0.17);
  TLineBorder2->SetLineColor(kGray+1);
  TLineBorder2->Draw();

  cInfoRes->Update();
    
}
