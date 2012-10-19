/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#if !defined( __CINT__) || defined (__MAKECINT__)

#ifndef ROOT_TStyle
#include "TStyle.h"
#endif

#endif

void Style_Plain()
{
   // Add the saved style to the current ROOT session.

   delete gROOT->GetStyle("Plain");

   TStyle *tmpStyle = new TStyle("Plain", "Plain Style (no colors/fill areas)");
   tmpStyle->SetNdivisions(510, "x");
   tmpStyle->SetNdivisions(510, "y");
   tmpStyle->SetNdivisions(510, "z");
   tmpStyle->SetAxisColor(1, "x");
   tmpStyle->SetAxisColor(1, "y");
   tmpStyle->SetAxisColor(1, "z");
   tmpStyle->SetLabelColor(1, "x");
   tmpStyle->SetLabelColor(1, "y");
   tmpStyle->SetLabelColor(1, "z");
   tmpStyle->SetLabelFont(132, "x");
   tmpStyle->SetLabelFont(132, "y");
   tmpStyle->SetLabelFont(132, "z");
   tmpStyle->SetLabelOffset(0.005, "x");
   tmpStyle->SetLabelOffset(0.010, "y");
   tmpStyle->SetLabelOffset(0.005, "z");
   tmpStyle->SetLabelSize(0.05, "x");
   tmpStyle->SetLabelSize(0.05, "y");
   tmpStyle->SetLabelSize(0.05, "z");
   tmpStyle->SetTickLength(0.03, "x");
   tmpStyle->SetTickLength(0.03, "y");
   tmpStyle->SetTickLength(0.03, "z");
   tmpStyle->SetTitleOffset(0.88, "x");
   tmpStyle->SetTitleOffset(1, "y");
   tmpStyle->SetTitleOffset(1, "z");
   tmpStyle->SetTitleSize(0.063, "x");
   tmpStyle->SetTitleSize(0.06, "y");
   tmpStyle->SetTitleSize(0.06, "z");
   tmpStyle->SetTitleColor(1, "x");
   tmpStyle->SetTitleColor(1, "y");
   tmpStyle->SetTitleColor(1, "z");
   tmpStyle->SetTitleFont(132, "x");
   tmpStyle->SetTitleFont(132, "y");
   tmpStyle->SetTitleFont(132, "z");
   tmpStyle->SetBarWidth(1);
   tmpStyle->SetBarOffset(0);
   tmpStyle->SetDrawBorder(0);
   tmpStyle->SetOptLogx(0);
   tmpStyle->SetOptLogy(0);
   tmpStyle->SetOptLogz(0);
   tmpStyle->SetOptDate(0);
   tmpStyle->SetOptStat(2210);
   tmpStyle->SetOptTitle(kTRUE);
   tmpStyle->SetOptFit(111);
   tmpStyle->SetNumberContours(20);
   tmpStyle->GetAttDate()->SetTextFont(132);
   tmpStyle->GetAttDate()->SetTextSize(0.025);
   tmpStyle->GetAttDate()->SetTextAngle(0);
   tmpStyle->GetAttDate()->SetTextAlign(11);
   tmpStyle->GetAttDate()->SetTextColor(1);
   tmpStyle->SetDateX(0.01);
   tmpStyle->SetDateY(0.01);
   tmpStyle->SetEndErrorSize(2);
   tmpStyle->SetErrorX(0.5);
   tmpStyle->SetFuncColor(2);
   tmpStyle->SetFuncStyle(1);
   tmpStyle->SetFuncWidth(3);
   tmpStyle->SetGridColor(15);
   tmpStyle->SetGridStyle(3);
   tmpStyle->SetGridWidth(1);
   tmpStyle->SetLegendBorderSize(1);
   tmpStyle->SetHatchesLineWidth(1);
   tmpStyle->SetHatchesSpacing(1);
   tmpStyle->SetFrameFillColor(10);
   tmpStyle->SetFrameLineColor(1);
   tmpStyle->SetFrameFillStyle(1001);
   tmpStyle->SetFrameLineStyle(1);
   tmpStyle->SetFrameLineWidth(1);
   tmpStyle->SetFrameBorderSize(1);
   tmpStyle->SetFrameBorderMode(0);
   tmpStyle->SetHistFillColor(0);
   tmpStyle->SetHistLineColor(1);
   tmpStyle->SetHistFillStyle(0);
   //tmpStyle->SetHistFillStyle(4000);
   tmpStyle->SetHistLineStyle(1);
   tmpStyle->SetHistLineWidth(2);
   tmpStyle->SetHistMinimumZero(kFALSE);
   tmpStyle->SetCanvasPreferGL(kFALSE);
   tmpStyle->SetCanvasColor(10);
   tmpStyle->SetCanvasBorderSize(2);
   tmpStyle->SetCanvasBorderMode(0);
   tmpStyle->SetCanvasDefH(500);
   tmpStyle->SetCanvasDefW(700);
   tmpStyle->SetCanvasDefX(10);
   tmpStyle->SetCanvasDefY(10);
   tmpStyle->SetPadColor(10);
   tmpStyle->SetPadBorderSize(2);
   tmpStyle->SetPadBorderMode(0);
   tmpStyle->SetPadBottomMargin(0.13);
   tmpStyle->SetPadTopMargin(0.06);
   tmpStyle->SetPadLeftMargin(0.14);
   tmpStyle->SetPadRightMargin(0.03);
   tmpStyle->SetPadGridX(kTRUE);
   tmpStyle->SetPadGridY(kTRUE);
   tmpStyle->SetPadTickX(0);
   tmpStyle->SetPadTickY(0);
   tmpStyle->SetPaperSize(20, 26);
   tmpStyle->SetScreenFactor(1);
   tmpStyle->SetStatColor(0);
   tmpStyle->SetStatTextColor(1);
   tmpStyle->SetStatBorderSize(1);
   //tmpStyle->SetStatFont(42);
   tmpStyle->SetStatFont(132);
   tmpStyle->SetStatFontSize(0);
   tmpStyle->SetStatStyle(1001);
   tmpStyle->SetStatFormat("6.4g");
   tmpStyle->SetStatX(0.98);
   tmpStyle->SetStatY(0.995);
   tmpStyle->SetStatW(0.2);
   tmpStyle->SetStatH(0.15);
   tmpStyle->SetStripDecimals(kFALSE);
   tmpStyle->SetTitleAlign(13);
   tmpStyle->SetTitleFillColor(0);
   tmpStyle->SetTitleTextColor(1);
   tmpStyle->SetTitleBorderSize(0);
   tmpStyle->SetTitleFont(132);
   tmpStyle->SetTitleFontSize(0);
   tmpStyle->SetTitleStyle(1001);
   tmpStyle->SetTitleX(0.34);
   tmpStyle->SetTitleY(0.98);
   tmpStyle->SetTitleW(0);
   tmpStyle->SetTitleH(0);
   tmpStyle->SetLegoInnerR(0.5);
/*   Int_t fPaletteColor[8] = {19, 18, 17, 16, 15, 14, 13, 12};
   tmpStyle->SetPalette(8, fPaletteColor);*/
   Int_t fPaletteColor[50] = {19, 18, 17, 16, 15, 14, 13, 12, 11, 
                             20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
                             30, 8, 31, 32, 33, 34, 35, 36, 37, 38, 
                             39, 40, 9, 41, 42, 43, 44, 45, 47, 48, 
                             49, 46, 50, 2, 7, 6, 5, 4, 3, 112, 1};
   tmpStyle->SetPalette(50, fPaletteColor);

   TString fLineStyleArrayTmp[30] = {"", "  ", " 12 12", " 4 8", 
                             " 12 16 4 16", " 20 12 4 12", " 20 12 4 12 4 12 4 12", " 20 20", " 20 12 4 12 4 12", 
                             " 80 20", " 80 40 4 40", "  ", "  ", "  ", 
                             "  ", "  ", "  ", "  ", "  ", 
                             "  ", "  ", "  ", "  ", "  ", 
                             "  ", "  ", "  ", "  ", "  ", "  "};
   for (Int_t i=0; i<30; i++)
      tmpStyle->SetLineStyleString(i, fLineStyleArrayTmp[i]);

   tmpStyle->SetHeaderPS("");
   tmpStyle->SetTitlePS("");
   tmpStyle->SetFitFormat("5.4g");
   tmpStyle->SetPaintTextFormat("g");
   tmpStyle->SetLineScalePS(2);
   tmpStyle->SetColorModelPS(0);
   tmpStyle->SetTimeOffset(788918400);

   tmpStyle->SetLineColor(1);
   tmpStyle->SetLineStyle(1);
   tmpStyle->SetLineWidth(1);
   tmpStyle->SetFillColor(19);
   tmpStyle->SetFillStyle(1001);
   //tmpStyle->SetFillStyle(4000);
   tmpStyle->SetMarkerColor(1);
   tmpStyle->SetMarkerSize(1.);
   tmpStyle->SetMarkerStyle(1);
   tmpStyle->SetTextAlign(11);
   tmpStyle->SetTextAngle(0);
   tmpStyle->SetTextColor(1);
   tmpStyle->SetTextFont(132);
   tmpStyle->SetTextSize(0.05);
}
