# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import sys
import re
from ROOT import *

def GetKeyNames(self,dir=""):
  self.cd(dir)
  return [key.GetName() for key in gDirectory.GetListOfKeys()]
TFile.GetKeyNames = GetKeyNames


if len(sys.argv) < 4:
    print "Too few arguments.  Expected the following:"
    print "  1. Output file"
    print "  2. Primary input file (to be overwritten in parts)"
    print "  3. New input file (to use to overwrite parts)"
    print "  4. Optional: whether to interpret new histograms (TRUE) as additions or to exit (FALSE)"
    sys.exit(1)

outFile     = TFile.Open(sys.argv[1],"RECREATE")
primaryFile = TFile.Open(sys.argv[2],"READ")
updateFile  = TFile.Open(sys.argv[3],"READ")
addNew      = True if len(sys.argv) > 4 and (sys.argv[4] == "TRUE" or sys.argv[4] == "True" or sys.argv[4] == "true") else False


# Figure out which components we want to replace and store the corresponding histograms
updates = {}
for histName in updateFile.GetKeyNames():
    hist = updateFile.Get(histName)
    hist.SetDirectory(0)
    updates[histName] = hist
updateFile.Close()

# Check that we can find the components to replace
newHists = []
for histName in updates.keys():
    if not histName in primaryFile.GetKeyNames():
        if not addNew:
            print "Failed to find the original histogram to replace for key =",histName
            sys.exit(2)
        else:
            newHists.append(histName)

# We found everything we want to replace, so do it
for histName in primaryFile.GetKeyNames():
    hist = None
    if histName in updates.keys():
        print "Updating component:",histName
        hist = updates[histName]
    else:
        hist = primaryFile.Get(histName)
    outFile.cd()
    hist.Write(histName)

# Add new histograms if requested
if addNew:
    for newHist in newHists:
        print "Adding new component:",newHist
        hist = updates[newHist]
        outFile.cd()
        hist.Write(newHist)


outFile.Close()
primaryFile.Close()

