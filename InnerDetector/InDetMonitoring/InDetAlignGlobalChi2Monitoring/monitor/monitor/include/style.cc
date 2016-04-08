/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void style() {

  // Default options
  ColorForUsed = kRed;
  ColorForAll  = kGray;
  ColorForGenParticle = kSpring+4;

  ColorForBarrel  = kGray;
  ColorForEndCapC = kRed;
  ColorForEndCapA = kBlue;

  Float_t isize = 0.04;
  font = 42;

  gStyle->SetOptStat(111110);

  gStyle->SetCanvasColor(kWhite);  
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);

  gStyle->SetPadBorderMode(0);

  gStyle->SetLabelFont(font,"X");
  gStyle->SetLabelFont(font,"Y");
  gStyle->SetLabelFont(font,"Z");
  gStyle->SetLabelSize(isize,"x");
  gStyle->SetLabelSize(isize,"y");
  gStyle->SetLabelSize(isize-0.01,"z");

  gStyle->SetTitleFont(font,"x");
  gStyle->SetTitleFont(font,"y");
  gStyle->SetTitleFont(font,"z");
  gStyle->SetTitleSize(isize,"x");  
  gStyle->SetTitleSize(isize,"y");
  gStyle->SetTitleSize(isize,"z");
  gStyle->SetTitleYOffset(1.5);

  gStyle->SetOptFit(0);

  gStyle->SetStatW(0.3);
  //  gStyle->SetStatH(0.15);
  gStyle->SetStatColor(kWhite);
  gStyle->SetStatFont(font);
  gStyle->SetStatFontSize(isize);
  // gStyle->SetStatStyle(0); // to view data behind box too!
  // gStyle->SetStatBorderSize(0);   // remove shade from stat box
  
  gStyle->SetStripDecimals(false);
  TGaxis::SetMaxDigits(4);

  gStyle->SetTextFont(font);

  gStyle->SetFrameBorderMode(0);

  gStyle->SetPalette(1);

  gStyle->SetPaintTextFormat("g");

  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadTopMargin(0.12);
  // gStyle->SetPadBottomMargin(0.15);

  gROOT->ForceStyle();
}

