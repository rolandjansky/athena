#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# -*- coding: utf-8 -*-

from ROOT import TStyle

atlasStyle = TStyle("myStyle","myStyle");

atlasStyle.SetOptStat(0)
atlasStyle.SetPalette(1)

icol=0; #WHITE
atlasStyle.SetFrameBorderMode(icol);
atlasStyle.SetFrameFillColor(icol);
atlasStyle.SetCanvasBorderMode(icol);
atlasStyle.SetCanvasColor(icol);
atlasStyle.SetPadBorderMode(icol);
atlasStyle.SetPadColor(icol);
atlasStyle.SetStatColor(icol);
atlasStyle.SetTitleFillColor(icol);

#atlasStyle.SetPaperSize(20,26);
#atlasStyle.SetPadTopMargin(0.02);
#atlasStyle.SetPadRightMargin(0.10);
#atlasStyle.SetPadBottomMargin(0.10);
#atlasStyle.SetPadLeftMargin(0.14);

atlasStyle.SetPaperSize(20,26);
atlasStyle.SetPadTopMargin(0.05);
atlasStyle.SetPadRightMargin(0.05);
atlasStyle.SetPadBottomMargin(0.16);
atlasStyle.SetPadLeftMargin(0.16);

#Int_t font=72;
font=42
tsize=0.05
atlasStyle.SetTextFont(font);
atlasStyle.SetTextSize(tsize);

atlasStyle.SetLabelFont(font,"x");
atlasStyle.SetLabelFont(font,"y");
atlasStyle.SetLabelFont(font,"z");

atlasStyle.SetTitleFont(font,"x");
atlasStyle.SetTitleFont(font,"y");
atlasStyle.SetTitleFont(font,"z");
atlasStyle.SetTitleFont(font,"");

atlasStyle.SetLabelSize(tsize,"x");
atlasStyle.SetLabelSize(tsize,"y");
atlasStyle.SetLabelSize(tsize,"z");

atlasStyle.SetTitleSize(tsize,"x");
atlasStyle.SetTitleSize(tsize,"y");
atlasStyle.SetTitleSize(tsize,"z");

atlasStyle.SetTitleSize(0.05,"");
atlasStyle.SetTitleOffset(1.40,"");

atlasStyle.SetStatFont(font);

#atlasStyle->SetLabelOffset(0.05,"x");
#atlasStyle->SetLabelOffset(0.05,"y");
#atlasStyle->SetLabelOffset(0.05,"z");

#tlasStyle.SetTitleY(0.99);
#tlasStyle.SetTitleX(0.);

#atlasStyle->SetTitleOffset(1.5,"x");
#tlasStyle.SetTitleOffset(1.2,"y");
#tlasStyle.SetTitleOffset(1.2,"z");

#use bold lines and markers
atlasStyle.SetMarkerStyle(20);
atlasStyle.SetMarkerSize(1.2);
atlasStyle.SetHistLineWidth(1);
atlasStyle.SetLineStyleString(2,"[12 12]"); #postscript dashes

#get rid of X error bars 
#atlasStyle->SetErrorX(0.001);
#get rid of error bar caps
atlasStyle.SetEndErrorSize(0.);

#do not display any of the standard histogram decorations
atlasStyle.SetOptTitle(0);
#atlasStyle.SetOptStat(1111);
atlasStyle.SetOptStat(0);
#atlasStyle.SetOptFit(1111);
atlasStyle.SetOptFit(0);
# put tick marks on top and RHS of plots
atlasStyle.SetPadTickX(1);
atlasStyle.SetPadTickY(1);

#atlasStyle.SetOptStat("rm");

atlasStyle.cd();
