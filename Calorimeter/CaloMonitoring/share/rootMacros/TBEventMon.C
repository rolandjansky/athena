////////////////////////////////////////////////////////////
// TBEventMon.C
// 
// last modified 2004/06/23
//
// Creates a menu bar for TestBeam histogram monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBEventMon() {

  TString stringToExecute, path, canvasTitle, canvasNameBase;

  TControlBar* TBEventbar = new TControlBar("vertical", "TBEvent objects monitoring", 90, 20);
  
  TBEventbar->AddButton("New Browser", "new TBrowser;", "Start the ROOT Browser");
  
  path = "/beamdetmon/BeamDetectors";
  canvasTitle = "TDC";
  canvasNameBase = "cBase";
  stringToExecute = "TBPhasePlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  TBEventbar->AddButton("TBPhase", stringToExecute, "Timing quantities");

  TBEventbar->AddButton("TBADC", "TBADCMon()", "Plot options for TestBeam ADC");
  TBEventbar->AddButton("TBTDC", "TBTDCMon()", "Plot options for TestBeam TDC");
  TBEventbar->AddButton("TBBPC", "TBBPCMon()", "Plot options for TestBeam BPC");
  TBEventbar->AddButton("TBScintillator",   "TBScintillatorMon()", "Plot options for Scintillators");
  TBEventbar->AddButton("TBTailCatcher",    "TBTailCatcherMon()", "Plot options for warm TailCatcher Scintillators");

  TBEventbar->Show();
  gROOT->SaveContext();
}

