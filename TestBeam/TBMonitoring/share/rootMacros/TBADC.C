#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBADC() {

  TString stringToExecute, path, canvasTitle, canvasNameBase;

  TControlBar* TBADCbar = new TControlBar("vertical", "TBADC monitoring", 160, 40);
  
  path = "/BeamDetectors/ADC";
  canvasTitle = "ADC";
  canvasNameBase = "cBase";
  stringToExecute = "TBADCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",3,3)";
  TBADCbar->AddButton("ADC (all frag all chan)", stringToExecute, "ADC frag quantities");
  stringToExecute = "TBADCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,2,0,7)";
  TBADCbar->AddButton("ADC frag0x10 (all chan)", stringToExecute, "ADC frag0x10 quantities");
  stringToExecute = "TBADCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,2,8,10)";
  TBADCbar->AddButton("ADC frag0x11 (all chan)", stringToExecute, "ADC frag0x11 quantities");
  stringToExecute = "TBADCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,2,11,16)";
  TBADCbar->AddButton("ADC frag0x12 (all chan)", stringToExecute, "ADC frag0x12 quantities");

  TBADCbar->Show();
  gROOT->SaveContext();
}

