# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
from math import fabs,exp,log,sqrt
import sys
import re
import os
import AtlasStyleMacro

from PlotHelpers import *


##################################################
#                                                #
# 4D histogram class                             #
#                                                #
##################################################

class CorrMat4D:
    def __init__(self):
        self.hist4D   = None
        self.jetDef   = None
        self.varType  = None
        self.plotType = None
    
    def setInfo(self,jetDef,varType,plotType):
        self.jetDef   = jetDef
        self.varType  = varType
        self.plotType = plotType

    def setInfoCopy(self,toCopy,plotType = ""):
        self.jetDef   = toCopy.jetDef
        self.varType  = toCopy.varType
        self.plotType = plotType if plotType != "" else toCopy.plotType

    def cloneHist4D(self,toClone):
        try:
            self.hist4D = toClone.Clone()
        except AttributeError:
            self.hist4D = toClone.hist4D.Clone()
        self.hist4D.SetName("4D_var%s_%s_%s"%(self.varType,self.jetDef,self.plotType))

    def applyAbsValue(self):
        if not self.hist4D:
            print "Cannot apply absolute value when the histogram hasn't been created"
            return False

        for binX in range(1,self.hist4D.GetNbinsX()+1):
            for binY in range(1,self.hist4D.GetNbinsY()+1):
                if self.hist4D.GetBinContent(binX,binY) > self.OOBT:
                    self.hist4D.SetBinContent(binX,binY,fabs(self.hist4D.GetBinContent(binX,binY)))
        return True
CorrMat4D.OOB  = -1234 # out of bounds value
CorrMat4D.OOBT = -1000 # out of bounds threshold
CorrMat4D.relativeMetric = False # Whether to use (nominal-reduced) or (1-nominal)/(1-reduced)

def relativeMetric(numerator,denominator):
    if fabs(denominator) < 1.e-3:
        if fabs(numerator) < 1.e-3:
            return 1
        else:
            return CorrMat4D.OOB
    if numerator/denominator < 0:
        print "NUM/DEN = %f/%f"%(numerator,denominator)
    return sqrt(numerator/denominator)

# Class methods are defined later where relevant


##################################################
#                                                #
# Plotting helpers specific to CorrelationMatrix #
#                                                #
##################################################

def DrawLabels(hist,jetDefString,scenarioString):
    if DrawLabels.DrawATLASLabel:
        AtlasStyleMacro.ATLASLabel(0.10,0.845,DrawLabels.ATLASLabelName)
    jetDefLabel = ""

    if jetDefString.startswith("AntiKt4"):
        jetDefLabel += "anti-k_{t} #it{R} = 0.4, "
    elif jetDefString.startswith("AntiKt6"):
        jetDefLabel += "anti-k_{t} #it{R} = 0.6, "
    
    if jetDefString.endswith("LCTopo") or jetDefString.endswith("TopoLC"):
        jetDefLabel += "LCW+JES"
    elif jetDefString.endswith("EMTopo") or jetDefString.endswith("TopoEM"):
        jetDefLabel += "EM+JES"
    
    DrawText(0.10,0.900,jetDefLabel+" 2015")
    DrawText(0.10,0.955,scenarioString)
DrawLabels.ATLASLabelName = "Internal"
DrawLabels.DrawATLASLabel = True

