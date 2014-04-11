////////////////////////////////////////////////////////////
// CaloCellBPCMon.C
// 
// last modified 2004/06/28
//
// This macro is called from the Monibuttons.C macro.
// It creates a button menu for viewing Calorimeter Cell 
// vs beam position histograms
////////////////////////////////////////////////////////////

#include <iostream>

#include "TCollection.h"
#include "TControlBar.h"
#include "TList.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void CaloCellBPCMon() {

  TString stringToExecute, buttonName, path, canvasTitle, canvasNameBase, axisTitleX, axisTitleY, axisTitleZ, drawOption;

  TControlBar* CaloCellBPCbar = new TControlBar("vertical", "Calorimeter Cell and BPC Monitoring",110,20);

  CaloCellBPCbar->AddButton("New Browser", "new TBrowser;", "Start the ROOT Browser");
  
  path = "/calomon";
  int iBPCMax = 6;
  
  // prepare a button for each BPC
  for (int iBPC = 0; iBPC < iBPCMax; iBPC++) {
    TString BPCName = "BPC"; BPCName += iBPC; 

    TString* s;
    TList* listOfPositions = new TList();
    s = new TString("X");  listOfPositions->Add((TObject*)s);
    s = new TString("Y");  listOfPositions->Add((TObject*)s);
    s = new TString("XY"); listOfPositions->Add((TObject*)s);

    // for each BPC, prepare a button for each position
    TIter myIter(listOfPositions);
    TString* iPos;
    // iterate over the positions
    while (iPos = (TString*)myIter.Next()) {

      TString pos = *iPos;
      //      cout << BPCName << " " << pos << endl;

      canvasTitle = BPCName + " energy vs " + pos;
      canvasNameBase = "cBase";
      if (pos == "XY") {
        axisTitleX = "X (mm)";
        axisTitleY = "Y (mm)";
        axisTitleZ = "<E> (GeV EM scale)";
        drawOption = "LEGO";
      } else {
        // axisTitleX = pos + " (mm)";  // this line crashes for some reason!
        axisTitleX = pos;  axisTitleX += " (mm)";
        axisTitleY = "<E> (GeV EM scale)";
        axisTitleZ = "";
        drawOption = "HIST";  // "" does the same
      }

      TString* iName;
      TList* listOfNames = new TList();
      TString name;
      name = BPCName + "_EME2_Evs" + pos;    iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = BPCName + "_EME3_Evs" + pos;    iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = BPCName + "_LArEM_Evs" + pos;   iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = BPCName + "_HEC0_Evs" + pos;    iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = BPCName + "_HEC1_Evs" + pos;    iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = BPCName + "_HEC2_Evs" + pos;    iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = BPCName + "_LArHEC_Evs" + pos;  iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = BPCName + "_FCal1_Evs" + pos;   iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = BPCName + "_FCal2_Evs" + pos;   iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = BPCName + "_FCal3_Evs" + pos;   iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = BPCName + "_LArFCal_Evs" + pos; iName = new TString(name); listOfNames->Add((TObject*)iName);
      name = BPCName + "_Evs" + pos;         iName = new TString(name); listOfNames->Add((TObject*)iName);

      buttonName = " energy vs " + pos + " " + BPCName;
      stringToExecute = "plotList(\"" + path + "\", (TList*)";
      stringToExecute += (int)listOfNames;
      stringToExecute += ", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 2, \"" + axisTitleX + "\", \"" + axisTitleY + "\", \"" + axisTitleZ + "\", \"\", \"" + drawOption + "\")";
      CaloCellBPCbar->AddButton(buttonName, stringToExecute, buttonName);

    }

  }

  CaloCellBPCbar->Show();
  gROOT->SaveContext();
}

