# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import sys
import os
import glob
import re
import math

from Parse2012Input         import Read2012Histograms
from ParseEtaIntercalInput  import ReadEtaIntercalibrationHistograms
from ParseInsituInput       import ReadInSituHistograms
from ParsePileupInput       import ReadPileupHistograms
from ParseNonClosureInput   import ReadNonClosureHistograms
from ParseHighPtInput       import ReadHighPtHistograms
from ParseFlavourInput      import ReadFlavourHistograms
from ParsebJESInput         import ReadBJESHistograms
from ParsePunchthroughInput import ReadPunchthroughHistograms
from ParseCrossCalInput     import ReadCrossCalHistograms,ReadCrossCalValidityHistograms

#http://stackoverflow.com/questions/2669059/how-to-sort-alpha-numeric-set-in-python
from itertools import groupby
def natural_sort(s):
    return [int(''.join(g)) if k else ''.join(g) for k, g in groupby(s[0], str.isdigit)]

# NOTES
# Option 1
#  - Freeze cross-calibration histograms by hand outside of eta=0.8 to twice the value of the last sensible bin
#  - Freeze flavour histograms by hand wherever input histogram falls below 0: extrapolate forward in pT from around 2 TeV centrally and from lower pT as eta increases.
#  - Set eta-intercalibration by hand to:
#    - The run-1 uncertainty for |eta| < 0.8
#    - Twice the run-1 uncertainty /in these same bins/ for 0.8 < |eta| < 2.5
#    - Three times the run-1 uncertainty /in these same bins/ for 2.5 < |eta|
#  - FlavorResponse histogram inverted from input file to match Run I convention
#  - Pileup values not changed!!
# Use flag OPT1

# Option 2
# 

flag = "NOMINAL"

##########################################

if flag == "NOMINAL" :
  ext = "3EtaScaleRegions_FreezeCrossCalEverywhereSingle"
  scaleCrossCalVal=1.0
  scaleEtaInter3Regions = True
  scaleEtaInter2Regions = False
  freezeCrossCalAll = True
  freezeCrossCalBarrel = False
  freezeCrossCalCentral = False
  freezeFlavourInPt = True

if flag == "OPT1" :
  ext = "3EtaScaleRegions_FreezeCrossCalEverywhereDouble"
  scaleCrossCalVal=2.0
  scaleEtaInter3Regions = True
  scaleEtaInter2Regions = False
  freezeCrossCalAll = True
  freezeCrossCalBarrel = False
  freezeFlavourInPt = True
  freezeCrossCalCentral = False

if flag == "OPT2" :
  ext = "2EtaScaleRegions_FreezeCrossCalBarrelSingle"
  scaleCrossCalVal=1.0
  scaleEtaInter3Regions = False
  scaleEtaInter2Regions = True
  freezeCrossCalAll = False
  freezeCrossCalBarrel = True
  freezeCrossCalCentral = False
  freezeFlavourInPt = True

if flag == "OPT3" :
  ext = "3EtaScaleRegions_FreezeCrossCalBarrelSingle"
  scaleCrossCalVal=1.0
  scaleEtaInter3Regions = True
  scaleEtaInter2Regions = False
  freezeCrossCalAll = False
  freezeCrossCalBarrel = True
  freezeFlavourInPt = True
  freezeCrossCalCentral = False

if flag == "OPT4" :
  ext = "2EtaScaleRegions_FreezeCrossCalEverywhereDouble"
  scaleCrossCalVal=2.0
  scaleEtaInter3Regions = False
  scaleEtaInter2Regions = True
  freezeCrossCalAll = True
  freezeCrossCalBarrel = False
  freezeCrossCalCentral = False
  freezeFlavourInPt = True

# Check useage and store arguments
if len(sys.argv) < 3:
    print "USAGE: Expected the following arguments"
    print "     1. Directory path to 2012 final calibrations"
    print "     2. Eta intercalibration directory path"
#    print "     2. InSitu directory path"
    print "     3. Pileup directory path"
    print "     4. MC nonclosure directory path"
#    print "     5. HighPt directory path"
    print "     5. Flavour directory path"
#    print "     7. bJES directory path"
#    print "     8. Punchthrough directory path"
    print "     6. Cross calibration directory path"
    exit(1)

outBaselineFile = "JESUncertainty_BaseComponents_{0}.root".format(ext)
#outMultijetFile = "MJESUncertainty_2015.root"
the2012Dir      = sys.argv[1]
etaIntercalDir  = sys.argv[2]
#inSituDir       = sys.argv[2]
pileupDir       = sys.argv[3]
nonclosureDir   = sys.argv[4]
#highPtDir       = sys.argv[5]
flavourDir      = sys.argv[5]
#bJESDir         = sys.argv[7]
#punchthroughDir = sys.argv[8]
crosscalDir     = sys.argv[6]

# Ensure that all of the directories exist
if not outBaselineFile.endswith(".root"):
    print "Output baseline ROOT file doesn't appear to be a root file:",outBaselineFile
    exit(2)
