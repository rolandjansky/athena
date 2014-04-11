////////////////////////////////////////////////////////////
// TBTDC.C
// 
// last modified 2004/06/23
//
// Creates menu bar for TestBeam TDC monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam Monitoring histograms 
void TBTDC() {

  TString stringToExecute, path, canvasTitle, canvasNameBase, axisTitleX, axisTitleY;

  TControlBar* TBTDCbar = new TControlBar("vertical", "TBTDC monitoring", 160, 40);
  
  path = "/BeamDetectors/TDC";
  canvasTitle = "TDC Monitoring";
  canvasNameBase = "cBase";
  axisTitleX = "TDC";
  axisTitleY = "No. of events";

  // plot all TDC histograms
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",3,3)";
  TBTDCbar->AddButton("TDC (all histos)", stringToExecute, "TDC frag quantities");


  // TDC frag0x03 histograms
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,0,5)";
  TBTDCbar->AddButton("TDC frag0x03 (all histos)", stringToExecute, "TDC word1frag0x03 and word2frag0x03 quantities");


  // TDC frag0x05 histograms
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,6,9)";
  TBTDCbar->AddButton("TDC frag0x05 chan 1,2", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,10,13)";
  TBTDCbar->AddButton("TDC frag0x05 chan 4,5", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,14,17)";
  TBTDCbar->AddButton("TDC frag0x05 chan 7,8", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,18,21)";
  TBTDCbar->AddButton("TDC frag0x05 chan 10,11", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,22,25)";
  TBTDCbar->AddButton("TDC frag0x05 chan 13,14", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,26,29)";
  TBTDCbar->AddButton("TDC frag0x05 chan 16,17", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",3,3,6,29)";
  TBTDCbar->AddButton("TDC frag0x05 (all histos)", stringToExecute, "TDC word1frag0x05 and word2frag0x05 quantities");


  // TDC frag0x06 histograms
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,30,34)";
  TBTDCbar->AddButton("TDC frag0x06 (all histos)", stringToExecute, "TDC word1frag0x06 and word2frag0x06 quantities");

  TBTDCbar->Show();
  gROOT->SaveContext();
}

