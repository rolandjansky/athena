# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Script by Steven Schramm, originally based off of 4D histogram code by Sophie Pataraia

from ROOT import *
from array import array
from math import fabs,exp,log
import sys
import re
import os
import AtlasStyleMacro

from PlotHelpers import *
from CorrelationMatrixHelpers import *

# Controls
#isSquaredMetric = False
granularityFactor = 1
SetAxisRange.forceOneSided = True
#DrawLabels.ATLASLabelName = "Preliminary"
DrawLabels.DrawATLASLabel = True
SetAxisRange.invertAxisColour = False
DetermineStatValues.minInsteadOfMax = False

# Check arguments
if len(sys.argv) < 6:
    print "Too few arguments.  Expected the following:"
    print "  1. Jet definition to study, such as AntiKt4LCTopo"
    print "  2. Output plot file"
    print "  3. Correlation uncertainties file, \"None\" is allowed"
    print "  4. Whether to use relative differences instead of absolute, False is default"
    print "  5. Input ROOT file in proper format"
    print "     Typically should be the output file from util/MakeCorrelationMatrixPlots.cxx"
    print "  6+ Additional input ROOT files in proper format (optional)"
    print "     If additional files are specified, minimum correlation differences are plotted"
    sys.exit(1)

# Store arguments and open inputs for reading
jetDef = sys.argv[1]
plotFileName = sys.argv[2]
if plotFileName.endswith(".root"):
    print "Output plot file appears to be a root file, blocking for safety to not overwrite inputs"
    print "Check input file:",plotFileName
    sys.exit(2)
corrUncFileName = sys.argv[3]
corrUncFile = None if not corrUncFileName.endswith(".root") else TFile.Open(corrUncFileName,"READ")

# Relative or absolute differences
CorrMat4D.relativeMetric = TString(sys.argv[4]).EqualTo("true",TString.kIgnoreCase)
SetAxisRange.relativeAxis = CorrMat4D.relativeMetric


inFileNames = sys.argv[5:]
inFiles = [TFile.Open(name,"READ") for name in inFileNames]

# Get the fixed values of pt1,pt2 and eta1,eta2 which are available in the files
print "Checking and retrieving fixed pT/eta scan values from input files"
fixedPtX,fixedPtY,fixedEtaX,fixedEtaY = getFixedValuesFromFiles(inFiles,jetDef)


# Set the style settings for the histograms
set_style()



# Build the raw difference matrices per file
print "Building raw 4D difference histograms by file"
rawDiffPt  = buildAndFillHists4DFromFiles(inFiles,jetDef,"eta","pt",fixedPtX,fixedPtY,"diff_",granularityFactor) if len(fixedPtX)>0 and len(fixedPtY)>0 else None
rawDiffEta = buildAndFillHists4DFromFiles(inFiles,jetDef,"pt","eta",fixedEtaX,fixedEtaY,"diff_",granularityFactor) if len(fixedEtaX)>0 and len(fixedEtaY)>0 else None

# Build the nominal histograms if multiple files for use below
nominalPt  = buildAndFillHists4DFromFiles(inFiles[0],jetDef,"eta","pt",fixedPtX,fixedPtY,"%s_0_"%(jetDef),granularityFactor)   if len(fixedPtX)>0  and len(fixedPtY)>0  and len(inFiles)>1 else None
nominalEta = buildAndFillHists4DFromFiles(inFiles[0],jetDef,"pt","eta",fixedEtaX,fixedEtaY,"%s_0_"%(jetDef),granularityFactor) if len(fixedEtaX)>0 and len(fixedEtaY)>0 and len(inFiles)>1 else None
if nominalPt:
    if len(nominalPt) > 1:
        print "Unexpected length of the nominal pT set, expected 1 but got ",len(nominalPt)
        sys.exit(3)
    else:
        nominalPt = nominalPt[0]
if nominalEta:
    if len(nominalEta) > 1:
        print "Unexpected length of the nominal eta set, expected 1 but got ",len(nominalEta)
        sys.exit(3)
    else:
        nominalEta = nominalEta[0]




# Get correlation uncertainties if provided
# Uncertainty is largest difference from 0 when doing any combination of subtractions
# So we need to build the 4D matrices for each scenario
# Then subtract them in each possible way
# Then fill the uncertainty matrix with the largest resulting value for each point
print "Building 4D correlation uncertainty histograms"
alternatesPt  = buildAndFillHists4DFromFile(corrUncFile,jetDef,"eta","pt",fixedPtX,fixedPtY,"diff_",granularityFactor)   if corrUncFile and len(fixedPtX)>0  and len(fixedPtY)>0  else None
alternatesEta = buildAndFillHists4DFromFile(corrUncFile,jetDef,"pt","eta",fixedEtaX,fixedEtaY,"diff_",granularityFactor) if corrUncFile and len(fixedEtaX)>0 and len(fixedEtaY)>0 else None
# Now determine the maximum value of the envelope for each point
alternatesUncPt  = buildAndFillHists4DFromEnvelopeOfSet(alternatesPt)  if alternatesPt  else None
alternatesUncEta = buildAndFillHists4DFromEnvelopeOfSet(alternatesEta) if alternatesEta else None





# Build minimum difference histograms from the set of 4D matrices if more than one
print "Building 4D minimum difference histograms (with respect to nominal)"
minDiffPt  = buildAndFillHist4DFromMinOfSet(rawDiffPt)  if rawDiffPt  and len(rawDiffPt)>1  else None
minDiffEta = buildAndFillHist4DFromMinOfSet(rawDiffEta) if rawDiffEta and len(rawDiffEta)>1 else None




