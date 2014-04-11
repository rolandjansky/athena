////////////////////////////////////////////////////////////
// TBTailCatcherMon.C
// 
// last modified 2004/06/28
//
// Creates menu bar for TestBeam Warm TailCatcher monitoring
// 48 warm TailCatcher scintillators
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBTailCatcherMon() {

  TString stringToExecute, path, pattern, canvasTitle, canvasNameBase, axisTitleX;

  TControlBar* TBTCbar = new TControlBar("vertical", "TBTailCatcher monitoring", 160, 40);
  
  path = "/BeamDetectors/TailCatcher";
  canvasTitle = "Warm TailCatcher Monitoring";
  canvasNameBase = "cBase";
  axisTitleX = "ADC";

  // plot all histos
  pattern = "";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 3, 3, \"" + axisTitleX + "\")";
  TBTCbar->AddButton("TailCatcher (all Scintillators)", stringToExecute, "WTC Scintillator ADC counts");

  // plot adc's for nScint tailcatcher scintillators at a time
  int nScintMax = 48;
  int nScint = 8;
  for (int firstScint = 0; firstScint < nScintMax; firstScint += nScint) {
    TString* iName;
    TList* listOfNames = new TList();
    for (int scintindex = firstScint; scintindex < firstScint + nScint; scintindex++) {
      TString name;
      name = "Scintillator "; name += scintindex; iName = new TString(name); listOfNames->Add((TObject*)iName);
    }
    stringToExecute = "plotList(\"" + path + "\", (TList*)";
    stringToExecute += (int)listOfNames;
    stringToExecute += ", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\")";
    TString scintRange = ""; scintRange += firstScint; scintRange += "-"; scintRange += firstScint + nScint - 1;
    TString buttonName = "TailCatcher Scintillator " + scintRange;
    TString buttonHint = "ADC for warm tailcatcher scintillators " + scintRange; 
    TBTCbar->AddButton(buttonName, stringToExecute, buttonHint);
  }
  
  TBTCbar->Show();
  gROOT->SaveContext();
}

