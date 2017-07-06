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

qa = ROOT.ana.QuickAna("QuickAna")

qa.eventinfoDef = "default";
qa.muonDef = "default";
qa.electronDef = "default";
qa.photonDef = "default";
qa.tauDef = "default";
qa.jetDef = "default";
qa.jetKine = "pt > 50e3";
qa.metDef = "default";
qa.orDef = "default";

# We just need some files for testing. Do not use these in your analysis
qa.muMcFiles.push_back(ROOT.string(
    "dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root"))
qa.muMcFiles.push_back(ROOT.string(
    "dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root"))
# Using test file from SUSYTools
qa.muDataFiles.push_back(ROOT.string("dev/SUSYTools/ilumicalc_histograms_None_276262-284154.root"))

# Set up the input files:

treeName = "CollectionTree" # default when making transient tree anyway
f = ROOT.TChain(treeName)

for d in data_files[0:10]:
    print d
    f.Add(d)
print f.GetEntries()
#t = ROOT.xAOD.MakeTransientTree(f)
t = ROOT.xAOD.MakeTransientTree(f, ROOT.xAOD.TEvent.kAthenaAccess)

# Initialize QuickAna
if not qa.initialize().isSuccess():
    raise Exception('Failed to initialize QuickAna')

entries = t.GetEntries()
if entries > 100 :
    entries = 100
    pass

# Print some information:
print( "Number of input events: %s" % entries )

# let's only run over the first 100 events for this example
for entry in xrange(entries) :
    t.GetEntry( entry )
    qa.process().ignore()
    qa.muons()
    qa.electrons()
    qa.taus()
    qa.jets()
    qa.photons()
    qa.met()
    store.clear()

del qa

ROOT.xAOD.ClearTransientTrees()
