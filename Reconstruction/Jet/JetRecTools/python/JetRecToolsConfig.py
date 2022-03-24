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
        # track selection from trkOpt but OVERWRITING the CutLevel for e.g. ghosts (typically, only a pt>500MeV cut remains): 
        idtracksel = getIDTrackSelectionTool(trkOpt, CutLevel=trkProperties['GhostTrackCutLevel'])
        outContainerKey = "JetTracks"
    else:
        # Track-jet selection criteria, use the selection from trkOpt
        idtracksel = getIDTrackSelectionTool(trkOpt)
        outContainerKey = "JetTracksQualityCuts"

    # build the selection alg 
    trkSelAlg = CompFactory.JetTrackSelectionAlg( f"trackselalg_qual{trackSelOpt}",
                                                  TrackSelector = idtracksel,
                                                  InputContainer = trkProperties["Tracks"],
                                                  OutputContainer = trkProperties[outContainerKey],
                                                  DecorDeps = ["TTVA_AMVFWeights_forReco", "TTVA_AMVFVertices_forReco"] # Hardcoded for now... we might want to have this context-dependent ??
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


def getJetTrackVtxAlg( trkOpt,   algname="jetTVA", **ttva_overide):
    """  theSequence and ttva_overide are options used in trigger  (HLT/Jet/JetTrackingConfig.py)"""
    from TrackVertexAssociationTool.getTTVAToolForReco import getTTVAToolForReco
    from JetRecConfig.StandardJetContext import jetContextDic

    trkProperties = jetContextDic[trkOpt]

    ttva_options = dict(
        returnCompFactory = True,
        addDecoAlg= isAthenaRelease(), # ?? it seems mandatory ??
        TrackContName = trkProperties["Tracks"],
        VertexContName = trkProperties["Vertices"],
    )
    # allow client to overide options : 
    ttva_options.update(**ttva_overide)
    
    idtvassoc = getTTVAToolForReco( "jetTVatool", **ttva_options )

    alg = CompFactory.JetTrackVtxAssoAlg(algname,
                                         TrackParticleContainer  = trkProperties["Tracks"],
                                         TrackVertexAssociation  = trkProperties["TVA"],
                                         VertexContainer         = trkProperties["Vertices"],
                                         TrackVertexAssoTool     = idtvassoc                                         
                                         )
    return alg



def getPV0TrackVertexAssocTool(trkOpt="", theSequence=None):
    if trkOpt: "_{}".format(trkOpt)
    from TrackVertexAssociationTool.getTTVAToolForReco import getTTVAToolForReco
    from JetRecConfig.StandardJetContext import jetContextDic

    trkProperties = jetContextDic[trkOpt]
    tvatool = getTTVAToolForReco("trackjettvassoc",
                                WorkingPoint = "Nonprompt_All_MaxWeight",
                                TrackContName = trkProperties["JetTracksQualityCuts"],
                                VertexContName = trkProperties["Vertices"],
                                returnCompFactory = True,
                                add2Seq = theSequence,
                                addDecoAlg = isAthenaRelease(),
                                )

    jettvassoc = CompFactory.TrackVertexAssociationTool("trackjetTTVAtool",
            TrackParticleContainer = trkProperties["JetTracksQualityCuts"],
            TrackVertexAssociation = "PV0"+trkProperties["TVA"],
            VertexContainer        = trkProperties["Vertices"],
            TrackVertexAssoTool    = tvatool
            )
    return jettvassoc, tvatool

def getPV0TrackSelAlg(tvaTool, trkOpt="default"):
    from JetRecConfig.StandardJetContext import jetContextDic
    trkProperties = jetContextDic[trkOpt]
    pv0trackselalg = CompFactory.PV0TrackSelectionAlg("pv0tracksel_trackjet",
            InputTrackContainer = trkProperties["JetTracksQualityCuts"],
            VertexContainer = trkProperties["Vertices"],
            OutputTrackContainer = "PV0"+trkProperties["JetTracks"],
            TVATool = tvaTool,
            )
    return pv0trackselalg

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
