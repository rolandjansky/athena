#include <iostream>

#include "TCanvas.h"
#include "TCollection.h"
#include "TList.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void closeCanvases(TString canvasNameBase) {

  // prepare loop over canvases
  TList* listOfCanvases = gROOT->GetListOfCanvases();
  TIter myIter(listOfCanvases);
  TCanvas* canvas;
  // iterate over the canvases
  while (canvas = (TCanvas*)myIter.Next()) {
    TString canvasName = canvas->GetName();
    // get the base part of the canvas name
    TString foundBase(canvasName.Data(), canvasNameBase.Length());
    //    cout << "found canvas " << canvasName 
    //         << " with base name " << foundBase 
    //         << " to match with " << canvasNameBase << endl;
    if (canvasNameBase == foundBase) {
      //      cout << "closing canvas " << canvasName << endl;
      canvas->Close();
    }
  }
}
