////////////////////////////////////////////////////////////
// TBBPCMon.C
// 
// last modified 2004/06/27
//
// Creates menu bar for TestBeam BPC monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBBPCMon() {

  TString stringToExecute, path, pattern, canvasTitle, canvasNameBase, axisTitleX;

  TControlBar* TBBPCbar = new TControlBar("vertical", "TBBPC monitoring", 160, 40);
  
  path = "/BeamDetectors/BPC";
  canvasTitle = "BPC Monitoring";
  canvasNameBase = "cBase";

  // plot all histos
  pattern = "";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 3, 3)";
  TBBPCbar->AddButton("BPC all histos", stringToExecute, "All BPCs (X Y pos, ADC, and TDC quantities)");


  // plot x and y positions from all BPCs
  axisTitleX = "Position (mm)";
  pattern = "pos";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3, \"" + axisTitleX + "\")";
  TBBPCbar->AddButton("All BPCs X and Y pos", stringToExecute, "X and Y position of BPCs 0 through 5");
  

  // ADC and TDC for individual BPCs
  TString* iName;
  int nBPC = 6;   // six BPCs labelled from 0 through 5

  // begin loop over all six BPCs creating a button for each
  for (int BPCcounter = 0; BPCcounter < nBPC; BPCcounter++) {
    pattern = "BPC"; pattern += BPCcounter;
    TList* listOfNames = new TList();
    TString name;
    name = pattern + "tdcLeft";    iName = new TString(name); listOfNames->Add((TObject*)iName);
    name = pattern + "tdcRight";   iName = new TString(name); listOfNames->Add((TObject*)iName);
    name = pattern + "tdcUp";      iName = new TString(name); listOfNames->Add((TObject*)iName);
    name = pattern + "tdcDown";    iName = new TString(name); listOfNames->Add((TObject*)iName);
    name = pattern + "adcVert";    iName = new TString(name); listOfNames->Add((TObject*)iName);
    name = pattern + "adcHoriz";   iName = new TString(name); listOfNames->Add((TObject*)iName);
    stringToExecute = "plotList(\"" + path + "\", (TList*)";
    stringToExecute += (int)listOfNames;
    stringToExecute += ", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 3)";
    TString buttonName = pattern + " (TDC and ADC)";
    TString buttonHint = "ADC and TDC quantities for " + pattern;
    TBBPCbar->AddButton(buttonName, stringToExecute, buttonHint);
  } // end loop over all 6 BPCs


  // plot all TDC for all BPCs
  axisTitleX = "TDC";
  pattern = "tdc";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\")";
  TBBPCbar->AddButton("All BPCs (TDC)", stringToExecute, "All BPCs all TDC signals");

  
  // plot all ADC for all BPCs
  axisTitleX = "ADC";
  pattern = "adc";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\")";
  TBBPCbar->AddButton("All BPCs (ADC)", stringToExecute, "All BPCs all ADC signals");


  TBBPCbar->Show();
  gROOT->SaveContext();
}

