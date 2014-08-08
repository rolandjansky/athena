#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBBPC() {

  TString stringToExecute, path, canvasTitle, canvasNameBase;

  TControlBar* TBBPCbar = new TControlBar("vertical", "TBBPC monitoring", 160, 40);
  
  path = "/BeamDetectors/BPC";
  canvasTitle = "BPC";
  canvasNameBase = "cBase";
  stringToExecute = "TBBPCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",3,3)";
  TBBPCbar->AddButton("BPC all histos", stringToExecute, "All BPC's ADC and TDC quantities");
  stringToExecute = "TBBPCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,3,0,5)";
  TBBPCbar->AddButton("BPC-2 (TDC and ADC)", stringToExecute, "ADC and TDC quantities for BPC-2");
  stringToExecute = "TBBPCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,3,6,11)";
  TBBPCbar->AddButton("BPC-1 (TDC and ADC)", stringToExecute, "ADC and TDC quantities for BPC-1");
  stringToExecute = "TBBPCPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,3,12,17)";
  TBBPCbar->AddButton("BPC0 (TDC and ADC)", stringToExecute, "ADC and TDC quantities for BPC0");

  //stringToExecute = "TBBPCPlots2(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",1,1,0)";
  //TBBPCbar->AddButton("TDC for all BPCs", stringToExecute, "TDC quantities for BPC-2, BPC-1, and BPC0");
  //stringToExecute = "TBBPCPlots2(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\",2,3,1)";
  //TBBPCbar->AddButton("ADC for all BPCs", stringToExecute, "ADC quantities for BPC-2, BPC-1, and BPC0");

  TBBPCbar->Show();
  gROOT->SaveContext();
}

