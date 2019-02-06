from ROOT import *
from array import array
import glob
import re
import math


def GetDefaultPtBins(numBins=100,lowVal=15.0,highVal=2500.0):
    dx = (math.log(highVal)-math.log(lowVal))/numBins
    bins = []
    for iBin in range(0,numBins+1):
        bins.append(math.exp(math.log(lowVal)+iBin*dx))
    
    return bins

def GetDefaultEtaBins():
    etaValuesAsym = [0,0.15,0.3,0.8,1.0,1.2,1.65,2.1,2.45,2.8,3.0,3.2,3.6,4.5]
    etaValuesSym = []
    for aVal in etaValuesAsym:
        etaValuesSym.append(aVal)
        if math.fabs(aVal) > 1.e-4: # Don't double-count zero
            etaValuesSym.append(-aVal)

    return sorted(etaValuesSym)

def GetCentredBins(values):
    if len(values) == 1:
        return [values[0]*0.5,values[0]*1.5]
    
    binList = []
    for iVal in range(0,len(values)-1):
        binList.append(values[iVal]+(values[iVal+1]-values[iVal])/2.0)
    binList.insert(0,values[0]-(values[1]-values[0])/2.0)
    binList.append(values[-1]+(values[-1]-values[-2])/2.0)

    return binList

def MakeProviderHisto(histoName,ptBins=GetDefaultPtBins(),etaBins=GetDefaultEtaBins()):
    return TH2D(histoName,histoName,len(ptBins)-1,array('d',ptBins),len(etaBins)-1,array('d',etaBins))


def ConvertPtHistoToProviderHisto(histo1D,histoName):
    histo2D = MakeProviderHisto(histoName,etaBins=[min(GetDefaultEtaBins()),0,max(GetDefaultEtaBins())])

    oldMinPt = histo1D.GetXaxis().GetBinLowEdge(1)
    oldMaxPt = histo1D.GetXaxis().GetBinLowEdge(histo1D.GetNbinsX()+1)
    tolerance = 1.0e-4
    for ptBin in range(1,histo2D.GetNbinsX()+1):
        pt = histo2D.GetXaxis().GetBinCenter(ptBin)
        unc = 0.0
        if pt <= oldMinPt:
            unc = histo1D.Interpolate(oldMinPt+tolerance)
        elif pt >= oldMaxPt:
            unc = histo1D.Interpolate(oldMaxPt+tolerance)
        else:
            unc = histo1D.Interpolate(pt)

        for etaBin in range(1,histo2D.GetNbinsY()+1):
            histo2D.SetBinContent(ptBin,etaBin,unc)

    return histo2D

def ConvertTGraphToProviderHisto(graph,histoName):
    pointsX = []
    pointsY = []
    for iPoint in range(0,graph.GetN()):
        px = Double(0)
        py = Double(0)
        graph.GetPoint(iPoint,px,py)
        pointsX.append(px)
        pointsY.append(py)

    # Make bins with the points as the centre where possible
    bins = GetCentredBins(pointsX)

    # Extend the range to cover [15,2500] if necessary
    if bins[0] > 15:
        bins.insert(0,15.)
        pointsX.inset(0,(bins[0]+bins[1])/2.0)
        pointsY.insert(0,pointsY[0])

    if bins[-1] < 2500:
        bins.append(2500.)
        pointsX.append((bins[-1]+bins[-2])/2.0)
        pointsY.append(pointsY[-1])

    # Make the histo and fill it
    histo = MakeProviderHisto(histoName,ptBins=bins,etaBins=[min(GetDefaultEtaBins()),0,max(GetDefaultEtaBins())])
    for ptBin in range(1,histo.GetNbinsX()+1):
        for etaBin in range(1,histo.GetNbinsY()+1):
            histo.SetBinContent(ptBin,etaBin,pointsY[ptBin-1])

    return histo

