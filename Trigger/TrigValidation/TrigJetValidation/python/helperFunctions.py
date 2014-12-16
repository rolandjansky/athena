#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from math import sqrt, ceil, log10

from ROOT import *

def myLatex(x, y, text, tsize = 0.05):
  a = TLatex()
  #a.SetTextAlign(align)
  a.SetTextSize(tsize)
  a.SetTextColor(1)
  a.SetTextFont(42)
  a.SetNDC()
  a.DrawLatex(x, y, text)

def ATLASLabel(x, y, addon = '', tsize = 0.05, col = 1):
  l = TLatex() #l.SetTextAlign(12); l.SetTextSize(tsize); 
  #width = [ [0.05, 0.04], [0.08, 0.10] ]
  l.SetNDC()
  l.SetTextSize( tsize )
  l.SetTextFont(72)
  l.SetTextColor(col)
  l.DrawLatex(x, y, 'ATLAS')
  if addon:
    p = TLatex()
    p.SetNDC()
    p.SetTextSize( tsize )
    p.SetTextFont(42)
    p.SetTextColor(col)
    p.DrawLatex(x + 0.11, y, addon)

def getArea(hist):  return sum([ hist.GetBinContent(i+1) for i in range(hist.GetNbinsX()) ])

def normalise(hist):
   area = sum([ hist.GetBinContent(i+1) for i in range(hist.GetNbinsX()) ])
   if area:
     hist.Scale( 1.0 / area )
     for i in range(1, hist.GetNbinsX()+1):
       hist.SetBinError(i, hist.GetBinError(i)/area)
   return hist

def smallestWidth(iterable):  return  min(iterable[i+1] - iterable[i] for i in range(len(iterable)-1))

def scaleBinWidth(hist, factor = -1.0):
   if factor < 0.0:  factor = hist.GetBinWidth(1)
   for i in xrange(hist.GetNbinsX() + 1):
      error = (hist.GetBinError(i) / hist.GetBinContent(i)) if hist.GetBinContent(i) else 0.0
      hist.SetBinContent(i, factor * hist.GetBinContent(i) / hist.GetBinWidth(i))
      hist.SetBinError(i,   error  * hist.GetBinContent(i))

def myLegend(x1, y1, x2, y2, tsize = 0.04):
  leg = TLegend(x1, y1, x2, y2)
  leg.SetTextColor(1)
  leg.SetTextFont(42)
  leg.SetTextSize(tsize)
  leg.SetBorderSize(0)
  leg.SetFillColor(0)
  return leg

def setupCurve(hist, col, lStyle = 1):
  hist.SetLineColor(col)
  hist.SetMarkerSize(0)
  hist.SetLineStyle(lStyle)


