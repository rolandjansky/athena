# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetRecStandard.py
#
# David Adams
# March 2014
#
# Constructs an instance jtm of the JetToolManager with
# standard definitions for jet tools.
#
# Usage:
#   from JetRec.JetRecStandard import jtm
#   jtm.addJetFinder("AntiKt4EMTopoJets", "AntiKt", 0.4, "em", "calib_topo_ungroomed")
#

import copy

myname = "JetRecStandard.py: "

print myname + "Defining standard tools"

#########################################################
# Create standard tool manager.
#########################################################

from JetRec.JetRecFlags import jetFlags

# Import the jet tool manager.
from JetRec.JetToolSupport import JetToolManager

# Global jet tool manager with standard definitions
jtm = JetToolManager()

# Pt thresholds in MeV
jtm.ptminFinder = 2000
jtm.ptminFilter =    0

# Add standard tool definitions to the tool manager.
import JetRec.JetRecStandardTools

print myname + "jetFlags.useTruth: " + str(jetFlags.useTruth())

#########################################################
# Getters
#########################################################

# Pseudojet getters
empfgetters =  [jtm.empflowget]
emcpfgetters = [jtm.emcpflowget]
lcpfgetters =  [jtm.lcpflowget]
emgetters = [jtm.emget]
lcgetters = [jtm.lcget]
trackgetters = [jtm.trackget]
# Add track ghosts
if jetFlags.useTracks():
  emgetters += [jtm.gtrackget]
  lcgetters += [jtm.gtrackget]
if jetFlags.useMuonSegments():
  emgetters += [jtm.gmusegget]
  lcgetters += [jtm.gmusegget]
# Add jet ghosts.
if 1:
  for gettername in jetFlags.additionalTopoGetters():
    getter = jtm[gettername]
    emgetters += [getter]
    lcgetters += [getter]
# Add truth getter and truth ghosts.
if jetFlags.useTruth():
  truthgetters = [jtm.truthget]
  truthwzgetters = [jtm.truthwzget]
  trackgetters += [jtm.gtruthget]
  emgetters += [jtm.gtruthget]
  lcgetters += [jtm.gtruthget]
  empfgetters += [jtm.gtruthget]
  emcpfgetters += [jtm.gtruthget]
  lcpfgetters += [jtm.gtruthget]
  # Add truth cone matching and truth flavor ghosts.
  flavorgetters = []
  for ptype in jetFlags.truthFlavorTags():
    flavorgetters += [getattr(jtm, "gtruthget_" + ptype)]
  emgetters      += flavorgetters
  lcgetters      += flavorgetters
  truthgetters   += flavorgetters
  truthwzgetters += flavorgetters
  trackgetters   += flavorgetters
  empfgetters    += flavorgetters
  emcpfgetters   += flavorgetters
  lcpfgetters    += flavorgetters
# Add track jet ghosts.
if jetFlags.useTracks():
  trackjetgetters = []
  trackjetgetters += [jtm.gakt3trackget]
  trackjetgetters += [jtm.gakt4trackget]
  emgetters += trackjetgetters
  lcgetters += trackjetgetters

# Add getter lists to jtm indexed by input type name.
jtm.gettersMap["emtopo"]    = list(emgetters)
jtm.gettersMap["lctopo"]    = list(lcgetters)
jtm.gettersMap["empflow"]   = list(empfgetters)
jtm.gettersMap["emcpflow"]  = list(emcpfgetters)
jtm.gettersMap["lcpflow"]   = list(lcpfgetters)
jtm.gettersMap["track"]     = list(trackgetters)
jtm.gettersMap["ztrack"]    = list(trackgetters)
jtm.gettersMap["pv0track"]  = list(trackgetters)
if jetFlags.useTruth():
  jtm.gettersMap["truth"]   = list(truthgetters)
  jtm.gettersMap["truthwz"] = list(truthwzgetters)

#########################################################
# Modifiers
#########################################################

# Modifiers for ungroomed jets from all input types.
common_ungroomed_modifiers = [
  jtm.width,
  jtm.ktdr,
  jtm.nsubjettiness,
  jtm.ktsplitter,
  jtm.angularity,
  jtm.dipolarity,
  jtm.planarflow,
  jtm.ktmassdrop,
  jtm.encorr,
  jtm.comshapes
]

