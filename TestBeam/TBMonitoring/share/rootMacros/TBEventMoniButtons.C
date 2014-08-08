#include <iostream>
#include "TROOT.h"
#include "TControlBar.h"

using namespace std;

void TBEventMoniButtons() {

  gROOT->Reset();
  gStyle->SetScreenFactor(1); //if you have a large screen, select 1,2 or 1.4

  TControlBar * TBbar = new TControlBar("vertical", "Main TB Event monitoring", 0, 0);

  TBbar->AddButton("New Browser",      "new TBrowser;",             "Start the ROOT Browser");
  TBbar->AddButton("TBPhase",          "TBPhase()",                  "Timing quantities");
  //bar->AddButton("LArDigitMon",      "LArDigitMon()",             "Plot options for LArDigits");
  //bar->AddButton("LArCalibDigitMon", "LArCalibDigitMon()",        "Plot options for LArCalibDigits");
  //bar->AddButton("LArWaveMon",       ".x LArWaveMon.C",           "Plot options for LArWaves");
  //bar->AddButton("LArRawChannelMon", ".x LArRawChannelMon.C",     "Plot options for LArRawChannels");
  //bar->AddButton("CaloCellMon",      ".x CaloCellMon.C",          "Plot options for CaloCells");
  //bar->AddButton("CaloClusterMon",   ".x CaloClusterMon.C",       "Plot options for CaloClusters");
  TBbar->AddButton("TBADC",            "TBADC()",                    "Plot options for TestBeam ADC");
  TBbar->AddButton("TBTDC",            "TBTDC()",                    "Plot options for TestBeam TDC");
  TBbar->AddButton("TBBPC",            "TBBPC()",                    "Plot options for TestBeam BPC");
  TBbar->AddButton("TBScintillator",   "TBScintillator()",           "Plot options for Scintillators");
  TBbar->AddButton("TBTailCatcher",    "TBTailCatcher()",            "Plot options for TailCatcher Scintillators");

  TBbar->AddButton("reload test.root", "TFile f(\"test.root\")",    "Reload file test.root");
  TBbar->AddButton("Save RunSum.ps",   ".x SaveRunSum.C",           "Save run summary postscript file RunSum.ps");
  // bar->AddButton("load file", ".x LoadFile.C", "Interactive File Load");
  TBbar->AddButton("Print canvas(es)", "printCanvases()",           "Print active canvas(es)");
  TBbar->AddButton("Quit root",        "gApplication->Terminate()", "Quit root");

  TBbar->Show();
  gROOT->SaveContext();
}
