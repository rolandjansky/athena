# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
from math import fabs,exp,log,log10
import sys
import re
import os
import AtlasStyleMacro

gROOT.SetBatch(True)
AtlasStyleMacro.SetAtlasStyle()

def GetKeyNames(self,dir=""):
    self.cd(dir)
    return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames

def GetNameNoDir(self):
    return os.path.basename(self.GetName())

TFile.GetNameNoDir = GetNameNoDir

# Set the palette
def set_palette(name="palette", ncontours=999):
    #"""Set a color palette from a given RGB list
    #stops, red, green and blue should all be lists of the same length
    #see set_decent_colors for an example"""
    if name == "gray" or name == "grayscale":
        stops = [0.00, 0.34, 0.61, 0.84, 1.00]
        red   = [1.00, 0.84, 0.61, 0.34, 0.00]
        green = [1.00, 0.84, 0.61, 0.34, 0.00]
        blue  = [1.00, 0.84, 0.61, 0.34, 0.00]
    elif name == "Sophie":
        ncontours=18
        stops = [0.00, 0.50, 0.75, 1.00]
        red   = [1.00, 0.20, 0.00, 0.20]
        green = [0.00, 0.20, 1.00, 1.00]
        blue  = [1.00, 1.00, 1.00, 0.20]
    elif name == "SophieInverse":
        ncontours=18
        stops = [0.00, 0.50, 0.75, 1.00]
        red   = [0.20, 0.00, 0.20, 1.00]
        green = [1.00, 1.00, 0.20, 0.00]
        blue  = [0.20, 1.00, 1.00, 1.00]
    elif name == "Steven":
        ncontours=50
        stops = [0.00, 0.50, 1.00]
        red   = [0.70, 0.00, 0.20]#[1.00, 0.00, 0.20]
        green = [0.00, 0.50, 1.00]#[0.00, 0.50, 1.00]
        blue  = [0.00, 0.70, 0.20]#[0.00, 0.70, 0.20]
    elif name == "StevenInverse":
        ncontours=50
        stops = [0.00, 0.50, 1.00]
        red   = [0.20, 0.00, 0.70]
        green = [1.00, 0.50, 0.00]
        blue  = [0.20, 0.70, 0.00]
    elif name == "StevenCentred":
        ncontours=50
        stops = [0.00, 0.15, 0.50, 0.85, 1.00]
        red   = [0.00, 0.00, 0.20, 0.70, 1.00]
        green = [0.00, 0.50, 1.00, 0.00, 0.00]
        blue  = [1.00, 0.70, 0.20, 0.00, 0.00]
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
    gStyle.SetNumberContours(ncontours)

# Set the style
def set_style(style="Plain"):
    gROOT.SetStyle(style) 
    gStyle.SetPaintTextFormat("0.3g") 
    gStyle.SetOptStat(0) 
    gStyle.SetPadTickX(1) 
    gStyle.SetPadTickY(1)






