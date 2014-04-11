////////////////////////////////////////////////////////////
// TBADCMon.C
// 
// last modified 2004/06/28
//
// Creates menu bar for TestBeam ADC monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam ADC Monitoring histograms 
void TBADCMon() {

  TString stringToExecute, path, pattern, canvasTitle, canvasNameBase, axisTitleX;

  TControlBar* TBADCbar = new TControlBar("vertical", "TBADC monitoring", 160, 40);
  
  path = "/BeamDetectors/ADC";
  canvasTitle = "ADC Monitoring";
  canvasNameBase = "cBase";
  axisTitleX = "ADC";

  // plot all histos
  pattern = "";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 4, 4, \"" + axisTitleX + "\")";
  TBADCbar->AddButton("ADC (all histos)", stringToExecute, "ADC frag quantities");

  // ADC frag0x04 in groups of nChannel channels
  int nChannelMax = 24;
  int nChannel = 4;
  for (int firstChannel = 0; firstChannel < nChannelMax; firstChannel += nChannel) {
    TString* iName;
    TList* listOfNames = new TList();
    for (int channel = firstChannel; channel < firstChannel + nChannel; channel++) {
      TString name;
      name = "word1frag0x04chan"; name += channel; iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = "word2frag0x04chan"; name += channel; iName = new TString(name); listOfNames->Add((TObject*)iName);
    }
    stringToExecute = "plotList(\"" + path + "\", (TList*)";
    stringToExecute += (int)listOfNames;
    stringToExecute += ", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\")";
    TString channelRange = ""; channelRange += firstChannel; channelRange += "-"; channelRange += firstChannel + nChannel - 1;
    TString buttonName = "ADC frag0x04 channels " + channelRange;
    TString buttonHint = "ADC word1frag0x04 and word2frag0x04 for channels " + channelRange; 
    TBADCbar->AddButton(buttonName, stringToExecute, buttonHint);
  }
  
  // ADC frag0x05
  pattern = "frag0x05";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3, \"" + axisTitleX + "\")";
  TBADCbar->AddButton("ADC frag0x05 (all histos)", stringToExecute, "ADC word1frag0x05 and word2frag0x05 all channels");

  // ADC frag0x06
  pattern = "frag0x06";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3, \"" + axisTitleX + "\")";
  TBADCbar->AddButton("ADC frag0x06 (all histos)", stringToExecute, "ADC word1frag0x06 and word2frag0x06 all channels");
  
  // All word1 ADC frags
  pattern = "word1";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 3, 3, \"" + axisTitleX + "\")";
  TBADCbar->AddButton("All word1 frag ADCs", stringToExecute, "All word1 fragments from ADCs");

  // All word2 ADC frags
  pattern = "word2";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 3, 3, \"" + axisTitleX + "\")";
  TBADCbar->AddButton("All word2 frag ADCs", stringToExecute, "All word2 fragments from ADCs");

  TBADCbar->Show();
  gROOT->SaveContext();
}

