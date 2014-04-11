////////////////////////////////////////////////////////////
// printSummary.C
// 
// last modified 2004/06/28
//
// prints out specific historgrams for shift crew monitoring
////////////////////////////////////////////////////////////

#include <iostream>

#include "TCanvas.h"
#include "TCollection.h"
#include "TList.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void printSummary(TString printCommand="xprint -h -p887-hna457", TString tempPs="canvas.ps") {

  // if printCommand is "", then the file is not printed and kept on disk
  // TString printCommand = "lp -dzork";                // UVic
  // TString printCommand = "xprint -h -p40-1d-cor";    // bat 40, -h is for no banner 
  // TString printCommand = "xprint -h -p887-hna457";   // counting room, -h is for no banner page

  TString stringToExecute, path, pattern, canvasTitle, canvasNameBase, axisTitleX, axisTitleY;
  TString name;
  TString* iName;
  TList* listOfNames;

  // close any open canvases
  TString stringToExecute = "closeCanvases(\"\")";
  gROOT->ProcessLine(stringToExecute);

  // start producing canvases

  TString file = "lastrun.txt";
  canvasTitle = "Event and Run header info";
  canvasNameBase = "cBaseA";
  stringToExecute = "TBInfo(\"" + file + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  gROOT->ProcessLine(stringToExecute);

  path = "/BeamDetectors";
  canvasTitle = "Trigger Pattern Unit Summary";
  canvasNameBase = "cBaseZ";
  stringToExecute = "TrigPattern(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";  
  gROOT->ProcessLine(stringToExecute);
  
  path = "/BeamDetectors/TDC";
  canvasTitle = "TDC Monitoring";
  canvasNameBase = "cBaseB";
  axisTitleX = "TDC";
  listOfNames = new TList();
  name = "word1frag0x03chan0";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "word2frag0x03chan0";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  stringToExecute = "plotList(\"" + path + "\", (TList*)";
  stringToExecute += (int)listOfNames;
  stringToExecute += ", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 1, 2, \"" + axisTitleX + "\")";
  gROOT->ProcessLine(stringToExecute);

  path = "/BeamDetectors/BPC";
  pattern = "pos";
  canvasTitle = "BPC Monitoring";
  canvasNameBase = "cBaseC";
  axisTitleX = "Position (mm)";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 4, 3, \"" + axisTitleX + "\")";
  gROOT->ProcessLine(stringToExecute);

  path = "/pedsamp0";
  canvasTitle = "FEB channel occupancy";
  canvasNameBase = "cBaseD";
  stringToExecute = "LArDigitGlobalPlots(\"" + path + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\")";
  gROOT->ProcessLine(stringToExecute);

  path = "/pedsamp0/febAdc";
  pattern = "";
  canvasTitle = "FEB: channel occupancy";
  canvasNameBase = "cBaseE";
  axisTitleX = "ADC channel number in FEB";
  axisTitleY = "<ADC(max)-ped> (ADC)";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 4, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
  gROOT->ProcessLine(stringToExecute);

  path = "/pedsamp0/samp0Rms";
  pattern = "";
  canvasTitle = "FEB: sample 0 RMS";
  canvasNameBase = "cBaseF";
  axisTitleX = "ADC channel number in FEB";
  axisTitleY = "sample 0 RMS (ADC)";
  stringToExecute = "plotPattern(\"" + path + "\", \"" + pattern + "\", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 2, 4, \"" + axisTitleX + "\", \"" + axisTitleY + "\")";
  gROOT->ProcessLine(stringToExecute);

  path = "/calomon";
  canvasTitle = "Energy";
  canvasNameBase = "cBaseG";
  axisTitleX = "Energy (GeV EM scale)";
  listOfNames = new TList();
  name = "EME2_E";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "EME3_E";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "skip";       iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "LArEM_E";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "HEC0_E";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "HEC1_E";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "HEC2_E";     iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "LArHEC_E";   iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "FCal1_E";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "FCal2_E";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "FCal3_E";    iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "LArFCal_E";  iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "skip";       iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "skip";       iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "skip";       iName = new TString(name); listOfNames->Add((TObject*)iName);
  name = "CaloE";      iName = new TString(name); listOfNames->Add((TObject*)iName);
  stringToExecute = "plotList(\"" + path + "\", (TList*)";
  stringToExecute += (int)listOfNames;
  stringToExecute += ", \"" + canvasTitle + "\", \"" + canvasNameBase + "\", 4, 4, \"" + axisTitleX + "\")";
  gROOT->ProcessLine(stringToExecute);

  // put all canvases in the ps file (do not print them yet)
  stringToExecute = "printCanvases(\"\", \"" + tempPs + "\")";
  gROOT->ProcessLine(stringToExecute);

  // close canvases
  stringToExecute = "closeCanvases(\"\")";
  gROOT->ProcessLine(stringToExecute);

  // prepare shell command for printing
  if (printCommand == "") {
    cout << "Summary in file " << tempPs << endl;
  } else {
    TString shellCommand = ".!" + printCommand + " " + tempPs + "; rm -f " + tempPs;
    cout << "Executing shell command: " << shellCommand << endl;
    gROOT->ProcessLine(shellCommand);
  }

}
