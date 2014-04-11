////////////////////////////////////////////////////////////
// TBADC.C
// 
// last modified 2004/06/23
//
// Creates menu bar for TestBeam ADC monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For TestBeam ADC Monitoring histograms 
void TBADC() {

  TString stringToExecute, path, canvasTitle, canvasNameBase, axisTitleX, axisTitleY;

  TControlBar* TBADCbar = new TControlBar("vertical", "TBADC monitoring", 160, 40);
  
  path = "/BeamDetectors/ADC";
  canvasTitle = "ADC Monitoring";
  canvasNameBase = "cBase";
  axisTitleX = "ADC";
  axisTitleY = "No. of events";

  // plot all histos
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",4,4)";
  TBADCbar->AddButton("ADC (all histos)", stringToExecute, "ADC frag quantities");


  // ADC frag0x04
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,0,7)";
  TBADCbar->AddButton("ADC frag0x04 chan 0-3  ", stringToExecute, "ADC word1frag0x04 and word2frag0x04 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,8,15)";
  TBADCbar->AddButton("ADC frag0x04 chan 4-7  ", stringToExecute, "ADC word1frag0x04 and word2frag0x04 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,16,23)";
  TBADCbar->AddButton("ADC frag0x04 chan 8-11 ", stringToExecute, "ADC word1frag0x04 and word2frag0x04 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,24,31)";
  TBADCbar->AddButton("ADC frag0x04 chan 12-15", stringToExecute, "ADC word1frag0x04 and word2frag0x04 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,32,39)";
  TBADCbar->AddButton("ADC frag0x04 chan 16-19", stringToExecute, "ADC word1frag0x04 and word2frag0x04 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,2,40,47)";
  TBADCbar->AddButton("ADC frag0x04 chan 20-23", stringToExecute, "ADC word1frag0x04 and word2frag0x04 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",3,3,0,47)";
  TBADCbar->AddButton("ADC frag0x04 (all histos)", stringToExecute, "ADC word1frag0x04 and word2frag0x04 quantities");

  
  // ADC frag0x05
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,48,53)";
  TBADCbar->AddButton("ADC frag0x05 chan 0,3,6  ", stringToExecute, "ADC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,54,59)";
  TBADCbar->AddButton("ADC frag0x05 chan 9,12,15", stringToExecute, "ADC word1frag0x05 and word2frag0x05 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,48,59)";
  TBADCbar->AddButton("ADC frag0x05 (all histos)", stringToExecute, "ADC word1frag0x05 and word2frag0x05 quantities");


  // ADC frag0x06
  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,60,65)";
  TBADCbar->AddButton("ADC frag0x06 chan 0-2", stringToExecute, "ADC word1frag0x06 and word2frag0x06 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,66,72)";
  TBADCbar->AddButton("ADC frag0x06 chan 3-6", stringToExecute, "ADC word1frag0x06 and word2frag0x06 quantities");

  stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",2,3,60,72)";
  TBADCbar->AddButton("ADC frag0x06 (all histos)", stringToExecute, "ADC word1frag0x06 and word2frag0x06 quantities");

  
  // All word1 ADC frags
   stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",3,3,0,-72)";
  TBADCbar->AddButton("All word1 frag ADCs", stringToExecute, "All word1 frag ADCs");


  // All word2 ADC frags
   stringToExecute = "keyPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", \"" + axisTitleX + "\", \"" + axisTitleY + "\",3,3,1,-72)";
  TBADCbar->AddButton("All word2 frag ADCs", stringToExecute, "All word2 frag ADCs");


  TBADCbar->Show();
  gROOT->SaveContext();
}

