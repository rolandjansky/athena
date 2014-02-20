#==============================================================
# Runs the comparison configured in CompareGeometries.py
# Author: John Alison (johnda@hep.upenn.edu)
#
# Date: 11 Nov 2008
#==============================================================

# python setup
import sys
from math import sqrt, atan2

# ROOT setup
from ROOT import TFile, gROOT,gStyle, TH2F, TH1F, kRed, kGreen, kBlue, kYellow, TCanvas, gPad, TArrow, TText, TFile,TLine
include("InDetSimpleVisual/rootlogon.py")
gROOT.Reset()
gStyle.SetOptStat(0)
gStyle.SetMarkerStyle(7)

# Setup needed functions
include("InDetSimpleVisual/comparisonUtils.py")

# Output file
rootFile = TFile(outputFile,"RECREATE")

# Open the inputFiles
inputfile1 = open(inputFile1,"r")
inputfile2 = open(inputFile2,"r")

# Seting the ranges
# ==================
if not "deltaR" in dir():
    deltaR = 1.5
if not "deltaPhiTrans" in dir():
    deltaPhiTrans = 2
if not "deltaPhi" in dir():
    deltaPhi = 0.0008
if not "deltaX" in dir():
    deltaX = 2 
if not "deltaY" in dir():
    deltaY = 2
if not "deltaZ" in dir():
    deltaZ = 2
if not "deltaXY" in dir():
    deltaXY = 3
if not "TRASL_FACTOR" in dir():
    TRASL_FACTOR = 500
if not "drawPix" in dir():
    drawPix = True
if not "drawSCT" in dir():
    drawSCT = True
if not "drawTRT" in dir():
    drawTRT = True

# Setting the detailed TRT defaults
if not "drawTRTModules" in dir():
    drawTRTModules = []
    
# Getting the first geometry
geometry1 = readInData(inputfile1)
pixelElements1 = geometry1[0]
sctElements1 = geometry1[1]
trtElements1 = geometry1[2]

# Getting the second geometry
geometry2 = readInData(inputfile2)
pixelElements2 = geometry2[0]
sctElements2 = geometry2[1]
trtElements2 = geometry2[2]

# Setup the Histograms
include("InDetSimpleVisual/makeHists.py")
include("InDetSimpleVisual/makeTransCanvas.py")

# Pixels 
#=============
if drawPix:
    include("InDetSimpleVisual/fillPixelHists.py")
    
    pixelDir = rootFile.mkdir("Pixel misalignments")
    for i in range(len(pixelTransCan)):
        pixelDir.cd()
        pixelTransCan[i].Write()
    rootFile.cd() 

# SCT
#=============
if drawSCT:
    include("InDetSimpleVisual/fillSCTHists.py")
    
    sctDir = rootFile.mkdir("SCT misalignments")
    for i in range(len(sctTransCan)):
        sctDir.cd()
        sctTransCan[i].Write()
    rootFile.cd()

# TRT 
#=============
if drawTRT:
    include("InDetSimpleVisual/fillTRTHists.py")

    trtDir = rootFile.mkdir("TRT misalignments")
    for i in range(len(trtTransCan)):
        trtDir.cd()
        trtTransCan[i].Write()
        rootFile.cd()

    for pair in trtModTransCan:
        thisCanPair = trtModTransCan[pair]
        trtDir.cd()
        thisCanPair[0].Write()
        thisCanPair[1].Write()
        rootFile.cd()

    EA_strawPlanDir = trtDir.mkdir("Endcap A Straw Planes")
    for i in range(len(trtEndcapAStrawPlanes)):
        EA_strawPlanDir.cd()
        trtEndcapAStrawPlanes[i].Write()
        rootFile.cd()

    EC_strawPlanDir = trtDir.mkdir("Endcap C Straw Planes")
    for i in range(len(trtEndcapCStrawPlanes)):
        EC_strawPlanDir.cd()
        trtEndcapCStrawPlanes[i].Write()
        rootFile.cd()


include("InDetSimpleVisual/makeTRTBarrelCans.py")

# Write out the plots
# ====================
include("InDetSimpleVisual/writePlots.py")

# Exit
sys.exit()
