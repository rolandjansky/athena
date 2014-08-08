#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Phase Monitoring histogram 
void TBPhase() {

  TString stringToExecute, path, canvasTitle, canvasNameBase;

  //TControlBar* TBEventbar = new TControlBar("vertical", "TBPhase monitoring", 90, 20);
  
  //TBEventbar->AddButton("New Browser", "new TBrowser;", "Start the ROOT Browser");
  
  path = "/beamdetmon/BeamDetectors";
  canvasTitle = "TBPhase";
  canvasNameBase = "cBase";
  //stringToExecute = "TBPhasePlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  TBPhasePlots(path,canvasTitle,canvasNameBase);
  gROOT->SaveContext();
}

