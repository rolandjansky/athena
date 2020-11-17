# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Script by Steven Schramm, originally based off of 4D histogram code by Sophie Pataraia
# somewhat altered by Kate, Dec 2016

from ROOT import *

from PlotHelpers import *

from inspect import getmembers, isfunction

print "Importing my new stuff"

import CorrelationMatrixHelpers
#from CorrelationMatrixHelpers import *

# Controls
print "Setting controls"
#isSquaredMetric = False
granularityFactor = 1
SetAxisRange.forceOneSided = True
#DrawLabels.ATLASLabelName = "Preliminary"
drawATLASLabel = True
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

print "Getting arguments"

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
print "Setting up matrices"
useRelativeMetric = TString(sys.argv[4]).EqualTo("true",TString.kIgnoreCase)
SetAxisRange.relativeAxis = useRelativeMetric

print "Getting file names"
inFileNames = sys.argv[5:]
inFiles = [TFile.Open(name,"READ") for name in inFileNames]

# Get the fixed values of pt1,pt2 and eta1,eta2 which are available in the files
print "Checking and retrieving fixed pT/eta scan values from input files"
fixedPtX,fixedPtY,fixedEtaX,fixedEtaY = CorrelationMatrixHelpers.getFixedValuesFromFiles(inFiles,jetDef)

print fixedPtX,fixedPtY, fixedEtaX, fixedEtaY


# Set the style settings for the histograms
set_style()

# Build the raw difference matrices per file
print "Building raw 4D difference histograms by file"
rawDiffPt  = CorrelationMatrixHelpers.buildAndFillHists4DFromFiles(inFiles,jetDef,"eta","pt",fixedPtX,fixedPtY,"diff_",granularityFactor,relativeMetric=useRelativeMetric) if len(fixedPtX)>0 and len(fixedPtY)>0 else None
rawDiffEta = CorrelationMatrixHelpers.buildAndFillHists4DFromFiles(inFiles,jetDef,"pt","eta",fixedEtaX,fixedEtaY,"diff_",granularityFactor,relativeMetric=useRelativeMetric) if len(fixedEtaX)>0 and len(fixedEtaY)>0 else None

print "1) still defined as:",fixedPtX,fixedPtY,fixedEtaX,fixedEtaY

# Build the nominal histograms if multiple files for use below
nominalPt  = CorrelationMatrixHelpers.buildAndFillHists4DFromFiles(inFiles[0],jetDef,"eta","pt",fixedPtX,fixedPtY,"%s_0_"%(jetDef),granularityFactor,relativeMetric=useRelativeMetric)   if len(fixedPtX)>0  and len(fixedPtY)>0  and len(inFiles)>1 else None
nominalEta = CorrelationMatrixHelpers.buildAndFillHists4DFromFiles(inFiles[0],jetDef,"pt","eta",fixedEtaX,fixedEtaY,"%s_0_"%(jetDef),granularityFactor,relativeMetric=useRelativeMetric) if len(fixedEtaX)>0 and len(fixedEtaY)>0 and len(inFiles)>1 else None
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

print "2) still defined as:",fixedPtX,fixedPtY,fixedEtaX,fixedEtaY



# Get correlation uncertainties if provided
# Uncertainty is largest difference from 0 when doing any combination of subtractions
# So we need to build the 4D matrices for each scenario
# Then subtract them in each possible way
# Then fill the uncertainty matrix with the largest resulting value for each point
print "Building 4D correlation uncertainty histograms"
alternatesPt  = CorrelationMatrixHelpers.buildAndFillHists4DFromFile(corrUncFile,jetDef,"eta","pt",fixedPtX,fixedPtY,"diff_",granularityFactor,relativeMetric=useRelativeMetric)   if corrUncFile and len(fixedPtX)>0  and len(fixedPtY)>0  else None
alternatesEta = CorrelationMatrixHelpers.buildAndFillHists4DFromFile(corrUncFile,jetDef,"pt","eta",fixedEtaX,fixedEtaY,"diff_",granularityFactor,relativeMetric=useRelativeMetric) if corrUncFile and len(fixedEtaX)>0 and len(fixedEtaY)>0 else None
# Now determine the maximum value of the envelope for each point
alternatesUncPt  = CorrelationMatrixHelpers.buildAndFillHists4DFromEnvelopeOfSet(alternatesPt,relativeMetric=useRelativeMetric)  if alternatesPt  else None
alternatesUncEta = CorrelationMatrixHelpers.buildAndFillHists4DFromEnvelopeOfSet(alternatesEta,relativeMetric=useRelativeMetric) if alternatesEta else None


