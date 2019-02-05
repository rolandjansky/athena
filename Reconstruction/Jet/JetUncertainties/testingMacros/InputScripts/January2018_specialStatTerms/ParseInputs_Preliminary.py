from ROOT import *
from array import array
import sys
import os
import glob
import re
import math

from ParseEtaIntercalInput  import ReadEtaIntercalibrationHistograms
from ParseInsituInput_MJB       import ReadInSituHistograms
from ParsePileupInput       import ReadPileupHistograms
from ParseHighPtInput       import ReadHighPtHistograms,ReadHighPtHistogramsFromOldFile
from ParseFlavourInput      import ReadFlavourHistograms
from ParsebJESInput         import ReadBJESHistograms
from ParsePunchthroughInput import ReadPunchthroughHistograms
from CreateNonClosureInput  import CreateNonClosureHistograms

#http://stackoverflow.com/questions/2669059/how-to-sort-alpha-numeric-set-in-python
from itertools import groupby
def natural_sort(s):
    return [int(''.join(g)) if k else ''.join(g) for k, g in groupby(s[0], str.isdigit)]

# Design: This is to make a file entirely from scratch. 
# At present the high pT term has not been updated since 2012, so 
# that one will be retrieved from the final 2012 recommendations. 
# All else is needed new. 

# Full list of required inputs, distinguished by needing them
# from a different person
# - In situ terms
# - Eta intercalibration
# - Single particle
# - MC non-closure
# - Pileup
# - Flavour
# - B-JES
# - Punchthrough


# NOTES
# Need to carefully check all flavour histograms in comparison to 2012 to make sure they look sensible!!
# Things below were done in pre-recs and are currently being done here:
#  - Freeze flavour histograms by hand wherever input histogram falls below 0: extrapolate forward in pT from around 2 TeV centrally and from lower pT as eta increases.
#  - FlavorResponse histogram inverted from input file to match Run I convention
freezeFlavourInPt = False

##########################################

# Check useage and store arguments
if len(sys.argv) < 3:
    print "USAGE: Expected the following arguments"
    print "     1. InSitu directory path"
    print "     2. Eta intercalibration directory path"
    print "     3. Path or directory for single-particle: currently 2012 recommendation"
    print "     5. Pileup directory path"
    print "     6. Flavour directory path"
    print "     7. bJES directory path"
    print "     8. Punchthrough directory path"
    exit(1)

outBaselineFile = "JESUncertainty_AllComponents.root"
inSituDir      = sys.argv[1]
etaIntercalDir  = sys.argv[2]
highPtDir       = sys.argv[3]
pileupDir       = sys.argv[4]
flavourDir      = sys.argv[5]
bJESDir         = sys.argv[6]
punchthroughDir = sys.argv[7]

# Ensure that all of the directories exist
if not outBaselineFile.endswith(".root"):
    print "Output baseline ROOT file doesn't appear to be a root file:",outBaselineFile
    exit(2)
if not os.path.exists(inSituDir):
    print "InSitu directory does not exist:",inSituDir
    exit(3)
if not os.path.exists(etaIntercalDir):
    print "Eta intercalibration directory does not exist:",etaIntercalDir
    exit(4)
if not os.path.exists(highPtDir):
    print "HighPt directory does not exist:",highPtDir
    exit(5)
if not os.path.exists(pileupDir):
    print "Pileup directory does not exist:",pileupDir
    exit(7)
if not os.path.exists(flavourDir):
    print "Flavour directory does not exist:",flavourDir
    exit(8)
if not os.path.exists(bJESDir):
    print "bJES directory does not exist:",bJESDir
    exit(9)
if not os.path.exists(punchthroughDir):
    print "Punchthrough directory does not exist:",punchthroughDir
    exit(10)

# Store everything in memory!
currentDir = gDirectory
gROOT.cd()

# Now read the histograms
print "Reading inputs..."
# For now, the high pT component
# is read in from the final 2012 calibration files.
inSituHistos        = ReadInSituHistograms(inSituDir)
etaIntercalHistos   = ReadEtaIntercalibrationHistograms(etaIntercalDir)
highPtHistos        = ReadHighPtHistogramsFromOldFile(highPtDir)#ReadHighPtHistograms(highPtDir)
pileupHistos        = ReadPileupHistograms(pileupDir)
flavourHistos       = ReadFlavourHistograms(flavourDir,freezeFlavourInPt) # True flag freezes uncertainty above pT = 2TeV (lower at higher eta)
bJESHistos          = ReadBJESHistograms(bJESDir)
punchthroughHistos  = ReadPunchthroughHistograms(punchthroughDir)
nonClosureHists     = CreateNonClosureHistograms()

# Make one mega-dictionary
print "Merging inputs..."
jetDefs = {"AntiKt4Topo_EMJES" : "AntiKt4EMTopo", "AntiKt4PFlow_EMJES" : "AntiKt4EMPFlow"}
systematics = {}
for aJetDef in jetDefs.keys():

    systematics[aJetDef] = dict(
                                inSituHistos[aJetDef].items()       +
                                etaIntercalHistos[aJetDef].items()  +
                                highPtHistos[aJetDef].items()       +
                                pileupHistos[aJetDef].items()       +
                                flavourHistos[aJetDef].items()      +
                                bJESHistos[aJetDef].items()         +
                                punchthroughHistos[aJetDef].items() +
                                nonClosureHists[aJetDef].items()
                               )

# Loop over the mega-dictionary and write results
print "Writing to output file",outBaselineFile,"..."
baselineFile = TFile(outBaselineFile,"RECREATE")
for aJetDef,aSyst in sorted(systematics.iteritems(),key=natural_sort):
    for aSystName,aSystHisto in sorted(aSyst.iteritems(),key=natural_sort):
        baselineFile.cd()
        aSystHisto.SetTitle(aSystName+"_"+jetDefs[aJetDef])
        aSystHisto.Write(aSystHisto.GetTitle())

# Done, close the files, revert directory
baselineFile.Close()
gDirectory = currentDir
print "Done!"

