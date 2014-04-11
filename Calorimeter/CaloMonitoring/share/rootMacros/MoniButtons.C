////////////////////////////////////////////////////////////
// Monibuttons.C
// 
// last modified 2004/06/26
//
// Initial menu for monitoring options
////////////////////////////////////////////////////////////

#include <iostream>
#include "TROOT.h"
#include "TControlBar.h"

using namespace std;

void MoniButtons() {

  TString file, canvasTitle, canvasNameBase, stringToExecute, printCommand, path;

  gROOT->Reset();
  gStyle->SetScreenFactor(1.3); //if you have a large screen, select 1,2 or 1.4

  TControlBar * bar = new TControlBar("vertical", "Main monitoring", 0, 0);

  // add buttons to the menu bar, which run the loaded monitoring macros
  bar->AddButton("New Browser",      "new TBrowser;",             "Start the ROOT Browser");

  file = "lastrun.txt";  // txt file with TB run information
  canvasTitle = "Event and Run header info";
  canvasNameBase = "cBase";
  stringToExecute = "TBInfo(\"" + file + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  bar->AddButton("TBInfo",           stringToExecute,             "Display run and event header info");

  path = "/BeamDetectors";
  canvasTitle = "Trigger Pattern Unit Summary";
  canvasNameBase = "cBase";
  stringToExecute = "TrigPattern(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  bar->AddButton("TrigPattern",      stringToExecute,             "Plot summary of trigger pattern unit bits");
  
  bar->AddButton("TBEventMon",       "TBEventMon()",              "Plot options for TBEvent objects");
  bar->AddButton("LArDigitMon",      "LArDigitMon()",             "Plot options for LArDigits");
  bar->AddButton("LArCalibDigitMon", "LArCalibDigitMon()",        "Plot options for LArCalibDigits");
  bar->AddButton("LArWaveMon",       "LArWaveMon()",              "Plot options for LArWaves");
  //  bar->AddButton("LArRawChannelMon", ".x LArRawChannelMon.C",     "Plot options for LArRawChannels");
  bar->AddButton("CaloCellMon",      "CaloCellMon()",             "Plot options for CaloCells");
  bar->AddButton("CaloCellBPCMon",   "CaloCellBPCMon()",          "Plot options for CaloCell energy vs beam position");
  //  bar->AddButton("CaloClusterMon",   ".x CaloClusterMon.C",       "Plot options for CaloClusters");
  bar->AddButton("reload lastrun.root", "TFile f(\"lastrun.root\")",    "Reload file test.root");

  printCommand = "xprint -h -p887-hna457 -oduplex";  // counting room, -h is for no banner page
  stringToExecute = "printSummary(\"" + printCommand + "\")";
  bar->AddButton("Print summary H6",   stringToExecute,           "Print summary 887-hna457");

  printCommand = "xprint -h -p887-hna457 -oduplex";  // counting room, -h is for no banner page
  stringToExecute = "printCanvases(\"" + printCommand + "\")";
  bar->AddButton("Print canvases H6",  stringToExecute,           "Print active canvas(es) 887-hna457");

  stringToExecute = "printSummary(\"\")";
  bar->AddButton("Save summary in a ps file",   stringToExecute,           "Save the summary in a postscript file");

  stringToExecute = "printCanvases(\"\")";
  bar->AddButton("Save canvases in a ps file",   stringToExecute,          "Save the canvases in a postscript file");

//   printCommand = "xprint -h -p40-1d-cor";   // bat 40, -h is for no banner
//   stringToExecute = "printSummary(\"" + printCommand + "\")";
//   bar->AddButton("Print summary B40",   stringToExecute,           "Print summary 40-1d-cor");

//   printCommand = "xprint -h -p40-1d-cor";   // bat 40, -h is for no banner
//   stringToExecute = "printCanvases(\"" + printCommand + "\")";
//   bar->AddButton("Print canvases B40", stringToExecute,           "Print active canvas(es) 40-1d-cor");

  bar->AddButton("Quit root",        "gApplication->Terminate()", "Quit root");
 
  bar->Show();
  gROOT->SaveContext();
}
