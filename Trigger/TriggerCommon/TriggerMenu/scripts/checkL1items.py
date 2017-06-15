#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
	This script reads the MC and Physics menu and identifies L1 items that are
	only assigned to MC chains.
	Currently it needs to be run from the script folder.
	One argument can be supplied specifying which "tag" to use, default is pp_v6 
"""

import importlib
import sys

tag = "pp_v6"
if len(sys.argv) > 1:
	tag = sys.argv[1]

l1menu          = importlib.import_module("TriggerMenu.l1menu.Menu_MC_%s" % tag)
physics_hltmenu = importlib.import_module("TriggerMenu.menu.Physics_%s" % tag)
mc_hltmenu      = importlib.import_module("TriggerMenu.menu.MC_%s" % tag)

from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
from TriggerJobOpts.TriggerFlags          import TriggerFlags


l1menu.defineMenu()
l1exist = set(Lvl1Flags.items())

slices = ("TestSlice","MuonSlice","JetSlice","BjetSlice","METSlice","TauSlice","EgammaSlice","BphysicsSlice","CombinedSlice","HeavyIonSlice","BeamspotSlice","MinBiasSlice","CalibSlice","CosmicSlice","StreamingSlice","MonitorSlice","EnhancedBiasSlice")

physics_hltmenu.setupMenu()
l1physics_used = set()

for sl in slices:
	signatures = getattr(TriggerFlags,sl).signatures()
	l1s = [ chain[1] for chain in signatures ]
	l1physics_used.update( l1s )
	multil1s = [ l1 for chain in signatures for l1 in chain[2]]
	l1physics_used.update( multil1s )

mc_hltmenu.setupMenu()
l1mc_used = set()

for sl in slices:
	signatures = getattr(TriggerFlags,sl).signatures()
	l1s = [ chain[1] for chain in signatures ]
	l1mc_used.update( l1s )
	multil1s = [ l1 for chain in signatures for l1 in chain[2]]
	l1mc_used.update( multil1s )

print "-----   In L1 but not used in HLT (Physics):",len(l1exist.difference(l1physics_used))
print l1exist.difference(l1physics_used)
print "-----   In L1 but not used in HLT (MC):",len(l1exist.difference(l1mc_used))
print l1exist.difference(l1mc_used)
print "-----   In HLT but not in L1 ????:",len((l1mc_used | l1physics_used).difference(l1exist))
print (l1mc_used | l1physics_used).difference(l1exist)
