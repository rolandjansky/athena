////////////////////////////////////////////////////////////
// AllWaveSumPlot.C
// 
// last modified 2004/06/26
//
// Plotting macro for waves summed over all febs
////////////////////////////////////////////////////////////

#include <iostream>

#include "TCanvas.h"
#include "TH1F.h"
#include "TPaveLabel.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void AllWaveSumPlot(TString path, TString canvasTitle, TString canvasNameBase) {
  // TString path           directory path
  // TString canvasTitle    each canvas has a title composed of the canvas name followed by canvasTitle
  // TString canvasNameBase each canvas name is composed of the canvasNameBase followed by an index

  float canvasWidthX = 600;
  float canvasWidthY = 462;
  float canvasX      = 400;
  float canvasY      =  20;

  if (!gDirectory->cd(path)) {
    TString message = "Path " + path + " not found";
    OKButton(message);
    return;
  }

  //close any open canvases with the same canvasNameBase
  TString stringToExecute = "closeCanvases(\"" + canvasNameBase + "\")";
  gROOT->ProcessLine(stringToExecute);
  
  TCanvas* oneCanvas = new TCanvas(canvasNameBase, canvasTitle, canvasX, canvasY, canvasWidthX, canvasWidthY);
  oneCanvas->Divide(1, 1);
  if (plotHisto("AllWaves", "", "", "", "All waves summed over all FEBs")) oneCanvas->Update();

  //return to the root file top directory
  gDirectory->cd();
}
