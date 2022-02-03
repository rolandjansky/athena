# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
                                                                      
 JetMomentToolsConfig: A helper module for configuring jet moment     
 tools, in support of JetRecConfig.JetModConfig. 
 Author: TJ Khoo                                                      


IMPORTANT : all the getXYZTool(jetdef, modspec) functions are meant to be used as callback from the main JetRecConfig module 
when we need to convert definitions of tools into the actual tools. At this point the functions are invoked as 
 func(jetdef, modspec)
Hence they have jetdef and modspec arguments even if not needed in every case.
"""

from AthenaCommon import Logging
jetmomentlog = Logging.logging.getLogger('JetMomentToolsConfig')

from JetRecTools import JetRecToolsConfig
from AthenaConfiguration.ComponentFactory import CompFactory
from JetRecConfig.StandardJetContext import jetContextDic

from xAODBase.xAODType import xAODType


def getEMScaleMomTool(jetdef, modspec=""):
    # This may need updating e.g. for evolving trigger cluster container names
    # We do the non-trivial summation over constituents unless the jets were
    # built directly from EM-scale topoclusters, in which case we can just
    # copy the constituent scale
    useUncalibConstits = False
    if jetdef.inputdef.basetype==xAODType.CaloCluster:
        builtFromEMClusters = jetdef.inputdef.inputname in ["CaloCalTopoClusters","HLT_CaloTopoClustersFS"] and jetdef.inputdef.modifiers==["EM"]
        useUncalibConstits = not builtFromEMClusters
    elif (jetdef.inputdef.basetype==xAODType.ParticleFlow or jetdef.inputdef.basetype==xAODType.FlowElement):
        useUncalibConstits = True
    else:
        raise ValueError("EM scale momentum not defined for input type {}".format(jetdef.inputdef.basetype))

    emscalemom = CompFactory.JetEMScaleMomTool(
        "emscalemom_{}".format(jetdef.basename),
        UseUncalibConstits = useUncalibConstits,
        JetContainer = jetdef.fullname(),
    )

    return emscalemom

def getConstitFourMomTool(jetdef, modspec=""):
    ### Not ideal, but because CaloCluster.Scale is an internal class
    ### it makes the dict load really slow.
    ### So just copy the enum to a dict...
    ### Defined in Event/xAOD/xAODCaloEvent/versions/CaloCluster_v1.h
    CaloClusterStates = {
      "UNKNOWN"       : -1,
      "UNCALIBRATED"  :  0,
      "CALIBRATED"    :  1,
      "ALTCALIBRATED" :  2,
      "NSTATES"       :  3
      }

    cfourmom = CompFactory.JetConstitFourMomTool("constitfourmom_{0}".format(jetdef.basename))
    if "LCTopo" in jetdef.basename or "EMTopo" in jetdef.basename:
        cfourmom.JetScaleNames = ["DetectorEtaPhi"]
        if "HLT_" in jetdef.fullname():
            cfourmom.AltConstitColls = [""]
            cfourmom.AltConstitScales = [0]
            cfourmom.AltJetScales = ["JetConstitScaleMomentum"]
        else:
            clstate = "CALIBRATED" if "LCTopo" in jetdef.basename else "UNCALIBRATED"
            cfourmom.AltConstitColls = ["CaloCalTopoClusters"]
            cfourmom.AltConstitScales = [CaloClusterStates[clstate]]
            cfourmom.AltJetScales = [""]
    # Drop the LC-calibrated four-mom for EMTopo jets as we only wanted it as a possibility
    # in MET CST calculations but never used it
    elif "EMPFlow" in jetdef.basename:
        cfourmom.JetScaleNames = ["DetectorEtaPhi"]
        cfourmom.AltConstitColls = [""]
        cfourmom.AltConstitScales = [0]
        cfourmom.AltJetScales = ["JetConstitScaleMomentum"]

    return cfourmom

# Jet vertex fraction with selection.
def getJVFTool(jetdef, modspec):
    jettrackselloose = JetRecToolsConfig.getTrackSelTool(modspec or jetdef.context)
    # retrieve the tracking keys to be used with modspec :
    trackingKeys = jetContextDic[modspec or jetdef.context]
    jvf = CompFactory.JetVertexFractionTool(
        "jvf",
        VertexContainer = trackingKeys["Vertices"],
        AssociatedTracks = trackingKeys["GhostTracksLabel"],
        TrackVertexAssociation = trackingKeys["TVA"],
        TrackParticleContainer  = trackingKeys["Tracks"],
        TrackSelector = jettrackselloose,
        SuppressInputDependence = True
    )
    return jvf


# Jet vertex fraction with selection.
def getJVTTool(jetdef, modspec):
    jvt = CompFactory.JetVertexTaggerTool(
        "jvt",
        VertexContainer = jetContextDic[modspec or jetdef.context]["Vertices"],
        SuppressInputDependence = True
    )
    return jvt


def getTrackMomentsTool(jetdef, modspec):
    jettrackselloose = JetRecToolsConfig.getTrackSelTool(modspec or jetdef.context)
    # retrieve the tracking keys to be used with modspec : 
    trackingKeys = jetContextDic[modspec or jetdef.context]

    trackmoments = CompFactory.JetTrackMomentsTool(
        "trkmoms",
        VertexContainer = trackingKeys["Vertices"],
        AssociatedTracks = trackingKeys["GhostTracksLabel"],
        TrackVertexAssociation = trackingKeys["TVA"],
        TrackMinPtCuts = [500, 1000],
        TrackSelector = jettrackselloose,
        DoPFlowMoments = 'PFlow' in jetdef.fullname() ,
    )
    return trackmoments

def getTrackSumMomentsTool(jetdef, modspec):
    jettrackselloose = JetRecToolsConfig.getTrackSelTool(modspec)
    # retrieve the tracking keys to be used with modspec : 
    trackingKeys = jetContextDic[modspec or jetdef.context]
    tracksummoments = CompFactory.JetTrackSumMomentsTool(
        "trksummoms",
        VertexContainer = trackingKeys["Vertices"],
        AssociatedTracks = trackingKeys["GhostTracksLabel"],
        TrackVertexAssociation = trackingKeys["TVA"],
        RequireTrackPV = True,
        TrackSelector = jettrackselloose
    )
    return tracksummoments

# This tool sets a decoration saying which the nominal HS PV was.
# Historically it did the origin correction, but now we do this to constituents
def getOriginCorrVxTool(jetdef, modspec):
    origin_setpv = CompFactory.JetOriginCorrectionTool(
      "jetorigin_setpv",
      VertexContainer = jetContextDic[modspec or jetdef.context]["Vertices"],
      OriginCorrectedName = "",
      OnlyAssignPV = True,
    )
    return origin_setpv
