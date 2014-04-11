////////////////////////////////////////////////////////////
// TBScintillatorMon.C
// 
// last modified 2004/06/28
//
// Creates menu bar for TestBeam Scintillator monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBScintillatorMon() {

  TString stringToExecute, path, pattern, canvasTitle, canvasNameBase, axisTitleX;

  TControlBar* TBScintbar = new TControlBar("vertical", "TBScintillator monitoring", 160, 40);
  
  path = "/BeamDetectors/Scintillator";
  canvasTitle = "TestBeam Scintillators";
  canvasNameBase = "cBase";

  // plot all TBScintillator  histograms
  pattern = "";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3)";
  TBScintbar->AddButton("Scintillators (all histos)", stringToExecute, "TB Scintillator rawADC and TDC quantities");

  // plot individual scintillators
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"\", \"\", \"\", \"\", \"\", 0, 3)";
  TBScintbar->AddButton("Scintillators S1,S2 ", stringToExecute, "S1 and S2 Scintillators (raw and TDC)");

  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"\", \"\", \"\", \"\", \"\", 4, 7)";
  TBScintbar->AddButton("Scintillators S3,B  ", stringToExecute, "S3 and B Scintillators (raw and TDC)");

  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"\", \"\", \"\", \"\", \"\", 8, 11)";
  TBScintbar->AddButton("Scintillators Veto,Halo", stringToExecute, "Veto and Halo Scintillators (raw and TDC)");

  // plot muon scintillators
  int nChannelMax = 8;
  int nChannel = 2;
  for (int firstChannel = 1; firstChannel < nChannelMax; firstChannel += nChannel) {
    TString* iName;
    TList* listOfNames = new TList();
    for (int channel = firstChannel; channel < firstChannel + nChannel; channel++) {
      TString name;
      name = "muon";     name += channel; iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = "Timemuon"; name += channel; iName = new TString(name); listOfNames->Add((TObject*)iName);
    }
    stringToExecute = "plotList(\"" + path + "\", (TList*)";
    stringToExecute += (int)listOfNames;
    stringToExecute += ", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2)";
    TString channelRange = "muon"; channelRange += firstChannel; channelRange += " - muon"; channelRange += firstChannel + nChannel - 1;
    TString buttonName = "Scintillators " + channelRange;
    TString buttonHint = "raw ADC and TDC for TB Scintillators " + channelRange; 
    TBScintbar->AddButton(buttonName, stringToExecute, buttonHint);
  }

  // plot all raw for all scintillators
  axisTitleX = "ADC";
  pattern = "";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3, \"" + axisTitleX + "\", \"\", \"\", \"\", \"\", 0, -27)";
  TBScintbar->AddButton("All Scintillators (raw)", stringToExecute, "All Scintillator raw signals");

  // plot all TDC for all scintillators
  axisTitleX = "TDC";
  pattern = "Time";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3, \"" + axisTitleX + "\")";
  TBScintbar->AddButton("All Scintillators (TDCs)", stringToExecute, "All Scintillator TDC signals");
 
  TBScintbar->Show();
  gROOT->SaveContext();
}

