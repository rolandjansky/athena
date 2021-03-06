# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

        --> should this be by default? prefer to avoid ignored args
"""
from .JetDefinition import JetModifier
from AthenaConfiguration.ComponentFactory import CompFactory

jetmoddict = {}

########################################################################
# Define the simple modifier setups here -- those defined in JetRec.

jetrecmods = {
    "Sort":   JetModifier("JetSorter","jetsort"),
    "Filter": JetModifier("JetFilterTool","jetptfilter_{modspec}",
                          # we give a function as PtMin : it will be evaluated when instantiating the tool (modspec will come alias usage like "Filter:10000" --> PtMin=100000) 
                          PtMin = lambda _,modspec: int(modspec) ) #
}
jetmoddict.update (jetrecmods)

########################################################################
# Below, we populate the jetmoddict with modifier definitions for tools
# that are defined in other packages.
# When necessary, the helper functions 'createfn' for tools in PackageName live in modules called
# PackageName.PackageConfig (modules to be moved)

# Calibration
from JetCalibTools import JetCalibToolsConfig
jetcalibmods = {
    "Calib": JetModifier("JetCalibrationTool","jetcalib_jetcoll_calibseq",
                         createfn=JetCalibToolsConfig.getJetCalibToolFromString,
                         prereqs=JetCalibToolsConfig.getJetCalibToolPrereqs)
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
                                   TimingCuts = [5,10],
                                   Calculations = ["LArQuality", "N90Constituents", "FracSamplingMax",  "NegativeE", "Timing", "HECQuality", "Centroid", "AverageLArQF", "BchCorrCell"],),

    "ConstitFourMom":  JetModifier("JetConstitFourMomTool", "constitfourmom_basename",
                                   createfn=JetMomentToolsConfig.getConstitFourMomTool,),
    "EMScaleMom":      JetModifier("JetEMScaleMomTool", "emscalemom_basename",
                                   createfn=JetMomentToolsConfig.getEMScaleMomTool,),

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
                                   prereqs = [ "input:JetTrackUsedInFitDeco","input:JetTrackVtxAssoc","ghost:Track" ]),

    "TrackSumMoments": JetModifier("JetTrackSumMomentsTool", "trksummoms",
                                   createfn=JetMomentToolsConfig.getTrackSumMomentsTool,
                                   prereqs = [ "input:JetTrackUsedInFitDeco","input:JetTrackVtxAssoc","ghost:Track" ]),
    "Charge" : JetModifier("JetChargeTool", "jetcharge", 
                                   prereqs = [ "ghost:Track" ]),
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
                                    JetTruthMatchTool = lambda *l : CompFactory.Analysis.JetQuarkLabel("jetquarklabel", McEventCollection='TruthEvents') ),

                                    
    "JetDeltaRLabel":   JetModifier("ParticleJetDeltaRLabelTool","jetdrlabeler_jetptmin",
                                    createfn=ParticleJetToolsConfig.getJetDeltaRLabelTool,
                                    prereqs=["ghost:BHadronsFinal",
                                             "ghost:CHadronsFinal",
                                             "ghost:TausFinal"])
    }
jetmoddict.update(particlejetmods)


# Substructure tools 

substrmods = dict( 
    nsubjettiness = JetModifier( "NSubjettinessTool", "nsubjettiness",Alpha = 1.0),
    nsubjettinessR = JetModifier( "NSubjettinessRatiosTool", "nsubjettinessR",),

    
    ktdr       = JetModifier("KtDeltaRTool", "ktdr", JetRadius = 0.4),

    ktsplitter = JetModifier( "KTSplittingScaleTool", "ktsplitter"),
    
    angularity = JetModifier( "AngularityTool", "angularity"),
    
    dipolarity = JetModifier( "DipolarityTool", "dipolarity",SubJetRadius = 0.3),
    
    planarflow = JetModifier( "PlanarFlowTool", "planarflow"),

    ktmassdrop = JetModifier( "KtMassDropTool", "ktmassdrop"),

    ecorr      = JetModifier( "EnergyCorrelatorTool", "ecorr", Beta = 1.0),
    ecorrR      = JetModifier( "EnergyCorrelatorRatiosTool", "ecorrR", ),

    ecorrgeneral = JetModifier( "EnergyCorrelatorGeneralizedTool", "ecorrgeneral"),

    ecorrgeneralratios = JetModifier( "EnergyCorrelatorGeneralizedRatiosTool", "ecorrgeneralratios"),

    comshapes = JetModifier( "CenterOfMassShapesTool","comshapes"),

    pull      = JetModifier("JetPullTool", "pull",  UseEtaInsteadOfY = False, IncludeTensorMoments = True ),

    charge    = JetModifier( "JetChargeTool", "charge", K=1.0),

    qw = JetModifier( "QwTool", "qw"),
    #showerdec = JetModifier( "  ShowerDeconstructionTool"),
)
jetmoddict.update(substrmods)
