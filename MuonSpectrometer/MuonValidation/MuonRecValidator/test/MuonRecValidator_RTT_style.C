/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////
// declaration
///////////////////////////////////////
#include "TROOT.h"
#include "TStyle.h"

void MuonRecValidator_RTT_style() {

  std::cout << "Setting RTT plots style ..." << std::endl;

  /* 
  //stat settings
  gStyle->SetOptStat("emr");
  gStyle->SetStatX(0.98);
  gStyle->SetStatY(0.88);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.15);
  gStyle->SetOptFit(0);
  gStyle->SetOptFit(1111);
  //gStyle->SetOptStat("nemruo");
  gStyle->SetOptStat(111110);
  gStyle->SetStatColor(0);
  
  //Canvas settings
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetCanvasColor(kWhite);
  
  //Pad settings
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadLeftMargin(0.2);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.15);
  
  //label settings  
  gStyle->SetLabelSize(0.03,"X");
  gStyle->SetLabelSize(0.03,"Y");
  gStyle->SetLabelSize(0.015,"Z");
  gStyle->SetLabelFont(31,"X");
  gStyle->SetLabelFont(31,"Y");
  gStyle->SetLabelFont(31,"Z");
  
  //title settings
  gStyle->SetTitleXOffset(1.1);
  gStyle->SetTitleYOffset(1.25);
  gStyle->SetTitleColor(0);
  gStyle->SetTitleH(0.08);
  gStyle->SetTitleX(0.13);
  gStyle->SetTitleY(0.98);
  
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPalette(1);
  gStyle->SetPaintTextFormat("g");  
  */
  
  gStyle->SetTitleW(0.5);
  gStyle->SetTitleH(0.08);
  gStyle->SetTitleX(0.13);
  gStyle->SetTitleY(0.98);
  gStyle->SetStatX(0.98);
  gStyle->SetStatY(0.88);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.15);
  
  gStyle->SetStatFontSize(0.12);
  gStyle->SetLabelSize(0.06);
  gStyle->SetOptStat(111110);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleColor(0);
  gStyle->SetStatColor(0);     
  
  gStyle->SetOptStat("emr");
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetLabelSize(0.03,"X");
  gStyle->SetLabelSize(0.03,"Y");
  gStyle->SetLabelSize(0.015,"Z");
  gStyle->SetOptFit(0);
  gStyle->SetStatColor(0);
  gStyle->SetPalette(1);
  gStyle->SetPaintTextFormat("g");
  gStyle->SetHistFillColor(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameFillColor(0);
  
  //gROOT->ForceStyle();
  
}

 
