# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
from array import array
import sys
import os
import glob
import re
import math

from ParseCurrentFile         import ReadCurrentHistograms

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

# Check useage and store arguments
if len(sys.argv) < 2:
    print "USAGE: Expected the following arguments"
    print "     1. Directory path to share directory for current 2015 recommendations"
    exit(1)

outBaselineFile = "JESUncertaintyComponentsForReductions.root"
#outMultijetFile = "MJESUncertainty_2015.root"
theCurrentDir      = sys.argv[1]

# Ensure that all of the directories exist
if not outBaselineFile.endswith(".root"):
    print "Output baseline ROOT file doesn't appear to be a root file:",outBaselineFile
    exit(2)
if not os.path.exists(theCurrentDir):
    print "2012 directory does not exist:",theCurrentDir
    exit(3)

# Store everything in memory!
currentDir = gDirectory
gROOT.cd()

# Now read the histograms
print "Reading inputs..."
# For now, everything except flavour and cross calibration inputs
# are read in from the final 2012 calibration files.
theCurrentHistos = ReadCurrentHistograms(theCurrentDir) # First true activates eta-intercalibration scaling. Second true turns off relative in situ components at large eta

print theCurrentHistos

# Make one mega-dictionary
print "Merging inputs..."
jetDefs = ["AntiKt4EMTopo"]#,"AntiKt4Topo_LCJES","AntiKt6Topo_EMJES","AntiKt6Topo_LCJES"]
systematics = {}
validities = {}
for aJetDef in jetDefs :
    systematics[aJetDef] = dict(
                                theCurrentHistos[aJetDef].items()
                               )

# Loop over the mega-dictionary and write results
print "Writing to output file",outBaselineFile,"..."
baselineFile = TFile(outBaselineFile,"RECREATE")
#multijetFile = TFile(outMultijetFile,"RECREATE")
for aJetDef,aSyst in sorted(systematics.iteritems(),key=natural_sort):
    for aSystName,aSystHisto in sorted(aSyst.iteritems(),key=natural_sort):
        baselineFile.cd()
        if (aSystName.startswith("LAr") or\
            aSystName.startswith("Zjet") or\
            aSystName.startswith("Gjet") or\
            aSystName.startswith("MJB") or\
            aSystName.startswith("SingleParticle") or\
            aSystName.startswith("Xcalib")) :
          if type(aSystHisto) is TH1D :
            aSystHisto.SetTitle(aSystName+"_"+aJetDef)
            aSystHisto.Write(aSystHisto.GetTitle())
          elif type(aSystHisto) is TH2D :
            newHist = aSystHisto.ProjectionX()
            newHist.Reset()
            newHist.SetDirectory(0)
            for xbin in range(aSystHisto.GetNbinsX()) :
              # Find biggest value in eta
              largest = 0.0
              for ybin in range(aSystHisto.GetNbinsY()) :
                thisval = aSystHisto.GetBinContent(xbin,ybin)
                if math.fabs(thisval) > math.fabs(largest) :
                  if "50nsVs25ns_Central" in aSystName :
                    print "New largest at ",xbin,",",ybin,":",thisval
                  largest = thisval
              newHist.SetBinContent(xbin,largest)
            newHist.SetTitle(aSystName+"_"+aJetDef)
            newHist.Write(newHist.GetTitle())
          else :
            print "Unexpected type!"

        else :
          print "Leaving out histo",aSystName

# Done, close the files, revert directory
baselineFile.Close()
#multijetFile.Close()
gDirectory = currentDir
print "Done!"

