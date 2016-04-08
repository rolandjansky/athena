/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "bigmatrix.h"
#include "../../monitor/lib/libbigmatrix.so"


//=====================================================================
//
// bigmatrix()
//
//=====================================================================
void bigmatrix(string bigmatrixpath) {

  cout << " bigmatrix..." << endl;

  // bigmatrix class
  bigmatrix *matrix = new bigmatrix(bigmatrixpath);

  // Set style
  style();

  // Check Status
  bool status = matrix->Status();

  if (status) {
    // Draw bigmatrix
    TH2F* hBigMatrix = matrix->GetPlot();
    cBigMatrix = new TCanvas("cBigMatrix","cBigMatrix",1000,750);
    hBigMatrix->Draw("COLZ");
    matrix->DrawLines(cBigMatrix);
    
    // store plot
    cout << endl;
    cout << " Storing the plot..." << endl;
    cBigMatrix->Print("bigmatrix.gif");
    if (matrix->NCols()<500) cBigMatrix->Print("bigmatrix.ps");
    
    // Compress it!
    sprintf(name,".!gzip -f bigmatrix.ps");
    gROOT->ProcessLine(name);
  }

  return;  
}


//=====================================================================
// style()
//=====================================================================
void style() {

  // Default options
  Float_t isize = 0.02;
  Int_t font = 42;

  gStyle->SetOptStat("e");

  gStyle->SetCanvasColor(kWhite);  
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);

  gStyle->SetPadBorderMode(0);

  gStyle->SetLabelFont(font,"X");
  gStyle->SetLabelFont(font,"Y");
  gStyle->SetLabelFont(font,"Z");
  gStyle->SetLabelSize(isize,"x");
  gStyle->SetLabelSize(isize,"y");
  gStyle->SetLabelSize(isize,"z");

  gStyle->SetTitleFont(font,"x");
  gStyle->SetTitleFont(font,"y");
  gStyle->SetTitleFont(font,"z");
  gStyle->SetTitleSize(isize,"x");  
  gStyle->SetTitleSize(isize,"y");
  gStyle->SetTitleSize(isize,"z");
  gStyle->SetTitleYOffset(1.35);

  gStyle->SetOptFit(0);

  gStyle->SetStatW(0.3);
  gStyle->SetStatColor(0);
  gStyle->SetStatFont(font);
  gStyle->SetStatFontSize(isize);
  
  gStyle->SetStripDecimals(false);
  TGaxis::SetMaxDigits(4);

  gStyle->SetTextFont(font);

  gStyle->SetFrameBorderMode(0);

  gStyle->SetPalette(1);

  gStyle->SetPaintTextFormat("g");

  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  gROOT->ForceStyle();
}
