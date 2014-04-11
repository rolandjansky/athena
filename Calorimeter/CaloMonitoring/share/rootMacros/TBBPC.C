////////////////////////////////////////////////////////////
// TBBPC.C
// 
// last modified 2004/06/23
//
// Creates menu bar for TestBeam BPC monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBBPC() {

  TString stringToExecute, path, canvasTitle, canvasNameBase, axisTitleX, axisTitleY;

  TControlBar* TBBPCbar = new TControlBar("vertical", "TBBPC monitoring", 160, 40);
  
  path = "/BeamDetectors/BPC";
  canvasTitle = "BPC Monitoring";
  canvasNameBase = "cBase";
  axisTitleX = "";
  axisTitleY = "No. of events";  

  // plot all histos
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",3,3)";
  TBBPCbar->AddButton("BPC all histos", stringToExecute, "All BPCs (X Y pos, ADC, and TDC quantities)");

  // plot x and y positions from all BPCs
  axisTitleX = "Position";
  axisTitleY = "No. of events";
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,0,11)";
  TBBPCbar->AddButton("All BPCs X and Y pos", stringToExecute, "X and Y position of BPCs 0 through 5");

  
    // ADC and TDC for individual BPCs
  axisTitleX = "";
  axisTitleY = "No. of events";
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,12,17)";
  TBBPCbar->AddButton("BPC0 (TDC and ADC)", stringToExecute, "ADC and TDC quantities for BPC0");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,18,23)";
  TBBPCbar->AddButton("BPC1 (TDC and ADC)", stringToExecute, "ADC and TDC quantities for BPC1");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,24,29)";
  TBBPCbar->AddButton("BPC2 (TDC and ADC)", stringToExecute, "ADC and TDC quantities for BPC2");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,30,35)";
  TBBPCbar->AddButton("BPC3 (TDC and ADC)", stringToExecute, "ADC and TDC quantities for BPC3");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,36,41)";
  TBBPCbar->AddButton("BPC4 (TDC and ADC)", stringToExecute, "ADC and TDC quantities for BPC4");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,42,47)";
  TBBPCbar->AddButton("BPC5 (TDC and ADC)", stringToExecute, "ADC and TDC quantities for BPC5");


  TBBPCbar->Show();
  gROOT->SaveContext();
}