# Build per-file difference histograms from the set of 4D matrices if more than one
print "Building 4D file-by-file difference histograms"
diffsPt  = buildAndFillHists4DFromDifferences(rawDiffPt)  if rawDiffPt  and len(rawDiffPt)>1  else None
diffsEta = buildAndFillHists4DFromDifferences(rawDiffEta) if rawDiffEta and len(rawDiffEta)>1 else None

# Build maximum difference histogram from the set of difference matrices if existing
print "Building 4D maximum difference histograms (with respect to each other)"
maxDiffPt  = buildAndFillHist4DFromMaxOfSet(diffsPt)  if diffsPt  and len(diffsPt)>1  else None
maxDiffEta = buildAndFillHist4DFromMaxOfSet(diffsEta) if diffsEta and len(diffsEta)>1 else None

# Set absolute value for max diff histograms if they exist, as the order was arbitrary
if maxDiffPt:  maxDiffPt.applyAbsValue()
if maxDiffEta: maxDiffEta.applyAbsValue()





# Build coverage plot of styles 1 (Metric2) and 2 (correlations of uncovered points)
print "Building 4D coverage histograms"
coveragePt  = buildAndFillHist4DFromCoverageOfSet(minDiffPt,maxDiffPt,1)   if rawDiffPt  and len(rawDiffPt)>1  else None
coverageEta = buildAndFillHist4DFromCoverageOfSet(minDiffEta,maxDiffEta,1) if rawDiffEta and len(rawDiffEta)>1 else None
#coverageCorrPt  = buildAndFillHist4DFromCoverageOfSet(minDiffPt,maxDiffPt,2,nominalPt)    if rawDiffPt  and len(rawDiffPt)>1  else None
#coverageCorrEta = buildAndFillHist4DFromCoverageOfSet(minDiffEta,maxDiffEta,2,nominalEta) if rawDiffEta and len(rawDiffEta)>1 else None
if coveragePt:  coveragePt.plotType  = "Metric2"
if coverageEta: coverageEta.plotType = "Metric2"





# Build coverage plots as above, but including correlation uncertainties (Metric3 and variant)
print "Building 4D coverage histograms including correlation uncertainties"
coverageWithUncPt  = buildAndFillHist4DFromCoverageOfSet(coveragePt,alternatesUncPt,1)   if coveragePt  and alternatesUncPt  else None
coverageWithUncEta = buildAndFillHist4DFromCoverageOfSet(coverageEta,alternatesUncEta,1) if coverageEta and alternatesUncEta else None
#coverageCorrWithUncPt  = buildAndFillHist4DFromCoverageOfSet(coveragePt,alternatesUncPt,2,nominalPt)    if coveragePt  and alternatesUncPt  else None
#coverageCorrWithUncEta = buildAndFillHist4DFromCoverageOfSet(coverageEta,alternatesUncEta,2,nominalEta) if coverageEta and alternatesUncEta else None
if coverageWithUncPt:  coverageWithUncPt.plotType  = "Metric3"
if coverageWithUncEta: coverageWithUncEta.plotType = "Metric3"







# Prepare the canvas
print "Writing results to file: ",(re.sub(".eps","-*.eps",plotFileName) if plotFileName.endswith(".eps") else (re.sub(".png","-*.png",plotFileName) if plotFileName.endswith(".png") else plotFileName))
canvas = TCanvas("canvas","4D Correlation Matrix",0,0,2500,2400)
canvas.SetLeftMargin(0.10 if not CorrMat4D.relativeMetric else 0.08)
canvas.SetRightMargin(0.15 if not CorrMat4D.relativeMetric else 0.17)
canvas.SetBottomMargin(0.04)
canvas.SetTopMargin(0.16)
canvas.SetLogz(CorrMat4D.relativeMetric)
canvas.cd()
canvas.Draw()

# Write the plots
if not (plotFileName.endswith(".eps") or plotFileName.endswith(".png")):
    canvas.Print(plotFileName+"[")

saveHists4D(canvas,plotFileName,rawDiffPt,            not CorrMat4D.relativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f")
saveHists4D(canvas,plotFileName,minDiffPt,            not CorrMat4D.relativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f")
saveHists4D(canvas,plotFileName,coveragePt,           not CorrMat4D.relativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f")
#saveHists4D(canvas,plotFileName,coverageCorrPt,       not CorrMat4D.relativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f")
saveHists4D(canvas,plotFileName,coverageWithUncPt,    not CorrMat4D.relativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f")
#saveHists4D(canvas,plotFileName,coverageCorrWithUncPt,not CorrMat4D.relativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f")

saveHists4D(canvas,plotFileName,rawDiffEta,            not CorrMat4D.relativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f")
saveHists4D(canvas,plotFileName,minDiffEta,            not CorrMat4D.relativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f")
saveHists4D(canvas,plotFileName,coverageEta,           not CorrMat4D.relativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f")
#saveHists4D(canvas,plotFileName,coverageCorrEta,       not CorrMat4D.relativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f")
saveHists4D(canvas,plotFileName,coverageWithUncEta,    not CorrMat4D.relativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f")
#saveHists4D(canvas,plotFileName,coverageCorrWithUncEta,not CorrMat4D.relativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f")

if not (plotFileName.endswith(".eps") or plotFileName.endswith(".png")):
    canvas.Print(plotFileName+"]")