print "3) still defined as:",fixedPtX,fixedPtY,fixedEtaX,fixedEtaY



# Build minimum difference histograms from the set of 4D matrices if more than one
print "Building 4D minimum difference histograms (with respect to nominal)"
minDiffPt  = CorrelationMatrixHelpers.buildAndFillHist4DFromMinOfSet(rawDiffPt,relativeMetric=useRelativeMetric)  if rawDiffPt  and len(rawDiffPt)>1  else None
minDiffEta = CorrelationMatrixHelpers.buildAndFillHist4DFromMinOfSet(rawDiffEta,relativeMetric=useRelativeMetric) if rawDiffEta and len(rawDiffEta)>1 else None


# Build per-file difference histograms from the set of 4D matrices if more than one
print "Building 4D file-by-file difference histograms"
diffsPt  = CorrelationMatrixHelpers.buildAndFillHists4DFromDifferenceHists(rawDiffPt,relativeMetric=useRelativeMetric)  if rawDiffPt  and len(rawDiffPt)>1  else None
diffsEta = CorrelationMatrixHelpers.buildAndFillHists4DFromDifferenceHists(rawDiffEta,relativeMetric=useRelativeMetric) if rawDiffEta and len(rawDiffEta)>1 else None

# Build maximum difference histogram from the set of difference matrices if existing
print "Building 4D maximum difference histograms (with respect to each other)"
maxDiffPt  = CorrelationMatrixHelpers.buildAndFillHist4DFromMaxOfSet(diffsPt,relativeMetric=useRelativeMetric)  if diffsPt  and len(diffsPt)>1  else None
maxDiffEta = CorrelationMatrixHelpers.buildAndFillHist4DFromMaxOfSet(diffsEta,relativeMetric=useRelativeMetric) if diffsEta and len(diffsEta)>1 else None

# Set absolute value for max diff histograms if they exist, as the order was arbitrary
if maxDiffPt:  maxDiffPt.applyAbsValue()
if maxDiffEta: maxDiffEta.applyAbsValue()


print "4) still defined as:",fixedPtX,fixedPtY,fixedEtaX,fixedEtaY



# Build coverage plot of styles 1 (Metric2) and 2 (correlations of uncovered points)
print "Building 4D coverage histograms"
coveragePt  = CorrelationMatrixHelpers.buildAndFillHist4DFromCoverageOfSet(minDiffPt,maxDiffPt,1,relativeMetric=useRelativeMetric)   if rawDiffPt  and len(rawDiffPt)>1  else None
coverageEta = CorrelationMatrixHelpers.buildAndFillHist4DFromCoverageOfSet(minDiffEta,maxDiffEta,1,relativeMetric=useRelativeMetric) if rawDiffEta and len(rawDiffEta)>1 else None
#coverageCorrPt  = buildAndFillHist4DFromCoverageOfSet(minDiffPt,maxDiffPt,2,nominalPt)    if rawDiffPt  and len(rawDiffPt)>1  else None
#coverageCorrEta = buildAndFillHist4DFromCoverageOfSet(minDiffEta,maxDiffEta,2,nominalEta) if rawDiffEta and len(rawDiffEta)>1 else None
if coveragePt:  coveragePt.plotType  = "Metric2"
if coverageEta: coverageEta.plotType = "Metric2"


print "5) still defined as:",fixedPtX,fixedPtY,fixedEtaX,fixedEtaY

saveFixedEtaY = fixedEtaY[:]
saveFixedEtaX = fixedEtaX[:]
saveFixedPtX = fixedPtX[:]
saveFixedPtY = fixedPtY[:]

