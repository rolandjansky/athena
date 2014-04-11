////////////////////////////////////////////////////////////
// CaloCellMon.C
// 
// last modified 2004/06/28
//
// This macro is called from the Monibuttons.C macro.
// It creates a button menu for viewing Calorimeter Cell 
// histograms
////////////////////////////////////////////////////////////

#include <iostream>

#include "TControlBar.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void CaloCellMon() {
  TString stringToExecute, path, canvasTitle, canvasNameBase, axisTitleX;

  TControlBar* CaloCellbar = new TControlBar("vertical", "Calorimeter Cell Monitoring",110,20);

  CaloCellbar->AddButton("New Browser",       "new TBrowser;",  "Start the ROOT Browser");
  
  // path = "/LArCellEMEC";
  path = "/CaloCell";
  canvasTitle = "LAr EMEC calorimeter cell";
  canvasNameBase = "cBaseEMEC";
  stringToExecute = "CaloCellMonPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  CaloCellbar->AddButton("LArCell EMEC", stringToExecute, "Energy, Eta, and Phi for EMEC LAr Cell");

  //path = "/LArCellHEC";
  path = "/CaloCell";
  canvasTitle = "LAr HEC calorimeter cell";
  canvasNameBase = "cBaseHEC";
  stringToExecute = "CaloCellMonPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  CaloCellbar->AddButton("LArCell HEC", stringToExecute, "Energy, Eta, and Phi for HEC LAr Cell");

  //path = "/LArCellFCAL";
  path = "/CaloCell";
  canvasTitle = "LAr FCal calorimeter cell";
  canvasNameBase = "cBaseFCAL";
  stringToExecute = "CaloCellMonPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  CaloCellbar->AddButton("LArCell FCal", stringToExecute, "Energy, Eta, and Phi for FCal LAr Cell");
 
  //path = "/LArCellFCAL";
  path = "/CaloCell";
  canvasTitle = "Cold TailCatcher cell";
  canvasNameBase = "cBaseCTC";
  stringToExecute = "CaloCellMonPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  CaloCellbar->AddButton("Cold TailCatcher", stringToExecute, "Energy, Eta, and Phi for Cold TailCatcher Cell");

  //path = "/LArCellFCAL";
  path = "/CaloCell";
  canvasTitle = "Calo Correlation Plots";
  canvasNameBase = "cBaseCorr";
  stringToExecute = "CaloCellMonPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  CaloCellbar->AddButton("Calo Correlations", stringToExecute, "Calo Correlation Plots");

  // prepare buttons from general energy histos from TBCaloResponseMonTool

  path = "/calomon";
  // path = "/CaloCell";a
  canvasTitle = "Energy";
  canvasNameBase = "cBase";
  axisTitleX = "Energy (GeV EM scale)";

  TString* iName;
  TList* listOfNames = new TList();
  TString name;
  name = "EME2_E";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "EME3_E";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "LArEM_E";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "skip";       iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "HEC0_E";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "HEC1_E";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "HEC2_E";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "LArHEC_E";   iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "FCal1_E";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "FCal2_E";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "FCal3_E";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "LArFCal_E";  iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "LArEM_E";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "LArHEC_E";   iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "LArFCal_E";  iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "CaloE";      iName = new TString(name); listOfNames->Add((TObject*)iName);

  stringToExecute = "plotList(\"" + path + "\", (TList*)";
  stringToExecute += (int)listOfNames;
  stringToExecute += ", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\")";
  CaloCellbar->AddButton("General", stringToExecute, "General energy histograms");
  
  CaloCellbar->Show();
  gROOT->SaveContext();
}

