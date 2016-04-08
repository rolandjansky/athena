#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Dumps the beam spot parameters to a text (ascii) file
"""
__author__  = 'Peter Loscutoff'
__version__ = '$Id $'
__usage__   = 'plotBeamSpotVxVal.py [options] bs.nt.root split.nt.root'

import sys, os
import time
from math import sqrt

# Argument parsing
from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-b', '--batch', dest='batch', action='store_true', default=False, help='run in batch mode')
parser.add_option('-o', '--output', dest='output', default='.gif', help='comma-separated list of output files or formats')
parser.add_option('-H', '--test', dest='test', default=False, help='a test variable (NYI)')
parser.add_option('', '--time', dest='time', default=False, help='use time instead of lumiblocks as an axis (NYI)')
parser.add_option('-t', '--tracks', dest='plotrange', default=100, help='maximum number of tracks/vertex to plot')

(options,args) = parser.parse_args()
if len(args) < 2:
    parser.error('wrong number of command line arguments')

# Setup ROOT
if options.batch:
    os.unsetenv('DISPLAY')

import math
from math import sqrt,sin,cos,floor,fabs
from ROOT import TFile,TH1D,gDirectory,TTree,TCanvas,TLegend,gStyle,gROOT,TMatrixDSym,gPad
from InDetBeamSpotExample import ROOTUtils
from InDetBeamSpotExample.BeamSpotData import *
import time, calendar
from array import array

canvas = TCanvas("PlotBsVxVal","PlotBsVxVal",1000,500)
canvas.SetFillColor(0)
canvas.Divide(2,1)

bsfile = TFile(args[0])
splitfile = TFile(args[1])

# Define our tools to get the time and fill number:
#from COOLUtils import RunTimeQuery
#c = RunTimeQuery()
#from CoolConvUtilities import AtlCoolTool
#tool = AtlCoolTool.AtlCoolTool("COOLOFL_DCS/COMP200")

subplotSVs = {}
ROOTUtils.setStyle()

fitbins = 3001
plotrange = int(options.plotrange) + 1
outlierCut = 20
plotSV = TH1D("p(s)SV","Pull vs # Tracks",plotrange*2,-.25,plotrange-.25) 

for i in xrange(0,plotrange*2):
    nameSV = "plotSV%s" % i
    subplotSVs[nameSV]=TH1D(nameSV,nameSV,fitbins,-1*outlierCut,outlierCut) 

splitfile.cd()
tree = gDirectory.Get("splitVertex")
entries = tree.GetEntriesFast()
for entry in xrange(entries):
    noprint = tree.GetEntry(entry)
    binSV = (tree.tracks_odd+tree.tracks_even)
    nameSV = "plotSV%i" % binSV
    if (binSV < (plotrange-1)*2 and tree.tracks_odd >= 4 and tree.tracks_even >= 4):
        subplotSVs[nameSV].Fill( (tree.x_odd - tree.x_even)/sqrt(tree.c00_odd+tree.c00_even) )

for i in xrange(0,plotrange*2):
    nameSV = "plotSV%s" % i
    if subplotSVs[nameSV].Integral() > 2:
        fitsigma = subplotSVs[nameSV].GetRMS()
        fserror = (subplotSVs[nameSV].GetRMS())/sqrt(subplotSVs[nameSV].Integral())
        bin = i+1
        plotSV.SetBinContent(bin,fitsigma)
        plotSV.SetBinError(bin,fserror)

canvas.cd(1)
gPad.SetLeftMargin(.15)
gPad.SetBottomMargin(.15)

plotSV.SetXTitle("Average # Tracks in Split Vertices")
plotSV.SetYTitle("Vertex Error Scale Factor")
#plotSV.SetMarkerColor(4)
#plotSV.SetMarkerStyle(26)
plotSV.SetMarkerStyle(1)
plotSV.Draw("e")

legend = TLegend(.60,.75,.92,.92)
legend.AddEntry(plotSV,"K Factor from Splitting")
legend.SetFillColor(0)
legend.SetBorderSize(0)

legend.Draw()

subplots = {}

bstable = {}

splitWeight = []
fullWeight = []

lumiedge = array('d')
lumibins = 0
lumiend = 0

bsfile.cd()
if gDirectory.Get("BeamSpotNt"):
    bstree = gDirectory.Get("BeamSpotNt")
elif gDirectory.Get("Beamspot/Beamspots"):
    bstree = gDirectory.Get("Beamspot/Beamspots")
entries = bstree.GetEntriesFast()
for i in xrange(entries):
    noprint = bstree.GetEntry(i)
    if bstree.k != 0:
        lumibegin = bstree.lbStart
        lumiedge.append(lumibegin)
        lumiend = bstree.lbEnd
        name = str(lumibegin) + "to" + str(lumiend)
        lumibins = lumibins + 1
        subplots[name]=TH1D(name,name,fitbins,-100,100)
        for lb in xrange(lumibegin,lumiend+1):
            bstable[lb] = {}
            bstable[lb]["x0"] = bstree.x0
            bstable[lb]["y0"] = bstree.y0
            bstable[lb]["ax"] = bstree.ax
            bstable[lb]["ay"] = bstree.ay
            bstable[lb]["z"] = bstree.z
            bstable[lb]["sx"] = bstree.sx
            bstable[lb]["sy"] = bstree.sy
            bstable[lb]["sz"] = bstree.sz
            bstable[lb]["rhoxy"] = bstree.rhoxy
lumiedge.append(lumiend)

plotX = TH1D("p(s)X","k vs Lumi Block",lumibins,lumiedge) 
plotBS = TH1D("p(s)bs","k vs Lumi Block",lumibins,lumiedge) 

for i in xrange(0,21):
    splitWeight.append(0.0)
    fullWeight.append(0.0)

splitfile.cd()
sptree = gDirectory.Get("splitVertex")
entries = sptree.GetEntriesFast()
for entry in xrange(entries):
    noprint = sptree.GetEntry(entry)
    splitBin = int(.5*(sptree.tracks_odd + sptree.tracks_even) )
    if splitBin < 11: splitWeight[splitBin] = splitWeight[splitBin] + 1.0
    else: splitWeight[11] = splitWeight[11] + 1.0

bsfile.cd()
if gDirectory.Get('Vertices'):
    vxtree = gDirectory.Get('Vertices')
elif gDirectory.Get('Beamspot/Vertices'):
    vxtree = gDirectory.Get('Beamspot/Vertices')
entries = vxtree.GetEntriesFast()
for entry in xrange(entries):
    noprint = vxtree.GetEntry(entry)
    fullBin = vxtree.nTracks
    if fullBin < 11: fullWeight[fullBin] = fullWeight[fullBin] + 1.0
    else: fullWeight[11] = fullWeight[11] + 1.0

for i in xrange(0,len(splitWeight)):
    if splitWeight[i] != 0 and fullWeight[i] != 0:
        splitWeight[i] = fullWeight[i]/splitWeight[i]

splitfile.cd()
sptree = gDirectory.Get("splitVertex")
entries = sptree.GetEntriesFast()
for entry in xrange(entries):
    noprint = sptree.GetEntry(entry)
    lumi = sptree.lumi
    if not lumi in bstable.keys(): continue
    bs_x0 = bstable[lumi]["x0"]
    bs_y0 = bstable[lumi]["y0"]
    bs_a = bstable[lumi]["ax"]
    bs_b = bstable[lumi]["ay"]
    bs_z = bstable[lumi]["z"]
    bs_sx = bstable[lumi]["sx"]
    bs_sy = bstable[lumi]["sy"]
    bs_sz = bstable[lumi]["sz"]
    bs_rho = bstable[lumi]["rhoxy"]
		    
    name = ""
    for k in subplots.keys():
        range = k.split("to")
        if  lumi >= int(range[0]) and lumi <= int(range[1]): name = k
    if name == "": continue
    splitBin = int(.5*(sptree.tracks_odd + sptree.tracks_even) )
    if (sptree.tracks_odd >= 4 and sptree.tracks_even >= 4 ):
        
        cov = array('d')
        cov.append(bs_sx*bs_sx + sptree.c00_odd)
        cov.append(bs_sx*bs_sy*bs_rho + sptree.c01_odd)
        cov.append(bs_sx*bs_sy*bs_rho + sptree.c01_odd)
        cov.append(bs_sy*bs_sy + sptree.c11_odd)
        covmat = TMatrixDSym(2)
        covmat.Use(2,cov)
        covmat.Invert()
        if (covmat(0,0)*(sptree.x_odd - (bs_x0 + sptree.z_odd*bs_a))**2 +
            covmat(1,1)*(sptree.y_odd - (bs_y0 + sptree.z_odd*bs_b))**2 +
            2*covmat(0,1)*(sptree.x_odd - (bs_x0 + sptree.z_odd*bs_a))*(sptree.y_odd - (bs_y0 + sptree.z_odd*bs_b))) > outlierCut:
            continue       
        cov = array('d')
        cov.append(bs_sx*bs_sx + sptree.c00_even)
        cov.append(bs_sx*bs_sy*bs_rho + sptree.c01_even)
        cov.append(bs_sx*bs_sy*bs_rho + sptree.c01_even)
        cov.append(bs_sy*bs_sy + sptree.c11_even)
        covmat = TMatrixDSym(2)
        covmat.Use(2,cov)
        covmat.Invert()
        if (covmat(0,0)*(sptree.x_even - (bs_x0 + sptree.z_even*bs_a))**2 +
            covmat(1,1)*(sptree.y_even - (bs_y0 + sptree.z_even*bs_b))**2 +
            2*covmat(0,1)*(sptree.x_even - (bs_x0 + sptree.z_even*bs_a))*(sptree.y_even - (bs_y0 + sptree.z_even*bs_b))) > outlierCut:
           continue

        if splitBin < 11:
            subplots[name].Fill( (sptree.x_odd - sptree.x_even)/sqrt(sptree.c00_odd+sptree.c00_even) , splitWeight[splitBin])
            subplots[name].Fill( (sptree.y_odd - sptree.y_even)/sqrt(sptree.c11_odd+sptree.c11_even) , splitWeight[splitBin])
        else:
            subplots[name].Fill( (sptree.x_odd - sptree.x_even)/sqrt(sptree.c00_odd+sptree.c00_even) , splitWeight[11])
            subplots[name].Fill( (sptree.y_odd - sptree.y_even)/sqrt(sptree.c11_odd+sptree.c11_even) , splitWeight[11])

min = .8
max = 1.2

sumw=0
sumwx=0

for name in subplots.keys():
    if subplots[name].Integral() > 100:
        midpoint = int(name.split("to")[0])+.5
        fitsigma = subplots[name].GetRMS()
        fserror = (subplots[name].GetRMS())/sqrt(subplots[name].Integral())
        if fitsigma > 0 and fserror > 0:
            sumw  += 1/fserror
            sumwx += fitsigma/fserror
        globalbin = plotBS.FindBin(midpoint)
        bin = plotBS.GetBin(globalbin)
        plotX.SetBinContent(bin,fitsigma)
        plotX.SetBinError(bin,fserror)
        if fitsigma*1.3 > max: max = fitsigma*1.3
        if fitsigma*.8 < min and fitsigma != 0: min = fitsigma*.8
        
vsavg = sumwx/sumw
vserr = 1.0/sumw

sumw=0
sumwx=0

bsfile.cd()
if gDirectory.Get('BeamSpotNt'):
    bstree = gDirecotry.Get('BeamSpotNt')
elif gDirectory.Get('Beamspot/Beamspots'):
    bstree = gDirectory.Get('Beamspot/Beamspots')
#bstree = gDirectory.Get("BeamSpotNt")
entries = bstree.GetEntriesFast()
for i in xrange(entries):
    noprint = bstree.GetEntry(i)
    fitsigma = bstree.k
    fserror = sqrt(bstree.kk)
    if fitsigma > 0 and fserror > 0:
        sumw  += 1/fserror
        sumwx += fitsigma/fserror
    midpoint = bstree.lbStart+.5
    globalbin = plotBS.FindBin(midpoint)
    bin = plotBS.GetBin(globalbin)
    plotBS.SetBinContent(bin,fitsigma)
    plotBS.SetBinError(bin,fserror)
    if fitsigma*1.3 > max: max = fitsigma*1.3
    if fitsigma*.8 < min and fitsigma != 0: min = fitsigma*.8

bsavg = sumwx/sumw
bserr = 1.0/sumw

print "Splitting: ",vsavg," +- ",vserr
print "Beam spot: ",bsavg," +- ",bserr

canvas.cd(2)

gPad.SetLeftMargin(.15)
gPad.SetBottomMargin(.15)
plotX.SetXTitle("Lumi Block")
plotX.SetYTitle("Vertex Error Scale Factor")
plotX.SetMaximum(max)
plotX.SetMinimum(min)
plotX.SetLineColor(33)
plotX.SetMarkerColor(33)
plotX.SetMarkerStyle(1)
plotX.SetFillColor(33)
plotX.Draw("e2")
plotBS.SetMarkerStyle(21)
plotBS.SetLineWidth(3)
plotBS.Draw("same,e")

legend = TLegend(.50,.70,.80,.85)
legend.AddEntry(plotBS,"k from beamspot")
legend.AddEntry(plotX,"k from vx split")
legend.SetFillColor(0)
legend.SetBorderSize(0)

legend.Draw()

if options.output:
    for o in options.output.split(','):
        if o[0]=='.':
            canvas.SaveAs(basename+what+o)
        else:
            canvas.SaveAs(o)
            
import os
os.environ['PYTHONINSPECT'] = '1'


