/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonShowerShapeFudgeTool/TPhotonMCShifterTool.h"

#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TString.h"

int main() {

  int preselection = 18;
  float eta=0.3;
  int isConv = 0;

  TPhotonMCShifterTool fftool;

  TCanvas* c = new TCanvas("c","c",800,600);

  TGraphErrors* gr = 0;
  gr = fftool.GetFFmap_Rhad1(eta, isConv,preselection);
  gr->Draw("AP");
  TString grtitle = Form("FF_RHAD1_TUNE%d_ETA%.2F_CONV%d",preselection,eta,isConv);
  gr->SetTitle(grtitle);
  c->Print(Form("%s.pdf",grtitle.Data()));
  delete c;

  return 0;
}
