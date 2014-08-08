#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBTailCatcher() {

  TString stringToExecute, path, canvasTitle, canvasNameBase;

  TControlBar* TBTailCatcherbar = new TControlBar("vertical", "TBTailCatcher monitoring", 160, 40);
  
  //  TBEventbar->AddButton("New Browser", "new TBrowser;", "Start the ROOT Browser");
  
  path = "/BeamDetectors/TailCatcher";
  canvasTitle = "TailCatcher";
  canvasNameBase = "cBase";
  stringToExecute = "TBTailCatcherPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",0,7)";
  TBTailCatcherbar->AddButton("TailCatcher ADC (Scint 0-7)", stringToExecute, "Scintillator quantities");
  stringToExecute = "TBTailCatcherPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",8,15)";
  TBTailCatcherbar->AddButton("TailCatcher ADC (Scint 8-15)", stringToExecute, "Scintillator quantities");
  stringToExecute = "TBTailCatcherPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",16,23)";
  TBTailCatcherbar->AddButton("TailCatcher ADC (Scint 16-23)", stringToExecute, "Scintillator quantities");
  stringToExecute = "TBTailCatcherPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",24,31)";
  TBTailCatcherbar->AddButton("TailCatcher ADC (Scint 24-31)", stringToExecute, "Scintillator quantities");
  stringToExecute = "TBTailCatcherPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",32,39)";
  TBTailCatcherbar->AddButton("TailCatcher ADC (Scint 32-39)", stringToExecute, "Scintillator quantities");
  stringToExecute = "TBTailCatcherPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",40,47)";
  TBTailCatcherbar->AddButton("TailCatcher ADC (Scint 40-47)", stringToExecute, "Scintillator quantities");
  stringToExecute = "TBTailCatcherPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  TBTailCatcherbar->AddButton("TailCatcher ADC (all Scintillators)", stringToExecute, "Scintillator quantities");

  TBTailCatcherbar->Show();
  gROOT->SaveContext();
}