def DrawLabelsGuessScenario(histo):
    jetDefString = histo.jetDef
    scenarioLabel = histo.hist4D.GetName().replace("4D_varpt_%s_"%(jetDefString),"").replace("4D_vareta_%s_"%(jetDefString),"").replace(".root","")
    plotType = histo.plotType
    if "_" in scenarioLabel:
        if scenarioLabel.startswith("diff_"):
            scenarioLabel = re.sub("diff_","",scenarioLabel)
            scenarioLabel = re.sub("_"," - ",scenarioLabel)
            scenarioLabel = "Correlation differences, "+scenarioLabel
        elif scenarioLabel.startswith("minDiff_"):
            scenarioLabel = re.sub("minDiff_","",scenarioLabel)
            scenarioLabel = re.sub("_",", ",scenarioLabel)
            #scenarioLabel = "Correlation differences, min[" + scenarioLabel + "]"
            scenarioLabel = "Metric 1 (minimum correlation differences)" if not CorrMat4D.relativeMetric else "Metric 1' (minimum correlation differences)"
        elif scenarioLabel.startswith("maxDiff_"):
            scenarioLabel = re.sub("maxDiff_","",scenarioLabel)
            scenarioLabel = re.sub("_",", ",scenarioLabel)
            scenarioLabel = "Correlation differences, max[" + scenarioLabel + "]"
        elif scenarioLabel.startswith("coverageRaw_"):
            scenarioLabel = re.sub("coverageRaw_","",scenarioLabel)
            scenarioLabel = re.sub("_",", ",scenarioLabel)
            #scenarioLabel = "Correlation loss coverage, [" + scenarioLabel + "]"
            scenarioLabel = "Metric 2 (raw)"
        elif scenarioLabel.startswith("coverageRes_") or plotType == "Metric2" or plotType == "Metric 2":
            scenarioLabel = re.sub("coverageRes_","",scenarioLabel)
            scenarioLabel = re.sub("_",", ",scenarioLabel)
            #scenarioLabel = "Correlation loss remaining, [" + scenarioLabel + "]"
            scenarioLabel = "Metric 2 (uncovered correlation differences)" if not CorrMat4D.relativeMetric else "Metric 2' (uncovered correlation differences)"
        elif scenarioLabel.startswith("uncertaintyRaw_coverageRaw"):
            #scenarioLabel = "Remaining loss not covered by correlation uncertainties"
            scenarioLabel = "Metric 3 (raw)"
        elif scenarioLabel.startswith("uncertaintyRes_coverageRes") or plotType == "Metric3" or plotType == "Metric 3":
            #scenarioLabel = "Remaining loss beyond correlation uncertainties"
            scenarioLabel = "Metric 3 (uncovered corr. diff. including uncertainties)" if not CorrMat4D.relativeMetric else "Metric 3' (uncovered corr. diff. including uncertainties)"
    elif scenarioLabel=="uncenvelope":
        scenarioLabel = "Correlation uncertainties"
    else:
        scenarioNum = re.sub("JER","",re.sub("4NP","",re.sub("3NP","",scenarioLabel)))
        if len(scenarioNum) == 1:
            scenarioLabel = "Correlation difference, Rep_{full}^{JES} - Rep_{str.red}^{%s,JES}"%(scenarioNum if not (scenarioNum == "3" and "JER" in scenarioLabel) else "evdm")
        else:
            scenarioNum = "Correlation difference, full - %s"%(scenarioLabel)
    
    DrawLabels(histo.hist4D,jetDefString,scenarioLabel)



##################################################
#                                                #
# Fixed value manipulation (pT and eta values)   #
#                                                #
##################################################


def getFixedValuesFromName(histName):
    # Parse the histogram name
    tokens = histName.split("_")
    fixed1 = ""
    fixed2 = ""
    for aToken in tokens:
        keep = False
        if aToken.startswith("pt"):
            keep = True
        elif aToken.startswith("eta"):
            keep = True
        
        if keep:
            if fixed1 == "":
                fixed1 = aToken
            elif fixed2 == "":
                fixed2 = aToken
            else:
                print "ERROR: More than two fixed values appear in name:",histName
                sys.exit(-1)
    return fixed1,fixed2


def getFixedValuesFromFile(inFile,jetDef):
    fixedPt = []
    fixedEta = []
    for histName in inFile.GetKeyNames():
        # Ensure this histogram is for the collection we want
        if not jetDef in histName:
            continue
        # Ensure this is a difference histogram
        if not histName.startswith("diff_"):
            continue

        fixed1,fixed2 = getFixedValuesFromName(histName)
        
        if fixed1 != "" and fixed2 != "":
            if fixed1.startswith("pt") and fixed2.startswith("pt"):
                fixedPt.append([float(fixed1.replace("pt","",1)),float(fixed2.replace("pt","",1))])
            elif fixed1.startswith("eta") and fixed2.startswith("eta"):
                fixedEta.append([float(fixed1.replace("eta","",1)),float(fixed2.replace("eta","",1))])
            else:
                print "ERROR: Unexpected mixture of fixed variables for histogram:",histName
                sys.exit(-1)
        else:
            print "ERROR: Failed to parse histogram name for fixed values:",histName
            sys.exit(-2)


    return fixedPt,fixedEta


