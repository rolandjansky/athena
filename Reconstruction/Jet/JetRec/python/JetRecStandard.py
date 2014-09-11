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
  # Add Truth flavor.
  flavorgetters = []
  if len(jetFlags.truthFlavorTags()):
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
jtm.gettersMap["emtopo"]    = emgetters
jtm.gettersMap["lctopo"]    = lcgetters
jtm.gettersMap["empflow"]   = empfgetters
jtm.gettersMap["emcpflow"]  = emcpfgetters
jtm.gettersMap["lcpflow"]   = lcpfgetters
jtm.gettersMap["track"]     = trackgetters
jtm.gettersMap["ztrack"]    = trackgetters
jtm.gettersMap["pv0track"]  = trackgetters
if jetFlags.useTruth():
  jtm.gettersMap["truth"]   = truthgetters
  jtm.gettersMap["truthwz"] = truthwzgetters

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
calname = jetFlags.applyCalibrationName()
if calname == "none":
  calname = ""
elif calname == "offset" or calname == "jes":
  if len(calname) and jetFlags.useTracks():
    calib_topo_ungroomed_modifiers += ["applyCalibrationTool:" + calname]
else:
  print "Invalid calibration name: ", calname
  raise Exception
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
  print "JetRecStandard.py: Removed tools: " + str(remtoolnames)
  return outtools

# Filter out skipped tools.
if len(jetFlags.skipTools()):
  print "JetRecStandard.py: Tools to be skipped: " + str(jetFlags.skipTools())
  topo_ungroomed_modifiers        = filterout(jetFlags.skipTools(), topo_ungroomed_modifiers)
  calib_topo_ungroomed_modifiers  = filterout(jetFlags.skipTools(), calib_topo_ungroomed_modifiers)
  truth_ungroomed_modifiers       = filterout(jetFlags.skipTools(), truth_ungroomed_modifiers)
  track_ungroomed_modifiers       = filterout(jetFlags.skipTools(), track_ungroomed_modifiers)
  groomed_modifiers               = filterout(jetFlags.skipTools(), groomed_modifiers)

# Add modifier lists to jtm indexed by modifier type name.
jtm.modifiersMap["none"]                  = []
jtm.modifiersMap["topo_ungroomed"]        =       topo_ungroomed_modifiers
jtm.modifiersMap["calib_topo_ungroomed"]  = calib_topo_ungroomed_modifiers
jtm.modifiersMap["calib"]                 = calib_topo_ungroomed_modifiers
jtm.modifiersMap["truth_ungroomed"]       =      truth_ungroomed_modifiers
jtm.modifiersMap["track_ungroomed"]       =      track_ungroomed_modifiers
jtm.modifiersMap["groomed"]               =              groomed_modifiers

# Also index modifier type names by input type name.
# These are used when the modifier list is omitted.
jtm.modifiersMap["emtopo"]   =  topo_ungroomed_modifiers
jtm.modifiersMap["lctopo"]   =  topo_ungroomed_modifiers
jtm.modifiersMap["track"]    = track_ungroomed_modifiers
jtm.modifiersMap["ztrack"]   = track_ungroomed_modifiers
jtm.modifiersMap["pv0track"] = track_ungroomed_modifiers
jtm.modifiersMap["truth"]    = truth_ungroomed_modifiers
jtm.modifiersMap["truthwz"]  = truth_ungroomed_modifiers

