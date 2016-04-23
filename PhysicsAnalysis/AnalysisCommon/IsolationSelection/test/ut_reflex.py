#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Set up ROOT and RootCore:
import ROOT

ROOT.gROOT.Macro('$ROOTCOREDIR/scripts/load_packages.C')

t1 = ROOT.CP.IsolationSelectionTool('test')
