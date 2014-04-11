////////////////////////////////////////////////////////////
// LArCalibDigitMon.C
// 
// last modified 2004/06/28
//
// Creates menu bar for LArCalibDigit Monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TControlBar.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void LArCalibDigitMon() {

  TString stringToExecute, path, canvasTitle, canvasNameBase, axisTitleX, axisTitleY;
  TString pattern = "";

  TControlBar* LArDigitbar = new TControlBar("vertical", "LArCalibDigit Monitoring",110,20);

  LArDigitbar->AddButton("New Browser",       "new TBrowser;",  "Start the ROOT Browser");
  
  path = "/pedsamp0/febAdcPulsed";
  canvasTitle = "FEB: pulsed channel occupancy";
  canvasNameBase = "cBase";
  axisTitleX = "ADC channel number in FEB";
  axisTitleY = "<ADC(max)-ped> (ADC)";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
  LArDigitbar->AddButton("pulsed <ADC(max)-ped> all FEBs", stringToExecute, "pulsed <ADC(max)-ped> per channel, all FEBs");

  path = "/pedsamp0/febAdcNotPulsed";
  canvasTitle = "FEB: unpulsed channel occupancy";
  canvasNameBase = "cBase";
  axisTitleX = "ADC channel number in FEB";
  axisTitleY = "<ADC(max)-ped> (ADC)";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
  LArDigitbar->AddButton("unpulsed <ADC(max)-ped> all FEBs", stringToExecute, "unpulsed <ADC(max)-ped> per channel, all FEBs");
  
  LArDigitbar->Show();
  gROOT->SaveContext();
}

