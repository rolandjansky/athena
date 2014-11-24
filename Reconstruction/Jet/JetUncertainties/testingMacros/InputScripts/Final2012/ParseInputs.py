# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import sys
import os
import glob
import re
import math

from ParseEtaIntercalInput  import ReadEtaIntercalibrationHistograms
from ParseInsituInput       import ReadInSituHistograms
from ParsePileupInput       import ReadPileupHistograms
from ParseNonClosureInput   import ReadNonClosureHistograms
from ParseHighPtInput       import ReadHighPtHistograms
from ParseFlavourInput      import ReadFlavourHistograms
from ParsebJESInput         import ReadBJESHistograms
from ParsePunchthroughInput import ReadPunchthroughHistograms


#http://stackoverflow.com/questions/2669059/how-to-sort-alpha-numeric-set-in-python
from itertools import groupby
def natural_sort(s):
    return [int(''.join(g)) if k else ''.join(g) for k, g in groupby(s[0], str.isdigit)]

# Check useage and store arguments
if len(sys.argv) < 9:
    print "USAGE: Expected the following arguments"
    print "     1. Eta intercalibration directory path"
    print "     2. InSitu directory path"
    print "     3. Pileup directory path"
    print "     4. MC nonclosure directory path"
    print "     5. HighPt directory path"
    print "     6. Flavour directory path"
    print "     7. bJES directory path"
    print "     8. Punchthrough directory path"
    exit(1)

outBaselineFile = "JESUncertainty_BaseComponents_2012.root"
outMultijetFile = "MJESUncertainty_2012.root"
etaIntercalDir  = sys.argv[1]
inSituDir       = sys.argv[2]
pileupDir       = sys.argv[3]
nonclosureDir   = sys.argv[4]
highPtDir       = sys.argv[5]
flavourDir      = sys.argv[6]
bJESDir         = sys.argv[7]
punchthroughDir = sys.argv[8]

# Ensure that all of the directories exist
if not outBaselineFile.endswith(".root"):
    print "Output baseline ROOT file doesn't appear to be a root file:",outBaselineFile
    exit(2)
if not outMultijetFile.endswith(".root"):
    print "Output multijet ROOT file doesn't appear to be a root file:",outMultijetFile
    exit(3)
if not os.path.exists(etaIntercalDir):
    print "Eta intercalibration directory does not exist:",etaIntercalDir
    exit(4)
if not os.path.exists(inSituDir):
    print "InSitu directory does not exist:",inSituDir
    exit(5)
if not os.path.exists(pileupDir):
    print "Pileup directory does not exist:",pileupDir
    exit(6)
if not os.path.exists(nonclosureDir):
    print "MC nonclosure directory does not exist:",nonclosureDir
    exit(7)
if not os.path.exists(highPtDir):
    print "HighPt directory does not exist:",highPtDir
    exit(8)
if not os.path.exists(flavourDir):
    print "Flavour directory does not exist:",flavourDir
    exit(9)
if not os.path.exists(bJESDir):
    print "bJES directory does not exist:",bJESDir
    exit(10)
if not os.path.exists(punchthroughDir):
    print "Punchthrough directory does not exist:",punchthroughDir
    exit(11)

# Store everything in memory!
currentDir = gDirectory
gROOT.cd()


# Now read the histograms
print "Reading inputs..."
etaIntercalHistos   = ReadEtaIntercalibrationHistograms(etaIntercalDir)
inSituHistos        = ReadInSituHistograms(inSituDir)
pileupHistos        = ReadPileupHistograms(pileupDir)
nonclosureHistos    = ReadNonClosureHistograms(nonclosureDir)
highPtHistos        = ReadHighPtHistograms(highPtDir)
flavourHistos       = ReadFlavourHistograms(flavourDir)
bJESHistos          = ReadBJESHistograms(bJESDir)
punchthroughHistos  = ReadPunchthroughHistograms(punchthroughDir)

#print etaIntercalHistos

# Make one mega-dictionary
print "Merging inputs..."
jetDefs = ["AntiKt4Topo_EMJES","AntiKt4Topo_LCJES","AntiKt6Topo_EMJES","AntiKt6Topo_LCJES"]
systematics = {}
for aJetDef in jetDefs:
    systematics[aJetDef] = dict(
                                etaIntercalHistos[aJetDef].items()  +
                                inSituHistos[aJetDef].items()       +
                                pileupHistos[aJetDef].items()       +
                                nonclosureHistos[aJetDef].items()   +
                                highPtHistos[aJetDef].items()       +
                                flavourHistos[aJetDef].items()      +
                                bJESHistos[aJetDef].items()         +
                                punchthroughHistos[aJetDef].items()
                               )

# Loop over the mega-dictionary and write results
print "Writing to output file..."
baselineFile = TFile(outBaselineFile,"RECREATE")
multijetFile = TFile(outMultijetFile,"RECREATE")
for aJetDef,aSyst in sorted(systematics.iteritems(),key=natural_sort):
    for aSystName,aSystHisto in sorted(aSyst.iteritems(),key=natural_sort):
        if "bJES" in aSystName or "flavor" in aSystName or "Flavor" in aSystName or "PunchThrough" in aSystName or "Close" in aSystName:
            multijetFile.cd()
        else:
            baselineFile.cd()
        
        aSystHisto.SetTitle(aSystName+"_"+aJetDef)
        aSystHisto.Write(aSystHisto.GetTitle())

# Done, close the files, revert directory
baselineFile.Close()
multijetFile.Close()
gDirectory = currentDir
print "Done!"

