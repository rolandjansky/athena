////////////////////////////////////////////////////////////
// TBTailCatcher.C
// 
// last modified 2004/06/23
//
// Creates menu bar for TestBeam Warm TailCatcher monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBTailCatcher() {

  TString stringToExecute, path, canvasTitle, canvasNameBase, axisTitleX, axisTitleY;

  TControlBar* TBTCbar = new TControlBar("vertical", "TBTailCatcher monitoring", 160, 40);
  
  path = "/BeamDetectors/TailCatcher";
  canvasTitle = "Warm TailCatcher Monitoring";
  canvasNameBase = "cBase";
  axisTitleX = "ADC";
  axisTitleY = "No. of events";

  // plot all TC adc's 
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",3,3)";
  TBTCbar->AddButton("TailCatcher ADC (all Scintillators)", stringToExecute, "WTC Scintillator quantities");


  // plot adc's for 8 scintillators at a time
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,0,7)";
  TBTCbar->AddButton("TailCatcher ADC (Scint 0-7)", stringToExecute, "WTC Scintillator quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,8,15)";
  TBTCbar->AddButton("TailCatcher ADC (Scint 8-15)", stringToExecute, "WTC Scintillator quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,16,23)";
  TBTCbar->AddButton("TailCatcher ADC (Scint 16-23)", stringToExecute, "WTC Scintillator quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,24,31)";
  TBTCbar->AddButton("TailCatcher ADC (Scint 24-31)", stringToExecute, "WTC Scintillator quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,32,39)";
  TBTCbar->AddButton("TailCatcher ADC (Scint 32-39)", stringToExecute, "WTC Scintillator quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,40,47)";
  TBTCbar->AddButton("TailCatcher ADC (Scint 40-47)", stringToExecute, "WTC Scintillator quantities");


  TBTCbar->Show();
  gROOT->SaveContext();
}