def SetAxisRange(histo,oneSided=True,lowBound=-1e10,highBound=1e10):
    max = -100
    min = 100
    for binX in range(1,histo.GetNbinsX()+1):
        for binY in range(1,histo.GetNbinsY()+1):
            value = histo.GetBinContent(binX,binY)
            if value > lowBound and value < highBound:
                if value > max: max = value
                if value < min: min = value
    
    # Check if this is ~ one-sided
    # Allow up to 10% relative magnitude
    actuallyOneSided = True
    if oneSided:
        # Only relevant if the min and max are opposite signs or around 1 if relative
        if not SetAxisRange.relativeAxis:
            if max > 0 and min < 0:
                balanceFrac = fabs(min/max) if max != 0 else 100
                if balanceFrac < 25 and balanceFrac > 0.25:
                    # Within 25% of relative magnitude
                    actuallyOneSided = False
        else:
            if max > 1 and min < 1:
                balanceFrac = fabs((1-min)/(max-1)) if max != 1 else 100
                if balanceFrac < 25 and balanceFrac > 0.25:
                    # Within 25% of relative magnitude
                    actuallyOneSided = False
    if not actuallyOneSided and not SetAxisRange.forceOneSided:
        print "Warning: Ignoring one-sided axis request due to magnitudes of min and max (|min/max| = |%f/%f| = %f)"%(min,max,balanceFrac)
        print "This can be overridden using SetAxisRange.forceOneSided = True"
        oneSided = False

    if oneSided:
        if not SetAxisRange.relativeAxis:
            isPositiveSide = True if max > 0 and max > fabs(min) else False
        else:
            isPositiveSide = True if max > 1 and (max > 1/min if min < 1 else max > min) else False

        # Warn when one-sided assumption is not strictly true
        if isPositiveSide:
            if not SetAxisRange.relativeAxis:
                if min < -1.e-4:
                    print "Warning: Ignoring min of %f and setting min to 0"%(min)
            else:
                if min-1 < -1.e-4:
                    print "Warning: Ignoring min of %f and setting min to 1"%(min)

            # Round to nearest 5% beyond the max
            #set_palette("SophieInverse")
            set_palette("StevenInverse" if not SetAxisRange.invertAxisColour else "Steven")
            
            if not SetAxisRange.relativeAxis:
                histo.GetZaxis().SetRangeUser(-1.e-4,int(max*20+1)/20.)
            else:
                histo.GetZaxis().SetRangeUser(1-1.e-4,int(max*20+1)/20.)

        elif not isPositiveSide:
            if not SetAxisRange.relativeAxis:
                if max > 1.e-4:
                    print "Warning: Ignoring max of %f and setting max to 0"%(max)
            else:
                if max-1 > 1.e-4:
                    print "Warning: Ignoring max of %f and setting max to 1"%(max)

            # Round to nearest 5% beyond the min
            #set_palette("Sophie")
            set_palette("Steven" if not SetAxisRange.invertAxisColour else "StevenInverse")

            if not SetAxisRange.relativeAxis:
                histo.GetZaxis().SetRangeUser(-int(fabs(min)*20+1)/20.,1.e-4)
            else:
                histo.GetZaxis().SetRangeUser(int(min*20-1)/20.,1+1.e-4)
        
    else:
        if not SetAxisRange.relativeAxis:
            extremeVal = 0
            if fabs(max) > fabs(min):
                extremeVal = fabs(max)
            else:
                extremeVal = fabs(min)

            # Round to nearest 5% beyond the extreme
            set_palette("default")
            histo.GetZaxis().SetRangeUser(-int(extremeVal*20+1)/20.,int(extremeVal*20+1)/20.)
        else:
            set_palette("StevenCentred")
            #histo.GetZaxis().SetRangeUser(0,int(max*20+1)/20.)
            # Want the middle in log-scale to be 1
            # If min is the extremum, then min = 10^{-X}, X = -log(min)
            # If max is the extremum, then max = 10^{X}, X = log(max)
            if min*max < 1:
                # Minimum dominates
                histo.GetZaxis().SetRangeUser(min,pow(10,-log10(min)))
            else:
                # Maximum dominates
                histo.GetZaxis().SetRangeUser(pow(10,-log10(max)),max)
SetAxisRange.forceOneSided = False
SetAxisRange.relativeAxis = False
SetAxisRange.invertAxisColour = False



def SetAxisLabels(histo,xValues,yValues,labelString):
    factorX = histo.GetNbinsX()/len(xValues)
    factorY = histo.GetNbinsY()/len(yValues)

    for index in range(1,len(xValues)+1):
        histo.GetXaxis().SetBinLabel(int(factorX*(index-0.5)),labelString%(xValues[index-1]))
    for index in range(1,len(yValues)+1):
        histo.GetYaxis().SetBinLabel(int(factorY*(index-0.5)),labelString%(yValues[index-1]))

    histo.GetXaxis().LabelsOption("h")
    histo.GetYaxis().LabelsOption("h")

    histo.GetXaxis().SetTickLength(0)
    histo.GetYaxis().SetTickLength(0)


