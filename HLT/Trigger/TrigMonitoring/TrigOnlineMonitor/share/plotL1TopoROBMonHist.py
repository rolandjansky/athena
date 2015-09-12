#!/bin/env python
import sys
try:
    rootFile=sys.argv[1]
except IndexError:
    rootFile="expert-monitoring.root"
print "Reading from %s" % rootFile

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gROOT.SetStyle("Plain")
ROOT.gStyle.SetOptStat(111111)
canv=ROOT.TCanvas("canv","canv", 800, 500)
pdfFile='L1TopoROBMonitorHist.pdf'
canv.Print(pdfFile+'[')

tfile = ROOT.TFile(rootFile)
tdir=tfile.GetDirectory('L1TopoROBMonitor')
keys=[key for key in tdir.GetListOfKeys()]
for k in keys:
    hist=k.ReadObj()
    canv.Clear()
    canv.SetName(hist.GetName())
    canv.SetTitle(hist.GetTitle())
    if 'payload size' in canv.GetName():
        canv.SetLogy(1)
    else:
        canv.SetLogy(0)
    hist.Draw()
    canv.Update()
    canv.Print(pdfFile)
canv.Print(pdfFile+']')
