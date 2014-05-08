# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: style.py,v 1.1 2005-05-11 19:47:29 ssnyder Exp $
# File: style.py
# Created: sss, 2004.
# Purpose: Centralize style settings for root plots.
#

"""Centralize style settings for root plots.
"""

import ROOT

def style():
    """Make a standard set of Root plot style settings."""
    s = ROOT.gStyle

    # Graphics style parameters to avoid gray background on figures
    s.SetCanvasColor(10)
    s.SetStatColor(10)
    #s.SetTitleColor(10)
    s.SetPadColor(10)
    s.SetPaperSize(20,24)
    s.SetStatFont(42)
    s.SetOptStat (1110)

    # title size
    s.SetOptFit(0111)
    s.SetTitleFont(42)
    s.SetTitleH(0.09)
    s.SetTitleX(0.1)

    # Turn on date stamp.
    s.SetOptDate (1)

    # histogram style
    s.SetHistLineWidth (3)
    s.SetHistLineColor (1)
    s.SetFrameLineWidth (3)
    s.SetFrameLineColor (1)
    s.SetLabelSize(0.05, "xyz")
    s.SetTitleOffset(1.2, "xyz")

    # root Defaults
    s.SetPadLeftMargin (0.1)
    s.SetErrorX (0.5)

    return


