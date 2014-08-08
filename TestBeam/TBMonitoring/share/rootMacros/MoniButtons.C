#include <iostream>
#include "TROOT.h"
#include "TControlBar.h"

using namespace std;

void MoniButtons() {

  gROOT->Reset();
  gStyle->SetScreenFactor(1); //if you have a large screen, select 1,2 or 1.4

  TControlBar * bar = new TControlBar("vertical", "Main monitoring", 0, 0);

  bar->AddButton("New Browser",      "new TBrowser;",             "Start the ROOT Browser");
  bar->AddButton("TBEventMon",       "TBEventMon()",              "Plot options for TBEvent objects");
  bar->AddButton("LArDigitMon",      "LArDigitMon()",             "Plot options for LArDigits");
  bar->AddButton("LArCalibDigitMon", "LArCalibDigitMon()",     "Plot options for LArCalibDigits");
  bar->AddButton("LArWaveMon",       ".x LArWaveMon.C",           "Plot options for LArWaves");
  bar->AddButton("LArRawChannelMon", ".x LArRawChannelMon.C",     "Plot options for LArRawChannels");
  bar->AddButton("CaloCellMon",      ".x CaloCellMon.C",          "Plot options for CaloCells");
  bar->AddButton("CaloClusterMon",   ".x CaloClusterMon.C",       "Plot options for CaloClusters");
  bar->AddButton("reload test.root", "TFile f(\"test.root\")",    "Reload file test.root");
  bar->AddButton("Save RunSum.ps",   ".x SaveRunSum.C",           "Save run summary postscript file RunSum.ps");
  // bar->AddButton("load file", ".x LoadFile.C", "Interactive File Load");
  bar->AddButton("Print canvas(es)", "printCanvases()",           "Print active canvas(es)");

  bar->AddButton("Quit root",        "gApplication->Terminate()", "Quit root");
 
  bar->Show();
  //gROOT->SaveContext();
}
