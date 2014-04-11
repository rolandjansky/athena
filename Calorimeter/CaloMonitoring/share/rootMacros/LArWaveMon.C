////////////////////////////////////////////////////////////
// LArWaveMon.C
// 
// last modified 2004/06/26
//
// Creates menu bar for LArWave monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"
#include "TString.h"
#include "TControlBar.h"

using namespace std;

// For LAr Wave Monitoring histograms 
void LArWaveMon() {

  TString stringToExecute, path, canvasTitle, canvasNameBase, dirName;

  TControlBar* LArWavebar = new TControlBar("vertical", "LAr Wave objects monitoring", 95, 20);
  
  LArWavebar->AddButton("New Browser", "new TBrowser;", "Start the ROOT Browser");
  
  path = "/wavemedium";
  /*if (!gDirectory->cd(path)) {
    TString message = "Path " + path + " not found";
    OKButton(message);
    return;
    }*/
  canvasTitle = "AllWaves";
  canvasNameBase = "cBase";
  stringToExecute = "AllWaveSumPlot(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  LArWavebar->AddButton("AllWaves", stringToExecute, "Summed over all FEBs");
  
  path = "/wavemedium/febWave/";
  if (!gDirectory->cd(path)) {
    TString message = "Path " + path + " not found";
    OKButton(message);
    return;
  }

  // create a list of directories within path
  TList* listOfKeys = gDirectory->GetListOfKeys();
  TIter myIter(listOfKeys);
  TKey* key;
  
  while (key = (TKey*)myIter.Next()) {   //begin loop for all feb directories in path
    dirName = key->GetName();
    stringToExecute = "LArWaveFebMon(\"" + path + "\",\"" + dirName + "\")";
    LArWavebar->AddButton("Wave "+dirName, stringToExecute , "Plot options for monitoring waves on "+dirName);
  }// end of TKey loop to add buttons for different feb directories within path

  LArWavebar->Show();
  gROOT->SaveContext();
}