def getFixedValuesFromFiles(inFiles,jetDef):
    # Get fixed pt and/or eta values from the first file
    fixedPt,fixedEta = getFixedValuesFromFile(inFiles[0],jetDef)
    fixedPt.sort()
    fixedEta.sort()
    
    # Ensure that subsequent file(s) match the fixed pt/eta values
    for aFile in inFiles[1:]:
        localFixedPt,localFixedEta = getFixedValuesFromFile(aFile,jetDef)
        localFixedPt.sort()
        localFixedEta.sort()
    
        # Check for equality
        # Note that these are floating point equalities, so be careful
        if len(localFixedPt) != len(fixedPt):
            print "ERROR: File %s has %d fixed pt values, while %s has %d values"%(aFile.GetNameNoDir(),len(localFixedPt),inFiles[0],len(fixedPt))
            sys.exit(3)
        elif len(localFixedEta) != len(fixedEta):
            print "ERROR: File %s has %d fixed eta values, while %s has %d values"%(aFile.GetNameNoDir(),len(localFixedEta),inFiles[0],len(fixedEta))
            sys.exit(4)
        for localVal,globalVal in zip(localFixedPt,fixedPt):
            if fabs(localVal[0]-globalVal[0]) > 1.e-4 or fabs(localVal[1]-globalVal[1]) > 1.e-4:
                print "ERROR: File %s and %s have different fixed pt values, was comparing %f and %f"%(aFile.GetNameNoDir(),inFiles[0],localVal,globalVal)
                sys.exit(5)
        for localVal,globalVal in zip(localFixedEta,fixedEta):
            if fabs(localVal[0]-globalVal[0]) > 1.e-4 or fabs(localVal[1]-globalVal[1]) > 1.e-4:
                print "ERROR: File %s and %s have different fixed eta values, was comparing %f and %f"%(aFile.GetNameNoDir(),inFiles[0],localVal,globalVal)
                sys.exit(6)
    
    # Files have now been confirmed to match in fixed values, assuming multiple files were specified
    # Determine the actual pT and eta values available now (not just pairs)
    fixedPtX = set([])
    fixedPtY = set([])
    for fixedPtSet in fixedPt:
        fixedPtX.add(fixedPtSet[0])
        fixedPtY.add(fixedPtSet[1])
    fixedPtX = sorted(fixedPtX)
    fixedPtY = sorted(fixedPtY)
    
    fixedEtaX = set([])
    fixedEtaY = set([])
    for fixedEtaSet in fixedEta:
        fixedEtaX.add(fixedEtaSet[0])
        fixedEtaY.add(fixedEtaSet[1])
    fixedEtaX = sorted(fixedEtaX)
    fixedEtaY = sorted(fixedEtaY)

    # Done, return the sorted lists of fixed values
    return fixedPtX,fixedPtY,fixedEtaX,fixedEtaY



##################################################
#                                                #
# 4D histogram building from 2D histograms       #
#                                                #
##################################################

