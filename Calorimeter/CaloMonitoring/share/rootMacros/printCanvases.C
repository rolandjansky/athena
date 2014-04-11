////////////////////////////////////////////////////////////
// printCanvases.C
// 
// last modified 2004/06/23
//
// Macro to print out canvases (by default, it prints to the 
// H6 counting room printer)
////////////////////////////////////////////////////////////

#include <iostream>

#include "TCanvas.h"
#include "TCollection.h"
#include "TList.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void printCanvases(TString printCommand="xprint -h -p887-hna457", TString tempPs="canvas.ps") {

  // if printCommand is "", then the file is not printed and kept on disk
  // TString printCommand = "lp -dzork";                // UVic
  // TString printCommand = "xprint -h -p40-1d-cor";    // bat 40, -h is for no banner 
  // TString printCommand = "xprint -h -p887-hna457";   // counting room, -h is for no banner page

  TString canvasTitle, canvasNameBase;

  float canvasWidthX = 600;
  float canvasWidthY = 462;
  float canvasX      = 400;
  float canvasY      =  20;

  // open ps file
  canvasTitle = "";
  canvasNameBase = "dummy";
  TCanvas* dummyCanvas1 = new TCanvas(canvasNameBase, canvasTitle, canvasX, canvasY, canvasWidthX, canvasWidthY);
  dummyCanvas1->Print(tempPs + "[", "Landscape");  // this just opens the ps file, but the dummy canvas size is important!
  dummyCanvas1->Close();

  // prepare loop over canvases
  TList* listOfCanvases = gROOT->GetListOfCanvases();
  TIter myIter(listOfCanvases);
  TCanvas* canvas;
  // iterate over the canvases
  while (canvas = (TCanvas*)myIter.Next()) {
    canvas->Print(tempPs);
    // cout << "canvas " << canvas->GetName() << endl;
  }

  // close ps file
  canvasTitle = "";
  canvasNameBase = "dummy";
  TCanvas* dummyCanvas2 = new TCanvas(canvasNameBase, canvasTitle, canvasX, canvasY, canvasWidthX, canvasWidthY);
  dummyCanvas2->Print(tempPs + "]");  // this just closes the ps file
  dummyCanvas2->Close();

  // prepare shell command for printing
  if (printCommand == "") {
    cout << "Canvases in file " << tempPs << endl;
  } else {
    TString shellCommand = ".!" + printCommand + " " + tempPs + "; rm -f " + tempPs;
    cout << "Executing shell command: " << shellCommand << endl;
    gROOT->ProcessLine(shellCommand);
  }

}
