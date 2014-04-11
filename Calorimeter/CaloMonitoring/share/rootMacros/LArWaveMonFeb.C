////////////////////////////////////////////////////////////
// LArWaveFebMon.C
// 
// last modified 2004/06/23
//
// Creates menu bar for LArWave monitoring for a specific feb
// For CBT-EC2 there are 128 channels per feb (8 febs)
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

void LArWaveMonFeb(TString path, TString dirName) {
  // For LArWave Monitoring of histograms for all channels for a specific feb
  //
  // TString path       directory path
  // TString dirName    directory within path of the specific feb in consideration

  TString stringToExecute, canvasTitle, canvasNameBase, axisTitleX, axisTitleY;

  TControlBar* WaveMonFebbar = new TControlBar("vertical", "Wave "+dirName+" monitoring", 160, 40);
  
  path = path+=dirName; // append directory for specific feb to the path
  canvasTitle = dirName;
  canvasNameBase = "cBase";
  axisTitleX = "";
  axisTitleY = "No. of events";

  stringToExecute = "keyPlots(\"" + path + "\", \"Wave " + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",4,4)";
  WaveMonFebbar->AddButton("All "+dirName+" channels", stringToExecute, "All wave quantities for "+dirName+" for all channels");
 
 stringToExecute = "keyPlots(\"" + path + "\", \"Wave " + canvasTitle + "\", \"" +canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",4,4,0,15)";
  WaveMonFebbar->AddButton(dirName+" channels 0-15", stringToExecute, "wave quantities for channels 0-15 of "+dirName);

  stringToExecute = "keyPlots(\"" + path + "\", \"Wave " + canvasTitle + "\", \"" +canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",4,4,16,31)";
  WaveMonFebbar->AddButton(dirName+" channels 16-31", stringToExecute, "wave quantities for channels 16-31 of "+dirName);

  stringToExecute = "keyPlots(\"" + path + "\", \"Wave " + canvasTitle + "\", \"" +canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",4,4,32,47)";
  WaveMonFebbar->AddButton(dirName+" channels 32-47", stringToExecute, "wave quantities for channels 32-47 of "+dirName);

  stringToExecute = "keyPlots(\"" + path + "\", \"Wave " + canvasTitle + "\", \"" +canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",4,4,48,63)";
  WaveMonFebbar->AddButton(dirName+" channels 48-63", stringToExecute, "wave quantities for channels 48-63 of "+dirName);

  stringToExecute = "keyPlots(\"" + path + "\", \"Wave " + canvasTitle + "\", \"" + canvasNameBase+ "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",4,4,64,79)";
  WaveMonFebbar->AddButton(dirName+" channels 64-79", stringToExecute, "wave quantities for channels 64-79 of "+dirName);

  stringToExecute = "keyPlots(\"" + path + "\", \"Wave " + canvasTitle + "\", \"" + canvasNameBase+ "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",4,4,80,95)";
  WaveMonFebbar->AddButton(dirName+" channels 80-95", stringToExecute, "wave quantities for channels 80-95 of "+dirName);

  stringToExecute = "keyPlots(\"" + path + "\", \"Wave " + canvasTitle + "\", \"" + canvasNameBase+"\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",4,4,96,111)";
  WaveMonFebbar->AddButton(dirName+" channels 96-111", stringToExecute,"wave quantities for channels 96-111 of "+dirName);

  stringToExecute = "keyPlots(\"" + path + "\", \"Wave " + canvasTitle + "\", \"" +canvasNameBase+"\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",4,4,112,127)";
  WaveMonFebbar->AddButton(dirName+" channels 112-127",stringToExecute,"wave quantities for channels 112-127 of "+dirName);

  WaveMonFebbar->Show();
  gROOT->SaveContext();
}

