////////////////////////////////////////////////////////////
// runNumberLabel.C
// 
// last modified 2004/06/27
//
// Creates a TPaveLabel with the run number
// The run number is extracted from the summary txt file
////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

#include "TPaveLabel.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

TPaveLabel* runNumberLabel(float x1 = 0.55, float y1 = 0.905, float x2 = 0.75, float y2 = 0.980) {

  TString file = "lastrun.txt";  // summary file name hardwired...
  TString runNumber = "";
  
  // open file
  ifstream fStream(file);
  // exit if problems opening the file
  if (fStream.is_open()) {
    TString lineBuffer;
    bool found = false;
    while (!fStream.eof() && !found) {
      lineBuffer.ReadLine(fStream, false);  // do not skip white spaces
      if (!fStream.eof()) {
        TString leadingPattern = "Run number ";
        TString try = TString(lineBuffer.Data(), leadingPattern.Length());
        if (try == leadingPattern) {
          // found the right line
          found = true;
          TString runN = lineBuffer.Remove(0, leadingPattern.Length());  // remove pattern to get the run number
          runNumber += "run " + runN;
        }
      }
    }
  }
  fStream.close();
  // cout << "Run number label: " << runNumber << endl;

  TPaveLabel* runNLabel = new TPaveLabel(x1, y1, x2, y2, runNumber, "NDC");
  runNLabel->SetBorderSize(0);
  runNLabel->SetFillColor(0);
  runNLabel->SetTextAlign(32); // right center align
  return runNLabel;

}

