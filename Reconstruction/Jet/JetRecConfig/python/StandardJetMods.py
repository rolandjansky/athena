# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
This module defines the standard JetModifier tools used in jet reco

Definitions are grouped in a dictionary of tool configurations using the helpers defined
in package configs.
This dict maps a modifier alias to the JetModifier config object
that in turn will be responsible for generating a configured tool.

The JetModifier config class is defined in JetDefinition.py 
[Optional] Args to the JetModifier constructor are:
   1. Tool Type (ignored if the helper is a custom one)
   2. Tool Name (ignored if the helper is a custom one)
  [3.] Config helper
  [4.] Prereqs (default to []). Can also be a function.
  [5.] Flag passJetDef specifying if the helper needs the jet definition
        --> should this be by default? prefer to avoid ignored args
"""
from .JetDefinition import JetModifier

jetmoddict = {}

########################################################################
# Define the simple modifier setups here -- those defined in JetRec.

from AthenaConfiguration.ComponentFactory import CompFactory
def getJetFilterTool(modspec):
    threshold = int(modspec)
    jetptfilter = CompFactory.JetFilterTool("jetptfilter_{0}mev".format(threshold))
    jetptfilter.PtMin = threshold
    return jetptfilter
jetrecmods = {
    "Sort":   JetModifier("JetSorter","jetsort"),
    "Filter": JetModifier("JetFilterTool","jetptfilter",createfn=getJetFilterTool),
}
jetmoddict.update (jetrecmods)

########################################################################
# Below, we populate the jetmoddict with modifier definitions for tools
# that are defined in other packages.
# The helper functions for tools in PackageName live in modules called
# PackageName.PackageConfig (modules to be moved)

# Calibration
from JetCalibTools import JetCalibToolsConfig
jetcalibmods = {
    "Calib": JetModifier("JetCalibrationTool","jetcalib_jetcoll_calibseq",
                         createfn=JetCalibToolsConfig.getJetCalibToolFromString,
                         prereqs=JetCalibToolsConfig.getJetCalibToolPrereqs,passJetDef=True)
    }
jetmoddict.update(jetcalibmods)

# TBD:
# All items below in principle will support decoration mode, rather
# than only non-const modification. Mode of operation should be
# determined by interface called from parent tool/alg.

# Standard jet moments
from JetMomentTools import JetMomentToolsConfig
jetmomentmods = {

    # Easy cases, no special config or prereqs, just default tool config
    "ClusterMoments":  JetModifier("JetClusterMomentsTool", "clsmoms"),
    "ECPSFrac":        JetModifier("JetECPSFractionTool", "ecpsfrac"),
    "Width":           JetModifier("JetWidthTool", "width"),

    # More complex cases here
    "CaloEnergies":    JetModifier("JetCaloEnergies", "jetens",
                                   prereqs=["mod:EMScaleMom"]
    ),
    "CaloQuality":     JetModifier("JetCaloQualityTool", "caloqual",
                                   createfn=JetMomentToolsConfig.getCaloQualityTool),
    "ConstitFourMom":  JetModifier("JetConstitFourMomTool", "constitfourmom_basename",
                                   createfn=JetMomentToolsConfig.getConstitFourMomTool,
                                   passJetDef=True),
    "EMScaleMom":      JetModifier("JetEMScaleMomTool", "emscalemom_basename",
                                   createfn=JetMomentToolsConfig.getEMScaleMomTool,
                                   passJetDef=True),
    "JVF":             JetModifier("JetVertexFractionTool", "jvf",
                                   createfn=JetMomentToolsConfig.getJVFTool,
                                   prereqs = ["mod:TrackMoments"] ),
    "JVT":             JetModifier("JetVertexTaggerTool", "jvt",
                                   createfn=JetMomentToolsConfig.getJVTTool,
                                   prereqs = [ "mod:JVF" ]),
    "LArHVCorr":       JetModifier("JetLArHVTool", "larhvcorr",
                                   prereqs = ["mod:EMScaleMom"]),
    "OriginSetPV":     JetModifier("JetOriginCorrectionTool", "origin_setpv",
                                   prereqs = [ "mod:JVF" ]),
    "TrackMoments":    JetModifier("JetTrackMomentsTool", "trkmoms",
                                   createfn=JetMomentToolsConfig.getTrackMomentsTool,
                                   prereqs = [ "input:JetTrackVtxAssoc","ghost:Track" ]),

    "TrackSumMoments": JetModifier("JetTrackSumMomentsTool", "trksummoms",
                                   createfn=JetMomentToolsConfig.getTrackSumMomentsTool,
                                   prereqs = [ "input:JetTrackVtxAssoc","ghost:Track" ]),
}
jetmoddict.update(jetmomentmods)

# Truth labelling moments
from ParticleJetTools import ParticleJetToolsConfig
particlejetmods = {
    # Easy cases, no special config or prereqs, just default tool config
    "PartonTruthLabel": JetModifier("Analysis::JetPartonTruthLabel","partontruthlabel",
                                    prereqs=["ghost:Partons"]),

    # More complex cases here
    "TruthPartonDR":    JetModifier("Analysis::JetConeLabeling","truthpartondr",
                                    createfn=ParticleJetToolsConfig.getJetConeLabeling),
    "JetDeltaRLabel":   JetModifier("ParticleJetDeltaRLabelTool","jetdrlabeler_jetptmin",
                                    createfn=ParticleJetToolsConfig.getJetDeltaRLabelTool,
                                    prereqs=["ghost:BHadronsFinal",
                                             "ghost:CHadronsFinal",
                                             "ghost:TausFinal"])
    }
jetmoddict.update(particlejetmods)

# Todo: jet substructure moment tools

# This can also be expanded by users if they would rather do this than
# pass in JetModifier instances in the JetDefinition


## TEMPORARY HACK (change the names of ghost tracks )
from JetRecTools.JetRecToolsConfig import trackcollectionmap
trackcollectionmap[""] =  {
    "Tracks":           "InDetTrackParticles",
    "JetTracks":        "JetSelectedTracks",
    "Vertices":         "PrimaryVertices",
    "TVA":              "JetTrackVtxAssoc",
    "GhostTracks":      "PseudoJetGhostTrack",
    "GhostTracksLabel": "GhostTrack",
    }
