#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBTDC() {

  TString stringToExecute, path, canvasTitle, canvasNameBase;

  TControlBar* TBTDCbar = new TControlBar("vertical", "TBTDC monitoring", 160, 40);
  
  path = "/BeamDetectors/TDC";
  canvasTitle = "TDC";
  canvasNameBase = "cBase";
  stringToExecute = "TBTDCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",3,3)";
  TBTDCbar->AddButton("TDC (all frag all chan)", stringToExecute, "TDC frag quantities");
  stringToExecute = "TBTDCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,3,0,5)";
  TBTDCbar->AddButton("TDC frag0x12 (all chan)", stringToExecute, "TDC frag0x12 quantities");
  stringToExecute = "TBTDCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,3,6,17)";
  TBTDCbar->AddButton("TDC frag0x13 (all chan)", stringToExecute, "TDC frag0x13 quantities");
  stringToExecute = "TBTDCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,3,18,25)";
  TBTDCbar->AddButton("TDC frag0x14 (all chan)", stringToExecute, "TDC frag0x14 quantities");

  TBTDCbar->Show();
  gROOT->SaveContext();
}

