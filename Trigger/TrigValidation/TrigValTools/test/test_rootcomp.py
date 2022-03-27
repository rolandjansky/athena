#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Create two ROOT files with histograms used in test_rootcomp.sh

from ROOT import TFile, TH1D

# Histogram for both files:
h1 = TH1D("h1", "", 10, 0, 10)
h1.Fill(5)

# First file:
f = TFile("hist1.root", "RECREATE")
h1.Write()

hr = TH1D("hrandom", "", 10, 0, 10)
hr.FillRandom("gaus", 1000)

hl = TH1D("hlabel", "", 5, 0, 5)
hl.Fill("a", 1)
hl.Fill("b", 2)

h2 = TH1D("hextra", "", 10, 0, 10)
f.Write()

# Second file:
f2 = TFile("hist2.root", "RECREATE")
h1.Write()

hr = TH1D("hrandom", "", 10, 0, 10)
hr.FillRandom("gaus", 1000)

hl = TH1D("hlabel", "", 4, 0, 4)   # one less (empty) bin than above
hl.Fill("b", 2)
hl.Fill("a", 1)
f2.Write()
