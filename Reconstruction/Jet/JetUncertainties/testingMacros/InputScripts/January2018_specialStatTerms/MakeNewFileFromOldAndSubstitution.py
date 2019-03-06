from ROOT import *
from array import array
import sys
import os
import glob
import re
import math

from ParseCurrentFile    import ReadCurrentHistograms


#http://stackoverflow.com/questions/2669059/how-to-sort-alpha-numeric-set-in-python
from itertools import groupby
def natural_sort(s):
    return [int(''.join(g)) if k else ''.join(g) for k, g in groupby(s[0], str.isdigit)]

# Here, set file(s) from which to take replacement histograms and what names to switch
jetreplacements = {"AntiKt4EMTopo":"AntiKt4Topo_EMJES"}
replacements = {}
replacements["/cluster/warehouse/kpachal/JetCalibration/JetUncertainties/JetUncertainties/inputs/PunchThrough/"] = {
     "PunchThrough_MC15" : "PunchThrough_MC15",
}

# Here, specify histograms you want to get rid of altogether
deletions = []

# Check useage and store arguments
if len(sys.argv) < 3:
    print "USAGE: Expected the following arguments"
    print "	1. Root file to use as template"
    print "	2. Name of output root file to create"
    exit(1)

inputFile = sys.argv[1]
outBaselineFile = sys.argv[2]

# Ensure that all of the directories exist
if not outBaselineFile.endswith(".root"):
    print "Output ROOT file doesn't appear to be a root file:",outBaselineFile
    exit(2)
if not os.path.exists(inputFile):
    print "Input file does not exist:",inputFile
    exit(3)
for filename in replacements.keys():
  if not os.path.exists(filename) :
    print "Replacement input file does not exist:",filename
    exit(4)

# Now read the histograms
print "Reading inputs..."
# For now, everything except flavour and cross calibration inputs
# are read in from the final 2012 calibration files.
# Input file and jet types
theCurrentHistos = ReadCurrentHistograms(inputFile,jetreplacements.keys()) 

import ParseFlavourInput
import ParseNonClosureInput
import ParsePunchthroughInput

theReplacementHistos = {}
for filename in replacements.keys() :
  if "flavor" in filename or "Flavor" in filename or "flavour" in filename or "Flavour" in filename :
    thisset = ParseFlavourInput.ReadFlavourHistograms(filename)
  elif "AFII_Uncertainty" in filename :
    thisset = ParseNonClosureInput.ReadNonClosureHistograms(filename,True)
    print "Just retrieved",thisset
  elif "PunchThrough" in filename :
    thisset = ParsePunchthroughInput.ReadPunchthroughHistograms(filename)
  else :
    thisset = ReadCurrentHistograms(filename,jetreplacements.keys())
  print thisset
  theReplacementHistos.update(thisset)

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
for aJetDef,aSyst in sorted(systematics.iteritems(),key=natural_sort):
    for aSystName,aSystHisto in sorted(aSyst.iteritems(),key=natural_sort):
        baselineFile.cd()
        for filename in replacements.keys() :
          if aSystName in replacements[filename].keys() :  
            print "Writing new version of",aSystName
            newName = replacements[filename][aSystName] 
            newHist = theReplacementHistos[jetreplacements[aJetDef]][newName]
            newHist.SetDirectory(0)
            newHist.SetTitle(aSystName+"_"+aJetDef)
            print "Giving it name",newHist.GetTitle()
            newHist.Write(newHist.GetTitle())
          elif aSystName in deletions :
            print "Deleting histogram",aSystName
            continue
          else :
            print "Keeping original version of",aSystName
            aSystHisto.SetTitle(aSystName+"_"+aJetDef)
            aSystHisto.Write(aSystHisto.GetTitle())

# Done, close the files, revert directory
baselineFile.Close()
print "Done!"

