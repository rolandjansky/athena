////////////////////////////////////////////////////////////
// LArWaveFebMon.C
// 
// last modified 2004/06/28
//
// Creates menu bar for LArWave monitoring for a specific feb
// For CBT-EC2 there are 128 channels per feb (8 febs)
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

void LArWaveFebMon(TString path, TString dirName) {
  // For LArWave Monitoring of histograms for all channels for a specific feb
  //
  // TString path       directory path
  // TString dirName    directory within path of the specific feb in consideration

  TString stringToExecute, pattern, canvasTitle, canvasNameBase, axisTitleX, axisTitleY;

  TControlBar* WaveFebMonbar = new TControlBar("vertical", "Wave "+dirName+" monitoring", 160, 40);
  
  path = path+=dirName; // append directory for specific feb to the path
  canvasTitle = dirName;
  canvasNameBase = "cBase";
  axisTitleX = "";
  axisTitleY = "";

  //plot all histos
  pattern = "";
  stringToExecute = "plotPattern(\"" + path + "\", \"" +pattern + "\", \"Wave " + canvasTitle + "\", \"" + canvasNameBase + "\", 4, 4, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
  WaveFebMonbar->AddButton("All "+dirName+" channels", stringToExecute, "All wave quantities for "+dirName+" for all channels");

  // Wave channels in groups of nChannel
  int nChannelMax = 128;
  int nChannel = 16;
  for (int firstChannel = 0; firstChannel < nChannelMax; firstChannel += nChannel) {
    TString* iName;
    TList* listOfNames = new TList();
    for (int channel = firstChannel; channel < firstChannel + nChannel; channel++) {
      TString name;
      name = "chan"; name += channel; iName = new TString(name); listOfNames->Add((TObject*)iName);
    }
    stringToExecute = "plotList(\"" + path + "\", (TList*)";
    stringToExecute += (int)listOfNames;
    stringToExecute += ", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 4, 4, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
    TString channelRange = ""; channelRange += firstChannel; channelRange += "-"; channelRange += firstChannel + nChannel - 1;
    TString buttonName = dirName + " channels " + channelRange;
    TString buttonHint = "waves for channels " + channelRange; 
    WaveFebMonbar->AddButton(buttonName, stringToExecute, buttonHint);
  }

  WaveFebMonbar->Show();
  gROOT->SaveContext();
}

