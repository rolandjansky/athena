# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: rebin2.py,v 1.1 2005-05-09 17:15:59 ssnyder Exp $
# File: rebin2.py
# Created: sss, 2004.
# Purpose: Rebin a 2D histogram.
#

import ROOT


def rebin2 (h, name, gx=1, gy=1):
    """Rebin the 2D histogram H.
Use NAME for the new histogram.
Group together GX bins in x and GY bins in y.
"""
    old_nx = h.GetNbinsX()
    old_ny = h.GetNbinsY()
    new_nx = old_nx//gx
    new_ny = old_ny//gy
    hnew = ROOT.TH2F (name,
                      h.GetTitle(),
                      new_nx,
                      h.GetXaxis().GetXmin(),
                      h.GetXaxis().GetXmax(),
                      new_ny,
                      h.GetYaxis().GetXmin(),
                      h.GetYaxis().GetXmax())
    for ix in range(0, new_nx):
        for iy in range(0, new_ny):
            sum = 0
            for iix in range(0, gx):
                for iiy in range(0, gy):
                    sum += h.GetBinContent(ix*gx+iix+1, iy*gy+iiy+1)
            hnew.SetBinContent(ix+1, iy+1, sum)
    return hnew

