#!/usr/bin/env python

"""
@file HTTPlotHits.py
@author Riley Xu - riley.xu@cern.ch
@date April 8th, 2021
@brief Plots the hits from a specific event

HTTPlotHits.py input_wrapper event
"""

import sys
sys.argv.append('-b') # Batch mode

import ROOT
ROOT.gStyle.SetOptStat(00000000)
ROOT.gROOT.ForceStyle();

import numpy as np

def plot(filename, event):
    print("Plotting event " + str(event) + " from file " + filename)

    f = ROOT.TFile(sys.argv[1], "READ")
    c = ROOT.TCanvas("c1","c1",1200,900)
    c.Divide(2,2)

    t = f.Get("HTTEventTree")
    if event >= t.GetEntries():
        raise IndexError("Bad entry " + str(event))
    t.GetEntry(event)

    hits = t.HTTEventInputHeader.hits()
    nhits = len(hits)
    coords = np.empty((nhits, 3))
    colors = np.empty(nhits, dtype=int)
    styles = np.empty(nhits, dtype=int)
    for i in range(nhits):
        h = hits[i]
        coords[i] = (h.getX(), h.getY(), h.getZ())
        colors[i] = 1 + h.getLayerDisk() % 9
        if colors[i] == 5:
            colors[i] = 28 # yellow -> brown
        styles[i] = ROOT.kCircle if h.getSide() else ROOT.kMultiply

    rs = np.sqrt(np.power(coords[:, 0], 2) + np.power(coords[:, 1], 2))

    c.cd(1)
    g = ROOT.TGraph(nhits, coords[:, 2].astype("float"), rs)
    g.SetTitle("r vs z")
    g.GetXaxis().SetTitle("z (mm)")
    g.GetYaxis().SetTitle("r (mm)")
    g.SetMarkerColor(0)
    g.Draw("AP")

    g._markers = []
    for i in range(nhits):
        x = ROOT.Double()
        y = ROOT.Double()
        g.GetPoint(i, x, y)
        m = ROOT.TMarker(x, y, styles[i])
        m.SetMarkerColor(colors[i])
        m.Draw()
        g._markers.append(m) # otherwise python will garbage collect the marker

    c.cd(2)
    g2 = ROOT.TGraph(nhits, coords[:, 0].astype("float"), coords[:, 1].astype("float"))
    g2.SetTitle("y vs x")
    g2.GetXaxis().SetTitle("x (mm)")
    g2.GetYaxis().SetTitle("y (mm)")
    g2.SetMarkerColor(0)
    g2.Draw("AP")

    g2._markers = []
    for i in range(nhits):
        x = ROOT.Double()
        y = ROOT.Double()
        g2.GetPoint(i, x, y)
        m = ROOT.TMarker(x, y, styles[i])
        m.SetMarkerColor(colors[i])
        m.Draw()
        g2._markers.append(m) # otherwise python will garbage collect the marker


    offline = t.HTTEventInputHeader.optional().getOfflineClusters()
    noff = len(offline)
    off_coords = np.empty((noff, 3))
    off_colors = np.empty(noff, dtype=int)
    off_styles = np.empty(noff, dtype=int)
    for i in range(noff):
        h = offline[i].getClusterEquiv()
        off_coords[i] = (h.getX(), h.getY(), h.getZ())
        off_colors[i] = 1 + h.getLayerDisk() % 9
        if off_colors[i] == 5:
            off_colors[i] = 28 # yellow -> brown
        off_styles[i] = ROOT.kCircle if h.getSide() else ROOT.kMultiply

    off_rs = np.sqrt(np.power(off_coords[:, 0], 2) + np.power(off_coords[:, 1], 2))


    c.cd(3)
    off_g = ROOT.TGraph(noff, off_coords[:, 2].astype("float"), off_rs)
    off_g.SetTitle("offline r vs z")
    off_g.GetXaxis().SetTitle("z (mm)")
    off_g.GetYaxis().SetTitle("r (mm)")
    off_g.SetMarkerColor(0)
    off_g.Draw("AP")

    off_g._markers = []
    for i in range(noff):
        x = ROOT.Double()
        y = ROOT.Double()
        off_g.GetPoint(i, x, y)
        m = ROOT.TMarker(x, y, off_styles[i])
        m.SetMarkerColor(off_colors[i])
        m.Draw()
        off_g._markers.append(m) # otherwise python will garbage collect the marker

    c.cd(4)
    off_g2 = ROOT.TGraph(noff, off_coords[:, 0].astype("float"), off_coords[:, 1].astype("float"))
    off_g2.SetTitle("offline y vs x")
    off_g2.GetXaxis().SetTitle("x (mm)")
    off_g2.GetYaxis().SetTitle("y (mm)")
    off_g2.SetMarkerColor(0)
    off_g2.Draw("AP")

    off_g2._markers = []
    for i in range(noff):
        x = ROOT.Double()
        y = ROOT.Double()
        off_g2.GetPoint(i, x, y)
        m = ROOT.TMarker(x, y, off_styles[i])
        m.SetMarkerColor(off_colors[i])
        m.Draw()
        off_g2._markers.append(m) # otherwise python will garbage collect the marker

    c.Print("Event_" + str(event) + ".png")

if __name__ == "__main__":
    plot(sys.argv[1], int(sys.argv[2]))
