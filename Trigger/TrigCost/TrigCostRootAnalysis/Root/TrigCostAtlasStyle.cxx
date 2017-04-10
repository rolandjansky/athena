/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// ATLAS Style, based on a style file from BaBar
// Note that this is a MODIFIED version for use in TrigCostRoot
//

// STL include(s):
#include <iostream>

// Local include(s):
#include "../TrigCostRootAnalysis/TrigCostAtlasStyle.h"

// ROOT include(s):
#include "TROOT.h"

/**
 * Create and set the Cost style. Based on AtlasStyle, but with some statistics enabled.
 */
void SetTrigCostAtlasStyle() {
  static TStyle* trigCostAtlasStyle = 0;

  std::cout << "\nApplying Trig Cost ATLAS style settings...\n" << std::endl;
  if (trigCostAtlasStyle == 0) trigCostAtlasStyle = TrigCostAtlasStyle();
  gROOT->SetStyle("ATLAS_TrigCost");
  gROOT->ForceStyle();
}

/**
 * Create a style object based on AtlasStyle, but with some statistics enabled.
 * @return Pointer to style object.
 */
TStyle* TrigCostAtlasStyle() {
  TStyle* atlasStyle = new TStyle("ATLAS_TrigCost", "Atlas Trig Cost style");

  // use plain black on white colors
  Int_t icol = 0; // WHITE

  atlasStyle->SetFrameBorderMode(icol);
  atlasStyle->SetFrameFillColor(icol);
  atlasStyle->SetCanvasBorderMode(icol);
  atlasStyle->SetCanvasColor(icol);
  atlasStyle->SetPadBorderMode(icol);
  atlasStyle->SetPadColor(icol);
  atlasStyle->SetStatColor(icol);
  //atlasStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  atlasStyle->SetPaperSize(20, 26);

  // set margin sizes
  atlasStyle->SetPadTopMargin(0.05);
  atlasStyle->SetPadRightMargin(0.10);
  atlasStyle->SetPadBottomMargin(0.16);
  atlasStyle->SetPadLeftMargin(0.16);

  // set title offsets (for axis label)
  atlasStyle->SetTitleXOffset(1.4);
  atlasStyle->SetTitleYOffset(1.4);

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font = 42; // Helvetica
  Double_t tsize = 0.05;
  atlasStyle->SetTextFont(font);

  atlasStyle->SetTextSize(tsize);
  atlasStyle->SetLabelFont(font, "x");
  atlasStyle->SetTitleFont(font, "x");
  atlasStyle->SetLabelFont(font, "y");
  atlasStyle->SetTitleFont(font, "y");
  atlasStyle->SetLabelFont(font, "z");
  atlasStyle->SetTitleFont(font, "z");

  atlasStyle->SetLabelSize(tsize, "x");
  atlasStyle->SetTitleSize(tsize, "x");
  atlasStyle->SetLabelSize(tsize, "y");
  atlasStyle->SetTitleSize(tsize, "y");
  atlasStyle->SetLabelSize(tsize, "z");
  atlasStyle->SetTitleSize(tsize, "z");

  // use bold lines and markers
  atlasStyle->SetMarkerStyle(20);
  atlasStyle->SetMarkerSize(1.2);
  atlasStyle->SetHistLineWidth(2.);
  atlasStyle->SetLineStyleString(2, "[12 12]"); // postscript dashes

  // get rid of X error bars
  //atlasStyle->SetErrorX(0.001);
  // get rid of error bar caps
  atlasStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  atlasStyle->SetOptTitle(0);
  atlasStyle->SetOptStat("emruoi");
  atlasStyle->SetStatBorderSize(1);
  atlasStyle->SetStatFont(font);
  //atlasStyle->SetOptStat(0);
  //atlasStyle->SetOptFit(1111);
  atlasStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  atlasStyle->SetPadTickX(1);
  atlasStyle->SetPadTickY(1);

  return atlasStyle;
}
