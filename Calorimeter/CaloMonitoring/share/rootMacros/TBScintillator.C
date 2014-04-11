////////////////////////////////////////////////////////////
// TBScintillator.C
// 
// last modified 2004/06/23
//
// Creates menu bar for TestBeam Scintillator monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBScintillator() {

  TString stringToExecute, path, canvasTitle, canvasNameBase, axisTitleX, axisTitleY;

  TControlBar* TBScintbar = new TControlBar("vertical", "TBScintillator monitoring", 160, 40);
  
  path = "/BeamDetectors/Scintillator";
  canvasTitle = "Scintillator";
  canvasNameBase = "cBase";
  axisTitleX = "";
  axisTitleY = "No. of events";


  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,0,3)";
  TBScintbar->AddButton("Scintillators S1,S2 ", stringToExecute, "S1 and S2 Scintillators (raw and TDC)");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,4,7)";
  TBScintbar->AddButton("Scintillators S3,B  ", stringToExecute, "S3 and B Scintillators (raw and TDC)");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,8,11)";
  TBScintbar->AddButton("Scintillators Veto,Halo", stringToExecute, "Veto and Halo Scintillators (raw and TDC)");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,12,15)";
  TBScintbar->AddButton("Scintillators muon1,muon2 ", stringToExecute, "muon1 and muon2 Scintillators (raw and TDC)");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,16,19)";
  TBScintbar->AddButton("Scintillators muon3,muon4", stringToExecute, "muon3 and muon4 Scintillators (raw and TDC)");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,20,23)";
  TBScintbar->AddButton("Scintillators muon5,muon6 ", stringToExecute, "muon5 and muon6 Scintillators (raw and TDC)");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,24,27)";
  TBScintbar->AddButton("Scintillators muon7,muon8", stringToExecute, "muon7 and muon8 Scintillators (raw and TDC)");

  // plot all raw for all scintillators
  axisTitleX = "ADC";
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,0,-27)";
  TBScintbar->AddButton("All Scintillators (raw)", stringToExecute, "All Scintillator raw signals");

  // plot all TDC for all scintillators
  axisTitleX = "TDC";
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,1,-27)";
  TBScintbar->AddButton("All Scintillators (TDCs)", stringToExecute, "All Scintillator TDC signals");

 
  TBScintbar->Show();
  gROOT->SaveContext();
}