def ExtendPtRangeOfHisto(histo,histoName):
    # Get the current bins
    ptBins = []
    for xBin in range(1,histo.GetNbinsX()+2):
        ptBins.append(histo.GetXaxis().GetBinLowEdge(xBin))

    # Extend only if necessary
    if math.fabs(ptBins[-1] - 2500) < 1.e-4:
        print "Histo already went to 2500 GeV, doing nothing"
        return histo
    elif ptBins[-1] > 2500:
        print "Histo beyond range, cannot extend"
        return None
    ptBins.append(2500.)

    # Get eta bins
    etaBins = []
    for yBin in range(1,histo.GetNbinsY()+2):
        etaBins.append(histo.GetYaxis().GetBinLowEdge(yBin))

    # Make and fill
    newHisto = MakeProviderHisto(histoName,ptBins=ptBins,etaBins=etaBins)
    for xBin in range(1,histo.GetNbinsX()+1):
        for yBin in range(1,histo.GetNbinsY()+1):
            newHisto.SetBinContent(xBin,yBin,histo.GetBinContent(xBin,yBin))
    for yBin in range(1,histo.GetNbinsY()+1):
        newHisto.SetBinContent(histo.GetNbinsX()+1,yBin,histo.GetBinContent(histo.GetNbinsX(),yBin))

    return newHisto

def SymmetrizeHistoInEta(histo,histoName):
    # Get the current eta bins and symmetrize
    etaBins = []
    for yBin in range(1,histo.GetNbinsY()+2):
        etaVal = histo.GetYaxis().GetBinLowEdge(yBin)
        if math.fabs(etaVal) > 1.e-4:
            etaBins.append(-etaVal)
        etaBins.append(etaVal)
    etaBins = sorted(etaBins)

    # Get the current pT bins
    ptBins = []
    for xBin in range(1,histo.GetNbinsX()+2):
        ptBins.append(histo.GetXaxis().GetBinLowEdge(xBin))

    # Make and fill
    newHisto = MakeProviderHisto(histoName,ptBins=ptBins,etaBins=etaBins)
    for xBin in range(1,histo.GetNbinsX()+1):
        xVal = histo.GetXaxis().GetBinCenter(xBin)
        for yBin in range(1,histo.GetNbinsY()+1):
            yVal = histo.GetYaxis().GetBinCenter(yBin)
            newHisto.SetBinContent(newHisto.GetXaxis().FindBin(xVal),newHisto.GetYaxis().FindBin(-yVal),histo.GetBinContent(xBin,yBin))
            newHisto.SetBinContent(newHisto.GetXaxis().FindBin(xVal),newHisto.GetYaxis().FindBin(yVal),histo.GetBinContent(xBin,yBin))

    return newHisto

def QuadratureSumHisto(histoName,histos):
    # If it's one histo, just return it
    if len(histos) == 1:
        return histos[0]

    # Ensure that the binning is the same
    tolerance = 1.e-4
    for aHisto in histos[1:]:
        if aHisto.GetNbinsX() != histos[0].GetNbinsX():
            print "Input histograms have different number of x bins - can't do quadrature sum safely"
            return None
        elif aHisto.GetNbinsY() != histos[0].GetNbinsY():
            print "Input histograms have different number of y bins - can't do quadrature sum safely"
            return None
    for xBin in range(1,histos[0].GetNbinsX()+2):
        edge = None
        for aHisto in histos:
            if edge is None:
                edge = aHisto.GetXaxis().GetBinLowEdge(xBin)
            elif math.fabs(edge-aHisto.GetXaxis().GetBinLowEdge(xBin)) > tolerance:
                print "Input histograms have different x binning - can't do quadrature sum safely"
                return None
    for yBin in range(1,histos[0].GetNbinsY()+2):
        edge = None
        for aHisto in histos:
            if edge is None:
                edge = aHisto.GetYaxis().GetBinLowEdge(yBin)
            elif math.fabs(edge-aHisto.GetYaxis().GetBinLowEdge(yBin)) > tolerance:
                print "Input histograms have different y binning - can't do quadrature sum safely"
                return None
    
    # Same binning - good, we can continue
    # Clone the first histo for the binning
    quadSumHisto = histos[0].Clone()
    for xBin in range(1,quadSumHisto.GetNbinsX()+1):
        for yBin in range(1,quadSumHisto.GetNbinsY()+1):
            quadSum = 0
            for aHisto in histos:
                quadSum += pow(aHisto.GetBinContent(xBin,yBin),2)
            quadSumHisto.SetBinContent(xBin,yBin,sqrt(quadSum))

    return quadSumHisto