#if not outMultijetFile.endswith(".root"):
#    print "Output multijet ROOT file doesn't appear to be a root file:",outMultijetFile
#    exit(3)
if not os.path.exists(the2012Dir):
    print "2012 directory does not exist:",the2012Dir
    exit(3)
if not os.path.exists(etaIntercalDir):
    print "Eta intercalibration directory does not exist:",etaIntercalDir
    exit(4)
#if not os.path.exists(inSituDir):
#    print "InSitu directory does not exist:",inSituDir
#    exit(5)
if not os.path.exists(pileupDir):
    print "Pileup directory does not exist:",pileupDir
    exit(6)
if not os.path.exists(nonclosureDir):
    print "MC nonclosure directory does not exist:",nonclosureDir
    exit(7)
#if not os.path.exists(highPtDir):
#    print "HighPt directory does not exist:",highPtDir
#    exit(8)
if not os.path.exists(flavourDir):
    print "Flavour directory does not exist:",flavourDir
    exit(9)
#if not os.path.exists(bJESDir):
#    print "bJES directory does not exist:",bJESDir
#    exit(10)
#if not os.path.exists(punchthroughDir):
#    print "Punchthrough directory does not exist:",punchthroughDir
#    exit(11)
if not os.path.exists(crosscalDir):
    print "Cross calibration directory does not exist:",crosscalDir
    exit(12)

# Store everything in memory!
currentDir = gDirectory
gROOT.cd()

# Now read the histograms
print "Reading inputs..."
# For now, everything except flavour and cross calibration inputs
# are read in from the final 2012 calibration files.
the2012Histos = Read2012Histograms(the2012Dir,scaleEtaInter3Regions,scaleEtaInter2Regions) # First true activates eta-intercalibration scaling. Second true turns off relative in situ components at large eta
etaIntercalHistos   = ReadEtaIntercalibrationHistograms(etaIntercalDir)
#inSituHistos        = ReadInSituHistograms(inSituDir)
pileupHistos        = ReadPileupHistograms(pileupDir)
nonclosureHistos    = ReadNonClosureHistograms(nonclosureDir,True)
#highPtHistos        = ReadHighPtHistograms(highPtDir)
flavourHistos       = ReadFlavourHistograms(flavourDir,freezeFlavourInPt) # True flag freezes uncertainty above pT = 2TeV (lower at higher eta)
#bJESHistos          = ReadBJESHistograms(bJESDir)
#punchthroughHistos  = ReadPunchthroughHistograms(punchthroughDir)
crosscalHistos = ReadCrossCalHistograms(crosscalDir,scaleCrossCalVal,freezeCrossCalAll,freezeCrossCalBarrel,freezeCrossCalCentral) # First true freezes uncertainty above eta = 0.8. Second freezes it between 0.8 and 2.5 and sets it to 0 outside 2.5. Third turns off uncertainty above eta = 0.8. Use only one!
crosscalValHistos   = ReadCrossCalValidityHistograms(crosscalDir)

# Make one mega-dictionary
print "Merging inputs..."
jetDefs = {"AntiKt4Topo_EMJES" : "AntiKt4EMTopo"}#,"AntiKt4Topo_LCJES","AntiKt6Topo_EMJES","AntiKt6Topo_LCJES"]
systematics = {}
validities = {}
for aJetDef in jetDefs.keys():
    systematics[aJetDef] = dict(
                                the2012Histos[aJetDef].items() +
                                etaIntercalHistos[aJetDef].items()  +
#                                inSituHistos[aJetDef].items()       +
                                pileupHistos[aJetDef].items()       +
                                nonclosureHistos[aJetDef].items()   +
#                                highPtHistos[aJetDef].items()       +
                                flavourHistos[aJetDef].items()      +
#                                bJESHistos[aJetDef].items()         +
#                                punchthroughHistos[aJetDef].items() +
                                crosscalHistos[aJetDef].items()
                               )
    validities[aJetDef] = dict(
                                crosscalValHistos[aJetDef].items()
                              )

# Loop over the mega-dictionary and write results
print "Writing to output file",outBaselineFile,"..."
baselineFile = TFile(outBaselineFile,"RECREATE")
#multijetFile = TFile(outMultijetFile,"RECREATE")
for aJetDef,aSyst in sorted(systematics.iteritems(),key=natural_sort):
    for aSystName,aSystHisto in sorted(aSyst.iteritems(),key=natural_sort):
#        if "bJES" in aSystName or "flavor" in aSystName or "Flavor" in aSystName or "PunchThrough" in aSystName or "Close" in aSystName:
#            multijetFile.cd()
#        else:
        baselineFile.cd()
        aSystHisto.SetTitle(aSystName+"_"+jetDefs[aJetDef])
        aSystHisto.Write(aSystHisto.GetTitle())
for aJetDef,aVal in sorted(validities.iteritems(),key=natural_sort):
    for aSystName,aValHisto in sorted(aVal.iteritems(),key=natural_sort):
        baselineFile.cd()
        aValHisto.SetTitle("valid_"+aSystName+"_"+jetDefs[aJetDef])

# Done, close the files, revert directory
baselineFile.Close()
#multijetFile.Close()
gDirectory = currentDir
print "Done!"

