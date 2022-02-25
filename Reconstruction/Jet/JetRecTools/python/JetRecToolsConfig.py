# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# JetModConfig: A helper module for configuring tools that support     #
# jet reconstruction                                                   #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

import os
from AthenaCommon import Logging
jrtlog = Logging.logging.getLogger('JetRecToolsConfig')

from AthenaConfiguration.ComponentFactory import CompFactory
from JetRecConfig.JetRecConfig import isAthenaRelease



def getIDTrackSelectionTool(trkOpt, **userProps):
    from JetRecConfig.StandardJetContext import jetContextDic
    # see the default options in jetContextDic from StandardJetContext.py
    selProperties = jetContextDic[trkOpt]["trackSelOptions"].clone( **userProps)
    idtracksel = CompFactory.getComp("InDet::InDetTrackSelectionTool")(
        "idtracksel_"+trkOpt,
        **selProperties )
    
    if os.environ.get("AtlasProject",None) != "AnalysisBase":
        # thes options can not be set in AnalysisBase. (but not setting them is equivalent to set them to False)
        idtracksel.UseTrkTrackTools = False
        idtracksel.Extrapolator     = ""
        idtracksel.TrackSummaryTool = ""
    return idtracksel

def getTrackSelAlg(trkOpt="default", trackSelOpt=False):
    from JetRecConfig.StandardJetContext import jetContextDic
    trkProperties = jetContextDic[trkOpt]

    trkSelAlg = None

    if not trackSelOpt:
        # Get a InDetTrackSelectionTool, OVERWRITING the CutLevel for e.g. ghosts:
        idtracksel = getIDTrackSelectionTool(trkOpt, CutLevel=trkProperties['GhostTrackCutLevel'])

        trkSelAlg = CompFactory.JetTrackSelectionAlg( "trackselalg",
                                                      TrackSelector = idtracksel,
                                                      InputContainer = trkProperties["Tracks"],
                                                      OutputContainer = trkProperties["JetTracks"],
                                                    )
                                                    
        
    else:
        # Track-jet selection criteria
        idtracksel = getIDTrackSelectionTool(trkOpt, CutLevel=trkProperties['trackSelOptions']['CutLevel'])

        trkSelAlg = CompFactory.JetTrackSelectionAlg( "trackseljetalg",
                                                      TrackSelector = idtracksel,
                                                      InputContainer = trkProperties["Tracks"],
                                                      OutputContainer = trkProperties["JetTracksQualityCuts"],
                                                    )

    return trkSelAlg

def getTrackSelTool(trkOpt=""):
    # this tool is still used by trk moment tools.
    # it should be deprecated in favor of simply the InDet tool
    idtrackselloose = getIDTrackSelectionTool(trkOpt)

    jettrackselloose = CompFactory.JetTrackSelectionTool(
        "jettrackselloose",
        Selector        = idtrackselloose
    )

    return jettrackselloose

def getTrackVertexAssocTool(trkOpt="", theSequence=None, ttva_opts = { "WorkingPoint" : "Custom", "d0_cut" : 2.0, "dzSinTheta_cut" : 2.0 }):
    if trkOpt: "_{}".format(trkOpt)
    # Track-vertex association
    # This is to be deprecated
    # In fact can probably be switched already to match legacy master
    # but for a future MR
    from TrackVertexAssociationTool.getTTVAToolForReco import getTTVAToolForReco
    from JetRecConfig.StandardJetContext import jetContextDic

    trkProperties = jetContextDic[trkOpt]
    
    idtvassoc = getTTVAToolForReco(
        "idloosetvassoc",
        TrackContName = trkProperties["Tracks"],
        VertexContName = trkProperties["Vertices"],
        returnCompFactory = True,
        add2Seq=theSequence,
        addDecoAlg= isAthenaRelease(), # ?? it seems mandatory ??
        **ttva_opts
    )

    jettvassoc = CompFactory.TrackVertexAssociationTool(
        "jettvassoc",
        TrackParticleContainer  = trkProperties["Tracks"],
        TrackVertexAssociation  = trkProperties["TVA"],
        VertexContainer         = trkProperties["Vertices"],
        TrackVertexAssoTool     = idtvassoc
    )
    return jettvassoc

def getTrackUsedInFitTool(trkOpt=""):
    if trkOpt: "_{}".format(trkOpt)
    # InDet decorator tool:
    from JetRecConfig.StandardJetContext import jetContextDic

    trkProperties = jetContextDic[trkOpt]
    IDUsedInFitTrkDecoTool = CompFactory.getComp("InDet::InDetUsedInFitTrackDecoratorTool")(
        "IDUsedInFitTrkDecoTool",
        TrackContainer       = trkProperties["Tracks"],
        VertexContainer      = trkProperties["Vertices"],
        AMVFVerticesDecoName = "TTVA_AMVFVertices_forReco",
        AMVFWeightsDecoName  = "TTVA_AMVFWeights_forReco"
    )
    # Jet wrapper:
    JetUsedInFitTrkDecoTool = CompFactory.JetUsedInFitTrackDecoratorTool(
        "JetUsedInFitTrkDecoTool",
        Decorator = IDUsedInFitTrkDecoTool
    )
    return JetUsedInFitTrkDecoTool

def getPFlowSelAlg():
    # PFlow objects matched to electrons/muons filtering algorithm 
    return  CompFactory.JetPFlowSelectionAlg( "pflowselalg",
                                              electronID = "LHMedium",
                                              ChargedPFlowInputContainer  = "JetETMissChargedParticleFlowObjects",
                                              NeutralPFlowInputContainer  = "JetETMissNeutralParticleFlowObjects",
                                              ChargedPFlowOutputContainer = "GlobalPFlowChargedParticleFlowObjects",
                                              NeutralPFlowOutputContainer = "GlobalPFlowNeutralParticleFlowObjects"
                                             )