def fillHist4DFromFile(self,inFile,fixedString,fixedX,fixedY,filterStartString="",granularityFactor=1):
    # Ensure the histogram wasn't already filled
    if self.hist4D:
        print "Blocking re-filling of existing CorrMat4D histogram of name ",self.hist4D.GetName()
        return False
    
    # Begin by checking the size of the 2D matrices
    numBins = -1
    for histName in inFile.GetKeyNames():
        if filterStartString != "" and not histName.startswith(filterStartString):
            continue
        if self.jetDef not in histName:
            continue
        if "_%s"%(fixedString) in histName:
            hist = inFile.Get(histName)
            numBins = hist.GetNbinsX()
            break
    if numBins < 0:
        print "Failed to find histogram matching criteria:"
        print "jetDef = \"%s\""%(self.jetDef)
        print "filterStartString = \"%s\""%(filterStartString)
        return False

    # Granularity scaling if requested
    localNumBins = int(numBins / granularityFactor) if granularityFactor > 1 else numBins
    if granularityFactor > 1:
        # Ensure the granularity fits
        if numBins % granularityFactor != 0:
            print "Cannot apply granularity factor: %d bins can't be divided by %d"%(numBins,granularityFactor)
            return False

    # Now build the empty 4D matrix
    histName4D = "4D_var%s_%s_%s"%(self.varType,self.jetDef,inFile.GetNameNoDir())
    self.hist4D = TH2D(histName4D,"",len(fixedX)*localNumBins,0.,len(fixedX)*localNumBins,len(fixedY)*localNumBins,0.,len(fixedY)*localNumBins)

    # Fill with the out-of-bounds value
    for binX in range(1,self.hist4D.GetNbinsX()+1):
        for binY in range(1,self.hist4D.GetNbinsY()+1):
            self.hist4D.SetBinContent(binX,binY,self.OOB)

    # Fill the histogram
    for histName in inFile.GetKeyNames():
        if filterStartString != "" and not histName.startswith(filterStartString):
            continue
        if self.jetDef not in histName:
            continue

        # Get the fixed values for this 2D histogram
        # Sanity checks were already done before, so we just need to retrieve them
        fixed1,fixed2 = getFixedValuesFromName(histName)
        if not fixed1.startswith(fixedString): continue
        fixed1 = float(fixed1.replace(fixedString,"",1))
        fixed2 = float(fixed2.replace(fixedString,"",1))

        # We have the fixed values, now determine the respective indices
        index1 = -1
        index2 = -1
        for aIndex in range(0,len(fixedX)):
            aValue = fixedX[aIndex]
            if fabs(aValue - fixed1) < 1.e-4:
                index1 = aIndex
                break
        for aIndex in range(0,len(fixedY)):
            aValue = fixedY[aIndex]
            if fabs(aValue - fixed2) < 1.e-4:
                index2 = aIndex
                break
        if index1 < 0 or index2 < 0:
            print "Failed to find index1 (%d) or index2 (%d) for histogram (%s)"%(index1,index2,histName)
            self.histName = None
            return False

        # We now have the indices
        # Get the 2D histogram and use it to fill the relevant portion of the 4D histogram
        # Watch for granularity factors
        hist2D = inFile.Get(histName)
        offsetX = int((self.hist4D.GetNbinsX()/len(fixedX))*index1)
        offsetY = int((self.hist4D.GetNbinsY()/len(fixedY))*index2)
        if localNumBins == numBins:
            for binX in range(1,hist2D.GetNbinsX()+1):
                for binY in range(1,hist2D.GetNbinsY()+1):
                    self.hist4D.SetBinContent(binX+offsetX,binY+offsetY,hist2D.GetBinContent(binX,binY))
        else:
            # Split into pieces by the granularity factor
            # Watch for the root indexing from 1, which has to be in the sub-bin indices
            # Also have to be careful about bins which are on the edge of the kinematic limit
            for binX in range(0,hist2D.GetNbinsX()/granularityFactor):
                for binY in range(0,hist2D.GetNbinsY()/granularityFactor):
                    subVal = 0
                    numVal = 0
                    for subBinX in range(1,granularityFactor+1):
                        for subBinY in range(1,granularityFactor+1):
                            binContent = hist2D.GetBinContent(binX*granularityFactor+subBinX,binY*granularityFactor+subBinY)
                            if binContent > self.OOBT:
                                subVal += binContent
                                numVal += 1
                    self.hist4D.SetBinContent(binX+1+offsetX,binY+1+offsetY,subVal/numVal if numVal > 0 else self.OOB)

        # Done with this 2D histogram
    # Done with this 4D histogram
    return True
CorrMat4D.fillHist4DFromFile = fillHist4DFromFile


def buildAndFillHists4DFromFiles(inFiles,jetDef,varString,fixedString,fixedX,fixedY,filterStartString="",granularityFactor=1):
    try:
        iterator = iter(inFiles)
    except TypeError:
        # Not iterable
        # Probably a single item
        inFiles = [inFiles]
    else:
        # Iterable
        # Probably a list of files, but watch for strings
        if isinstance(inFiles,str):
            inFiles = [TFile.Open(inFiles,"READ")]
        else:
            for iFile in range(0,len(inFiles)):
                if isinstance(inFiles[iFile],str):
                    inFiles[iFile] = TFile.Open(inFiles[iFile],"READ")

    hists4D = [CorrMat4D() for aFile in inFiles]
    for aFile,aHist in zip(inFiles,hists4D):
        aHist.setInfo(jetDef,varString,"raw" if filterStartString == "" else "raw_"+ re.sub("_","",filterStartString))
        if not aHist.fillHist4DFromFile(aFile,fixedString,fixedX,fixedY,filterStartString,granularityFactor):
            return None

    return hists4D


