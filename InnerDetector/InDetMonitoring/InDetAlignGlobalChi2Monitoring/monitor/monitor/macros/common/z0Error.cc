/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void z0Error() {

  TH1F *hTrkz0error;
  TH2F *hTrkz0Errorphi0;
  TH2F *hTrkz0ErrorEta;
  TH1F *hUsedTrkz0error;
  TH2F *hUsedTrkz0Errorphi0;
  TH2F *hUsedTrkz0ErrorEta;

  cout << " sigma(z0) plots..." << endl;

  cz0Error = new TCanvas("cz0Error","z0 Error tests",
			 CanvasSizeX6[0],CanvasSizeX6[1]);
  cz0Error->Divide(3,2);
  
  cz0Error->cd(1);
  hTrkz0error = (TH1F*)file->Get("trkana/Track_Parameters/hRecodZ0_0");
  hTrkz0error->SetTitle("#sigma(z_{0}) (All tracks)");
  hTrkz0error->GetXaxis()->SetTitle("#sigma(z_{}0) (mm)");
  if (normalizePlots) hTrkz0error->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrkz0error->SetLineColor(ColorForAll+2);
  hTrkz0error->SetLineWidth(1);
  hTrkz0error->SetFillColor(ColorForAll);
  hTrkz0error->SetFillStyle(3001);
  hTrkz0error->DrawCopy();
  delete hTrkz0error;
  setTitleFont(font);
  ScalePlots();

  //
  // -----------------------------------------------------------------
  //

  cz0Error->cd(2);
  hTrkz0Errorphi0 = (TH2F*)file->Get("trkana/Track_Parameters/hRecodZ0_RecoPhi0_0");
  hTrkz0Errorphi0->SetTitle("#sigma(z_{0}) Vs #phi_{0} (All tracks)");
  hTrkz0Errorphi0->GetYaxis()->SetTitle("#sigma(z_{0}) (mm)");
  hTrkz0Errorphi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrkz0Errorphi0->Draw("Cont4Z");
  cz0Error->Update();
  if(!AtlasStyle){
    TPaveStats *st = (TPaveStats*)hTrkz0Errorphi0->FindObject("stats"); 
    st->SetX1NDC(0.66); //new x start position
    st->SetX2NDC(0.88); //new x start position
    st->SetY1NDC(0.82); //new y end position
    st->SetOptStat(1110);
    st->Draw();
  }
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hTrkz0Errorphi0->Scale(1./(float)iEvents);
 
  //
  // -----------------------------------------------------------------
  //

  cz0Error->cd(3);
  hTrkz0ErrorEta = (TH2F*)file->Get("trkana/Track_Parameters/hRecodZ0_RecoEta_0");
  hTrkz0ErrorEta->SetTitle("#sigma(z_{0}) Vs #eta (All tracks)");
  hTrkz0ErrorEta->GetYaxis()->SetTitle("#sigma(z_{0}) (mm)");
  hTrkz0ErrorEta->GetXaxis()->SetTitle("#eta");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrkz0ErrorEta->Draw("Cont4Z");
  cz0Error->Update();
  if(!AtlasStyle){
    TPaveStats *st = (TPaveStats*)hTrkz0ErrorEta->FindObject("stats"); 
    st->SetX1NDC(0.66); //new x start position
    st->SetX2NDC(0.88); //new x start position
    st->SetY1NDC(0.82); //new y end position
    st->SetOptStat(1110);
    st->Draw();
  }
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hTrkz0ErrorEta->Scale(1./(float)iEvents);

  //
  // -----------------------------------------------------------------
  //

  cz0Error->cd(4);
  sprintf(name,"trkana/Track_Parameters/hRecodZ0_%d",level);
  hUsedTrkz0error = (TH1F*)file->Get(name);
  hUsedTrkz0error->SetTitle("#sigma(z_{0}) (Processed tracks)");
  hUsedTrkz0error->GetXaxis()->SetTitle("#sigma(z_{0}) (mm)");
  if (normalizePlots) hUsedTrkz0error->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hUsedTrkz0error->SetLineColor(ColorForUsed+2);
  hUsedTrkz0error->SetLineWidth(1);
  hUsedTrkz0error->SetFillColor(ColorForUsed);
  hUsedTrkz0error->SetFillStyle(3001);
  hUsedTrkz0error->DrawCopy();
  delete hUsedTrkz0error;
  setTitleFont(font);
  ScalePlots();
 
  //
  // -----------------------------------------------------------------
  //

  cz0Error->cd(5);
  sprintf(name,"trkana/Track_Parameters/hRecodZ0_RecoPhi0_%d",level);
  hUsedTrkz0Errorphi0 = (TH2F*)file->Get(name);
  if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
  hUsedTrkz0Errorphi0->SetTitle("#sigma(z_{0}) Vs #phi_{0} (Processed tracks)");
  hUsedTrkz0Errorphi0->GetYaxis()->SetTitle("#sigma(z_{0}) (mm)");
  hUsedTrkz0Errorphi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
  hUsedTrkz0Errorphi0->Draw("Cont4Z");
  cz0Error->Update();
  if(!AtlasStyle){
    TPaveStats *st = (TPaveStats*)hUsedTrkz0Errorphi0->FindObject("stats"); 
    st->SetX1NDC(0.66); //new x start position
    st->SetX2NDC(0.88); //new x start position
    st->SetY1NDC(0.82); //new y end position
    st->SetOptStat(1110);
    st->Draw();
  }
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hUsedTrkz0Errorphi0->Scale(1./(float)iEvents);
 
  //
  // -----------------------------------------------------------------
  //

  cz0Error->cd(6);
  sprintf(name,"trkana/Track_Parameters/hRecodZ0_RecoEta_%d",level);
  hUsedTrkz0ErrorEta = (TH2F*)file->Get(name);
  hUsedTrkz0ErrorEta->SetTitle("#sigma(z_{0}) Vs #eta (Processed tracks)");
  hUsedTrkz0ErrorEta->GetYaxis()->SetTitle("#sigma(z_{0}) (mm)");
  hUsedTrkz0ErrorEta->GetXaxis()->SetTitle("#eta");
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hUsedTrkz0ErrorEta->Draw("Cont4Z");
  cz0Error->Update();
  if(!AtlasStyle){
    TPaveStats *st = (TPaveStats*)hUsedTrkz0ErrorEta->FindObject("stats"); 
    st->SetX1NDC(0.66); //new x start position
    st->SetX2NDC(0.88); //new x start position
    st->SetY1NDC(0.82); //new y end position
    st->SetOptStat(1110);
    st->Draw();
  }
  setTitleFont(font);
  ScalePlots(1);

  return;
}
