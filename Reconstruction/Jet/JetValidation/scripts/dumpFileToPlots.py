# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# External dependencies
import ROOT
import sys
import re
import math


# Control switch
IsDebug = False

##################################################
# Read and store argument values                 #
##################################################

fileName = "/afs/cern.ch/user/d/delsart/public/COMMON/NTUP_COMMON_new.root"
treeName = "physics"
pdfName  = "plots.pdf"

if len(sys.argv) < 2:
  print "Too few arguments.  Expected the following:"
  print "1. Root file to dump (required)"
  print "2. Tree name in the file (optional, default=\"physics\")"
  print "3. Output plot file (optional, default=\"plots.pdf\")"
  if not IsDebug:
    exit(1)

if len(sys.argv) > 1:
  fileName = sys.argv[1]
if len(sys.argv) > 2:
  treeName = sys.argv[2]
if len(sys.argv) > 3:
  pdfName  = sys.argv[3]


inFile = ROOT.TFile(fileName,"READ")
tree   = inFile.Get(treeName)
if not isinstance(tree,ROOT.TTree) :
  print "File does not contain the specified tree.  Is the tree name correct?"
  print "File: ",fileName
  print "Tree: ",treeName
  exit(2)


##################################################
# Dump everything                                #
##################################################

from JetValidation.D3PDHistoBuildLib import HistoBuilderFromD3PD,isSupportedType

builder = HistoBuilderFromD3PD(tree)

print "Adding branches..."
for aBranch in sorted(tree.GetSetOfTreeBranchNames()):
  if str.startswith(aBranch,"jet_") or str.startswith(aBranch,"MET_"):
    if isSupportedType(tree.GetBranchType(aBranch)):
      builder.addHistos(aBranch)

print "\nRunning event loop..."
builder.eventLoop()
print "Done event loop!"

#builder.dumpAllHistosToFile(pdfName)
builder.dumpStructuredPDF(pdfName)
builder.saveHistos(pdfName.replace('.pdf','.root'))
builder.saveHistoBins(pdfName.replace('.pdf','.pck'))

inFile.Close()


