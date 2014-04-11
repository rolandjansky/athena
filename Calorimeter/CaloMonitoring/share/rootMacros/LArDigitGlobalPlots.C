////////////////////////////////////////////////////////////
// LArDigitGlobalPlots.C
// 
// last modified 2004/06/23
//
// Plotting macro for histograms summed over all febs
////////////////////////////////////////////////////////////

#include <iostream>

#include "TCanvas.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void LArDigitGlobalPlots(TString path, TString canvasTitle, TString canvasNameBase) {
  // TString path           directory path
  // TString canvasTitle    each canvas has a title composed of the canvas name followed by canvasTitle
  // TString canvasNameBase each canvas name is composed of the canvasNameBase followed by an index

  if (!gDirectory->cd(path)) {
    TString message = "Path " + path + " not found";
    OKButton(message);
    return;
  }

  TString stringToExecute = "closeCanvases(\"" + canvasNameBase + "\")";
  gROOT->ProcessLine(stringToExecute);

  TCanvas* oneCanvas = new TCanvas(canvasNameBase, canvasTitle, 400, 10, 600, 462);
  oneCanvas->Divide(1, 2);
  oneCanvas->cd(2);
  gPad->Divide(2, 1);

  oneCanvas->cd(1);
  if (plotHisto("ADCmPed", "ADC-ped sum")) oneCanvas->Update();
  
  oneCanvas->cd(2);
  gPad->cd(1);
  if (plotHisto("MaxSamp", "Time sample with max ADC")) oneCanvas->Update();
 
  oneCanvas->cd(2);
  gPad->cd(2);
  if (plotHisto("Sample0", "ADC of time sample 0")) oneCanvas->Update();

  gDirectory->cd();
}

