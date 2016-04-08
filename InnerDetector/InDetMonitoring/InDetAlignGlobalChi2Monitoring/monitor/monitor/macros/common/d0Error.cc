/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> 

void d0Error() {

  TH1F *hTrkd0error;
  TH2F *hTrkd0Errorphi0;
  TH1F *hUsedTrkd0error;
  TH2F *hUsedTrkd0Errorphi0;

  cout << " sigma(d0) plots..." << endl;

  cd0Error = new TCanvas("cd0Error","d0 Error tests",
			 CanvasSizeX4[0],CanvasSizeX4[1]);
  cd0Error->Divide(2,2);
  
  cd0Error->cd(1);
  if (iCosmics) hTrkd0error = (TH1F*)file->Get("trkana/Track_Parameters/hRecodD0Wide_0");
  else {
    if (SCTOnly) hTrkd0error = (TH1F*)file->Get("trkana/Track_Parameters/hRecodD0Wide_0");
    else  hTrkd0error = (TH1F*)file->Get("trkana/Track_Parameters/hRecodD0_0");
  }
    hTrkd0error->SetTitle("#sigma(d_{0}) (All tracks)");
  hTrkd0error->GetXaxis()->SetTitle("#sigma(d_{}0) (mm)");
  if (normalizePlots) hTrkd0error->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hTrkd0error->SetLineColor(ColorForAll+2);
  hTrkd0error->SetLineWidth(1);
  hTrkd0error->SetFillColor(ColorForAll);
  hTrkd0error->SetFillStyle(3001);
  hTrkd0error->DrawCopy();
  delete hTrkd0error;
  setTitleFont(font);
  ScalePlots();

  cd0Error->cd(2);
  hTrkd0Errorphi0 = (TH2F*)file->Get("trkana/Track_Parameters/hRecodD0_RecoPhi0_0");
  if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
  hTrkd0Errorphi0->SetTitle("#sigma(d_{0}) Vs #phi_{0} (All tracks)");
  hTrkd0Errorphi0->GetYaxis()->SetTitle("#sigma(d_{0}) (mm)");
  hTrkd0Errorphi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
  hTrkd0Errorphi0->Draw("Cont4Z");
  cd0Error->Update();
  if(!AtlasStyle){
    TPaveStats *st = (TPaveStats*)hTrkd0Errorphi0->FindObject("stats"); 
    st->SetX1NDC(0.66); //new x start position
    st->SetX2NDC(0.88); //new x start position
    st->SetY1NDC(0.82); //new y end position
    st->SetOptStat(1110);
    st->Draw();
  }
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hTrkd0Errorphi0->Scale(1./(float)iEvents);
 
  cd0Error->cd(3);
  if (iCosmics) {
    sprintf(name,"trkana/Track_Parameters/hRecodD0Wide_%d",level);
    hUsedTrkd0error = (TH1F*)file->Get(name);
  }
  else {
    if (SCTOnly) {
      sprintf(name,"trkana/Track_Parameters/hRecodD0Wide_%d",level);
      hUsedTrkd0error = (TH1F*)file->Get(name);
    }
    else {
      sprintf(name,"trkana/Track_Parameters/hRecodD0_%d",level);
      hUsedTrkd0error = (TH1F*)file->Get(name);
    }
  }
  hUsedTrkd0error->SetTitle("#sigma(d_{0}) (Processed tracks)");
  hUsedTrkd0error->GetXaxis()->SetTitle("#sigma(d_{0}) (mm)");
  if (normalizePlots) hUsedTrkd0error->Scale(1./(float)iEvents);
  if (!AtlasStyle) { gPad->SetGridy(); gPad->SetGridx(); }
  hUsedTrkd0error->SetLineColor(ColorForUsed+2);
  hUsedTrkd0error->SetLineWidth(1);
  hUsedTrkd0error->SetFillColor(ColorForUsed);
  hUsedTrkd0error->SetFillStyle(3001);
  hUsedTrkd0error->DrawCopy();
  delete hUsedTrkd0error;
  setTitleFont(font);
  ScalePlots();

  cd0Error->cd(4);
  sprintf(name,"trkana/Track_Parameters/hRecodD0_RecoPhi0_%d",level);
  hUsedTrkd0Errorphi0 = (TH2F*)file->Get(name);
  if (!AtlasStyle) { gPad->SetGridx(); gPad->SetGridy(); }
  hUsedTrkd0Errorphi0->SetTitle("#sigma(d_{0}) Vs #phi_{0} (Processed tracks)");
  hUsedTrkd0Errorphi0->GetYaxis()->SetTitle("#sigma(d_{0}) (mm)");
  hUsedTrkd0Errorphi0->GetXaxis()->SetTitle("#phi_{0} (rad)");
  hUsedTrkd0Errorphi0->Draw("Cont4Z");
  cd0Error->Update();
  if(!AtlasStyle){
    TPaveStats *st = (TPaveStats*)hUsedTrkd0Errorphi0->FindObject("stats"); 
    st->SetX1NDC(0.66); //new x start position
    st->SetX2NDC(0.88); //new x start position
    st->SetY1NDC(0.82); //new y end position
    st->SetOptStat(1110);
    st->Draw();
  }
  setTitleFont(font);
  ScalePlots(1);
  if (normalizePlots) hUsedTrkd0Errorphi0->Scale(1./(float)iEvents);
}
