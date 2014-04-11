////////////////////////////////////////////////////////////
// LArDigitMon.C
// 
// last modified 2004/06/28
//
// Creates menu bar for LArDigit monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TControlBar.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

// For histograms produced by LArDigMonTool and LArDigTBTDCMonTool
void LArDigitMon() {

  TString stringToExecute, path, canvasTitle, canvasNameBase, axisTitleX, axisTitleY;
  TString pattern = "";

  TControlBar* LArDigitbar = new TControlBar("vertical", "LArDigit monitoring", 100, 20);
  
  LArDigitbar->AddButton("New Browser",       "new TBrowser;",  "Start the ROOT Browser");
  
  path = "/pedsamp0";
  canvasTitle = "FEB channel occupancy";
  canvasNameBase = "cBase";
  stringToExecute = "LArDigitGlobalPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  LArDigitbar->AddButton("all FEBs, all channels", stringToExecute, "Summed over all FEBs");
  
  path = "/pedsamp0/samp0";
  canvasTitle = "FEB: <Sample 0>";
  canvasNameBase = "cBase";
  axisTitleX = "ADC channel number in FEB";
  axisTitleY = "<sample 0> (ADC)";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
  LArDigitbar->AddButton("<sample 0> all FEBs", stringToExecute, "<sample 0> mean per channel, all FEBs");
  
  path = "/pedsamp0/samp0Rms";
  canvasTitle = "FEB: sample 0 RMS";
  canvasNameBase = "cBase";
  axisTitleX = "ADC channel number in FEB";
  axisTitleY = "sample 0 RMS (ADC)";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
  LArDigitbar->AddButton("sample 0 RMS all FEBs", stringToExecute, "sample 0 RMS per channel, all FEBs");

  path = "/pedsamp0/febAdc";
  canvasTitle = "FEB: channel occupancy";
  canvasNameBase = "cBase";
  axisTitleX = "ADC channel number in FEB";
  axisTitleY = "<ADC(max)-ped> (ADC)";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
  LArDigitbar->AddButton("<ADC(max)-ped> all FEBs", stringToExecute, "<ADC(max)-ped> per channel, all FEBs");
  
  path = "/pedsamp0/febMaxSamp";
  canvasTitle = "FEB: index of maximum time sample";
  canvasNameBase = "cBase";
  axisTitleX = "time sample";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\")";
  LArDigitbar->AddButton("max sample all FEBs", stringToExecute, "index of maximum time sample, all FEBS");

  path = "/pedsamp0/febSamples";
  canvasTitle = "FEB: samples";
  canvasNameBase = "cBase";
  axisTitleX = "time sample";
  axisTitleY = "<ADC-ped> (ADC)";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
  LArDigitbar->AddButton("samples all FEBs", stringToExecute, "<ADC-ped> per time sample, all FEBs");

  path = "/pedsamp0/febAmp";
  canvasTitle = "FEB: sample amplitudes";
  canvasNameBase = "cBase";
  axisTitleX = "time sample";
  axisTitleY = "ADC";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
  LArDigitbar->AddButton("ADC vs time sample all FEBs", stringToExecute, "ADC vs time samples, all FEBs");
  
  path = "/LArDigitTBTDC0/febTDCSamples";
  canvasTitle = "FEB: TDC corrected samples";
  canvasNameBase = "cBase";
  axisTitleX = "time (ns)";
  axisTitleY = "<ADC-ped> (ADC)";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
  LArDigitbar->AddButton("TDC corrected samples all FEBs", stringToExecute, "<ADC-ped> vs TDC corrected time, all FEBs");

  LArDigitbar->Show();
  gROOT->SaveContext();
}