def DrawGrid(histo,xValues,yValues):
    DrawGrid.lines = []

    factorX = histo.GetNbinsX()/len(xValues)
    factorY = histo.GetNbinsY()/len(yValues)
    
    for index in range(1,len(xValues)):
        line = TLine()
        line.SetLineWidth(2)
        line.SetLineColor(kBlack)
        line.SetLineStyle(1)
        line.DrawLine(int(factorX*index),histo.GetYaxis().GetBinLowEdge(1),int(factorX*index),histo.GetYaxis().GetBinLowEdge(histo.GetNbinsY()+1))
        DrawGrid.lines.append(line)
    for index in range(1,len(yValues)):
        line = TLine()
        line.SetLineWidth(2)
        line.SetLineColor(kBlack)
        line.SetLineStyle(1)
        line.DrawLine(histo.GetXaxis().GetBinLowEdge(1),int(factorY*index),histo.GetXaxis().GetBinLowEdge(histo.GetNbinsX()+1),int(factorY*index))
        DrawGrid.lines.append(line)
DrawGrid.lines = []

#def DrawStatValues(histo,maxVals,meanVals,xValues,yValues,percent=True):
#    if not DrawStatValues.tex:
#        DrawStatValues.tex = TLatex()
#        DrawStatValues.tex.SetTextSize(0.02)
#        DrawStatValues.tex.SetTextFont(42)
#        DrawStatValues.tex.SetTextColor(kBlack)
#
#    factorX = histo.GetNbinsX()/len(xValues)
#    factorY = histo.GetNbinsY()/len(yValues)
#    for maxVal in maxVals:
#        DrawStatValues.tex.DrawLatex(int(factorX*(maxVal[0]+0.4)),int(factorY*(maxVal[1]+0.60)),"%.f"%(maxVal[2]*100))
#    for meanVal in meanVals:
#        DrawStatValues.tex.DrawLatex(int(factorX*(meanVal[0]+0.4)),int(factorY*(meanVal[1]+0.40)),"%.f"%(meanVal[2]*100))
#DrawStatValues.tex = None

def DetermineStatValues(histo,xValues,yValues,lowBound=-1e10,highBound=1e10,percent=True):
    
    if not DetermineStatValues.tex:
        DetermineStatValues.tex = TLatex()
        DetermineStatValues.tex.SetTextSize(0.025)
        DetermineStatValues.tex.SetTextFont(42)
        DetermineStatValues.tex.SetTextColor(kBlack)
    
    # Proceed region by region
    for indexX in range(0,len(xValues)):
        factorX = int(histo.GetNbinsX()/len(xValues))
        lowX  = indexX*factorX+1
        highX = (indexX+1)*factorX
        for indexY in range(0,len(yValues)):
            factorY = int(histo.GetNbinsY()/len(yValues))
            lowY  = indexY*factorY+1
            highY = (indexY+1)*factorY
            
            # Stat info
            extremeVal = 0
            meanVal = 0
            numValid = 0

            # Now loop
            for binX in range(lowX,highX):
                for binY in range(lowY,highY):
                    binValue = histo.GetBinContent(binX,binY)
                    if binValue > lowBound and binValue < highBound:
                        numValid += 1
                        meanVal += binValue
                        if not DetermineStatValues.minInsteadOfMax and fabs(binValue) > fabs(extremeVal):
                            extremeVal = binValue
                        elif DetermineStatValues.minInsteadOfMax and fabs(binValue) < fabs(extremeVal):
                            extremeVal = binValue
            meanVal /= numValid
            DetermineStatValues.tex.DrawLatex(int(factorX*(indexX+0.4)),int(factorY*(indexY+0.70)),"%.f"%(extremeVal*100) if percent else "%.1f"%(extremeVal))
            DetermineStatValues.tex.DrawLatex(int(factorX*(indexX+0.4)),int(factorY*(indexY+0.30)),"%.f"%(meanVal*100) if percent else "%.1f"%(meanVal))
DetermineStatValues.tex = None
DetermineStatValues.minInsteadOfMax = False


def DrawText(xPos,yPos,text):
    if not DrawText.tex:
        DrawText.tex = TLatex()
        DrawText.tex.SetNDC()
        DrawText.tex.SetTextFont(42)
        DrawText.tex.SetTextSize(0.040)

    DrawText.tex.DrawLatex(xPos,yPos,text)

DrawText.tex = None





