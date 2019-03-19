from JetDefinition import JetModifier

########################################################################
# Define the dictionary of tool configurations using the helpers defined
# in package configs.
# This dict maps a modifier alias to the JetModifier config object
# that in turn will be responsible for generating a configured tool.
#
# [Optional] Args to the JetModifier constructor are:
#    1. Tool Type (ignored if the helper is a custom one)
#    2. Tool Name (ignored if the helper is a custom one)
#   [3.] Config helper
#   [4.] Prereqs (default to []). Can also be a function.
#   [5.] Flag passJetDef specifying if the helper needs the jet definition
#         --> should this be by default? prefer to avoid ignored args
jetmoddict = {}

########################################################################
# Define the simple modifier setups here -- those defined in JetRec.
from JetRec import JetRecConf
def getJetFilterTool(modspec):
    threshold = int(modspec)
    jetptfilter = JetRecConf.JetFilterTool("jetptfilter_{0}mev".format(threshold))
    jetptfilter.PtMin = threshold
    return jetptfilter
jetrecmods = {
    "Sort":   JetModifier("JetSorter","jetsort"),
    "Filter": JetModifier("JetFilterTool","jetptfilter",helperfn=getJetFilterTool),
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
                         helperfn=JetCalibToolsConfig.getJetCalibToolFromString,
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
    "CaloEnergies":    JetModifier("JetCaloEnergies", "jetens"),
    "ClusterMoments":  JetModifier("JetClusterMomentsTool", "clsmoms"),
    "LArHVCorr":       JetModifier("JetLArHVTool", "larhvcorr"),
    "ECPSFrac":        JetModifier("JetECPSFractionTool", "ecpsfrac"),
    "Width":           JetModifier("JetWidthTool", "width"),

    # More complex cases here
    "CaloQuality":     JetModifier("JetCaloQualityTool", "caloqual",
                                   helperfn=JetMomentToolsConfig.getCaloQualityTool),
    "ConstitFourMom":  JetModifier("JetConstitFourMomTool", "jetconstitfourmom_basename",
                                   helperfn=JetMomentToolsConfig.getConstitFourMomTool,
                                   passJetDef=True),
    "JVF":             JetModifier("JetVertexFractionTool", "jvf",
                                   helperfn=JetMomentToolsConfig.getJVFTool,
                                   prereqs = ["mod:TrackSumMoments"] ),
    "JVT":             JetModifier("JetVertexTaggerTool", "jvt",
                                   prereqs = [ "mod:JVF" ]),
    "OriginSetPV":     JetModifier("JetOriginCorrectionTool", "origin_setpv",
                                   prereqs = [ "mod:JVF" ]),
    "TrackMoments":    JetModifier("JetTrackMomentsTool", "trkmoms",
                                   helperfn=JetMomentToolsConfig.getTrackMomentsTool,
                                   prereqs = [ "input:JetSelectedTracks","input:JetTrackVtxAssoc","ghost:Track" ]),
    "TrackSumMoments": JetModifier("JetTrackSumMomentsTool", "trksummoms",
                                   helperfn=JetMomentToolsConfig.getTrackSumMomentsTool,
                                   prereqs = [ "input:JetSelectedTracks","input:JetTrackVtxAssoc","ghost:Track" ]),
    }
jetmoddict.update(jetmomentmods)

# Truth labelling moments
from ParticleJetTools import ParticleJetToolsConfig
particlejetmods = {
    # Easy cases, no special config or prereqs, just default tool config
    "PartonTruthLabel": JetModifier("Analysis__JetPartonTruthLabel","partontruthlabel",
                                    prereqs=["ghost:TruthLabelPartons"]),

    # More complex cases here
    "TruthPartonDR":    JetModifier("Analysis__JetConeLabeling","truthpartondr",
                                    helperfn=ParticleJetToolsConfig.getJetConeLabeling),
    "JetDeltaRLabel":   JetModifier("ParticleJetDeltaRLabelTool","jetdrlabeler_jetptmin",
                                    helperfn=ParticleJetToolsConfig.getJetDeltaRLabelTool,
                                    prereqs=["input:TruthLabelBHadronsFinal",
                                             "input:TruthLabelCHadronsFinal",
                                             "input:TruthLabelTausFinal"])
    }
jetmoddict.update(particlejetmods)

# Todo: jet substructure moment tools

# This can also be expanded by users if they would rather do this than
# pass in JetModifier instances in the JetDefinition