def buildAndFillHists4DFromFile(inFile,jetDef,varString,fixedString,fixedX,fixedY,excludeStartString="",granularityFactor=1):
    try:
        iterator = iter(inFile)
    except TypeError:
        # Not iterable
        # Probably a single item
        # Nothing to do in this case
        pass
    else:
        # Iterable
        # Probably a list of files, but watch for a single string
        if isinstance(inFile,str):
            inFile = TFile.Open(inFile,"READ")
        else:
            print "Expected a string, but did not receive one: ",inFile
            print "This method is for building a set of histograms from a single file"
            return None

    # Get the list of starts (the string before _var*)
    startStrings = set()
    for histName in inFile.GetKeyNames():
        if excludeStartString != "" and histName.startswith(excludeStartString):
            continue
        # Find the _var and get what's in front of it
        startStrings.add(histName[:histName.find("_var")])

    # Now run over the scenarios
    hists4D = [CorrMat4D() for aString in startStrings]
    for aString,aHist in zip(startStrings,hists4D):
        aHist.setInfo(jetDef,varString,"raw_"+aString)
        if not aHist.fillHist4DFromFile(inFile,fixedString,fixedX,fixedY,aString,granularityFactor):
            return None

    return hists4D



##################################################
#                                                #
# 4D histograms from differences of 4D hist sets #
#                                                #
##################################################

def fillHist4DFromDifference(self,hist1,hist2):
    # Ensure the histogram wasn't already filled
    if self.hist4D:
        print "Blocking re-filling of existing CorrMat4D histogram of name ",self.hist4D.GetName()
        return False

    # Ensure the histograms exist
    if not hist1 or not hist2:
        print "Argument(s) are None: ",hist1,hist2
        return False

    # Copy the first histogram and subtract the second
    self.cloneHist4D(hist1)

    # Manually do the difference so that we can watch for out-of-bounds
    for binX in range(1,hist1.hist4D.GetNbinsX()+1):
        for binY in range(1,hist2.hist4D.GetNbinsX()+1):
            if hist1.hist4D.GetBinContent(binX,binY) > self.OOBT and hist2.hist4D.GetBinContent(binX,binY) > self.OOBT:
                if not CorrMat4D.relativeMetric:
                    self.hist4D.SetBinContent(binX,binY,hist1.hist4D.GetBinContent(binX,binY) - hist2.hist4D.GetBinContent(binX,binY))
                else:
                    self.hist4D.SetBinContent(binX,binY,relativeMetric(hist1.hist4D.GetBinContent(binX,binY),hist2.hist4D.GetBinContent(binX,binY)))

    return True
CorrMat4D.fillHist4DFromDifference = fillHist4DFromDifference

def buildAndFillHists4DFromDifferences(hists):
    # Must do all possible comparisons
    diffHists = []
    for iHist1 in range(0,len(hists)):
        for iHist2 in range(iHist1+1,len(hists)):
            hist4D = CorrMat4D()
            hist4D.setInfoCopy(hists[iHist1],"diff_%s_%s"%(hists[iHist1].plotType,hists[iHist2].plotType))
            if not hist4D.fillHist4DFromDifference(hists[iHist1],hists[iHist2]):
                return None
            diffHists.append(hist4D)
    return diffHists




##################################################
#                                                #
# 4D histogram from minimum value of a 4D set    #
#                                                #
##################################################

def fillHist4DFromMinOfSet(self,hists):
    # Ensure the histogram wasn't already filled
    if self.hist4D:
        print "Blocking re-filling of existing CorrMat4D histogram of name ",self.hist4D.GetName()
        return False

    # Ensure the histogram(s) exist
    if not hists or len(hists) == 0:
        print "Argument is None or empty list: ",hists
        return False

    # Copy the first histogram for formatting purposes
    self.cloneHist4D(hists[0])

    # The values from the first histogram are already set from cloning
    # Now just check if any other hists have smaller values
    # Note that smaller may be relative to 1, not zero (for the relative metric)
    # Watch for out-of-bounds
    for aHist in hists[1:]:
        for binX in range(1,aHist.hist4D.GetNbinsX()+1):
            for binY in range(1,aHist.hist4D.GetNbinsY()+1):
                if aHist.hist4D.GetBinContent(binX,binY) > aHist.OOBT:
                    if not CorrMat4D.relativeMetric:
                        if fabs(aHist.hist4D.GetBinContent(binX,binY)) < fabs(self.hist4D.GetBinContent(binX,binY)):
                            self.hist4D.SetBinContent(binX,binY,aHist.hist4D.GetBinContent(binX,binY))
                    else:
                        currVal = self.hist4D.GetBinContent(binX,binY)
                        newVal  = aHist.hist4D.GetBinContent(binX,binY)
                        # If the values bracket one, we're safe
                        if currVal <= 1 and newVal >= 1:
                            self.hist4D.SetBinContent(binX,binY,1)
                        elif currVal >= 1 and newVal <= 1:
                            self.hist4D.SetBinContent(binX,binY,1)
                        elif currVal < 1 and newVal < 1:
                            if newVal > currVal:
                                self.hist4D.SetBinContent(binX,binY,newVal)
                        #elif currVal < 1 and newVal > 1:
                        #    if newVal < 1/currVal:
                        #        self.hist4D.SetBinContent(binX,binY,newVal)
                        #elif currVal > 1 and newVal < 1:
                        #    if newVal > 1/currVal:
                        #        self.hist4D.SetBinContent(binX,binY,newVal)
                        elif currVal > 1 and newVal > 1:
                            if newVal < currVal:
                                self.hist4D.SetBinContent(binX,binY,newVal)
    return True
