#include <iostream>

#include "TCanvas.h"
#include "TCollection.h"
#include "TList.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void printCanvases() {

  // set printer command
  //  TString printCommand = "lp -dzork";               // UVic
  //TString printCommand = "xprint -h -p40-1d-cor";   // bat 40, -h is for no banner 
  TString printCommand = "xprint -h -p887-hna457";  // counting room, -h is for no banner page
  //  TString printCommand = "ggv";                      // for debugging
  //  TString printCommand = "ls";                       // for debugging

  // prepare shell command
  TString tempPs = "canvas.ps";
  TString shellCommand = ".!" + printCommand + " " + tempPs + "; rm -f " + tempPs;
  //  cout << "prepared shell command " << shellCommand << endl;

  // prepare loop over canvases
  TList* listOfCanvases = gROOT->GetListOfCanvases();
  TIter myIter(listOfCanvases);
  TCanvas* canvas;
  // iterate over the canvases
  while (canvas = (TCanvas*)myIter.Next()) {
    canvas->Print(tempPs);
    //    cout << "canvas " << canvas->GetName() << endl;
    //    cout << "executing shell command " << shellCommand << endl;
    gROOT->ProcessLine(shellCommand);
  }
}
