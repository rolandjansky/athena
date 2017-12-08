# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Script to test EGammaAmbiguityTool. If input file not given, one from
valid3.160009.PowhegPythia8_AU2CT10_ggH125_gamgam.recon.AOD.e3458_s2579_r6162 is used"""
__author__ = "Bruno Lenzi"

import ROOT, sys, os
ROOT.gROOT.Macro( '$ROOTCOREDIR/scripts/load_packages.C' )
from ElectronPhotonSelectorTools import TestEGammaAmbiguityTool

# Initialize the xAOD infrastructure: 
ROOT.xAOD.Init()

try:
  filename = sys.argv[1]
except IndexError:  filename = "root://eosatlas//eos/atlas/atlasdatadisk/rucio/valid3/24/c7/AOD.05062229._000129.pool.root.1"

# Set up ambiguity tool
tool = ROOT.EGammaAmbiguityTool('tool')
tool.initialize()

# Create transient tree
f = ROOT.TFile.Open(filename)
t = ROOT.xAOD.MakeTransientTree( f )
import xAODRootAccess.GenerateDVIterators

for entry in xrange(10):
  print 'Analysing entry %s' % entry
  _ = t.GetEntry(entry)
    
  for container in 'Photons', 'Electrons':
    print '\n*** %s ****' % container
    for obj in getattr(t, container):
      TestEGammaAmbiguityTool.printAmbiguityInfo(obj, tool)
  print '\n'