CorrMat4D.fillHist4DFromMinOfSet = fillHist4DFromMinOfSet


def buildAndFillHist4DFromMinOfSet(hists):
    # Ensure the histogram(s) exist
    if not hists or len(hists) == 0:
        print "Argument is None or empty list: ",hists
        return None
    
    minHist = CorrMat4D()
    minHist.setInfoCopy(hists[0],"minDiff_%dinputs"%(len(hists)))
    if not minHist.fillHist4DFromMinOfSet(hists):
        return None
    return minHist



##################################################
#                                                #
# 4D histogram from maximum value of a 4D set    #
#                                                #
##################################################

def fillHist4DFromMaxOfSet(self,hists):
    # Ensure the histogram wasn't already filled
    if self.hist4D:
        print "Blocking re-filling of existing CorrMat4D histogram of name ",self.hist4D.GetName()
        return False

    # Ensure the histogram(s) exist
    if not hists or len(hists) == 0:
        print "Argument is None or empty list: ",hists
        return False

    # Copy the first histogram for formatting purposes
    self.cloneHist4D(hists[0])

    # The values from the first histogram are already set from cloning
    # Now just check if any other hists have smaller values
    # Note that this may be values with respect to 1 if relative metric
    # Watch for out-of-bounds
    for aHist in hists[1:]:
        for binX in range(1,aHist.hist4D.GetNbinsX()+1):
            for binY in range(1,aHist.hist4D.GetNbinsY()+1):
                if aHist.hist4D.GetBinContent(binX,binY) > aHist.OOBT and aHist.hist4D.GetBinContent(binX,binY) < -aHist.OOBT:
                    if not CorrMat4D.relativeMetric:
                        if fabs(aHist.hist4D.GetBinContent(binX,binY)) > fabs(self.hist4D.GetBinContent(binX,binY)):
                            self.hist4D.SetBinContent(binX,binY,aHist.hist4D.GetBinContent(binX,binY))
                    else:
                        currVal = self.hist4D.GetBinContent(binX,binY)
                        newVal  = aHist.hist4D.GetBinContent(binX,binY)
                        if currVal < 1 and newVal < 1:
                            if newVal < currVal:
                                self.hist4D.SetBinContent(binX,binY,newVal)
                        elif currVal < 1 and newVal > 1:
                            if newVal > 1/currVal:
                                self.hist4D.SetBinContent(binX,binY,newVal)
                        elif currVal > 1 and newVal < 1:
                            if newVal < 1/currVal:
                                self.hist4D.SetBinContent(binX,binY,newVal)
                        elif currVal > 1 and newVal > 1:
                            if newVal > currVal:
                                self.hist4D.SetBinContent(binX,binY,newVal)
    return True
CorrMat4D.fillHist4DFromMaxOfSet = fillHist4DFromMaxOfSet


def buildAndFillHist4DFromMaxOfSet(hists):
    # Ensure the histogram(s) exist
    if not hists or len(hists) == 0:
        print "Argument is None or empty list: ",hists
        return None
    
    maxHist = CorrMat4D()
    maxHist.setInfoCopy(hists[0],"maxDiff_%dinputs"%(len(hists)))
    if not maxHist.fillHist4DFromMaxOfSet(hists):
        return None
    return maxHist


##################################################
#                                                #
# 4D histogram from envelope of a 4D set         #
#                                                #
##################################################

