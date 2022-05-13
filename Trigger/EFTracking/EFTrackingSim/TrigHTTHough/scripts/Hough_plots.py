# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python

'''
@file Hough_plots.py
@author Riley Xu - riley.xu@cern.ch
@date Nov 1st, 2020
@brief Dumps images from a Hough transform monitoring file

Usage: Hough_plots.py loghits.root
'''

import sys
sys.argv.append('-b')

import ROOT

def main(filepath):
    ROOT.gStyle.SetOptStat(0)
    f = ROOT.TFile(filepath)
    for key in f.Get("Hough/Images").GetListOfKeys():
        if key.GetClassName() == "TH2I":
            c = ROOT.TCanvas("c1","c1",800,600)
            h = key.ReadObj()
            h.Draw("COLZ")

            # Draw boxes around fired roads.
            # Hough monitoring tool uses the bin error to indicate if a road was found.
            # Bin errors are sqrt(n), so anything > sqrt(n_layers) is safe to use.
            # Error is set to 100 + nLayers_missed.
            boxes = [] # need to store so they don't get garbage collected
            for x in range(1, h.GetNbinsX() + 1):
                for y in range(1, h.GetNbinsY() + 1):
                    if h.GetBinError(x, y) >= 100:
                        b = ROOT.TBox(h.GetXaxis().GetBinLowEdge(x),
                                 h.GetYaxis().GetBinLowEdge(y),
                                 h.GetXaxis().GetBinWidth(x) + h.GetXaxis().GetBinLowEdge(x),
                                 h.GetYaxis().GetBinWidth(y) + h.GetYaxis().GetBinLowEdge(y))
                        b.SetFillColor(ROOT.kRed + int(h.GetBinError(x, y)) - 100)
                        b.Draw()
                        boxes.append(b)

            c.Print(h.GetName() + ".png")


if __name__ == "__main__":
    main(sys.argv[1])
