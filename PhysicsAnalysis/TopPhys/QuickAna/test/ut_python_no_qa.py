#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Set up ROOT and RootCore:
import ROOT
from glob import glob
import itertools
import os
import sys

if os.getenv("ROOTCORE_AUTO_UT") == "1" and os.getenv("ROOTCORE_SLOW_UT") != "1":
    sys.exit(0)

ROOT.gROOT.Macro( '$ROOTCOREDIR/scripts/load_packages.C' )

# Initialize the xAOD infrastructure:
if not ROOT.xAOD.Init().isSuccess():
    raise Exception('Failed xAOD.Init')

ROOT.xAOD.AuxContainerBase()
event = ROOT.xAOD.TEvent();
store = ROOT.xAOD.TStore();

data_files = [ os.getenv("ROOTCORE_TEST_FILE") ]

# Set quickana do this before opening root file to prevent dictionary clash problems

# Set up the input files:

treeName = "CollectionTree" # default when making transient tree anyway
f = ROOT.TChain(treeName)

for d in data_files[0:10]:
    print d
    f.Add(d)
print f.GetEntries()
#t = ROOT.xAOD.MakeTransientTree(f)
t = ROOT.xAOD.MakeTransientTree(f, ROOT.xAOD.TEvent.kAthenaAccess)

entries = t.GetEntries()
if entries > 100 :
    entries = 100
    pass

# Print some information:
print( "Number of input events: %s" % entries )

# let's only run over the first 100 events for this example
for entry in xrange(entries) :
    t.GetEntry( entry )
    store.clear()

ROOT.xAOD.ClearTransientTrees()
