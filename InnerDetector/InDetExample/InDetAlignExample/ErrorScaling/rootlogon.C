/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


{
 
  
  TStyle* GloStyle;
  GloStyle = gStyle;            

  TStyle* RaviStyle = new TStyle("RaviStyle"
				,"The Even More Perfect Style for Plots ;-)");
  gStyle = RaviStyle;
  // Paper size
  RaviStyle->SetPaperSize(gStyle->kUSLetter);
  
  // Canvas
  
  RaviStyle->SetCanvasColor     (0);
  RaviStyle->SetCanvasBorderSize(10);
  RaviStyle->SetCanvasBorderMode(0);
  RaviStyle->SetCanvasDefH      (600);
  RaviStyle->SetCanvasDefW      (971);
  RaviStyle->SetCanvasDefX      (10);
  RaviStyle->SetCanvasDefY      (10);

  // Pads

  RaviStyle->SetPadColor       (0);
  RaviStyle->SetPadBorderSize  (10);
  RaviStyle->SetPadBorderMode  (0);
  RaviStyle->SetPadBottomMargin(0.1);//0.05?
  RaviStyle->SetPadTopMargin   (0.10);
  RaviStyle->SetPadLeftMargin  (0.12);
  RaviStyle->SetPadRightMargin (0.05);
  RaviStyle->SetPadGridX       (0);
  RaviStyle->SetPadGridY       (0);
  RaviStyle->SetPadTickX       (1);
  RaviStyle->SetPadTickY       (1);

  // Frames

  RaviStyle->SetFrameFillStyle ( 0);
  RaviStyle->SetFrameFillColor ( 0);
  RaviStyle->SetFrameLineColor ( 1);
  RaviStyle->SetFrameLineStyle ( 0);
  RaviStyle->SetFrameLineWidth ( 1);
  RaviStyle->SetFrameBorderSize(10);
  RaviStyle->SetFrameBorderMode( 0);

  // Histograms

  // RaviStyle->SetHistFillColor(38);
  // RaviStyle->SetHistFillStyle(1);
  RaviStyle->SetHistLineColor(1);
  RaviStyle->SetHistLineStyle(1);
  RaviStyle->SetHistLineWidth(1);

  // Functions

  RaviStyle->SetFuncColor(2);
  RaviStyle->SetFuncStyle(1);
  RaviStyle->SetFuncWidth(1);

  // Various

  RaviStyle->SetTickLength ( 0.02,"X");

  RaviStyle->SetTitleSize  ( 0.050,"X");
  RaviStyle->SetTitleOffset( 0.900,"X");
  RaviStyle->SetLabelOffset( 0.005,"X");
  RaviStyle->SetLabelSize  ( 0.05,"X");
  RaviStyle->SetLabelFont  ( 42   ,"X");
  //RaviStyle->SetTitleFont  ( 42   ,"XY");


  RaviStyle->SetTickLength ( 0.02,"Y");
  RaviStyle->SetTitleSize  ( 0.050,"Y");
  RaviStyle->SetTitleOffset( 1.25,"Y");
  RaviStyle->SetLabelOffset( 0.005,"Y");
  RaviStyle->SetLabelSize  ( 0.05,"Y");
  RaviStyle->SetLabelFont  ( 42   ,"Y");
  
  //  RaviStyle->SetMarkerSize (1);
  //  RaviStyle->SetMarkerStyle(1);

  // Options
  //  RaviStyle->SetOptStat(1110);
  RaviStyle->SetPalette(1);
  RaviStyle->SetLineWidth(1);


  // stats
  RaviStyle->SetStatBorderSize(0);
  RaviStyle->SetStatColor(0);
  RaviStyle->SetStatX(0.93);
  RaviStyle->SetStatY(0.88);
  RaviStyle->SetStatH(0.025);
  RaviStyle->SetStatStyle(0);
  RaviStyle->SetStatFont(42);
  RaviStyle->SetStatFontSize(0.04);
  RaviStyle->SetStatW(0.15);
  RaviStyle->SetOptStat(1111);
  
  RaviStyle->SetOptFit(1111);
  RaviStyle->SetFitFormat(".3f");


  // title
  RaviStyle->SetOptTitle(1);
  RaviStyle->SetTitleColor(1);
  RaviStyle->SetTitleStyle(0);
  RaviStyle->SetTitleBorderSize(0);
  RaviStyle->SetTitleFont(42,"Junk");
  RaviStyle->SetTitleFont(42,"XY");
  RaviStyle->SetTitleFontSize(0.06);
  RaviStyle->SetTitleX(0.08);
  RaviStyle->SetTitleY(0.98 );


 // gROOT->LoadMacro("~/rootfiles/my_functions.C");
 // printf(">> gStyle initialized with favorite parameters!\n");
}