def fillHist4DFromEnvelopeOfSet(self,hists):
    # Ensure the histogram wasn't already filled
    if self.hist4D:
        print "Blocking re-filling of existing CorrMat4D histogram of name ",self.hist4D.GetName()
        return False

    # Ensure the histogram(s) exist
    if not hists or len(hists) < 2:
        print "Argument is None or contains less than two histograms: ",hists
        return False

    # Copy the first histogram for formatting purposes
    self.cloneHist4D(hists[0])

    # The values of the first histogram were copied from cloning
    # We need to set these back to 0 (excluding out-of-bounds values)
    for binX in range(1,self.hist4D.GetNbinsX()+1):
        for binY in range(1,self.hist4D.GetNbinsY()+1):
            if self.hist4D.GetBinContent(binX,binY) > self.OOBT:
                self.hist4D.SetBinContent(binX,binY,0)

    # Now construct the envelope
    # Fill with the maximum |difference| from zero when subtracting config X from config Y for all combinations of X,Y
    for iHist1 in range(0,len(hists)):
        hist1 = hists[iHist1]
        for iHist2 in range(iHist1+1,len(hists)):
            hist2 = hists[iHist2]
            for binX in range(self.hist4D.GetNbinsX()+1):
                for binY in range(self.hist4D.GetNbinsY()+1):
                    if self.hist4D.GetBinContent(binX,binY) > self.OOBT:
                        if CorrMat4D.relativeMetric:
                            diff = relativeMetric(1-hist1.hist4D.GetBinContent(binX,binY),1-hist2.hist4D.GetBinContent(binX,binY))
                        else:
                            diff = fabs(hist2.hist4D.GetBinContent(binX,binY) - hist1.hist4D.GetBinContent(binX,binY))
                        if diff > self.hist4D.GetBinContent(binX,binY):
                            self.hist4D.SetBinContent(binX,binY,diff)

    return True
CorrMat4D.fillHist4DFromEnvelopeOfSet = fillHist4DFromEnvelopeOfSet

def buildAndFillHists4DFromEnvelopeOfSet(hists):
    # Ensure the histogram(s) exist
    if not hists or len(hists) < 2:
        print "Argument is None or contains less than two histograms: ",hists
        return None

    envelopeHist = CorrMat4D()
    envelopeHist.setInfoCopy(hists[0],"envelope_%dinputs"%(len(hists)))
    if not envelopeHist.fillHist4DFromEnvelopeOfSet(hists):
        return None
    return envelopeHist


##################################################
#                                                #
# 4D histogram from coverage of a 4D set         #
#                                                #
##################################################

def fillHist4DFromCoverageOfSet(self,minDiffFromNominal,maxDiffBetweenScenarios,plotStyle,nominalHist=None):
    # Ensure the histogram wasn't already filled
    if self.hist4D:
        print "Blocking re-filling of existing CorrMat4D histogram of name ",self.hist4D.GetName()
        return False

    # Ensure the histogram(s) exist
    if not minDiffFromNominal or not maxDiffBetweenScenarios:
        print "Argument is None or empty list: ",minDiffFromNominal,maxDiffBetweenScenarios
        return False
    if plotStyle==2 and not nominalHist:
        print "NominalHist is None for style which requires it"
        return False

    # Copy the minDiff histogram for formatting purposes
    self.cloneHist4D(minDiffFromNominal)

    # Now fill with the value controlled by plotStyle
    #   0: 0 if max(scenarioDiff) > min(nominalDiff) else min(nominalDiff)
    #   1: 0 if max(scenarioDiff) > min(nominalDiff) else min(nominalDiff) - max(scenarioDiff)
    #   2: 0 if max(scenarioDiff) > min(nominalDiff) else nominalValue

    for binX in range(1,self.hist4D.GetNbinsX()+1):
        for binY in range(1,self.hist4D.GetNbinsY()+1):
            if self.hist4D.GetBinContent(binX,binY) > self.OOBT:
                minDiff = fabs(minDiffFromNominal.hist4D.GetBinContent(binX,binY))
                maxDiff = fabs(maxDiffBetweenScenarios.hist4D.GetBinContent(binX,binY))
                
                if not CorrMat4D.relativeMetric:
                    if minDiff <= maxDiff:
                        self.hist4D.SetBinContent(binX,binY,0)
                    elif plotStyle == 0:
                        self.hist4D.SetBinContent(binX,binY,minDiffFromNominal.hist4D.GetBinContent(binX,binY))
                    elif plotStyle == 1:
                        self.hist4D.SetBinContent(binX,binY,maxDiff-minDiff)
                    elif plotStyle == 2:
                        self.hist4D.SetBinContent(binX,binY,nominalHist.hist4D.GetBinContent(binX,binY))
                    else:
                        print "Unrecognized plotStyle of ",plotStyle
                        return False
                else:
                    # The min difference from 1 (nominal vs reduced) should be smaller 
                    # than the max difference from 1 (reduced vs reduced) as a relative comparison
                    if (minDiff <= 1 and maxDiff <= 1) and (minDiff >= maxDiff):
                        self.hist4D.SetBinContent(binX,binY,1)
                    elif (minDiff >= 1 and maxDiff >= 1) and (minDiff <= maxDiff):
                        self.hist4D.SetBinContent(binX,binY,1)
                    elif (minDiff >= 1 and maxDiff <= 1) and (minDiff <= 1/maxDiff):
                        self.hist4D.SetBinContent(binX,binY,1)
                    elif (minDiff <= 1 and maxDiff >= 1) and (minDiff >= 1/maxDiff):
                        self.hist4D.SetBinContent(binX,binY,1)
                    else:
                        if plotStyle == 0:
                            self.hist4D.SetBinContent(binX,binY,minDiffFromNominal.hist4D.GetBinContent(binX,binY))
                        elif plotStyle == 1:
                            if (minDiff <= 1 and maxDiff <= 1) or (minDiff >= 1 and maxDiff >= 1):
                                self.hist4D.SetBinContent(binX,binY,minDiff/maxDiff)
                            else:
                                self.hist4D.SetBinContent(binX,binY,minDiff*maxDiff)
                        elif plotStyle == 2:
                            self.hist4D.SetBinContent(binX,binY,nominalHist.hist4D.GetBinContent(binX,binY))
                        else:
                            print "Unrecognized plotStyle of ",plotStyle
                            return False

    return True