# Build coverage plots as above, but including correlation uncertainties (Metric3 and variant)
print "Building 4D coverage histograms including correlation uncertainties"
coverageWithUncPt  = CorrelationMatrixHelpers.buildAndFillHist4DFromCoverageOfSet(coveragePt,alternatesUncPt,1,relativeMetric=useRelativeMetric)   if coveragePt  and alternatesUncPt  else None
#print "a)",fixedEtaX
coverageWithUncEta = CorrelationMatrixHelpers.buildAndFillHist4DFromCoverageOfSet(coverageEta,alternatesUncEta,1,relativeMetric=useRelativeMetric) if coverageEta and alternatesUncEta else None
#print "b)",fixedEtaX
#coverageCorrWithUncPt  = buildAndFillHist4DFromCoverageOfSet(coveragePt,alternatesUncPt,2,nominalPt)    if coveragePt  and alternatesUncPt  else None
#coverageCorrWithUncEta = buildAndFillHist4DFromCoverageOfSet(coverageEta,alternatesUncEta,2,nominalEta) if coverageEta and alternatesUncEta else None
if coverageWithUncPt:  coverageWithUncPt.plotType  = "Metric3"
#print "c)",fixedEtaX
if coverageWithUncEta: coverageWithUncEta.plotType = "Metric3"

fixedEtaX = saveFixedEtaX
fixedEtaY = saveFixedEtaY
fixedPtX = saveFixedPtX
fixedPtY = saveFixedPtY

print "6) still defined as:",fixedPtX,fixedPtY,fixedEtaX,fixedEtaY


# Prepare the canvas
print "Writing results to file: ",(re.sub(".eps","-*.eps",plotFileName) if plotFileName.endswith(".eps") else (re.sub(".png","-*.png",plotFileName) if plotFileName.endswith(".png") else plotFileName))
canvas = TCanvas("canvas","4D Correlation Matrix",0,0,2600,2200)
canvas.SetLeftMargin(0.09 if not useRelativeMetric else 0.08)
canvas.SetRightMargin(0.19 if not useRelativeMetric else 0.17)
canvas.SetBottomMargin(0.04)
canvas.SetTopMargin(0.12)
canvas.SetLogz(useRelativeMetric)
canvas.cd()
canvas.Draw()

# Write the plots
if not (plotFileName.endswith(".eps") or plotFileName.endswith(".png")):
    canvas.Print(plotFileName+"[")

#rawDiffPt.iEPS = 0
#CorrelationMatrixHelpers.saveHists4D(canvas,plotFileName,rawDiffPt, not useRelativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f",scenarioLabel="",drawATLASLabel=drawATLASLabel)
#minDiffPt.iEPS = 1
#CorrelationMatrixHelpers.saveHists4D(canvas,plotFileName,minDiffPt, not useRelativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f",scenarioLabel="",drawATLASLabel=drawATLASLabel)
#coveragePt.iEPS = 2
#CorrelationMatrixHelpers.saveHists4D(canvas,plotFileName,coveragePt, not useRelativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f",scenarioLabel="",drawATLASLabel=drawATLASLabel)
##saveHists4D(canvas,plotFileName,coverageCorrPt, not useRelativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f",drawATLASLabel)
#coverageWithUncPt.iEPS = 3
#CorrelationMatrixHelpers.saveHists4D(canvas,plotFileName,coverageWithUncPt, not useRelativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f",scenarioLabel="",drawATLASLabel=drawATLASLabel)
#saveHists4D(canvas,plotFileName,coverageCorrWithUncPt,not useRelativeMetric,fixedPtX,fixedPtY,"#it{p}_{T}=%.f",drawATLASLabel)

print "here, it is",fixedEtaX

CorrelationMatrixHelpers.saveHists4D(canvas,plotFileName,rawDiffEta,not useRelativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f",scenarioLabel="",drawATLASLabel=drawATLASLabel,additionalString =4)
CorrelationMatrixHelpers.saveHists4D(canvas,plotFileName,minDiffEta,not useRelativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f",scenarioLabel="",drawATLASLabel=drawATLASLabel,additionalString =5)
CorrelationMatrixHelpers.saveHists4D(canvas,plotFileName,coverageEta,not useRelativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f",scenarioLabel="",drawATLASLabel=drawATLASLabel,additionalString=6)
#saveHists4D(canvas,plotFileName,coverageCorrEta,not useRelativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f",drawATLASLabel)
CorrelationMatrixHelpers.saveHists4D(canvas,plotFileName,coverageWithUncEta,not useRelativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f",scenarioLabel="",drawATLASLabel=drawATLASLabel,additionalString=7)
#saveHists4D(canvas,plotFileName,coverageCorrWithUncEta,not useRelativeMetric,fixedEtaX,fixedEtaY,"#eta=%.1f",drawATLASLabel)

if not (plotFileName.endswith(".eps") or plotFileName.endswith(".png")):
    canvas.Print(plotFileName+"]")


