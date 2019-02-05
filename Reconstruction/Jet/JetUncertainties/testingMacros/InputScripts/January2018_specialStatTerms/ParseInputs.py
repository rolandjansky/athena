from ROOT import *
from array import array
import sys
import os
import glob
import re
import math

from ParseEtaIntercalInput_detailed  import ReadEtaIntercalibrationHistograms
from ParseInsituInput       import ReadInSituHistograms
from ParsePileupInput       import ReadPileupHistograms
from ParseNonClosureInput   import ReadNonClosureHistograms
from ParseHighPtInput       import ReadHighPtHistograms,ReadHighPtHistogramsFromOldFile
from ParseFlavourInput      import ReadFlavourHistograms
from ParsebJESInput         import ReadBJESHistograms
from ParsePunchthroughInput import ReadPunchthroughHistograms
from CopyJERHists           import ReadJERHistograms

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
    print "     4. MC nonclosure directory path"
    print "     5. Pileup directory path"
    print "     6. Flavour directory path"
    print "     7. bJES directory path"
    print "     8. Punchthrough directory path"
    exit(1)

outBaselineFile = "JESUncertainty_AllComponents_{0}.root"
inSituDir      = sys.argv[1]
etaIntercalDir  = sys.argv[2]
highPtDir       = sys.argv[3]
nonclosureDir   = sys.argv[4]
pileupDir       = sys.argv[5]
flavourDir      = sys.argv[6]
bJESDir         = sys.argv[7]
punchthroughDir = sys.argv[8]
oldJERFile      = sys.argv[9]

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
if not os.path.exists(nonclosureDir):
    print "MC nonclosure directory does not exist:",nonclosureDir
    exit(6)
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
if not os.path.exists(oldJERFile) :
    print "You need to specify a file to pick up JER from!",oldJERFile

# Store everything in memory!
currentDir = gDirectory
gROOT.cd()

# Now read the histograms
print "Reading inputs..."
# For now, the high pT component
# is read in from the final 2012 calibration files.
inSituHistos        = ReadInSituHistograms(inSituDir)
etaIntercalHistos_20152016 = ReadEtaIntercalibrationHistograms(etaIntercalDir,"2015_16_")
etaIntercalHistos_2017 = ReadEtaIntercalibrationHistograms(etaIntercalDir,"2017_")
etaIntercalHistos_general = ReadEtaIntercalibrationHistograms(etaIntercalDir,"")
highPtHistos        = ReadHighPtHistogramsFromOldFile(highPtDir)#ReadHighPtHistograms(highPtDir)
nonclosureHistos    = ReadNonClosureHistograms(nonclosureDir,True)
pileupHistos        = ReadPileupHistograms(pileupDir)
flavourHistos       = ReadFlavourHistograms(flavourDir,freezeFlavourInPt) # True flag freezes uncertainty above pT = 2TeV (lower at higher eta)
bJESHistos          = ReadBJESHistograms(bJESDir)
punchthroughHistos  = ReadPunchthroughHistograms(punchthroughDir)
JERHistos           = ReadJERHistograms(oldJERFile)

# Make one mega-dictionary
print "Merging inputs..."
jetDefs = {"AntiKt4Topo_EMJES" : "AntiKt4EMTopo", 'AntiKt4PFlow_EMJES' : 'AntiKt4EMPFlow'}
systematics = {}

for years in ("2015_16","2017") :
  systematics[years] = {}
  for aJetDef in jetDefs.keys():

    etaIntercalHistos_timeDep = etaIntercalHistos_2017 if "2017" in years else etaIntercalHistos_20152016

    systematics[years][aJetDef] = dict(
                                inSituHistos[aJetDef].items()       +
                                etaIntercalHistos_timeDep[aJetDef].items()  +
                                etaIntercalHistos_general[aJetDef].items() +
                                highPtHistos[aJetDef].items()       +
                                nonclosureHistos[aJetDef].items()   +
                                pileupHistos[aJetDef].items()       +
                                flavourHistos[aJetDef].items()      +
                                bJESHistos[aJetDef].items()         +
                                punchthroughHistos[aJetDef].items()
                               )

# Loop over the mega-dictionary and write results
# Want one output file per dataset
for years in ("2015_16","2017") :
    outFileName = outBaselineFile.format(years)
    print "Writing to output file",outFileName,"..."
    baselineFile = TFile(outFileName,"RECREATE")
    theseHists = systematics[years]
    for aJetDef,aSyst in sorted(theseHists.iteritems(),key=natural_sort):
        for aSystName,aSystHisto in sorted(aSyst.iteritems(),key=natural_sort):
            baselineFile.cd()
            aSystHisto.SetTitle(aSystName+"_"+jetDefs[aJetDef])
            aSystHisto.Write(aSystHisto.GetTitle())

    # Write JER last cause it's kind of separate
    for aJetDef,aSyst in sorted(JERHistos.iteritems(),key=natural_sort):
        for aSystName,aSystHisto in sorted(aSyst.iteritems(),key=natural_sort):
            aSystHisto.SetTitle(aSystName+"_"+jetDefs[aJetDef])
            aSystHisto.Write(aSystHisto.GetTitle())

    # Done, close the files, revert directory
    baselineFile.Close()
    gDirectory = currentDir

print "Done!"



