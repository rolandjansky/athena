////////////////////////////////////////////////////////////
// plotList.C
// 
// last modified 2004/06/28
//
// plots histos given by a path and a list of histogram names
////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>

#include "TCanvas.h"
#include "TCollection.h"
#include "TH1.h"
#include "TList.h"
#include "TObject.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void plotList(TString path, 
              TList* listOfNames, 
              TString canvasTitle="", 
              TString canvasNameBase="cBase", 
              int nPadX=2, 
              int nPadY=2, 
              TString axisTitleX="", 
              TString axisTitleY="", 
              TString axisTitleZ="", 
              TString histoTitle="", 
              TString drawOption="") 
{
  // draw histograms and perform canvas management
  //
  // TString path           directory path
  // TList* listOfNames     list of histogram names to look for.  "skip" is a special name to skip a pad
  // TString canvasTitle    each canvas has a title composed of the canvas name followed by canvasTitle
  // TString canvasNameBase each canvas name is composed of the canvasNameBase followed by an index
  //                        upon entry, close canvases whose name starts with canvasNameBase
  // int nPadX              number of pads in X
  // int nPadY              number of pads in Y
  // TString axisTitleX     X axis title
  // TString axisTitleY     Y axis title
  // TString axisTitleZ     Z axis title (only relevant for TH2* histos)
  // TString histoTitle     title of histogram.  "" means do not change the existing title
  // TString drawOption     histo Draw() option.  If "", the default is HIST for 1D, and LEGO for 2D

  float canvasWidthX = 600;
  float canvasWidthY = 462;
  float canvasX      = 400;
  float canvasY      =  20;
  float canvasStepX  =  20;
  float canvasStepY  =  20;

  // display error button if path not found
  if (!gDirectory->cd(path)) {
    TString message = "Path " + path + " not found";
    OKButton(message);
    return;
  }
  
  // close any open canvases with same canvasNameBase
  TString stringToExecute = "closeCanvases(\"" + canvasNameBase + "\")";
  gROOT->ProcessLine(stringToExecute);
  
  TString skipName = "skip";
  vector<TCanvas*> allCanvas;
  int canvasIndex = -1;
  int padIndex = 0;

  // loop over names
  TIter namesIter(listOfNames);
  TString* iName;
  while (iName = (TString*)namesIter.Next()) {
    TString name = *iName;
    // cout << "Name to look for: " << name << endl;

    // pad and canvas management
    if (padIndex%(nPadX*nPadY) == 0) {
      padIndex = 0;
      ++canvasIndex;
      TString canvasName = canvasNameBase;
      canvasName+=canvasIndex;  //appends canvasIndex to end of canvasNameBase
      // cout << "creating canvas " << canvasName << endl;
      TCanvas* oneCanvas = new TCanvas(canvasName, canvasName + " - " + canvasTitle, canvasX+canvasStepX*canvasIndex, canvasY+canvasStepY*canvasIndex, canvasWidthX, canvasWidthY);
      oneCanvas->Divide(nPadX, nPadY);
      allCanvas.push_back(oneCanvas);
    }
    allCanvas[canvasIndex]->cd(++padIndex);
    
    // plot histo
    if (name != skipName) {
      if (plotHisto(name, axisTitleX, axisTitleY, axisTitleZ, histoTitle, drawOption)) allCanvas[canvasIndex]->Update();
    } 
  }

  gDirectory->cd();
  
}