CorrMat4D.fillHist4DFromCoverageOfSet = fillHist4DFromCoverageOfSet

def buildAndFillHist4DFromCoverageOfSet(minDiffFromNominal,maxDiffBetweenScenarios,plotStyle,nominalHist=None):
    coverageHist = CorrMat4D()
    coverageHist.setInfoCopy(minDiffFromNominal,"coverage_type%d"%(plotStyle))
    
    if not coverageHist.fillHist4DFromCoverageOfSet(minDiffFromNominal,maxDiffBetweenScenarios,plotStyle,nominalHist):
        return None
    return coverageHist




##################################################
#                                                #
# 4D histogram plotting helpers                  #
#                                                #
##################################################

def saveHists4D(canvas,plotFileName,hist,oneSidedAxis,fixedX,fixedY,fixedStr,scenarioLabel=""):
    # Check for empty inputs
    if not hist:
        pass
    elif isinstance(hist,list):
        # A list, iterate on the entries
        for aHist in hist:
            saveHists4D(canvas,plotFileName,aHist,oneSidedAxis,fixedX,fixedY,fixedStr)
    else:
        # Not a list, save the output
        SetAxisRange(hist.hist4D,oneSidedAxis,CorrMat4D.OOBT,1e10)
        SetAxisLabels(hist.hist4D,fixedX,fixedY,fixedStr)

        if canvas.GetLogx():
            hist.hist4D.GetXaxis().SetMoreLogLabels()
        if canvas.GetLogy():
            hist.hist4D.GetYaxis().SetMoreLogLabels()
        if canvas.GetLogz():
            hist.hist4D.GetZaxis().SetMoreLogLabels()
            hist.hist4D.GetZaxis().SetLabelOffset(0.0010)

        if CorrMat4D.relativeMetric:
            for binX in range(1,hist.hist4D.GetNbinsX()+1):
                for binY in range(1,hist.hist4D.GetNbinsY()+1):
                    if hist.hist4D.GetBinContent(binX,binY) < hist.OOBT:
                        hist.hist4D.SetBinContent(binX,binY,1.e-10)
        hist.hist4D.Draw("colz")
        DrawGrid(hist.hist4D,fixedX,fixedY)
        DetermineStatValues(hist.hist4D,fixedX,fixedY,CorrMat4D.OOBT if not CorrMat4D.relativeMetric else 1.e-9,1e10,not CorrMat4D.relativeMetric)
        if scenarioLabel != "": DrawLabels(hist.hist4D,hist.jetDef,scenarioLabel)
        else: DrawLabelsGuessScenario(hist)
            
        if not (plotFileName.endswith(".eps") or plotFileName.endswith(".png")):
            canvas.Print(plotFileName)
        else:
            canvas.Print(re.sub(".png","-%d.png"%(saveHists4D.iEPS),re.sub(".eps","-%d.eps"%(saveHists4D.iEPS),plotFileName)))
            saveHists4D.iEPS += 1
        #canvas.Print(getPlotFileName(plotFileName))
saveHists4D.iEPS = 0





