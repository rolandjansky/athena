////////////////////////////////////////////////////////////
// plotPattern.C
// 
// last modified 2004/06/28
//
// plots histos given by a path and a histogram name pattern
// a range of key indices can also be specified
////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>

#include "TCanvas.h"
#include "TCollection.h"
#include "TKey.h"
#include "TList.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void plotPattern(TString path, 
                 TString pattern="", 
                 TString canvasTitle="", 
                 TString canvasNameBase="cBase", 
                 int nPadX=2, 
                 int nPadY=2, 
                 TString axisTitleX="", 
                 TString axisTitleY="", 
                 TString axisTitleZ="", 
                 TString histoTitle="", 
                 TString drawOption="", 
                 int minKeyIndex=0, 
                 int maxKeyIndex=999) 
{
  // draw histograms and perform canvas management
  //
  // TString path           directory path
  // TString pattern        histogram name pattern
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
  // int minKeyIndex        key index of first histogram to plot
  // int maxKeyIndex        key index of last histogram to plot
  //                        if maxKeyIndex is negative, then only consider every other
  //                           index starting from minKeyIndex up to Abs(maxKeyIndex).
  //                        By giving a sufficiently large negative number (eg. -999), then every other 
  //                           index from minKeyIndex to the end of the keylist is considered.

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
  
  vector<TCanvas*> allCanvas;
  int canvasIndex = -1;
  int padIndex = 0;

  int keyIndex = 0;
  bool even = false;
  bool odd = false;

  // if maxKeyIndex is negative, then consider every other key starting at minKeyIndex up to 
  // the absolute value of maxKeyIndex
  if (maxKeyIndex < 0) {
    if (minKeyIndex%2 == 0) {
      even = true;
    } else if (minKeyIndex%2 == 1) {
      odd = true;
    }
    maxKeyIndex *= -1.;
  }

  // prepare loop over histograms
  // the directory has a list of keys
  TList* listOfKeys = gDirectory->GetListOfKeys();
  TIter myIter(listOfKeys);
  TKey* key;
  // iterate over the keys
  while (key = (TKey*)myIter.Next()) {
    // key->Dump();

    // only consider keys with required index
    if ((keyIndex >= minKeyIndex && keyIndex <= maxKeyIndex) && ((!odd && !even) || (odd && keyIndex%2==1) || (even && keyIndex%2==0))) {
      TString name = key->GetName();
      // cout << "Name of key: " << name << endl;

      // only consider TObjects with the required pattern in their name
      if (!name.SubString(pattern).IsNull()) {
        // cout << "Pattern " << pattern << " found in key name " << name << endl;

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
        if (plotHisto(name, axisTitleX, axisTitleY, axisTitleZ, histoTitle, drawOption)) allCanvas[canvasIndex]->Update();
      }
    }
    ++keyIndex;
  }

  gDirectory->cd();
  
}
