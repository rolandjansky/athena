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

  TBEventbar->AddButton("TBADC", "TBADC()", "Plot options for TestBeam ADC");
  TBEventbar->AddButton("TBTDC", "TBTDC()", "Plot options for TestBeam TDC");
  TBEventbar->AddButton("TBBPC", "TBBPC()", "Plot options for TestBeam BPC");
  TBEventbar->AddButton("TBScintillator", "TBScintillator()", "Plot options for Scintillators");
  TBEventbar->AddButton("TBTailCatcher", "TBTailCatcher()", "Plot options for TailCatcher Scintillators");

  TBEventbar->Show();
  gROOT->SaveContext();
}