# Add truth parton cone matching.
if jetFlags.useTruth():
  common_ungroomed_modifiers += [jtm.truthpartondr]

# Add parton truth label.
if jetFlags.useTruth():
  common_ungroomed_modifiers += [jtm.partontruthlabel]

# Modifiers for truth jets.
truth_ungroomed_modifiers = common_ungroomed_modifiers

# Modifiers for track jets.
track_ungroomed_modifiers = common_ungroomed_modifiers

# Modifiers for topo jets.
topo_ungroomed_modifiers = ["jetfilter"]
topo_ungroomed_modifiers += common_ungroomed_modifiers
topo_ungroomed_modifiers += [jtm.jetens]
if jetFlags.useCaloQualityTool():
  topo_ungroomed_modifiers += [jtm.caloqual_cluster]
topo_ungroomed_modifiers += [jtm.bchcorrclus]
if jetFlags.useTracks():
  topo_ungroomed_modifiers += [jtm.jvf, jtm.trkmoms]
  topo_ungroomed_modifiers += ["trackassoc"]
if jetFlags.useTruth():
  topo_ungroomed_modifiers += ["truthassoc"]

# Modifiers for calibrated topo jets.
calib_topo_ungroomed_modifiers = []
calib_topo_ungroomed_modifiers += ["calib"]
calib_topo_ungroomed_modifiers += topo_ungroomed_modifiers

# Modifiers for groomed jets.
groomed_modifiers = [ ]

# Function to filter out skipped tools.
def filterout(skiptoolnames, tools):
  outtools = []
  remtoolnames = []
  nrem = 0
  for tool in tools:
    keep = True
    for toolname in skiptoolnames:
      skiptool = jtm[toolname]
      same = tool == skiptool
      if same:
        keep = False
        remtoolnames += [toolname]
    if keep:
      outtools += [tool]
  print myname + "Removed tools: " + str(remtoolnames)
  return outtools

# Filter out skipped tools.
if len(jetFlags.skipTools()):
  print myname + "Tools to be skipped: " + str(jetFlags.skipTools())
  topo_ungroomed_modifiers        = filterout(jetFlags.skipTools(), topo_ungroomed_modifiers)
  calib_topo_ungroomed_modifiers  = filterout(jetFlags.skipTools(), calib_topo_ungroomed_modifiers)
  truth_ungroomed_modifiers       = filterout(jetFlags.skipTools(), truth_ungroomed_modifiers)
  track_ungroomed_modifiers       = filterout(jetFlags.skipTools(), track_ungroomed_modifiers)
  groomed_modifiers               = filterout(jetFlags.skipTools(), groomed_modifiers)

# Add modifier lists to jtm indexed by modifier type name.
jtm.modifiersMap["none"]                  = []
jtm.modifiersMap["topo_ungroomed"]        =       list(topo_ungroomed_modifiers)
jtm.modifiersMap["calib_topo_ungroomed"]  = list(calib_topo_ungroomed_modifiers)
jtm.modifiersMap["calib"]                 = list(calib_topo_ungroomed_modifiers)
jtm.modifiersMap["truth_ungroomed"]       =      list(truth_ungroomed_modifiers)
jtm.modifiersMap["track_ungroomed"]       =      list(track_ungroomed_modifiers)
jtm.modifiersMap["groomed"]               =              list(groomed_modifiers)

# Also index modifier type names by input type name.
# These are used when the modifier list is omitted.
jtm.modifiersMap["emtopo"]   =  list(topo_ungroomed_modifiers)
jtm.modifiersMap["lctopo"]   =  list(topo_ungroomed_modifiers)
jtm.modifiersMap["track"]    = list(track_ungroomed_modifiers)
jtm.modifiersMap["ztrack"]   = list(track_ungroomed_modifiers)
jtm.modifiersMap["pv0track"] = list(track_ungroomed_modifiers)
jtm.modifiersMap["truth"]    = list(truth_ungroomed_modifiers)
jtm.modifiersMap["truthwz"]  = list(truth_ungroomed_modifiers)

