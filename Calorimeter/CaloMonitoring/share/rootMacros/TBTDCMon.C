////////////////////////////////////////////////////////////
// TBTDCMon.C
// 
// last modified 2004/06/28
//
// Creates menu bar for TestBeam TDC monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBTDCMon() {

  TString stringToExecute, path, pattern, canvasTitle, canvasNameBase, axisTitleX;

  TControlBar* TBTDCbar = new TControlBar("vertical", "TBTDC monitoring", 160, 40);
  
  path = "/BeamDetectors/TDC";
  canvasTitle = "TDC Monitoring";
  canvasNameBase = "cBase";
  axisTitleX = "TDC";

  // plot all TDC histograms
  pattern = "";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 3, 3, \"" + axisTitleX + "\")";
  TBTDCbar->AddButton("TDC (all histos)", stringToExecute, "TDC frag quantities");

  // TDC frag0x03 histograms
  pattern = "frag0x03";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3, \"" + axisTitleX + "\")";
  TBTDCbar->AddButton("TDC frag0x03 (all histos)", stringToExecute, "TDC word1frag0x03 and word2frag0x03 all channels");
 
  /*
  // TDC frag0x05 histograms
  int nChannelMax = 17;
  int nChannel = 6;
  for (int firstChannel = 0; firstChannel < nChannelMax; firstChannel += nChannel) {
    TString* iName;
    TList* listOfNames = new TList();
    for (int channel = firstChannel; channel < firstChannel + nChannel; channel++) {
      TString name;
      name = "word1frag0x05chan"; name += channel; iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = "word2frag0x05chan"; name += channel; iName = new TString(name); listOfNames->Add((TObject*)iName);
    }
    stringToExecute = "plotList(\"" + path + "\", (TList*)";
    stringToExecute += (int)listOfNames;
    stringToExecute += ", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3, \"" + axisTitleX + "\")";
    TString channelRange = ""; channelRange += firstChannel; channelRange += "-"; channelRange += firstChannel + nChannel - 1;
    TString buttonName = "TDC frag0x05 channels " + channelRange;
    TString buttonHint = "TDC word1frag0x05 and word2frag0x05 for channels " + channelRange; 
    TBTDCbar->AddButton(buttonName, stringToExecute, buttonHint);
  }
  */

  // TDC frag0x05 histograms
  pattern = "frag0x05";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"\", \"\", \"\", \"\", 6, 9)";
  TBTDCbar->AddButton("TDC frag0x05 chan 1,2", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"\", \"\", \"\", \"\", 10, 13)";
  TBTDCbar->AddButton("TDC frag0x05 chan 4,5", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"\", \"\", \"\", \"\", 14, 17)";
  TBTDCbar->AddButton("TDC frag0x05 chan 7,8", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"\", \"\", \"\", \"\", 18, 21)";
  TBTDCbar->AddButton("TDC frag0x05 chan 10,11", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"\", \"\", \"\", \"\", 22, 25)";
  TBTDCbar->AddButton("TDC frag0x05 chan 13,14", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"\", \"\", \"\", \"\", 26, 29)";
  TBTDCbar->AddButton("TDC frag0x05 chan 16,17", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  // plot all TDC frag0x05 histograms
  pattern = "frag0x05";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3, \"" + axisTitleX + "\")";
  TBTDCbar->AddButton("TDC frag0x05 (all histos)", stringToExecute, "TDC word1frag0x05 and word2frag0x05 all channels");

  // plot all TDC frag0x06 histograms
  pattern = "frag0x06";
  TString* iName;
  TList* listOfNames = new TList();
  TString name;
  name = "skip";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "word2" + pattern + "chan6";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "word1" + pattern + "chan7";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "word2" + pattern + "chan7";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "word1" + pattern + "chan8";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "word2" + pattern + "chan8";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  
  stringToExecute = "plotList(\"" + path + "\", (TList*)";
  stringToExecute += (int)listOfNames;
  stringToExecute += ", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3, \"" + axisTitleX + "\")";
  TBTDCbar->AddButton("TDC frag0x06 (all histos)", stringToExecute, "TDC word1frag0x06 and word2frag0x06 all channels");

  /*
  // TDC frag0x06 histograms
  pattern = "frag0x06";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3, \"" + axisTitleX + "\")";
  TBTDCbar->AddButton("TDC frag0x06 (all histos)", stringToExecute, "TDC word1frag0x06 and word2frag0x06 all channels");
  */

  TBTDCbar->Show();
  gROOT->SaveContext();
}

