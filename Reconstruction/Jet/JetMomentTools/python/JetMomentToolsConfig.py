# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# JetMomentToolsConfig: A helper module for configuring jet moment     #
# tools, in support of JetRecConfig.JetModConfig.                      #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

from AthenaCommon import Logging
jetmomentlog = Logging.logging.getLogger('JetMomentToolsConfig')

from JetRecTools import JetRecToolsConfig
from JetMomentTools import JetMomentToolsConf

def getCaloQualityTool():
    caloqual = JetMomentToolsConf.JetCaloQualityTool(
      "caloqual",
      TimingCuts = [5, 10],
      Calculations = ["LArQuality", "N90Constituents", "FracSamplingMax",  "NegativeE", "Timing", "HECQuality", "Centroid", "AverageLArQF", "BchCorrCell"],
    )
    return caloqual

def getConstitFourMomTool(jetdef):
    # All this code needs to live in a JetMomentTools module

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

    ### Workaround for inability of Gaudi to parse single-element tuple
    try:
        import GaudiPython.Bindings as GPB
        _old_setattr = GPB.iProperty.__setattr__
        def _new_setattr(self, name, value):
           if type(value) == tuple:
               value = list(value)
           return _old_setattr(self, name, value)
        GPB.iProperty.__setattr__ = _new_setattr
    except:
        pass
    ###
    cfourmom = JetMomentToolsConf.JetConstitFourMomTool("constitfourmom_{0}".format(jetdef.basename))
    if "LCTopo" in jetdef.basename or "EMTopo" in jetdef.basename:
        cfourmom.JetScaleNames = ["DetectorEtaPhi"]
        cfourmom.AltConstitColls = ["CaloCalTopoClusters"]
        cfourmom.AltConstitScales = [CaloClusterStates["CALIBRATED"]]
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
def getJVFTool():
    jettrackselloose = JetRecToolsConfig.getTrackSelTool()

    jvf = JetMomentToolsConf.JetVertexFractionTool(
        "jvf",
        VertexContainer = "PrimaryVertices",
        AssociatedTracks = "GhostTrack",
        TrackVertexAssociation = "JetTrackVtxAssoc",
        TrackParticleContainer  = "JetSelectedTracks",
        TrackSelector = jettrackselloose,
    )
    return jvf


def getTrackMomentsTool():
    jettrackselloose = JetRecToolsConfig.getTrackSelTool()

    trackmoments = JetMomentToolsConf.JetTrackMomentsTool(
        "trkmoms",
        VertexContainer = "PrimaryVertices",
        AssociatedTracks = "GhostTrack",
        TrackVertexAssociation = "JetTrackVtxAssoc",
        TrackMinPtCuts = [500, 1000],
        TrackSelector = jettrackselloose
    )
    return trackmoments

def getTrackSumMomentsTool():
    jettrackselloose = JetRecToolsConfig.getTrackSelTool()

    tracksummoments = JetMomentToolsConf.JetTrackSumMomentsTool(
        "trksummoms",
        VertexContainer = "PrimaryVertices",
        AssociatedTracks = "GhostTrack",
        TrackVertexAssociation = "JetTrackVtxAssoc",
        RequireTrackPV = True,
        TrackSelector = jettrackselloose
    )
    return tracksummoments

# This tool sets a decoration saying which the nominal HS PV was.
# Historically it did the origin correction, but now we do this to constituents
def getOriginCorrVxTool():
    origin_setpv = JetMomentToolsConf.JetOriginCorrectionTool(
      "jetorigin_setpv",
      VertexContainer = "PrimaryVertices",
      OriginCorrectedName = "",
      OnlyAssignPV = True,
    )
    return origin_setpv
