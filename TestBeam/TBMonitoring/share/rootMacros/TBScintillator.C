#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBScintillator() {

  TString stringToExecute, path, canvasTitle, canvasNameBase;

  TControlBar* TBScintillatorbar = new TControlBar("vertical", "TBScintillator monitoring", 160, 40);
  
  path = "/BeamDetectors/Scintillator";
  canvasTitle = "Scintillator";
  canvasNameBase = "cBase";

  stringToExecute = "TBScintillatorPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,2,0,3)";
  TBScintillatorbar->AddButton("Scintillators S0,S1 ", stringToExecute, "S0 and S1 Scintillators (raw and TDC)");
  stringToExecute = "TBScintillatorPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,2,4,7)";
  TBScintillatorbar->AddButton("Scintillator S2 (U/D)", stringToExecute, "Scintillator S2 Up and Down (raw and TDC)");
  stringToExecute = "TBScintillatorPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,2,8,11)";
  TBScintillatorbar->AddButton("Scintillator S3 (L/R)", stringToExecute, "Scintillator S3 Left and Right (raw and TDC)");
  stringToExecute = "TBScintillatorPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,2,12,15)";
  TBScintillatorbar->AddButton("Scintillators C1,C2 ", stringToExecute, "C1 and C2 Scintillators (raw and TDC)");
  stringToExecute = "TBScintillatorPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,3,16,21)";
  TBScintillatorbar->AddButton("Muon Scintillators", stringToExecute, "Muon Scintillators Tag, Halo, Veto (raw and TDC)");
 
  stringToExecute = "TBScintillatorPlots_2(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,3,0)";
  TBScintillatorbar->AddButton("All Scintillators (raw)", stringToExecute, "All Scintillator raw signals");
  stringToExecute = "TBScintillatorPlots_2(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,3,1)";
  TBScintillatorbar->AddButton("All Scintillators (TDCs)", stringToExecute, "All Scintillator TDC signals");
 
  TBScintillatorbar->Show();
  gROOT->SaveContext();
}

