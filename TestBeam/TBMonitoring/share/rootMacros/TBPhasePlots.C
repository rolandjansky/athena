#include <iostream>

#include "TCanvas.h"
#include "TH1F.h"
#include "TPaveLabel.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void TBPhasePlots(TString path, TString canvasTitle, TString canvasNameBase) {

  gDirectory->cd(path);

  TString stringToExecute = "closeCanvases(\"" + canvasNameBase + "\")";
  gROOT->ProcessLine(stringToExecute);

  TCanvas* oneCanvas = new TCanvas(canvasNameBase, canvasTitle, 400, 10, 600, 462);
  oneCanvas->Divide(1, 1);
  
  gStyle->SetOptStat(111111);
  TPaveLabel* dateLabel = dateAndTimeLabel();

  TH1F* h;
  if (h = (TH1F*)gDirectory->Get("TDCphase")) {
    oneCanvas->cd(1);
    h->GetXaxis()->SetLabelSize(0.05);
    h->GetXaxis()->SetTitle("TDC phase (ns)");
    h->GetYaxis()->SetLabelSize(0.04);
    h->Draw("HIST");
    dateLabel->Draw();
    oneCanvas->Update();
  }

  gDirectory->cd();
}
