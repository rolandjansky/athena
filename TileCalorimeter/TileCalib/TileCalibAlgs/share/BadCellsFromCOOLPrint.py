#============================================================
#=== graphic tuning of the histograms
#============================================================

from ROOT import *
from array import array
gStyle.SetOptStat(0)
gStyle.SetFrameBorderMode(0)
gStyle.SetCanvasBorderMode(0)
gStyle.SetPadBorderMode(0)
gStyle.SetPadColor(0)
gStyle.SetCanvasColor(0)
gStyle.SetStatColor(0)
gStyle.SetFrameFillColor(10)
gStyle.SetTitleFillColor(10)
gStyle.SetFillColor(10)
def set_palette(ncontours=50):
    """Set a color palette from a given RGB list
    stops, red, green and blue should all be lists of the same length
    see set_decent_colors for an example"""

    stops = [0.00, 0.25, 0.75, 1.00]
    red   = [0.00, 1.00, 1.00, 1.00]
    green = [1.00, 1.00, 0.70, 0.00]
    blue  = [0.00, 0.00, 0.00, 0.00]

    s = array('d', stops)
    r = array('d', red)
    g = array('d', green)
    b = array('d', blue)

    npoints = len(s)
    TColor.CreateGradientColorTable(npoints, s, r, g, b, ncontours)
    gStyle.SetNumberContours(ncontours)


f = TFile('tileBadCells.root')
set_palette()
c1 = TCanvas("c1","c1",0,0,700,600)
for cell in ['','A','BC','D','Gap']:
    c1.SetGridx(1)
    c1.SetGridy(1)
    c1.SetTickx(1)
    c1.SetTicky(1)
    h = f.Get('m_h_badCell'+cell)
    h.SetMaximum(3)
    h.Draw('colz')
    c1.SetFrameFillColor(kGreen)
    h.SetXTitle("#eta")
    h.SetYTitle("#phi")
    h.GetXaxis().SetTitleOffset(0.8)
    h.GetYaxis().SetTitleOffset(0.8)

    c1.Update()
    c1.SaveAs("tileBadCells%s.png" % cell)
