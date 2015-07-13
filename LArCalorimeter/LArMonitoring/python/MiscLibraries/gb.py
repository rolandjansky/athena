# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
import math
from array import array

# global graphic settings
ROOT.gROOT.LoadMacro("../MiscLibraries/AtlasStyle.C")
SetAtlasStyle()
gStyle.SetOptFit(00111)
gStyle.SetPalette(1)
gStyle.SetOptStat("emrou")
#gStyle.SetOptStat("emr")
gStyle.SetOptTitle(1)
gStyle.SetPadRightMargin(0.15);

def MakeLegend(xmin,ymin,xmax,ymax):
    l = TLegend(xmin,ymin,xmax,ymax)
    l.SetFillColor(kWhite)
    l.SetTextSize(0.03)
    l.SetBorderSize(0)
    return l

def SetXLabel(h,list):
    for i in xrange(len(list)):
         h.GetXaxis().SetBinLabel(i+1,str(list[i]))
    return

def SetYLabel(h,list):
    for i in xrange(len(list)):
         h.GetYaxis().SetBinLabel(i+1,str(list[i]))
    return

def MakeTProfile(name,xtitle,ytitle,xmin,xmax,nbins,color):
   h = TProfile(name,name, nbins, xmin, xmax)
   h.SetYTitle(ytitle)
   h.SetXTitle(xtitle)
   h.SetMarkerStyle(22)
   h.SetMarkerColor(color)
   h.SetLineColor(1)
   h.SetFillColor(color)
   h.SetStats(0)
   h.SetTitle("")
   return h

def MakeTH1(name,xtitle,ytitle,xmin,xmax,nbins,color):
   h = TH1F(name,name,nbins, xmin,xmax)
   #h.SetStats(0)
   #h.GetXaxis().SetTitleOffset(-3.)
   #h.GetYaxis().SetTitleOffset(-3.)
   h.SetYTitle(ytitle)
   h.SetXTitle(xtitle)
   h.SetLineColor(1)
   h.SetMarkerStyle(22)
   h.SetMarkerColor(color)
   h.SetFillColor(color)
   #h.SetTitle("")
   h.Sumw2()
   return h

def MakeTH1VarBin(name,xtitle,ytitle,nbins,xval,color):
   h = TH1F(name,name,nbins, xval)
   #h.SetStats(0)
   #h.GetXaxis().SetTitleOffset(-3.)
   #h.GetYaxis().SetTitleOffset(-3.)
   h.SetYTitle(ytitle)
   h.SetXTitle(xtitle)
   h.SetLineColor(1)
   h.SetMarkerStyle(22)
   h.SetMarkerColor(color)
   h.SetFillColor(color)
   #h.SetTitle("")
   h.Sumw2()
   return h

def MakeTH2(name,xtitle,ytitle,xmin,xmax,nbinx,ymin,ymax,nbiny):
   h = TH2F(name,name,nbinx, xmin,xmax,nbiny,ymin,ymax)
   h.SetYTitle(ytitle)
   h.SetXTitle(xtitle)
   h.SetMarkerStyle(22)
   h.SetTitle("")
   #h.SetStats(0)
   return h

def MakeFrame(title,xtitle,ytitle,xmin,xmax,nbins):
   h = TH1F(title,title,nbins,xmin,xmax)
   h.SetStats(0)
   h.SetYTitle(ytitle)
   h.SetXTitle(xtitle)
   return h

def MakeEffPlot(title,xtitle,ytitle,xmin,xmax,nbins,color):
    h=TH1F(title,title,nbins,xmin,xmax)
    h.SetStats(0)
    h.SetYTitle(ytitle)
    h.SetXTitle(xtitle)
    h0=TGraphAsymmErrors(h)
    return h0
        
def Normalize(h,value):
    if value != 0:
        scale = float(value)/h.Integral()
        h.Scale(scale)
    return

def TransMass(ptele,phiele,exmiss,eymiss):
    phimiss = math.atan2(eymiss, exmiss)
    etmiss = math.sqrt(exmiss*exmiss+eymiss*eymiss)
    tm = math.sqrt(2*ptele*etmiss*(1-cos(phiele-phimiss)))
    return tm

def dr(eta1,phi1,eta2,phi2):
    deta=eta1-eta2
    dphi=phi1-phi2
    if dphi > math.pi:
        dphi=fabs(dphi)-math.pi
    dist=deta*deta+dphi*dphi
    return math.sqrt(dist)

# fancy Root palette found on: http://ultrahigh.org/2007/08/20/making-pretty-root-color-palettes/
def set_palette(name="palette", ncontours=999):
    """Set a color palette from a given RGB list
    stops, red, green and blue should all be lists of the same length
    see set_decent_colors for an example"""

    if name == "gray":
        stops = [0.00, 0.34, 0.61, 0.84, 1.00]
        red   = [1.00, 0.84, 0.61, 0.34, 0.00]
        green = [1.00, 0.84, 0.61, 0.34, 0.00]
        blue  = [1.00, 0.84, 0.61, 0.34, 0.00]
    # elif name == "whatever":
        # (define more palettes)
    else:
        # default palette, looks cool
        stops = [0.00, 0.34, 0.61, 0.84, 1.00]
        red   = [0.00, 0.00, 0.87, 1.00, 0.51]
        green = [0.00, 0.81, 1.00, 0.20, 0.00]
        blue  = [0.51, 1.00, 0.12, 0.00, 0.00]

    s = array('d', stops)
    r = array('d', red)
    g = array('d', green)
    b = array('d', blue)

    npoints = len(s)
    TColor.CreateGradientColorTable(npoints, s, r, g, b, ncontours)
    gStyle.SetNumberContours(ncontours) # set to 999 for an awesome result
