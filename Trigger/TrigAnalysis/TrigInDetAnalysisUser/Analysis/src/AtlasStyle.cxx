/**
 **     @file    AtlasStyle.cxx
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:41:26 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/

//
// ATLAS Style, based on a style file from BaBar
//

#include <iostream>

#include "AtlasStyle.h"

#include "TROOT.h"
#include "TColor.h"

void setpalette(TStyle* style);


void SetAtlasStyle ()
{
  static TStyle* atlasStyle = 0;
  std::cout << "\nApplying ATLAS style settings...\n" << std::endl ;
  if ( atlasStyle==0 ) atlasStyle = AtlasStyle();
  //  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();
  atlasStyle->cd();
}

TStyle* AtlasStyle() 
{
  // TStyle *atlasStyle = new TStyle("ATLAS","Atlas style");
  TStyle *atlasStyle = gROOT->GetStyle("Plain");

  // use plain black on white colors
  Int_t icol=0; // WHITE
  atlasStyle->SetFrameBorderMode(icol);
  atlasStyle->SetFrameFillColor(icol);
  atlasStyle->SetCanvasBorderMode(icol);
  atlasStyle->SetCanvasColor(icol);
  atlasStyle->SetPadBorderMode(icol);
  atlasStyle->SetPadColor(icol);
  atlasStyle->SetStatColor(icol);
  //atlasStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  atlasStyle->SetPaperSize(20,26);

  // set margin sizes
  atlasStyle->SetPadTopMargin(0.05);
  atlasStyle->SetPadRightMargin(0.05);
  atlasStyle->SetPadBottomMargin(0.16);
  atlasStyle->SetPadLeftMargin(0.16);

  // set title offsets (for axis label)
  atlasStyle->SetTitleXOffset(1.4);
  atlasStyle->SetTitleYOffset(1.4);

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  atlasStyle->SetTextFont(font);

  atlasStyle->SetTextSize(tsize);
  atlasStyle->SetLabelFont(font,"x");
  atlasStyle->SetTitleFont(font,"x");
  atlasStyle->SetLabelFont(font,"y");
  atlasStyle->SetTitleFont(font,"y");
  atlasStyle->SetLabelFont(font,"z");
  atlasStyle->SetTitleFont(font,"z");
  
  atlasStyle->SetLabelSize(tsize,"x");
  atlasStyle->SetTitleSize(tsize,"x");
  atlasStyle->SetLabelSize(tsize,"y");
  atlasStyle->SetTitleSize(tsize,"y");
  atlasStyle->SetLabelSize(tsize,"z");
  atlasStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
  atlasStyle->SetMarkerStyle(20);
  atlasStyle->SetMarkerSize(1.2);
  atlasStyle->SetHistLineWidth(2.);
  atlasStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //atlasStyle->SetErrorX(0.001);
  // get rid of error bar caps
  atlasStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  atlasStyle->SetOptTitle(0);
  //atlasStyle->SetOptStat(1111);
  atlasStyle->SetOptStat(0);
  //atlasStyle->SetOptFit(1111);
  atlasStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  atlasStyle->SetPadTickX(1);
  atlasStyle->SetPadTickY(1);

  setpalette(atlasStyle);

  return atlasStyle;

}



void setpalette(TStyle* style)
{
    const Int_t NCont = 98;


#if 0
    const Int_t NRGBs = 7;

    Double_t stops[NRGBs] = { 0.00, 0.20, 0.50, 0.60, 0.70, 0.90, 1.00 };

    Double_t red[NRGBs]   = { 0.00, 0.00, 0.00, 0.60, 0.90, 1.00, 0.50 };
    Double_t green[NRGBs] = { 0.00, 0.00, 0.40, 0.95, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.00, 1.00, 1.00, 0.10, 0.00, 0.00, 0.00 };
#endif


    const Int_t NRGBs = 7;

    Double_t stops[NRGBs] = { 0.00, 0.20, 0.30, 0.60, 0.70, 0.90, 1.00 };

    Double_t red[NRGBs]   = { 0.00, 0.00, 0.00, 0.60, 0.90, 1.00, 0.80 };
    Double_t green[NRGBs] = { 0.00, 0.00, 0.40, 0.95, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.00, 1.00, 1.00, 0.10, 0.00, 0.00, 0.00 };


    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    style->SetNumberContours(NCont);
}



