////////////////////////////////////////////////////////////
// TBInfo.C
// 
// last modified 2004/06/27
//
// Displays the contents of an ASCII file in a canvas of 
// constant size.
////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

#include "TCanvas.h"
#include "TH1F.h"
#include "TPaveLabel.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void TBInfo(TString file, TString canvasTitle, TString canvasNameBase) {
  // TString file           text file to display
  // TString canvasTitle    each canvas has a title composed of the canvas name followed by canvasTitle
  // TString canvasNameBase each canvas name is composed of the canvasNameBase followed by an index

  float canvasWidthX = 600;
  float canvasWidthY = 462;
  float canvasX      = 400;
  float canvasY      =  20;

  // open file
  ifstream fStream(file);
  // exit if problems opening the file
  if (!fStream.is_open()) {
    TString message = "Cannot open file " + file;
    OKButton(message);
    return;
  }

  TString stringToExecute = "closeCanvases(\"" + canvasNameBase + "\")";
  gROOT->ProcessLine(stringToExecute);

  // get number of lines and max number of characters per line
  int nLine = 0;
  int nCharMax = 0;
  TString lineBuffer;
  while (!fStream.eof()) {
    lineBuffer.ReadLine(fStream, false);  // do not skip white spaces
    if (!fStream.eof()) {
      nLine++;
      int nChar = lineBuffer.Length();
      if (nChar > nCharMax) nCharMax = nChar;
    }
  }
  fStream.close();
  //  cout << "Number of lines in the file:            " << nLine << endl;
  //  cout << "Maximum number of characters in a line: " << nCharMax << endl;

  // compute textsize
  float textSizeMax = 1.;
  float fudgeFactor = 1.3;  // estimate of average height/width for characters, done with "O" as character!
  float nCharCorr = nCharMax*canvasWidthY/canvasWidthX/fudgeFactor;
  int n = (nLine > nCharCorr) ? nLine : nCharCorr;
  float textSize = (n > 0) ? 1./n : textSizeMax;
  //  cout << "n: " << n << "    textsize: " << textSize << endl;

  // fill canvas
  TCanvas* oneCanvas = new TCanvas(canvasNameBase, canvasTitle, canvasX, canvasY, canvasWidthX, canvasWidthY);  
  TPaveText* pave = new TPaveText(0.01, 0.01, 0.99, 0.99, "NDC");
  pave->ReadFile(file);
  pave->SetTextSize(textSize);
  pave->SetTextAlign(11);  // bottom left align
  pave->SetBorderSize(0);
  pave->SetMargin(0.02);
  pave->SetFillColor(0);
  pave->Draw();

}

